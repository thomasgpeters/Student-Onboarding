#ifndef STUDENT_INTAKE_APP_H
#define STUDENT_INTAKE_APP_H

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <memory>

#include "AppConfig.h"
#include "session/StudentSession.h"
#include "session/SessionManager.h"
#include "api/ApiClient.h"
#include "api/FormSubmissionService.h"
#include "auth/AuthManager.h"
#include "auth/LoginWidget.h"
#include "auth/RegisterWidget.h"
#include "curriculum/CurriculumManager.h"
#include "curriculum/CurriculumSelector.h"
#include "forms/FormFactory.h"
#include "widgets/NavigationWidget.h"
#include "widgets/ProgressWidget.h"
#include "widgets/FormContainer.h"
#include "widgets/DashboardWidget.h"

namespace StudentIntake {
namespace App {

/**
 * @brief Main application class for the Student Intake Forms system
 */
class StudentIntakeApp : public Wt::WApplication {
public:
    StudentIntakeApp(const Wt::WEnvironment& env);
    ~StudentIntakeApp();

    // Application states
    enum class AppState {
        Login,
        Register,
        CurriculumSelection,
        Dashboard,
        Forms,
        Completion
    };

    // State management
    void setState(AppState state);
    AppState getState() const { return currentState_; }

private:
    void initialize();
    void setupServices();
    void setupUI();
    void loadFormConfiguration();

    // State handlers
    void showLogin();
    void showRegister();
    void showCurriculumSelection();
    void showDashboard();
    void showForms();
    void showCompletion();

    // Event handlers
    void handleLoginSuccess();
    void handleRegistrationSuccess();
    void handleLogout();
    void handleCurriculumSelected(const Models::Curriculum& curriculum);
    void handleFormCompleted(const std::string& formId);
    void handleAllFormsCompleted();

    // Configuration
    AppConfig& config_;

    // State
    AppState currentState_;

    // Services
    std::shared_ptr<Api::ApiClient> apiClient_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::shared_ptr<Auth::AuthManager> authManager_;
    std::shared_ptr<Curriculum::CurriculumManager> curriculumManager_;
    std::shared_ptr<Forms::FormFactory> formFactory_;
    std::shared_ptr<Session::StudentSession> session_;

    // UI Components
    Wt::WContainerWidget* mainContainer_;
    Widgets::NavigationWidget* navigationWidget_;
    Wt::WStackedWidget* contentStack_;

    // Views
    Auth::LoginWidget* loginWidget_;
    Auth::RegisterWidget* registerWidget_;
    Curriculum::CurriculumSelector* curriculumSelector_;
    Widgets::DashboardWidget* dashboardWidget_;
    Wt::WContainerWidget* formsView_;
    Widgets::ProgressWidget* progressWidget_;
    Widgets::FormContainer* formContainer_;
    Wt::WContainerWidget* completionView_;
};

} // namespace App
} // namespace StudentIntake

#endif // STUDENT_INTAKE_APP_H
