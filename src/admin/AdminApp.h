#ifndef ADMIN_APP_H
#define ADMIN_APP_H

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <memory>

#include "app/AppConfig.h"
#include "api/ApiClient.h"
#include "api/FormSubmissionService.h"
#include "admin/AdminAuthManager.h"
#include "admin/AdminLoginWidget.h"
#include "admin/AdminNavigation.h"
#include "admin/AdminSidebar.h"
#include "admin/AdminDashboard.h"
#include "admin/models/AdminSession.h"
#include "admin/students/StudentListWidget.h"
#include "admin/students/StudentDetailWidget.h"
#include "admin/students/StudentFormViewer.h"
#include "admin/curriculum/CurriculumListWidget.h"
#include "admin/curriculum/CurriculumEditorWidget.h"
#include "admin/forms/FormSubmissionsWidget.h"
#include "admin/forms/FormDetailViewer.h"
#include "admin/forms/FormPdfPreviewWidget.h"
#include "admin/forms/FormTypesListWidget.h"
#include "admin/forms/FormTypeDetailWidget.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Main application class for the Admin Portal
 */
class AdminApp : public Wt::WApplication {
public:
    AdminApp(const Wt::WEnvironment& env);
    ~AdminApp();

    // Application states
    enum class AppState {
        Login,
        Dashboard,
        Students,
        StudentDetail,
        StudentForms,
        Forms,           // Now shows FormTypesListWidget (form definitions)
        FormDetail,      // Now shows FormTypeDetailWidget (form type details)
        FormPdfPreview,
        Curriculum,
        CurriculumEdit,
        Settings
    };

    // State management
    void setState(AppState state);
    AppState getState() const { return currentState_; }

private:
    void initialize();
    void setupServices();
    void setupUI();

    // State handlers - using simple show/hide pattern like StudentIntakeApp
    void hideAllViews();
    void showLogin();
    void showDashboard();
    void showStudents();
    void showStudentDetail(int studentId);
    void showStudentForms(int studentId);
    void showForms();
    void showFormDetail(int formTypeId);
    // showFormPdfPreview removed - PDF preview is now a modal dialog
    void handleFormTypeSelected(int formTypeId);
    void showCurriculum();
    void showCurriculumEdit(const std::string& curriculumId);
    void showSettings();

    // Event handlers
    void handleLoginSuccess();
    void handleLogout();
    void handleSectionChange(AdminSection section);
    void handleStudentSelected(int studentId);
    void handleViewStudentForms(int studentId);
    void handleRevokeAccess(int studentId);
    void handleRestoreAccess(int studentId);
    void handleCurriculumSelected(const std::string& curriculumId);
    void handleAddCurriculum();
    void handleCurriculumSaved();
    void handleFormSubmissionSelected(int submissionId);
    void handleFormApproved(int submissionId);
    void handleFormRejected(int submissionId);
    void handleFormPdfPreview(int submissionId);
    void handlePrintAllStudentForms(int studentId);

    // Configuration
    App::AppConfig& config_;

    // State
    AppState currentState_;
    int selectedStudentId_;
    std::string selectedStudentName_;
    std::string selectedCurriculumId_;
    int selectedFormTypeId_;

    // Services
    std::shared_ptr<Api::ApiClient> apiClient_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::shared_ptr<AdminAuthManager> authManager_;
    std::shared_ptr<Models::AdminSession> session_;

    // UI Components
    Wt::WContainerWidget* mainContainer_;
    AdminNavigation* navigationWidget_;
    Wt::WContainerWidget* contentWrapper_;
    AdminSidebar* sidebarWidget_;
    Wt::WContainerWidget* contentContainer_;

    // Views
    AdminLoginWidget* loginWidget_;
    AdminDashboard* dashboardWidget_;
    StudentListWidget* studentListWidget_;
    StudentDetailWidget* studentDetailWidget_;
    StudentFormViewer* studentFormViewer_;
    FormSubmissionsWidget* formSubmissionsWidget_;  // Legacy - kept for FormDetailViewer
    FormDetailViewer* formDetailViewer_;            // Legacy - kept for submission viewing
    FormPdfPreviewWidget* formPdfPreviewWidget_;
    FormTypesListWidget* formTypesListWidget_;      // New - lists form definitions
    FormTypeDetailWidget* formTypeDetailWidget_;    // New - shows form type details
    CurriculumListWidget* curriculumListWidget_;
    CurriculumEditorWidget* curriculumEditorWidget_;
    Wt::WContainerWidget* settingsView_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_APP_H
