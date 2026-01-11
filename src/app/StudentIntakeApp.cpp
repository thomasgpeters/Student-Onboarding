#include "StudentIntakeApp.h"
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <fstream>

namespace StudentIntake {
namespace App {

StudentIntakeApp::StudentIntakeApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , config_(AppConfig::getInstance())
    , currentState_(AppState::Login)
    , mainContainer_(nullptr)
    , navigationWidget_(nullptr)
    , contentStack_(nullptr)
    , loginWidget_(nullptr)
    , registerWidget_(nullptr)
    , curriculumSelector_(nullptr)
    , dashboardWidget_(nullptr)
    , formsView_(nullptr)
    , progressWidget_(nullptr)
    , formContainer_(nullptr)
    , completionView_(nullptr) {

    setTitle(config_.applicationTitle);

    // Use Bootstrap 5 theme
    auto theme = std::make_shared<Wt::WBootstrap5Theme>();
    setTheme(theme);

    // Add custom CSS
    useStyleSheet("resources/styles.css");

    initialize();
}

StudentIntakeApp::~StudentIntakeApp() {
}

void StudentIntakeApp::initialize() {
    setupServices();
    loadFormConfiguration();
    setupUI();
    setState(AppState::Login);
}

void StudentIntakeApp::setupServices() {
    // Create API client
    apiClient_ = std::make_shared<Api::ApiClient>(config_.apiBaseUrl);
    apiClient_->setTimeout(config_.apiTimeout);

    // Create API service
    apiService_ = std::make_shared<Api::FormSubmissionService>(apiClient_);

    // Create auth manager
    authManager_ = std::make_shared<Auth::AuthManager>(apiService_);

    // Create curriculum manager
    curriculumManager_ = std::make_shared<Curriculum::CurriculumManager>(apiService_);
    curriculumManager_->loadCurriculums();

    // Create form factory
    formFactory_ = std::make_shared<Forms::FormFactory>();

    // Create session
    session_ = Session::SessionManager::getInstance().createSession();
}

void StudentIntakeApp::loadFormConfiguration() {
    // Load form types configuration
    std::string configPath = config_.configPath + "forms_config.json";

    try {
        std::ifstream file(configPath);
        if (file.is_open()) {
            nlohmann::json config;
            file >> config;

            if (config.contains("formTypes")) {
                std::vector<Models::FormTypeInfo> formTypes;
                for (const auto& item : config["formTypes"]) {
                    formTypes.push_back(Models::FormTypeInfo::fromJson(item));
                }
                Session::SessionManager::getInstance().setFormTypeInfos(formTypes);
            }
        }
    } catch (const std::exception& e) {
        // Use default form types if config file not found
        std::vector<Models::FormTypeInfo> defaultTypes = {
            {"personal_info", "Personal Information", "Basic personal details", "core", 1, true, {}, {}, false, false, false, false, 0, 0},
            {"emergency_contact", "Emergency Contacts", "Emergency contact information", "core", 2, true, {}, {}, false, false, false, false, 0, 0},
            {"medical_info", "Medical Information", "Health information", "health", 3, false, {}, {}, false, false, false, false, 0, 0},
            {"academic_history", "Academic History", "Educational background", "academic", 4, true, {}, {}, false, true, false, false, 0, 0},
            {"financial_aid", "Financial Aid", "Financial aid application", "financial", 5, false, {}, {}, false, false, false, true, 0, 0},
            {"documents", "Document Upload", "Required documents", "documents", 6, false, {}, {}, true, true, false, false, 0, 0},
            {"consent", "Terms and Consent", "Agreements and signatures", "legal", 7, true, {}, {}, false, false, false, false, 0, 0}
        };
        Session::SessionManager::getInstance().setFormTypeInfos(defaultTypes);
    }
}

void StudentIntakeApp::setupUI() {
    mainContainer_ = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->addStyleClass("main-container");

    // Navigation
    navigationWidget_ = mainContainer_->addWidget(std::make_unique<Widgets::NavigationWidget>());
    navigationWidget_->setSession(session_);
    navigationWidget_->logoutClicked().connect(this, &StudentIntakeApp::handleLogout);
    navigationWidget_->homeClicked().connect([this]() {
        if (session_->isLoggedIn()) {
            setState(AppState::Dashboard);
        }
    });

    // Content area
    auto contentContainer = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer->addStyleClass("content-container");

    contentStack_ = contentContainer->addWidget(std::make_unique<Wt::WStackedWidget>());
    contentStack_->addStyleClass("content-stack");

    // Login view
    loginWidget_ = contentStack_->addWidget(std::make_unique<Auth::LoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &StudentIntakeApp::handleLoginSuccess);
    loginWidget_->registerRequested().connect([this]() {
        setState(AppState::Register);
    });

    // Register view
    registerWidget_ = contentStack_->addWidget(std::make_unique<Auth::RegisterWidget>());
    registerWidget_->setAuthManager(authManager_);
    registerWidget_->setSession(session_);
    registerWidget_->registrationSuccess().connect(this, &StudentIntakeApp::handleRegistrationSuccess);
    registerWidget_->loginRequested().connect([this]() {
        setState(AppState::Login);
    });

    // Curriculum selection view
    curriculumSelector_ = contentStack_->addWidget(std::make_unique<Curriculum::CurriculumSelector>());
    curriculumSelector_->setCurriculumManager(curriculumManager_);
    curriculumSelector_->setSession(session_);
    curriculumSelector_->curriculumSelected().connect(this, &StudentIntakeApp::handleCurriculumSelected);
    curriculumSelector_->backRequested().connect([this]() {
        setState(AppState::Dashboard);
    });

    // Dashboard view
    dashboardWidget_ = contentStack_->addWidget(std::make_unique<Widgets::DashboardWidget>());
    dashboardWidget_->setSession(session_);
    dashboardWidget_->continueClicked().connect([this]() {
        if (!session_->hasCurriculumSelected()) {
            setState(AppState::CurriculumSelection);
        } else {
            setState(AppState::Forms);
        }
    });

    // Forms view (with progress sidebar)
    formsView_ = contentStack_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsView_->addStyleClass("forms-view");

    auto formsLayout = formsView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsLayout->addStyleClass("forms-layout");

    // Progress sidebar
    auto sidebar = formsLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    sidebar->addStyleClass("forms-sidebar");

    progressWidget_ = sidebar->addWidget(std::make_unique<Widgets::ProgressWidget>());
    progressWidget_->setSession(session_);
    progressWidget_->setFormFactory(formFactory_);
    progressWidget_->stepClicked().connect([this](const std::string& formId) {
        formContainer_->showForm(formId);
    });

    // Forms content
    auto formsContent = formsLayout->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsContent->addStyleClass("forms-content");

    formContainer_ = formsContent->addWidget(std::make_unique<Widgets::FormContainer>());
    formContainer_->setSession(session_);
    formContainer_->setFormFactory(formFactory_);

    formContainer_->formChanged().connect([this](const std::string& formId) {
        progressWidget_->setCurrentStep(formId);
        session_->setCurrentFormId(formId);
    });

    formContainer_->formSubmitted().connect(this, &StudentIntakeApp::handleFormCompleted);
    formContainer_->allFormsCompleted().connect(this, &StudentIntakeApp::handleAllFormsCompleted);

    // Completion view
    completionView_ = contentStack_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionView_->addStyleClass("completion-view");

    auto completionContent = completionView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionContent->addStyleClass("completion-content text-center");

    completionContent->addWidget(std::make_unique<Wt::WText>(
        "<div class='completion-icon'>&#10003;</div>"
        "<h1>Application Submitted Successfully!</h1>"
        "<p class='lead'>Thank you for completing your student intake application.</p>"
        "<p>Our admissions team will review your submission and contact you within 5-7 business days.</p>"
        "<p>A confirmation email has been sent to your registered email address.</p>"));

    auto completionButtons = completionContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionButtons->addStyleClass("completion-buttons");

    auto dashboardButton = completionButtons->addWidget(std::make_unique<Wt::WPushButton>("Return to Dashboard"));
    dashboardButton->addStyleClass("btn btn-primary btn-lg");
    dashboardButton->clicked().connect([this]() {
        setState(AppState::Dashboard);
    });
}

void StudentIntakeApp::setState(AppState state) {
    currentState_ = state;

    switch (state) {
        case AppState::Login:
            showLogin();
            break;
        case AppState::Register:
            showRegister();
            break;
        case AppState::CurriculumSelection:
            showCurriculumSelection();
            break;
        case AppState::Dashboard:
            showDashboard();
            break;
        case AppState::Forms:
            showForms();
            break;
        case AppState::Completion:
            showCompletion();
            break;
    }
}

void StudentIntakeApp::showLogin() {
    contentStack_->setCurrentWidget(loginWidget_);
    loginWidget_->reset();
    loginWidget_->focus();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showRegister() {
    contentStack_->setCurrentWidget(registerWidget_);
    registerWidget_->reset();
    registerWidget_->focus();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showCurriculumSelection() {
    contentStack_->setCurrentWidget(curriculumSelector_);
    curriculumSelector_->refresh();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showDashboard() {
    contentStack_->setCurrentWidget(dashboardWidget_);
    dashboardWidget_->refresh();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showForms() {
    // Calculate required forms based on student data
    auto requiredFormIds = Session::SessionManager::getInstance().calculateRequiredForms(
        session_->getStudent(),
        session_->getCurrentCurriculum());

    session_->setRequiredFormIds(requiredFormIds);

    // Configure form factory with session and API service
    formFactory_->setSession(session_);
    formFactory_->setApiService(apiService_);

    // Load forms
    formContainer_->loadForms(requiredFormIds);
    progressWidget_->setRequiredForms(requiredFormIds);

    // Show first incomplete form
    formContainer_->showFirstIncompleteForm();

    contentStack_->setCurrentWidget(formsView_);
    progressWidget_->refresh();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showCompletion() {
    contentStack_->setCurrentWidget(completionView_);
    navigationWidget_->refresh();

    // Submit finalization to API
    if (apiService_) {
        apiService_->finalizeIntake(session_->getStudent().getId());
    }
}

void StudentIntakeApp::handleLoginSuccess() {
    navigationWidget_->refresh();

    if (session_->hasCurriculumSelected()) {
        setState(AppState::Dashboard);
    } else {
        setState(AppState::CurriculumSelection);
    }
}

void StudentIntakeApp::handleRegistrationSuccess() {
    navigationWidget_->refresh();
    setState(AppState::CurriculumSelection);
}

void StudentIntakeApp::handleLogout() {
    if (authManager_) {
        authManager_->logout(*session_);
    }
    session_->reset();
    navigationWidget_->refresh();
    setState(AppState::Login);
}

void StudentIntakeApp::handleCurriculumSelected(const Models::Curriculum& curriculum) {
    session_->setCurrentCurriculum(curriculum);
    session_->getStudent().setCurriculumId(curriculum.getId());

    // Update student profile on server
    if (apiService_) {
        apiService_->updateStudentProfile(session_->getStudent());
    }

    setState(AppState::Dashboard);
}

void StudentIntakeApp::handleFormCompleted(const std::string& formId) {
    progressWidget_->refresh();
}

void StudentIntakeApp::handleAllFormsCompleted() {
    setState(AppState::Completion);
}

} // namespace App
} // namespace StudentIntake
