#include "StudentIntakeApp.h"
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <fstream>
#include <iostream>

namespace StudentIntake {
namespace App {

StudentIntakeApp::StudentIntakeApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , config_(AppConfig::getInstance())
    , currentState_(AppState::Login)
    , mainContainer_(nullptr)
    , navigationWidget_(nullptr)
    , contentContainer_(nullptr)
    , loginWidget_(nullptr)
    , registerWidget_(nullptr)
    , curriculumSelector_(nullptr)
    , dashboardWidget_(nullptr)
    , formsView_(nullptr)
    , programHeaderText_(nullptr)
    , progressWidget_(nullptr)
    , formContainer_(nullptr)
    , completionView_(nullptr) {

    setTitle(config_.applicationTitle);

    // Use our custom CSS which includes all necessary styling (no theme)
    // Path is relative to docroot - use "styles.css" when docroot is "resources"
    useStyleSheet("styles.css");

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
    // Clear any existing content from root
    root()->clear();

    // Add main container directly to root
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

    // Content area - using simple container with manual show/hide
    contentContainer_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer_->addStyleClass("content-container");

    // Login view - add directly to content container
    loginWidget_ = contentContainer_->addWidget(std::make_unique<Auth::LoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &StudentIntakeApp::handleLoginSuccess);
    loginWidget_->registerRequested().connect([this]() {
        setState(AppState::Register);
    });

    // Register view
    registerWidget_ = contentContainer_->addWidget(std::make_unique<Auth::RegisterWidget>());
    registerWidget_->setAuthManager(authManager_);
    registerWidget_->setSession(session_);
    registerWidget_->registrationSuccess().connect(this, &StudentIntakeApp::handleRegistrationSuccess);
    registerWidget_->loginRequested().connect([this]() {
        setState(AppState::Login);
    });

    // Curriculum selection view
    curriculumSelector_ = contentContainer_->addWidget(std::make_unique<Curriculum::CurriculumSelector>());
    curriculumSelector_->setCurriculumManager(curriculumManager_);
    curriculumSelector_->setSession(session_);
    curriculumSelector_->curriculumSelected().connect(this, &StudentIntakeApp::handleCurriculumSelected);
    curriculumSelector_->backRequested().connect([this]() {
        setState(AppState::Dashboard);
    });

    // Dashboard view
    dashboardWidget_ = contentContainer_->addWidget(std::make_unique<Widgets::DashboardWidget>());
    dashboardWidget_->setSession(session_);
    dashboardWidget_->continueClicked().connect([this]() {
        std::cout << "[StudentIntakeApp] continueClicked - hasCurriculumSelected: " << session_->hasCurriculumSelected() << std::endl;
        if (!session_->hasCurriculumSelected()) {
            std::cout << "[StudentIntakeApp] Going to CurriculumSelection from Dashboard" << std::endl;
            setState(AppState::CurriculumSelection);
        } else {
            std::cout << "[StudentIntakeApp] Going to Forms from Dashboard" << std::endl;
            setState(AppState::Forms);
        }
    });
    dashboardWidget_->viewFormClicked().connect([this](const std::string& formId) {
        // Navigate to forms view and show the specific form
        setState(AppState::Forms);
        formContainer_->showForm(formId);
    });
    dashboardWidget_->changeProgramClicked().connect(this, &StudentIntakeApp::handleChangeProgram);

    // Forms view (with progress sidebar)
    formsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsView_->addStyleClass("forms-view");

    // Program header - shows selected program name for context
    auto programHeader = formsView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    programHeader->addStyleClass("program-header");
    programHeaderText_ = programHeader->addWidget(std::make_unique<Wt::WText>());
    programHeaderText_->addStyleClass("program-header-text");

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
    completionView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionView_->addStyleClass("completion-view");

    auto completionContent = completionView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionContent->addStyleClass("completion-content text-center");

    completionContent->addWidget(std::make_unique<Wt::WText>(
        "<div class='completion-icon'>&#10003;</div>"
        "<h1>Onboarding Complete!</h1>"
        "<p class='lead'>Thank you for completing your student onboarding forms.</p>"
        "<p>Our admissions team will review your submission and contact you within 5-7 business days.</p>"
        "<p>A confirmation email has been sent to your registered email address.</p>"));

    auto completionButtons = completionContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionButtons->addStyleClass("completion-buttons");

    auto dashboardButton = completionButtons->addWidget(std::make_unique<Wt::WPushButton>("Return to Dashboard"));
    dashboardButton->addStyleClass("btn btn-primary btn-lg");
    dashboardButton->clicked().connect([this]() {
        setState(AppState::Dashboard);
    });

    // Footer
    auto footer = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("app-footer");
    auto footerContent = footer->addWidget(std::make_unique<Wt::WContainerWidget>());
    footerContent->addStyleClass("footer-content");
    footerContent->addWidget(std::make_unique<Wt::WText>(
        "<span class='copyright-text'>&copy; 2026 Imagery Business Systems LLC. All rights reserved.</span>"));

    // Hide all views initially except login
    hideAllViews();
    loginWidget_->show();
}

void StudentIntakeApp::hideAllViews() {
    loginWidget_->hide();
    registerWidget_->hide();
    curriculumSelector_->hide();
    dashboardWidget_->hide();
    formsView_->hide();
    completionView_->hide();
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
    std::cout << "[StudentIntakeApp] showLogin called" << std::endl;
    hideAllViews();
    loginWidget_->show();
    loginWidget_->reset();
    loginWidget_->focus();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showRegister() {
    hideAllViews();
    registerWidget_->show();
    registerWidget_->reset();
    registerWidget_->focus();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showCurriculumSelection() {
    std::cout << "[StudentIntakeApp] showCurriculumSelection called" << std::endl;
    hideAllViews();
    curriculumSelector_->show();
    curriculumSelector_->refresh();
    navigationWidget_->refresh();
    std::cout << "[StudentIntakeApp] showCurriculumSelection completed" << std::endl;
}

void StudentIntakeApp::showDashboard() {
    // Calculate and set required forms so dashboard can show completion status
    if (session_->hasCurriculumSelected()) {
        auto requiredFormIds = Session::SessionManager::getInstance().calculateRequiredForms(
            session_->getStudent(),
            session_->getCurrentCurriculum());
        session_->setRequiredFormIds(requiredFormIds);
    }

    hideAllViews();
    dashboardWidget_->show();
    dashboardWidget_->refresh();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showForms() {
    std::cout << "[StudentIntakeApp] showForms called" << std::endl;

    // Calculate required forms based on student data
    auto requiredFormIds = Session::SessionManager::getInstance().calculateRequiredForms(
        session_->getStudent(),
        session_->getCurrentCurriculum());

    std::cout << "[StudentIntakeApp] Calculated " << requiredFormIds.size() << " required forms" << std::endl;
    session_->setRequiredFormIds(requiredFormIds);

    // Configure form factory with session and API service
    formFactory_->setSession(session_);
    formFactory_->setApiService(apiService_);

    // Update program header with selected curriculum name
    if (session_->hasCurriculumSelected()) {
        auto& curriculum = session_->getCurrentCurriculum();
        programHeaderText_->setText("<h3>" + curriculum.getName() + "</h3>");
    }

    // Load forms
    formContainer_->loadForms(requiredFormIds);
    progressWidget_->setRequiredForms(requiredFormIds);

    // Show first incomplete form
    formContainer_->showFirstIncompleteForm();

    hideAllViews();
    formsView_->show();
    progressWidget_->refresh();
    navigationWidget_->refresh();
}

void StudentIntakeApp::showCompletion() {
    hideAllViews();
    completionView_->show();
    navigationWidget_->refresh();

    // Submit finalization to API
    if (apiService_) {
        apiService_->finalizeIntake(session_->getStudent().getId());
    }
}

void StudentIntakeApp::handleLoginSuccess() {
    std::cout << "[StudentIntakeApp] handleLoginSuccess called" << std::endl;
    navigationWidget_->refresh();

    // Check if returning student has a curriculum_id - if so, load the curriculum
    std::string curriculumId = session_->getStudent().getCurriculumId();
    std::cout << "[StudentIntakeApp] Student curriculum_id: '" << curriculumId << "'" << std::endl;

    if (!curriculumId.empty() && !session_->hasCurriculumSelected()) {
        // Load curriculum from manager
        std::cout << "[StudentIntakeApp] Loading curriculum from manager..." << std::endl;
        auto curriculum = curriculumManager_->getCurriculum(curriculumId);
        std::cout << "[StudentIntakeApp] Loaded curriculum id: '" << curriculum.getId() << "'" << std::endl;

        if (!curriculum.getId().empty()) {
            session_->setCurrentCurriculum(curriculum);

            // Load required forms for this curriculum so we can check completion
            auto requiredFormIds = curriculum.getRequiredForms();
            std::cout << "[StudentIntakeApp] Required forms count: " << requiredFormIds.size() << std::endl;
            session_->setRequiredFormIds(requiredFormIds);
        }
    }

    std::cout << "[StudentIntakeApp] hasCurriculumSelected: " << session_->hasCurriculumSelected() << std::endl;

    if (session_->hasCurriculumSelected()) {
        // Check if all forms are already completed (returning student)
        std::cout << "[StudentIntakeApp] isIntakeComplete: " << session_->isIntakeComplete() << std::endl;
        if (session_->isIntakeComplete()) {
            // All forms are complete - go directly to completion view
            std::cout << "[StudentIntakeApp] Going to Completion" << std::endl;
            setState(AppState::Completion);
        } else {
            // Some forms still need to be filled
            std::cout << "[StudentIntakeApp] Going to Dashboard" << std::endl;
            setState(AppState::Dashboard);
        }
    } else {
        std::cout << "[StudentIntakeApp] Going to CurriculumSelection" << std::endl;
        setState(AppState::CurriculumSelection);
    }
}

void StudentIntakeApp::handleRegistrationSuccess() {
    std::cout << "[StudentIntakeApp] handleRegistrationSuccess called" << std::endl;
    std::cout << "[StudentIntakeApp] Session student ID: '" << session_->getStudent().getId() << "'" << std::endl;
    std::cout << "[StudentIntakeApp] Session student email: '" << session_->getStudent().getEmail() << "'" << std::endl;
    std::cout.flush();

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

void StudentIntakeApp::handleChangeProgram() {
    std::cout << "[StudentIntakeApp] handleChangeProgram - resetting completed forms" << std::endl;

    // Reset completed forms for the student
    session_->getStudent().resetCompletedForms();

    // Clear curriculum selection
    session_->setCurrentCurriculum(Models::Curriculum());
    session_->getStudent().setCurriculumId("");

    // Clear required forms in session
    session_->setRequiredFormIds({});

    // Update student profile on server to persist the reset
    if (apiService_) {
        std::cout << "[StudentIntakeApp] Updating student profile with cleared forms" << std::endl;
        apiService_->updateStudentProfile(session_->getStudent());
    }

    // Navigate to curriculum selection
    setState(AppState::CurriculumSelection);
}

void StudentIntakeApp::handleFormCompleted(const std::string& formId) {
    progressWidget_->refresh();
}

void StudentIntakeApp::handleAllFormsCompleted() {
    setState(AppState::Completion);
}

} // namespace App
} // namespace StudentIntake
