#include "AdminApp.h"
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WText.h>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

AdminApp::AdminApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , config_(App::AppConfig::getInstance())
    , currentState_(AppState::Login)
    , selectedStudentId_(0)
    , selectedCurriculumId_("")
    , selectedFormTypeId_(0)
    , selectedUserId_(0)
    , apiClient_(nullptr)
    , apiService_(nullptr)
    , authService_(nullptr)
    , authManager_(nullptr)
    , session_(nullptr)
    , mainContainer_(nullptr)
    , navigationWidget_(nullptr)
    , contentWrapper_(nullptr)
    , sidebarWidget_(nullptr)
    , contentContainer_(nullptr)
    , unifiedLoginWidget_(nullptr)
    , loginWidget_(nullptr)
    , dashboardWidget_(nullptr)
    , studentListWidget_(nullptr)
    , studentDetailWidget_(nullptr)
    , studentFormViewer_(nullptr)
    , formSubmissionsWidget_(nullptr)
    , formDetailViewer_(nullptr)
    , formPdfPreviewWidget_(nullptr)
    , formTypesListWidget_(nullptr)
    , formTypeDetailWidget_(nullptr)
    , curriculumListWidget_(nullptr)
    , curriculumEditorWidget_(nullptr)
    , settingsWidget_(nullptr)
    , userListWidget_(nullptr)
    , userEditorWidget_(nullptr) {

    setTitle("Admin Portal - Student Onboarding");

    // Use admin-specific CSS only - no dependency on student styles
    useStyleSheet("admin-styles.css");

    initialize();
}

AdminApp::~AdminApp() {
}

void AdminApp::initialize() {
    setupServices();
    setupUI();

    // Check for session token in URL (from unified login redirect)
    const std::string* tokenParam = environment().getParameter("token");
    const std::string* userIdParam = environment().getParameter("user_id");

    if (tokenParam && !tokenParam->empty() && userIdParam && !userIdParam->empty()) {
        LOG_INFO("AdminApp", "Found session token in URL, attempting auto-login...");

        try {
            int userId = std::stoi(*userIdParam);

            // Get user info using the user_id
            auto user = authService_->getUserFromSession(*tokenParam);

            // If user ID is 0, the session lookup failed - try fetching user directly
            if (user.getId() == 0) {
                // Fetch user by ID from AppUser table
                std::string endpoint = "/AppUser/" + *userIdParam;
                auto response = apiClient_->get(endpoint);
                if (response.success) {
                    auto json = response.getJson();
                    if (json.contains("data")) {
                        user = StudentIntake::Models::User::fromJson(json["data"]);
                        // Get roles for this user
                        user.setRoles(authService_->getUserRoles(userId));
                    }
                }
            }

            if (user.getId() > 0 && user.hasRole(StudentIntake::Models::UserRole::Admin)) {
                LOG_INFO("AdminApp", "Auto-login successful for user: " << user.getEmail());

                // Store the authenticated user
                currentUser_ = user;

                // Update admin session with user data
                if (session_) {
                    Admin::Models::AdminUser adminUser;
                    adminUser.setId(user.getId());
                    adminUser.setEmail(user.getEmail());
                    adminUser.setFirstName(user.getFirstName());
                    adminUser.setLastName(user.getLastName());
                    session_->setAdminUser(adminUser);
                    session_->setAuthenticated(true);
                    session_->setToken(*tokenParam);
                }

                // Go directly to dashboard
                setState(AppState::Dashboard);
                return;
            } else {
                LOG_WARN("AdminApp", "User not found or not an admin");
            }
        } catch (const std::exception& e) {
            LOG_ERROR("AdminApp", "Error during auto-login: " << e.what());
        }
    }

    // No valid session token, show login
    setState(AppState::Login);
}

void AdminApp::setupServices() {
    // Create API client
    apiClient_ = std::make_shared<Api::ApiClient>(config_.apiBaseUrl);

    // Create form submission service
    apiService_ = std::make_shared<Api::FormSubmissionService>(apiClient_);

    // Create activity log service
    activityLogService_ = std::make_shared<Api::ActivityLogService>(apiClient_);

    // Create unified auth service (single source of truth)
    authService_ = std::make_shared<Auth::AuthService>(apiClient_);

    // Create legacy auth manager (kept for backward compatibility)
    authManager_ = std::make_shared<AdminAuthManager>(apiService_);

    // Create admin session
    session_ = std::make_shared<Models::AdminSession>();
}

void AdminApp::setupUI() {
    // Clear any existing content from root - handles Wt's progressive bootstrap
    // which can create multiple app instances
    root()->clear();

    // Main container
    mainContainer_ = root()->addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->addStyleClass("admin-main-container");

    // Navigation bar (always visible - user info hidden until authenticated)
    navigationWidget_ = mainContainer_->addWidget(std::make_unique<AdminNavigation>());
    navigationWidget_->setSession(session_);
    navigationWidget_->logoutClicked().connect(this, &AdminApp::handleLogout);
    navigationWidget_->homeClicked().connect([this]() {
        setState(AppState::Dashboard);
    });
    // Navigation bar is always visible - AdminNavigation handles hiding user info when not authenticated

    // Content wrapper (sidebar + content)
    contentWrapper_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentWrapper_->addStyleClass("admin-content-wrapper");

    // Sidebar (hidden initially for login)
    sidebarWidget_ = contentWrapper_->addWidget(std::make_unique<AdminSidebar>());
    sidebarWidget_->setSession(session_);
    sidebarWidget_->sectionClicked().connect(this, &AdminApp::handleSectionChange);
    sidebarWidget_->hide();

    // Content container - single container for all views
    contentContainer_ = contentWrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer_->addStyleClass("admin-content-container");

    // Unified login widget - uses AuthService for unified authentication
    unifiedLoginWidget_ = contentContainer_->addWidget(std::make_unique<Auth::UnifiedLoginWidget>());
    unifiedLoginWidget_->setAuthService(authService_);
    unifiedLoginWidget_->loginSuccess().connect(this, &AdminApp::handleUnifiedLoginSuccess);

    // Legacy login widget (kept for fallback) - use simple show/hide pattern like StudentIntakeApp
    loginWidget_ = contentContainer_->addWidget(std::make_unique<AdminLoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &AdminApp::handleLoginSuccess);
    loginWidget_->hide();  // Hide legacy widget by default

    // Dashboard widget (hidden initially)
    dashboardWidget_ = contentContainer_->addWidget(std::make_unique<AdminDashboard>());
    dashboardWidget_->setSession(session_);
    dashboardWidget_->setApiService(apiService_);
    dashboardWidget_->setActivityService(activityLogService_);
    dashboardWidget_->viewStudentsClicked().connect([this]() { setState(AppState::Students); });
    dashboardWidget_->viewFormsClicked().connect([this]() { setState(AppState::Forms); });
    dashboardWidget_->viewCurriculumClicked().connect([this]() { setState(AppState::Curriculum); });
    dashboardWidget_->viewSettingsClicked().connect([this]() { setState(AppState::Settings); });
    dashboardWidget_->viewTodaysStudentsClicked().connect([this]() { setState(AppState::Students); });
    dashboardWidget_->viewActivityLogClicked().connect([this]() { setState(AppState::ActivityLog); });
    dashboardWidget_->hide();

    // Student List widget (hidden initially)
    studentListWidget_ = contentContainer_->addWidget(std::make_unique<StudentListWidget>());
    studentListWidget_->setApiService(apiService_);
    studentListWidget_->studentSelected().connect(this, &AdminApp::handleStudentSelected);
    studentListWidget_->hide();

    // Student Detail widget (hidden initially)
    // Form submissions are now embedded directly in this widget
    studentDetailWidget_ = contentContainer_->addWidget(std::make_unique<StudentDetailWidget>());
    studentDetailWidget_->setApiService(apiService_);
    studentDetailWidget_->backClicked().connect([this]() {
        setState(AppState::Students);
    });
    studentDetailWidget_->revokeAccessClicked().connect(this, &AdminApp::handleRevokeAccess);
    studentDetailWidget_->restoreAccessClicked().connect(this, &AdminApp::handleRestoreAccess);
    studentDetailWidget_->previewFormClicked().connect(this, &AdminApp::handleFormPdfPreview);
    studentDetailWidget_->printAllFormsClicked().connect(this, &AdminApp::handlePrintAllStudentForms);
    studentDetailWidget_->hide();

    // Student Form Viewer widget (hidden initially)
    studentFormViewer_ = contentContainer_->addWidget(std::make_unique<StudentFormViewer>());
    studentFormViewer_->setApiService(apiService_);
    studentFormViewer_->backClicked().connect([this]() {
        showStudentDetail(selectedStudentId_);
    });
    studentFormViewer_->hide();

    // Form Types List widget (hidden initially) - Shows form definitions
    formTypesListWidget_ = contentContainer_->addWidget(std::make_unique<FormTypesListWidget>());
    formTypesListWidget_->setApiService(apiService_);
    formTypesListWidget_->formTypeSelected().connect(this, &AdminApp::handleFormTypeSelected);
    formTypesListWidget_->hide();

    // Form Type Detail widget (hidden initially) - Shows form type metadata and fields
    formTypeDetailWidget_ = contentContainer_->addWidget(std::make_unique<FormTypeDetailWidget>());
    formTypeDetailWidget_->setApiService(apiService_);
    formTypeDetailWidget_->backClicked().connect([this]() {
        setState(AppState::Forms);
    });
    formTypeDetailWidget_->hide();

    // Legacy Form Submissions widget (hidden - kept for FormDetailViewer compatibility)
    formSubmissionsWidget_ = contentContainer_->addWidget(std::make_unique<FormSubmissionsWidget>());
    formSubmissionsWidget_->setApiService(apiService_);
    formSubmissionsWidget_->viewSubmissionClicked().connect(this, &AdminApp::handleFormSubmissionSelected);
    formSubmissionsWidget_->approveClicked().connect([this](int id, const std::string&) {
        handleFormApproved(id);
    });
    formSubmissionsWidget_->rejectClicked().connect([this](int id, const std::string&) {
        handleFormRejected(id);
    });
    formSubmissionsWidget_->hide();

    // Form Detail Viewer (hidden initially)
    formDetailViewer_ = contentContainer_->addWidget(std::make_unique<FormDetailViewer>());
    formDetailViewer_->setApiService(apiService_);
    formDetailViewer_->backClicked().connect([this]() {
        setState(AppState::Forms);
    });
    formDetailViewer_->approveClicked().connect(this, &AdminApp::handleFormApproved);
    formDetailViewer_->rejectClicked().connect(this, &AdminApp::handleFormRejected);
    formDetailViewer_->previewPdfClicked().connect(this, &AdminApp::handleFormPdfPreview);
    formDetailViewer_->printAllFormsClicked().connect(this, &AdminApp::handlePrintAllStudentForms);
    formDetailViewer_->hide();

    // Form PDF Preview widget (modal dialog - not added to content container)
    formPdfPreviewWidget_ = addChild(std::make_unique<FormPdfPreviewWidget>());
    formPdfPreviewWidget_->setApiService(apiService_);

    // Curriculum List widget (hidden initially)
    curriculumListWidget_ = contentContainer_->addWidget(std::make_unique<CurriculumListWidget>());
    curriculumListWidget_->setApiService(apiService_);
    curriculumListWidget_->curriculumSelected().connect(this, &AdminApp::handleCurriculumSelected);
    curriculumListWidget_->addCurriculumClicked().connect(this, &AdminApp::handleAddCurriculum);
    curriculumListWidget_->hide();

    // Curriculum Editor widget (hidden initially)
    curriculumEditorWidget_ = contentContainer_->addWidget(std::make_unique<CurriculumEditorWidget>());
    curriculumEditorWidget_->setApiService(apiService_);
    curriculumEditorWidget_->backClicked().connect([this]() {
        setState(AppState::Curriculum);
    });
    curriculumEditorWidget_->saveSuccess().connect(this, &AdminApp::handleCurriculumSaved);
    curriculumEditorWidget_->hide();

    // Institution Settings widget (hidden initially)
    settingsWidget_ = contentContainer_->addWidget(std::make_unique<InstitutionSettingsWidget>());
    settingsWidget_->addStyleClass("admin-section-view");
    settingsWidget_->setApiService(apiService_);
    settingsWidget_->hide();

    // User List widget (hidden initially)
    userListWidget_ = contentContainer_->addWidget(std::make_unique<UserListWidget>());
    userListWidget_->setApiClient(apiClient_);
    userListWidget_->setAuthService(authService_);
    userListWidget_->userSelected().connect(this, &AdminApp::handleUserSelected);
    userListWidget_->addUserClicked().connect(this, &AdminApp::handleAddUser);
    userListWidget_->hide();

    // User Editor widget (hidden initially)
    userEditorWidget_ = contentContainer_->addWidget(std::make_unique<UserEditorWidget>());
    userEditorWidget_->setApiClient(apiClient_);
    userEditorWidget_->setAuthService(authService_);
    userEditorWidget_->saved().connect(this, &AdminApp::handleUserSaved);
    userEditorWidget_->cancelled().connect(this, &AdminApp::handleUserCancelled);
    userEditorWidget_->hide();

    // Activity Log widget (full page view, hidden initially)
    activityLogWidget_ = contentContainer_->addWidget(
        std::make_unique<ActivityListWidget>(ActivityListWidget::DisplayMode::Full));
    activityLogWidget_->setActivityService(activityLogService_);
    activityLogWidget_->hide();

    // Footer
    auto footer = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("admin-footer");
    auto footerText = footer->addWidget(std::make_unique<Wt::WText>(
        "© 2026 Imagery Business Systems LLC. All rights reserved."));
    footerText->addStyleClass("admin-footer-text");

    // Hide all views initially except login - same pattern as StudentIntakeApp
    hideAllViews();
    loginWidget_->show();
}

void AdminApp::setState(AppState state) {
    currentState_ = state;
    hideAllViews();

    switch (state) {
        case AppState::Login:
            showLogin();
            break;
        case AppState::Dashboard:
            showDashboard();
            break;
        case AppState::Users:
            showUsers();
            break;
        case AppState::UserEdit:
            showUserEdit(selectedUserId_);
            break;
        case AppState::Students:
            showStudents();
            break;
        case AppState::StudentDetail:
            showStudentDetail(selectedStudentId_);
            break;
        case AppState::StudentForms:
            showStudentForms(selectedStudentId_);
            break;
        case AppState::Forms:
            showForms();
            break;
        case AppState::FormDetail:
            showFormDetail(selectedFormTypeId_);
            break;
        case AppState::FormPdfPreview:
            // PDF preview is now a modal dialog, shown via handleFormPdfPreview
            break;
        case AppState::Curriculum:
            showCurriculum();
            break;
        case AppState::CurriculumEdit:
            showCurriculumEdit(selectedCurriculumId_);
            break;
        case AppState::Settings:
            showSettings();
            break;
        case AppState::ActivityLog:
            showActivityLog();
            break;
    }
}

void AdminApp::hideAllViews() {
    // Use simple hide() pattern - same as StudentIntakeApp
    unifiedLoginWidget_->hide();
    loginWidget_->hide();  // Legacy
    dashboardWidget_->hide();
    studentListWidget_->hide();
    studentDetailWidget_->hide();
    studentFormViewer_->hide();
    formTypesListWidget_->hide();
    formTypeDetailWidget_->hide();
    formSubmissionsWidget_->hide();
    formDetailViewer_->hide();
    // formPdfPreviewWidget_ is a WDialog - it manages its own visibility
    curriculumListWidget_->hide();
    curriculumEditorWidget_->hide();
    settingsWidget_->hide();
    userListWidget_->hide();
    userEditorWidget_->hide();
    activityLogWidget_->hide();
}

void AdminApp::showLogin() {
    // hideAllViews() already called by setState()

    // Hide sidebar for login state
    sidebarWidget_->hide();
    contentWrapper_->removeStyleClass("with-sidebar");
    contentWrapper_->addStyleClass("login-state");
    navigationWidget_->refresh();

    // Show unified login widget - uses AuthService for single source of truth
    unifiedLoginWidget_->show();
    unifiedLoginWidget_->reset();
    unifiedLoginWidget_->focus();
}

void AdminApp::showDashboard() {
    // hideAllViews() already called by setState() - no need to call hideLoginWidget()

    // Show sidebar (navigation already visible)
    sidebarWidget_->show();
    sidebarWidget_->refresh();
    sidebarWidget_->setActiveSection(AdminSection::Dashboard);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    dashboardWidget_->show();
    dashboardWidget_->refresh();
}

void AdminApp::showStudents() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Students);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    studentListWidget_->show();
    studentListWidget_->refresh();
}

void AdminApp::showStudentDetail(int studentId) {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Students);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::StudentDetail;
    selectedStudentId_ = studentId;
    studentDetailWidget_->loadStudent(studentId);
    studentDetailWidget_->show();
}

void AdminApp::showStudentForms(int studentId) {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Students);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::StudentForms;
    studentFormViewer_->loadStudentForms(studentId, selectedStudentName_);
    studentFormViewer_->show();
}

void AdminApp::showForms() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Forms);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    // Show form definitions list (repurposed from form submissions)
    formTypesListWidget_->show();
    formTypesListWidget_->loadData();
}

void AdminApp::showFormDetail(int formTypeId) {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Forms);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::FormDetail;
    selectedFormTypeId_ = formTypeId;
    formTypeDetailWidget_->loadFormType(formTypeId);
    formTypeDetailWidget_->show();
}

void AdminApp::showCurriculum() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Curriculum);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    curriculumListWidget_->show();
    curriculumListWidget_->loadData();
}

void AdminApp::showCurriculumEdit(const std::string& curriculumId) {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Curriculum);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::CurriculumEdit;
    selectedCurriculumId_ = curriculumId;
    if (curriculumId.empty()) {
        curriculumEditorWidget_->createNew();
    } else {
        curriculumEditorWidget_->loadCurriculum(curriculumId);
    }
    curriculumEditorWidget_->show();
}

void AdminApp::showSettings() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Settings);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    settingsWidget_->loadSettings();
    settingsWidget_->show();
}

void AdminApp::showActivityLog() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Dashboard);  // Keep Dashboard highlighted
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    activityLogWidget_->refresh();
    activityLogWidget_->show();
}

void AdminApp::showUsers() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Users);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    // Pass current user's roles to control permissions
    userListWidget_->setCurrentUserRoles(currentUser_.getRoles());
    userListWidget_->show();
    userListWidget_->refresh();
}

void AdminApp::showUserEdit(int userId) {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Users);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::UserEdit;
    selectedUserId_ = userId;
    // Pass current user's roles to control permissions
    userEditorWidget_->setCurrentUserRoles(currentUser_.getRoles());
    userEditorWidget_->loadUser(userId);
    userEditorWidget_->show();
}

void AdminApp::showNewUser() {
    // hideAllViews() already called by setState()

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Users);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    currentState_ = AppState::UserEdit;
    selectedUserId_ = 0;
    // Pass current user's roles to control permissions
    userEditorWidget_->setCurrentUserRoles(currentUser_.getRoles());
    userEditorWidget_->newUser();
    userEditorWidget_->show();
}

void AdminApp::handleLoginSuccess() {
    LOG_INFO("AdminApp", "Login successful, showing dashboard");
    setState(AppState::Dashboard);
}

void AdminApp::handleUnifiedLoginSuccess(const StudentIntake::Models::User& user) {
    LOG_INFO("AdminApp", "Unified login successful for user: " << user.getEmail());

    // Store the authenticated user
    currentUser_ = user;

    // Check if user has admin or instructor role (both can access admin portal)
    bool isAdmin = user.hasRole(StudentIntake::Models::UserRole::Admin);
    bool isInstructor = user.hasRole(StudentIntake::Models::UserRole::Instructor);

    if (!isAdmin && !isInstructor) {
        LOG_WARN("AdminApp", "User does not have admin or instructor role");
        unifiedLoginWidget_->showError("You do not have access to this portal. Please use the student portal.");
        return;
    }

    // Update admin session with user data
    if (session_) {
        Admin::Models::AdminUser adminUser;
        adminUser.setId(user.getId());
        adminUser.setEmail(user.getEmail());
        adminUser.setFirstName(user.getFirstName());
        adminUser.setLastName(user.getLastName());

        // Set role based on user's roles (Admin takes precedence over Instructor)
        if (isAdmin) {
            adminUser.setRole(Admin::Models::AdminRole::SuperAdmin);  // Full admin access
        } else if (isInstructor) {
            adminUser.setRole(Admin::Models::AdminRole::Instructor);  // Limited instructor access
        }

        session_->setAdminUser(adminUser);
        session_->setAuthenticated(true);
        session_->setToken(unifiedLoginWidget_->getSessionToken());
    }

    setState(AppState::Dashboard);
}

void AdminApp::handleLogout() {
    if (authService_ && !unifiedLoginWidget_->getSessionToken().empty()) {
        authService_->logout(unifiedLoginWidget_->getSessionToken());
    }
    currentUser_ = StudentIntake::Models::User();
    session_->setAuthenticated(false);

    // Redirect to unified login at root
    redirect("/");
}

void AdminApp::handleSectionChange(AdminSection section) {
    switch (section) {
        case AdminSection::Dashboard:
            setState(AppState::Dashboard);
            break;
        case AdminSection::Users:
            setState(AppState::Users);
            break;
        case AdminSection::Students:
            setState(AppState::Students);
            break;
        case AdminSection::Forms:
            setState(AppState::Forms);
            break;
        case AdminSection::Curriculum:
            setState(AppState::Curriculum);
            break;
        case AdminSection::Settings:
            setState(AppState::Settings);
            break;
        case AdminSection::ActivityLog:
            setState(AppState::ActivityLog);
            break;
    }
}

void AdminApp::handleStudentSelected(int studentId) {
    LOG_DEBUG("AdminApp", "Student selected: " << studentId);
    selectedStudentId_ = studentId;
    hideAllViews();
    showStudentDetail(studentId);
}

void AdminApp::handleViewStudentForms(int studentId) {
    LOG_DEBUG("AdminApp", "View forms for student: " << studentId);
    hideAllViews();
    showStudentForms(studentId);
}

void AdminApp::handleRevokeAccess(int studentId) {
    LOG_INFO("AdminApp", "Revoking access for student: " << studentId);

    if (!apiService_) {
        LOG_ERROR("AdminApp", "API service not available");
        return;
    }

    try {
        // Update student record to revoke access
        nlohmann::json updateData;
        updateData["data"]["type"] = "Student";
        updateData["data"]["id"] = std::to_string(studentId);
        updateData["data"]["attributes"]["is_login_revoked"] = true;

        std::string endpoint = "/Student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->patch(endpoint, updateData);

        if (response.success) {
            LOG_INFO("AdminApp", "Access revoked successfully");
            // Refresh the student list to reflect the change
            studentListWidget_->refresh();
        } else {
            LOG_ERROR("AdminApp", "Failed to revoke access: " << response.errorMessage);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("AdminApp", "Exception revoking access: " << e.what());
    }
}

void AdminApp::handleRestoreAccess(int studentId) {
    LOG_INFO("AdminApp", "Restoring access for student: " << studentId);

    if (!apiService_) {
        LOG_ERROR("AdminApp", "API service not available");
        return;
    }

    try {
        // Update student record to restore access
        nlohmann::json updateData;
        updateData["data"]["type"] = "Student";
        updateData["data"]["id"] = std::to_string(studentId);
        updateData["data"]["attributes"]["is_login_revoked"] = false;

        std::string endpoint = "/Student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->patch(endpoint, updateData);

        if (response.success) {
            LOG_INFO("AdminApp", "Access restored successfully");
            // Refresh the student list to reflect the change
            studentListWidget_->refresh();
        } else {
            LOG_ERROR("AdminApp", "Failed to restore access: " << response.errorMessage);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("AdminApp", "Exception restoring access: " << e.what());
    }
}

void AdminApp::handleCurriculumSelected(const std::string& curriculumId) {
    LOG_DEBUG("AdminApp", "Curriculum selected: " << curriculumId);
    selectedCurriculumId_ = curriculumId;
    hideAllViews();
    showCurriculumEdit(curriculumId);
}

void AdminApp::handleAddCurriculum() {
    LOG_DEBUG("AdminApp", "Adding new curriculum");
    selectedCurriculumId_ = "";
    hideAllViews();
    showCurriculumEdit("");
}

void AdminApp::handleCurriculumSaved() {
    LOG_INFO("AdminApp", "Curriculum saved, returning to list");
    // Optionally stay on the edit page or go back to list
    // For now, refresh the list view
    setState(AppState::Curriculum);
}

void AdminApp::handleFormSubmissionSelected(int submissionId) {
    LOG_DEBUG("AdminApp", "Form submission selected: " << submissionId);
    // Legacy handler - no longer used for main Forms section
    // Kept for FormDetailViewer compatibility
}

void AdminApp::handleFormTypeSelected(int formTypeId) {
    LOG_DEBUG("AdminApp", "Form type selected: " << formTypeId);
    selectedFormTypeId_ = formTypeId;
    hideAllViews();
    showFormDetail(formTypeId);
}

void AdminApp::handleFormApproved(int submissionId) {
    LOG_INFO("AdminApp", "Form approved: " << submissionId);
    // Refresh the submissions list if we're on it
    if (currentState_ == AppState::Forms) {
        formSubmissionsWidget_->loadData();
    }
}

void AdminApp::handleFormRejected(int submissionId) {
    LOG_INFO("AdminApp", "Form rejected: " << submissionId);
    // Refresh the submissions list if we're on it
    if (currentState_ == AppState::Forms) {
        formSubmissionsWidget_->loadData();
    }
}

void AdminApp::handleFormPdfPreview(int submissionId) {
    LOG_DEBUG("AdminApp", "Form PDF preview requested: " << submissionId);
    // Show the PDF preview as a modal dialog - no state change needed
    formPdfPreviewWidget_->showFormSubmission(submissionId);
}

void AdminApp::handlePrintAllStudentForms(int studentId) {
    LOG_DEBUG("AdminApp", "Print all student forms requested: " << studentId);
    // Show the PDF preview as a modal dialog - no state change needed
    formPdfPreviewWidget_->showStudentForms(studentId);
}

void AdminApp::handleUserSelected(int userId) {
    LOG_DEBUG("AdminApp", "User selected: " << userId);
    selectedUserId_ = userId;
    hideAllViews();
    showUserEdit(userId);
}

void AdminApp::handleAddUser() {
    LOG_DEBUG("AdminApp", "Adding new user");
    hideAllViews();
    showNewUser();
}

void AdminApp::handleUserSaved() {
    LOG_INFO("AdminApp", "User saved, returning to list");
    setState(AppState::Users);
}

void AdminApp::handleUserCancelled() {
    LOG_DEBUG("AdminApp", "User edit cancelled, returning to list");
    setState(AppState::Users);
}

} // namespace Admin
} // namespace StudentIntake
