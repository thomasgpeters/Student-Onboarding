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
    , studentsView_(nullptr)
    , formsView_(nullptr)
    , curriculumView_(nullptr)
    , settingsView_(nullptr) {

    setTitle("Admin Portal - Student Onboarding");

    // Use custom CSS
    useStyleSheet("resources/styles.css");
    useStyleSheet("resources/admin-styles.css");

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
    apiClient_ = std::make_shared<Api::ApiClient>(config_.getApiBaseUrl());

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

    // Navigation bar
    navigationWidget_ = mainContainer_->addWidget(std::make_unique<AdminNavigation>());
    navigationWidget_->setSession(session_);
    navigationWidget_->logoutClicked().connect(this, &AdminApp::handleLogout);
    navigationWidget_->homeClicked().connect([this]() {
        setState(AppState::Dashboard);
    });

    // Content wrapper (sidebar + content)
    contentWrapper_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentWrapper_->addStyleClass("admin-content-wrapper");

    // Sidebar
    sidebarWidget_ = contentWrapper_->addWidget(std::make_unique<AdminSidebar>());
    sidebarWidget_->setSession(session_);
    sidebarWidget_->sectionClicked().connect(this, &AdminApp::handleSectionChange);

    // Content container
    contentContainer_ = contentWrapper_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer_->addStyleClass("admin-content-container");

    // Login widget (full screen, separate from sidebar layout)
    loginWidget_ = contentContainer_->addWidget(std::make_unique<AdminLoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &AdminApp::handleLoginSuccess);

    // Dashboard widget
    dashboardWidget_ = contentContainer_->addWidget(std::make_unique<AdminDashboard>());
    dashboardWidget_->setSession(session_);
    dashboardWidget_->setApiService(apiService_);
    dashboardWidget_->viewStudentsClicked().connect([this]() { setState(AppState::Students); });
    dashboardWidget_->viewFormsClicked().connect([this]() { setState(AppState::Forms); });
    dashboardWidget_->viewCurriculumClicked().connect([this]() { setState(AppState::Curriculum); });

    // Students view placeholder
    studentsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentsView_->addStyleClass("admin-section-view");
    auto studentsTitle = studentsView_->addWidget(std::make_unique<Wt::WText>("<h2>Student Management</h2>"));
    studentsTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto studentsPlaceholder = studentsView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Student list, search, and management features will be implemented in Phase 2.</p>"));
    studentsPlaceholder->setTextFormat(Wt::TextFormat::XHTML);

    // Forms view placeholder
    formsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsView_->addStyleClass("admin-section-view");
    auto formsTitle = formsView_->addWidget(std::make_unique<Wt::WText>("<h2>Form Submissions</h2>"));
    formsTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto formsPlaceholder = formsView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Form review and approval features will be implemented in Phase 4.</p>"));
    formsPlaceholder->setTextFormat(Wt::TextFormat::XHTML);

    // Curriculum view placeholder
    curriculumView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumView_->addStyleClass("admin-section-view");
    auto curriculumTitle = curriculumView_->addWidget(std::make_unique<Wt::WText>("<h2>Curriculum Management</h2>"));
    curriculumTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto curriculumPlaceholder = curriculumView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Syllabus editing and form requirements management will be implemented in Phase 3.</p>"));
    curriculumPlaceholder->setTextFormat(Wt::TextFormat::XHTML);

    // Settings view placeholder
    settingsView_ = contentContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    settingsView_->addStyleClass("admin-section-view");
    auto settingsTitle = settingsView_->addWidget(std::make_unique<Wt::WText>("<h2>System Settings</h2>"));
    settingsTitle->setTextFormat(Wt::TextFormat::XHTML);
    auto settingsPlaceholder = settingsView_->addWidget(std::make_unique<Wt::WText>(
        "<p>Admin user management and system configuration will be implemented in Phase 5.</p>"));
    settingsPlaceholder->setTextFormat(Wt::TextFormat::XHTML);

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
    loginWidget_->hide();
    dashboardWidget_->hide();
    studentsView_->hide();
    formsView_->hide();
    curriculumView_->hide();
    settingsView_->hide();
}

void AdminApp::showLogin() {
    // Hide sidebar and navigation for login
    sidebarWidget_->hide();
    navigationWidget_->hide();
    contentWrapper_->removeStyleClass("with-sidebar");

    loginWidget_->show();
    loginWidget_->focus();
}

void AdminApp::showDashboard() {
    // Show sidebar and navigation
    sidebarWidget_->show();
    sidebarWidget_->refresh();
    sidebarWidget_->setActiveSection(AdminSection::Dashboard);
    navigationWidget_->show();
    navigationWidget_->refresh();
    contentWrapper_->addStyleClass("with-sidebar");

    dashboardWidget_->show();
    dashboardWidget_->refresh();
}

void AdminApp::showStudents() {
    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Students);
    navigationWidget_->show();
    contentWrapper_->addStyleClass("with-sidebar");

    studentsView_->show();
}

void AdminApp::showForms() {
    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Forms);
    navigationWidget_->show();
    contentWrapper_->addStyleClass("with-sidebar");

    formsView_->show();
}

void AdminApp::showCurriculum() {
    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Curriculum);
    navigationWidget_->show();
    contentWrapper_->addStyleClass("with-sidebar");

    curriculumView_->show();
}

void AdminApp::showSettings() {
    sidebarWidget_->show();
    sidebarWidget_->setActiveSection(AdminSection::Settings);
    navigationWidget_->show();
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

} // namespace Admin
} // namespace StudentIntake
