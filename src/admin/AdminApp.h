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
        Forms,
        Curriculum,
        Settings
    };

    // State management
    void setState(AppState state);
    AppState getState() const { return currentState_; }

private:
    void initialize();
    void setupServices();
    void setupUI();

    // State handlers
    void hideAllViews();
    void showLogin();
    void showDashboard();
    void showStudents();
    void showForms();
    void showCurriculum();
    void showSettings();

    // Event handlers
    void handleLoginSuccess();
    void handleLogout();
    void handleSectionChange(AdminSection section);

    // Configuration
    App::AppConfig& config_;

    // State
    AppState currentState_;

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
    Wt::WContainerWidget* studentsView_;
    Wt::WContainerWidget* formsView_;
    Wt::WContainerWidget* curriculumView_;
    Wt::WContainerWidget* settingsView_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_APP_H
