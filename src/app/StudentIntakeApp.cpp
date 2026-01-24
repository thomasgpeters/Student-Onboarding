#include "StudentIntakeApp.h"
#include <Wt/WText.h>
#include <Wt/WBreak.h>
#include <fstream>
#include "utils/Logger.h"

namespace StudentIntake {
namespace App {

StudentIntakeApp::StudentIntakeApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , config_(AppConfig::getInstance())
    , currentState_(AppState::Login)
    , isInstructorMode_(false)
    , mainContainer_(nullptr)
    , navigationWidget_(nullptr)
    , contentContainer_(nullptr)
    , unifiedLoginWidget_(nullptr)
    , loginWidget_(nullptr)
    , registerWidget_(nullptr)
    , roleNavigationWidget_(nullptr)
    , curriculumSelector_(nullptr)
    , dashboardWidget_(nullptr)
    , formsView_(nullptr)
    , programHeaderText_(nullptr)
    , progressWidget_(nullptr)
    , formContainer_(nullptr)
    , completionView_(nullptr)
    , administrationView_(nullptr) {

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

    // Create auth manager (legacy - for backwards compatibility)
    authManager_ = std::make_shared<Auth::AuthManager>(apiService_);

    // Create unified auth service
    authService_ = std::make_shared<Auth::AuthService>(apiClient_);

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

    // Role navigation widget (for multi-role admins) - above content
    roleNavigationWidget_ = mainContainer_->addWidget(std::make_unique<Widgets::RoleNavigationWidget>());
    roleNavigationWidget_->roleSelected().connect(this, &StudentIntakeApp::handleRoleSwitch);
    roleNavigationWidget_->hide();  // Hidden until user with multiple roles logs in

    // Content area - using simple container with manual show/hide
    contentContainer_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer_->addStyleClass("content-container");

    // Unified login view - new authentication flow at "/"
    unifiedLoginWidget_ = contentContainer_->addWidget(std::make_unique<Auth::UnifiedLoginWidget>());
    unifiedLoginWidget_->setAuthService(authService_);
    unifiedLoginWidget_->loginSuccess().connect(this, &StudentIntakeApp::handleUnifiedLoginSuccess);

    // Login view (legacy) - add directly to content container
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
        LOG_DEBUG("StudentIntakeApp", "continueClicked - hasCurriculumSelected: " << session_->hasCurriculumSelected());
        if (!session_->hasCurriculumSelected()) {
            LOG_INFO("StudentIntakeApp", "Going to CurriculumSelection from Dashboard");
            setState(AppState::CurriculumSelection);
        } else {
            LOG_INFO("StudentIntakeApp", "Going to Forms from Dashboard");
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

    // Administration view
    administrationView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    administrationView_->addStyleClass("administration-view");

    auto adminContent = administrationView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    adminContent->addStyleClass("admin-content");

    adminContent->addWidget(std::make_unique<Wt::WText>(
        "<div class='admin-header'>"
        "<h1>Administration Dashboard</h1>"
        "<p class='lead'>Welcome to the administration portal.</p>"
        "</div>"));

    auto adminGrid = adminContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    adminGrid->addStyleClass("admin-grid");

    // Admin feature cards
    auto usersCard = adminGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    usersCard->addStyleClass("admin-card");
    usersCard->addWidget(std::make_unique<Wt::WText>(
        "<div class='card-icon'>&#128101;</div>"
        "<h3>User Management</h3>"
        "<p>Manage students, instructors, and administrators</p>"));

    auto programsCard = adminGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    programsCard->addStyleClass("admin-card");
    programsCard->addWidget(std::make_unique<Wt::WText>(
        "<div class='card-icon'>&#128218;</div>"
        "<h3>Program Management</h3>"
        "<p>Configure curriculums and training programs</p>"));

    auto reportsCard = adminGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    reportsCard->addStyleClass("admin-card");
    reportsCard->addWidget(std::make_unique<Wt::WText>(
        "<div class='card-icon'>&#128202;</div>"
        "<h3>Reports</h3>"
        "<p>View enrollment and completion reports</p>"));

    auto settingsCard = adminGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    settingsCard->addStyleClass("admin-card");
    settingsCard->addWidget(std::make_unique<Wt::WText>(
        "<div class='card-icon'>&#9881;</div>"
        "<h3>System Settings</h3>"
        "<p>Configure application settings</p>"));

    // Footer
    auto footer = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("app-footer");
    auto footerContent = footer->addWidget(std::make_unique<Wt::WContainerWidget>());
    footerContent->addStyleClass("footer-content");
    footerContent->addWidget(std::make_unique<Wt::WText>(
        "<span class='copyright-text'>&copy; 2026 Imagery Business Systems LLC. All rights reserved.</span>"));

    // Hide all views initially except unified login
    hideAllViews();
    unifiedLoginWidget_->show();
}

void StudentIntakeApp::hideAllViews() {
    if (unifiedLoginWidget_) unifiedLoginWidget_->hide();
    if (loginWidget_) loginWidget_->hide();
    if (registerWidget_) registerWidget_->hide();
    if (curriculumSelector_) curriculumSelector_->hide();
    if (dashboardWidget_) dashboardWidget_->hide();
    if (formsView_) formsView_->hide();
    if (completionView_) completionView_->hide();
    if (administrationView_) administrationView_->hide();
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
        case AppState::Administration:
            showAdministration();
            break;
        case AppState::Classroom:
            showClassroom();
            break;
        case AppState::InstructorDashboard:
            showInstructorDashboard();
            break;
        case AppState::InstructorStudents:
            showInstructorStudents();
            break;
        case AppState::InstructorSchedule:
            showInstructorSchedule();
            break;
        case AppState::InstructorFeedback:
            showInstructorFeedback();
            break;
        case AppState::InstructorValidation:
            showInstructorValidation();
            break;
    }
}

void StudentIntakeApp::showLogin() {
    LOG_DEBUG("StudentIntakeApp", "showLogin called");
    hideAllViews();

    // Use unified login widget for all authentication
    unifiedLoginWidget_->show();
    unifiedLoginWidget_->reset();
    unifiedLoginWidget_->focus();

    // Hide role navigation when at login
    roleNavigationWidget_->hide();

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
    LOG_DEBUG("StudentIntakeApp", "showCurriculumSelection called");
    hideAllViews();
    curriculumSelector_->show();
    curriculumSelector_->refresh();
    navigationWidget_->refresh();
    LOG_DEBUG("StudentIntakeApp", "showCurriculumSelection completed");
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
    LOG_DEBUG("StudentIntakeApp", "showForms called");

    // Calculate required forms based on student data
    auto requiredFormIds = Session::SessionManager::getInstance().calculateRequiredForms(
        session_->getStudent(),
        session_->getCurrentCurriculum());

    LOG_DEBUG("StudentIntakeApp", "Calculated " << requiredFormIds.size() << " required forms");
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
    LOG_DEBUG("StudentIntakeApp", "handleLoginSuccess called");
    navigationWidget_->refresh();

    // Check if returning student has a curriculum_id - if so, load the curriculum
    std::string curriculumId = session_->getStudent().getCurriculumId();
    LOG_DEBUG("StudentIntakeApp", "Student curriculum_id: '" << curriculumId << "'");

    if (!curriculumId.empty() && !session_->hasCurriculumSelected()) {
        // Load curriculum from manager
        LOG_DEBUG("StudentIntakeApp", "Loading curriculum from manager...");
        auto curriculum = curriculumManager_->getCurriculum(curriculumId);
        LOG_DEBUG("StudentIntakeApp", "Loaded curriculum id: '" << curriculum.getId() << "'");

        if (!curriculum.getId().empty()) {
            session_->setCurrentCurriculum(curriculum);

            // Load required forms for this curriculum so we can check completion
            auto requiredFormIds = curriculum.getRequiredForms();
            LOG_DEBUG("StudentIntakeApp", "Required forms count: " << requiredFormIds.size());
            session_->setRequiredFormIds(requiredFormIds);
        }
    }

    LOG_DEBUG("StudentIntakeApp", "hasCurriculumSelected: " << session_->hasCurriculumSelected());

    if (session_->hasCurriculumSelected()) {
        // Check if all forms are already completed (returning student)
        LOG_DEBUG("StudentIntakeApp", "isIntakeComplete: " << session_->isIntakeComplete());
        if (session_->isIntakeComplete()) {
            // All forms are complete - go directly to completion view
            LOG_INFO("StudentIntakeApp", "Going to Completion");
            setState(AppState::Completion);
        } else {
            // Some forms still need to be filled
            LOG_INFO("StudentIntakeApp", "Going to Dashboard");
            setState(AppState::Dashboard);
        }
    } else {
        LOG_INFO("StudentIntakeApp", "Going to CurriculumSelection");
        setState(AppState::CurriculumSelection);
    }
}

void StudentIntakeApp::handleRegistrationSuccess() {
    LOG_DEBUG("StudentIntakeApp", "handleRegistrationSuccess called");
    LOG_DEBUG("StudentIntakeApp", "Session student ID: '" << session_->getStudent().getId() << "'");
    LOG_DEBUG("StudentIntakeApp", "Session student email: '" << session_->getStudent().getEmail() << "'");

    navigationWidget_->refresh();
    setState(AppState::CurriculumSelection);
}

void StudentIntakeApp::handleLogout() {
    // Logout from unified auth service
    if (authService_ && !unifiedLoginWidget_->getSessionToken().empty()) {
        authService_->logout(unifiedLoginWidget_->getSessionToken());
    }

    // Also logout from legacy auth manager
    if (authManager_) {
        authManager_->logout(*session_);
    }

    session_->reset();
    currentUser_ = Models::User();

    // Redirect to unified login at root
    redirect("/");
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
    LOG_INFO("StudentIntakeApp", "handleChangeProgram - resetting completed forms");

    // Reset completed forms for the student
    session_->getStudent().resetCompletedForms();

    // Clear curriculum selection
    session_->setCurrentCurriculum(Models::Curriculum());
    session_->getStudent().setCurriculumId("");

    // Clear required forms in session
    session_->setRequiredFormIds({});

    // Delete consent records and update student profile on server
    if (apiService_) {
        LOG_DEBUG("StudentIntakeApp", "Deleting consent records for student");
        apiService_->deleteStudentConsents(session_->getStudent().getId());

        LOG_DEBUG("StudentIntakeApp", "Updating student profile with cleared forms");
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

void StudentIntakeApp::showAdministration() {
    LOG_DEBUG("StudentIntakeApp", "showAdministration called");
    hideAllViews();
    administrationView_->show();

    // Update role navigation for multi-role users
    if (currentUser_.hasMultipleRoles()) {
        roleNavigationWidget_->setUser(currentUser_);
        roleNavigationWidget_->setActiveRole(Models::UserRole::Admin);
        roleNavigationWidget_->show();
    }

    navigationWidget_->refresh();
}

void StudentIntakeApp::showClassroom() {
    LOG_DEBUG("StudentIntakeApp", "showClassroom called");
    hideAllViews();
    // TODO: Implement classroom view
    navigationWidget_->refresh();
}

void StudentIntakeApp::showInstructorDashboard() {
    LOG_DEBUG("StudentIntakeApp", "showInstructorDashboard called");
    hideAllViews();

    // Update role navigation for multi-role users
    if (currentUser_.hasMultipleRoles()) {
        roleNavigationWidget_->setUser(currentUser_);
        roleNavigationWidget_->setActiveRole(Models::UserRole::Instructor);
        roleNavigationWidget_->show();
    }

    // TODO: Implement instructor dashboard view
    navigationWidget_->refresh();
}

void StudentIntakeApp::showInstructorStudents() {
    LOG_DEBUG("StudentIntakeApp", "showInstructorStudents called");
    hideAllViews();
    // TODO: Implement instructor students view
    navigationWidget_->refresh();
}

void StudentIntakeApp::showInstructorSchedule() {
    LOG_DEBUG("StudentIntakeApp", "showInstructorSchedule called");
    hideAllViews();
    // TODO: Implement instructor schedule view
    navigationWidget_->refresh();
}

void StudentIntakeApp::showInstructorFeedback() {
    LOG_DEBUG("StudentIntakeApp", "showInstructorFeedback called");
    hideAllViews();
    // TODO: Implement instructor feedback view
    navigationWidget_->refresh();
}

void StudentIntakeApp::showInstructorValidation() {
    LOG_DEBUG("StudentIntakeApp", "showInstructorValidation called");
    hideAllViews();
    // TODO: Implement instructor validation view
    navigationWidget_->refresh();
}

void StudentIntakeApp::handleUnifiedLoginSuccess(const Models::User& user) {
    LOG_DEBUG("StudentIntakeApp", "handleUnifiedLoginSuccess called for user: " << user.getEmail());

    // Store the authenticated user
    currentUser_ = user;

    // Update navigation
    navigationWidget_->refresh();

    // Route user based on their primary role
    routeUserByRole(user);
}

void StudentIntakeApp::handleRoleSwitch(Models::UserRole role) {
    LOG_DEBUG("StudentIntakeApp", "handleRoleSwitch called to role: " << static_cast<int>(role));

    // Get session token and user ID for passing to other portals
    std::string sessionToken = unifiedLoginWidget_->getSessionToken();
    int userId = currentUser_.getId();

    // Route to appropriate portal based on new role
    switch (role) {
        case Models::UserRole::Admin:
            LOG_INFO("StudentIntakeApp", "Redirecting to /administration");
            redirect("/administration?token=" + sessionToken + "&user_id=" + std::to_string(userId));
            return;
        case Models::UserRole::Instructor:
            LOG_INFO("StudentIntakeApp", "Redirecting to /classroom");
            redirect("/classroom?token=" + sessionToken + "&user_id=" + std::to_string(userId));
            return;
        case Models::UserRole::Student:
            LOG_INFO("StudentIntakeApp", "Redirecting to /student");
            redirect("/student?token=" + sessionToken + "&user_id=" + std::to_string(userId));
            return;
    }
}

void StudentIntakeApp::routeUserByRole(const Models::User& user) {
    LOG_DEBUG("StudentIntakeApp", "routeUserByRole - Primary role: " << static_cast<int>(user.getPrimaryRole()));

    // Get current path to determine if we're already at the correct endpoint
    std::string currentPath = environment().deploymentPath();
    LOG_DEBUG("StudentIntakeApp", "Current deployment path: " << currentPath);

    // Get session token for passing to other portals
    std::string sessionToken = unifiedLoginWidget_->getSessionToken();

    // Route based on primary role
    Models::UserRole primaryRole = user.getPrimaryRole();

    switch (primaryRole) {
        case Models::UserRole::Admin:
            // Redirect to Admin Portal with session token
            LOG_INFO("StudentIntakeApp", "Redirecting admin to /administration");
            redirect("/administration?token=" + sessionToken + "&user_id=" + std::to_string(user.getId()));
            return;

        case Models::UserRole::Instructor:
            // Redirect to Classroom Portal if not already there
            if (currentPath != "/classroom") {
                LOG_INFO("StudentIntakeApp", "Redirecting instructor to /classroom");
                redirect("/classroom?token=" + sessionToken + "&user_id=" + std::to_string(user.getId()));
                return;
            }
            // Already at /classroom - show instructor dashboard
            LOG_INFO("StudentIntakeApp", "Showing instructor dashboard");
            if (user.hasMultipleRoles()) {
                roleNavigationWidget_->setUser(user);
                roleNavigationWidget_->show();
            }
            setState(AppState::InstructorDashboard);
            break;

        case Models::UserRole::Student:
        default:
            // Redirect to Student Portal if not already there
            if (currentPath != "/student") {
                LOG_INFO("StudentIntakeApp", "Redirecting student to /student");
                redirect("/student?token=" + sessionToken + "&user_id=" + std::to_string(user.getId()));
                return;
            }
            // Already at /student - show student dashboard
            LOG_INFO("StudentIntakeApp", "Showing student dashboard");
            if (user.hasMultipleRoles()) {
                roleNavigationWidget_->setUser(user);
                roleNavigationWidget_->show();
            } else {
                roleNavigationWidget_->hide();
            }
            // For students, follow the existing flow
            if (session_->hasCurriculumSelected()) {
                if (session_->isIntakeComplete()) {
                    setState(AppState::Completion);
                } else {
                    setState(AppState::Dashboard);
                }
            } else {
                setState(AppState::CurriculumSelection);
            }
            break;
    }
}

// Instructor event handler stubs
void StudentIntakeApp::handleInstructorLogin() {
    LOG_DEBUG("StudentIntakeApp", "handleInstructorLogin called");
}

void StudentIntakeApp::handleInstructorBack() {
    setState(AppState::InstructorDashboard);
}

void StudentIntakeApp::handleViewStudents() {
    setState(AppState::InstructorStudents);
}

void StudentIntakeApp::handleViewSchedule() {
    setState(AppState::InstructorSchedule);
}

void StudentIntakeApp::handleViewFeedback() {
    setState(AppState::InstructorFeedback);
}

void StudentIntakeApp::handleViewValidations() {
    setState(AppState::InstructorValidation);
}

void StudentIntakeApp::handleStudentSelected(int studentId) {
    LOG_DEBUG("StudentIntakeApp", "handleStudentSelected: " << studentId);
}

void StudentIntakeApp::handleScheduleSession() {
    LOG_DEBUG("StudentIntakeApp", "handleScheduleSession called");
}

void StudentIntakeApp::handleAddFeedback(int studentId) {
    LOG_DEBUG("StudentIntakeApp", "handleAddFeedback for student: " << studentId);
}

// Classroom event handlers
void StudentIntakeApp::handleEnterClassroom() {
    setState(AppState::Classroom);
}

void StudentIntakeApp::handleClassroomBack() {
    setState(AppState::Dashboard);
}

void StudentIntakeApp::handleCourseCompleted() {
    LOG_DEBUG("StudentIntakeApp", "handleCourseCompleted called");
    setState(AppState::Completion);
}

} // namespace App
} // namespace StudentIntake
