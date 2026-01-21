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
    , selectedCurriculumId_("")
    , selectedFormTypeId_(0)
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
    , formSubmissionsWidget_(nullptr)
    , formDetailViewer_(nullptr)
    , formPdfPreviewWidget_(nullptr)
    , formTypesListWidget_(nullptr)
    , formTypeDetailWidget_(nullptr)
    , curriculumListWidget_(nullptr)
    , curriculumEditorWidget_(nullptr)
    , settingsWidget_(nullptr) {

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

    // Login widget - use simple show/hide pattern like StudentIntakeApp
    loginWidget_ = contentContainer_->addWidget(std::make_unique<AdminLoginWidget>());
    loginWidget_->setAuthManager(authManager_);
    loginWidget_->setSession(session_);
    loginWidget_->loginSuccess().connect(this, &AdminApp::handleLoginSuccess);

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
    }
}

void AdminApp::hideAllViews() {
    // Use simple hide() pattern - same as StudentIntakeApp
    loginWidget_->hide();
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
}

void AdminApp::showLogin() {
    // hideAllViews() already called by setState()

    // Hide sidebar for login state
    sidebarWidget_->hide();
    contentWrapper_->removeStyleClass("with-sidebar");
    contentWrapper_->addStyleClass("login-state");
    navigationWidget_->refresh();

    // Show login widget using simple show() - same as StudentIntakeApp
    loginWidget_->show();
    loginWidget_->reset();
    loginWidget_->focus();
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
        auto response = apiService_->getApiClient()->patch(endpoint, updateData);

        if (response.success) {
            std::cerr << "[AdminApp] Access revoked successfully" << std::endl;
            // Refresh the student list to reflect the change
            studentListWidget_->refresh();
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
        auto response = apiService_->getApiClient()->patch(endpoint, updateData);

        if (response.success) {
            std::cerr << "[AdminApp] Access restored successfully" << std::endl;
            // Refresh the student list to reflect the change
            studentListWidget_->refresh();
        } else {
            std::cerr << "[AdminApp] Failed to restore access: " << response.errorMessage << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[AdminApp] Exception restoring access: " << e.what() << std::endl;
    }
}

void AdminApp::handleCurriculumSelected(const std::string& curriculumId) {
    std::cerr << "[AdminApp] Curriculum selected: " << curriculumId << std::endl;
    selectedCurriculumId_ = curriculumId;
    hideAllViews();
    showCurriculumEdit(curriculumId);
}

void AdminApp::handleAddCurriculum() {
    std::cerr << "[AdminApp] Adding new curriculum" << std::endl;
    selectedCurriculumId_ = "";
    hideAllViews();
    showCurriculumEdit("");
}

void AdminApp::handleCurriculumSaved() {
    std::cerr << "[AdminApp] Curriculum saved, returning to list" << std::endl;
    // Optionally stay on the edit page or go back to list
    // For now, refresh the list view
    setState(AppState::Curriculum);
}

void AdminApp::handleFormSubmissionSelected(int submissionId) {
    std::cerr << "[AdminApp] Form submission selected: " << submissionId << std::endl;
    // Legacy handler - no longer used for main Forms section
    // Kept for FormDetailViewer compatibility
}

void AdminApp::handleFormTypeSelected(int formTypeId) {
    std::cerr << "[AdminApp] Form type selected: " << formTypeId << std::endl;
    selectedFormTypeId_ = formTypeId;
    hideAllViews();
    showFormDetail(formTypeId);
}

void AdminApp::handleFormApproved(int submissionId) {
    std::cerr << "[AdminApp] Form approved: " << submissionId << std::endl;
    // Refresh the submissions list if we're on it
    if (currentState_ == AppState::Forms) {
        formSubmissionsWidget_->loadData();
    }
}

void AdminApp::handleFormRejected(int submissionId) {
    std::cerr << "[AdminApp] Form rejected: " << submissionId << std::endl;
    // Refresh the submissions list if we're on it
    if (currentState_ == AppState::Forms) {
        formSubmissionsWidget_->loadData();
    }
}

void AdminApp::handleFormPdfPreview(int submissionId) {
    std::cerr << "[AdminApp] Form PDF preview requested: " << submissionId << std::endl;
    // Show the PDF preview as a modal dialog - no state change needed
    formPdfPreviewWidget_->showFormSubmission(submissionId);
}

void AdminApp::handlePrintAllStudentForms(int studentId) {
    std::cerr << "[AdminApp] Print all student forms requested: " << studentId << std::endl;
    // Show the PDF preview as a modal dialog - no state change needed
    formPdfPreviewWidget_->showStudentForms(studentId);
}

} // namespace Admin
} // namespace StudentIntake
