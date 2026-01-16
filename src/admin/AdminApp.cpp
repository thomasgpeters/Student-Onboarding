#include "AdminApp.h"
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WText.h>
#include <iostream>

namespace StudentIntake {
namespace Admin {

AdminApp::AdminApp(const Wt::WEnvironment& env)
    : WApplication(env)
    , config_(App::AppConfig::getInstance())
    , currentState_(AppState::Login)
    , selectedStudentId_(0)
    , apiClient_(nullptr)
    , apiService_(nullptr)
    , authManager_(nullptr)
    , session_(nullptr)
    , mainContainer_(nullptr)
    , navigationWidget_(nullptr)
    , contentWrapper_(nullptr)
    , sidebarWidget_(nullptr)
    , contentContainer_(nullptr)
    , loginWidget_(nullptr)
    , dashboardWidget_(nullptr)
    , studentListWidget_(nullptr)
    , studentDetailWidget_(nullptr)
    , studentFormViewer_(nullptr)
    , formsView_(nullptr)
    , curriculumView_(nullptr)
    , settingsView_(nullptr) {

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
    setState(AppState::Login);
}

void AdminApp::setupServices() {
    // Create API client
    apiClient_ = std::make_shared<Api::ApiClient>(config_.apiBaseUrl);

    // Create form submission service
    apiService_ = std::make_shared<Api::FormSubmissionService>(apiClient_);

    // Create auth manager
    authManager_ = std::make_shared<AdminAuthManager>(apiService_);

    // Create admin session
    session_ = std::make_shared<Models::AdminSession>();
}

void AdminApp::setupUI() {
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
    // Start with login-state class since we initialize in Login state - prevents flash before setState
    contentWrapper_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentWrapper_->addStyleClass("admin-content-wrapper");
    contentWrapper_->addStyleClass("login-state");

    // Sidebar (hidden initially for login)
    sidebarWidget_ = contentWrapper_->addWidget(std::make_unique<AdminSidebar>());
    sidebarWidget_->setSession(session_);
    sidebarWidget_->sectionClicked().connect(this, &AdminApp::handleSectionChange);
    sidebarWidget_->hide();

    // Content container - single container for all views
    contentContainer_ = contentWrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer_->setId("admin-content-container-main");
    contentContainer_->addStyleClass("admin-content-container");

    // Login widget - THE ONLY login widget instance
    // Start with hidden class to prevent initial flash, shown via setState
    loginWidget_ = contentContainer_->addWidget(std::make_unique<AdminLoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &AdminApp::handleLoginSuccess);
    // Start with hidden class AND hidden state - will be explicitly shown by setState(Login)
    loginWidget_->addStyleClass("admin-login-hidden");
    loginWidget_->setHidden(true);
    std::cerr << "[AdminApp] Login widget created with ID: admin-login-widget-main" << std::endl;

    // Dashboard widget (hidden initially)
    dashboardWidget_ = contentContainer_->addWidget(std::make_unique<AdminDashboard>());
    dashboardWidget_->setSession(session_);
    dashboardWidget_->setApiService(apiService_);
    dashboardWidget_->viewStudentsClicked().connect([this]() { setState(AppState::Students); });
    dashboardWidget_->viewFormsClicked().connect([this]() { setState(AppState::Forms); });
    dashboardWidget_->viewCurriculumClicked().connect([this]() { setState(AppState::Curriculum); });
    dashboardWidget_->hide();

    // Student List widget (hidden initially)
    studentListWidget_ = contentContainer_->addWidget(std::make_unique<StudentListWidget>());
    studentListWidget_->setApiService(apiService_);
    studentListWidget_->studentSelected().connect(this, &AdminApp::handleStudentSelected);
    studentListWidget_->hide();

    // Student Detail widget (hidden initially)
    studentDetailWidget_ = contentContainer_->addWidget(std::make_unique<StudentDetailWidget>());
    studentDetailWidget_->setApiService(apiService_);
    studentDetailWidget_->backClicked().connect([this]() {
        setState(AppState::Students);
    });
    studentDetailWidget_->viewFormsClicked().connect(this, &AdminApp::handleViewStudentForms);
    studentDetailWidget_->revokeAccessClicked().connect(this, &AdminApp::handleRevokeAccess);
    studentDetailWidget_->restoreAccessClicked().connect(this, &AdminApp::handleRestoreAccess);
    studentDetailWidget_->hide();

    // Student Form Viewer widget (hidden initially)
    studentFormViewer_ = contentContainer_->addWidget(std::make_unique<StudentFormViewer>());
    studentFormViewer_->setApiService(apiService_);
    studentFormViewer_->backClicked().connect([this]() {
        showStudentDetail(selectedStudentId_);
    });
    studentFormViewer_->hide();

    // Forms view placeholder (hidden initially)
    formsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsView_->addStyleClass("admin-section-view");
    auto formsTitle = formsView_->addWidget(std::make_unique<Wt::WText>("<h2>Form Submissions</h2>"));
    formsTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto formsPlaceholder = formsView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Form review and approval features will be implemented in Phase 4.</p>"));
    formsPlaceholder->setTextFormat(Wt::TextFormat::XHTML);
    formsView_->hide();

    // Curriculum view placeholder (hidden initially)
    curriculumView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumView_->addStyleClass("admin-section-view");
    auto curriculumTitle = curriculumView_->addWidget(std::make_unique<Wt::WText>("<h2>Curriculum Management</h2>"));
    curriculumTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto curriculumPlaceholder = curriculumView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Syllabus editing and form requirements management will be implemented in Phase 3.</p>"));
    curriculumPlaceholder->setTextFormat(Wt::TextFormat::XHTML);
    curriculumView_->hide();

    // Settings view placeholder (hidden initially)
    settingsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    settingsView_->addStyleClass("admin-section-view");
    auto settingsTitle = settingsView_->addWidget(std::make_unique<Wt::WText>("<h2>System Settings</h2>"));
    settingsTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto settingsPlaceholder = settingsView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Admin user management and system configuration will be implemented in Phase 5.</p>"));
    settingsPlaceholder->setTextFormat(Wt::TextFormat::XHTML);
    settingsView_->hide();

    // Footer
    auto footer = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("admin-footer");
    auto footerText = footer->addWidget(std::make_unique<Wt::WText>(
        "© 2026 Imagery Business Systems LLC. All rights reserved."));
    footerText->addStyleClass("admin-footer-text");
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
        case AppState::Curriculum:
            showCurriculum();
            break;
        case AppState::Settings:
            showSettings();
            break;
    }
}

void AdminApp::hideAllViews() {
    hideLoginWidget();
    dashboardWidget_->hide();
    studentListWidget_->hide();
    studentDetailWidget_->hide();
    studentFormViewer_->hide();
    formsView_->hide();
    curriculumView_->hide();
    settingsView_->hide();
}

void AdminApp::hideLoginWidget() {
    // Hide login widget using both Wt's setHidden and CSS class
    loginWidget_->addStyleClass("admin-login-hidden");
    loginWidget_->setHidden(true);
    std::cerr << "[AdminApp] hideLoginWidget called" << std::endl;
}

void AdminApp::showLoginWidget() {
    // Show login widget by removing hidden class and setting visible
    loginWidget_->removeStyleClass("admin-login-hidden");
    loginWidget_->setHidden(false);
    // Ensure the widget has the correct ID (defensive check)
    if (loginWidget_->id().empty()) {
        loginWidget_->setId("admin-login-widget-main");
    }
    std::cerr << "[AdminApp] showLoginWidget called, widget ID: " << loginWidget_->id() << std::endl;
}

void AdminApp::showLogin() {
    // Hide sidebar for login (navigation stays visible)
    sidebarWidget_->hide();
    contentWrapper_->removeStyleClass("with-sidebar");
    // Add login-state class for proper navbar spacing without sidebar
    contentWrapper_->addStyleClass("login-state");
    navigationWidget_->refresh();  // Update to hide user info since not authenticated

    showLoginWidget();
    loginWidget_->focus();
}

void AdminApp::showDashboard() {
    hideLoginWidget();

    // Show sidebar (navigation already visible)
    sidebarWidget_->show();
    sidebarWidget_->refresh();
    sidebarWidget_->setActiveSection(AdminSection::Dashboard);
    navigationWidget_->refresh();  // Update user info display
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    dashboardWidget_->show();
    dashboardWidget_->refresh();
}

void AdminApp::showStudents() {
    hideLoginWidget();

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Students);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    studentListWidget_->show();
    studentListWidget_->refresh();
}

void AdminApp::showStudentDetail(int studentId) {
    hideLoginWidget();

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
    hideLoginWidget();

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
    hideLoginWidget();

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Forms);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    formsView_->show();
}

void AdminApp::showCurriculum() {
    hideLoginWidget();

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Curriculum);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    curriculumView_->show();
}

void AdminApp::showSettings() {
    hideLoginWidget();

    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Settings);
    navigationWidget_->refresh();
    contentWrapper_->removeStyleClass("login-state");
    contentWrapper_->addStyleClass("with-sidebar");

    settingsView_->show();
}

void AdminApp::handleLoginSuccess() {
    std::cerr << "[AdminApp] Login successful, showing dashboard" << std::endl;
    setState(AppState::Dashboard);
}

void AdminApp::handleLogout() {
    authManager_->logout(*session_);
    setState(AppState::Login);
}

void AdminApp::handleSectionChange(AdminSection section) {
    switch (section) {
        case AdminSection::Dashboard:
            setState(AppState::Dashboard);
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
    }
}

void AdminApp::handleStudentSelected(int studentId) {
    std::cerr << "[AdminApp] Student selected: " << studentId << std::endl;
    selectedStudentId_ = studentId;
    hideAllViews();
    showStudentDetail(studentId);
}

void AdminApp::handleViewStudentForms(int studentId) {
    std::cerr << "[AdminApp] View forms for student: " << studentId << std::endl;
    hideAllViews();
    showStudentForms(studentId);
}

void AdminApp::handleRevokeAccess(int studentId) {
    std::cerr << "[AdminApp] Revoking access for student: " << studentId << std::endl;

    if (!apiService_) {
        std::cerr << "[AdminApp] API service not available" << std::endl;
        return;
    }

    try {
        // Update student record to revoke access
        nlohmann::json updateData;
        updateData["data"]["type"] = "student";
        updateData["data"]["id"] = std::to_string(studentId);
        updateData["data"]["attributes"]["is_login_revoked"] = true;

        std::string endpoint = "/Student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->patch(endpoint, updateData.dump());

        if (response.success) {
            std::cerr << "[AdminApp] Access revoked successfully" << std::endl;
        } else {
            std::cerr << "[AdminApp] Failed to revoke access: " << response.errorMessage << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminApp] Exception revoking access: " << e.what() << std::endl;
    }
}

void AdminApp::handleRestoreAccess(int studentId) {
    std::cerr << "[AdminApp] Restoring access for student: " << studentId << std::endl;

    if (!apiService_) {
        std::cerr << "[AdminApp] API service not available" << std::endl;
        return;
    }

    try {
        // Update student record to restore access
        nlohmann::json updateData;
        updateData["data"]["type"] = "student";
        updateData["data"]["id"] = std::to_string(studentId);
        updateData["data"]["attributes"]["is_login_revoked"] = false;

        std::string endpoint = "/Student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->patch(endpoint, updateData.dump());

        if (response.success) {
            std::cerr << "[AdminApp] Access restored successfully" << std::endl;
        } else {
            std::cerr << "[AdminApp] Failed to restore access: " << response.errorMessage << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminApp] Exception restoring access: " << e.what() << std::endl;
    }
}

} // namespace Admin
} // namespace StudentIntake
