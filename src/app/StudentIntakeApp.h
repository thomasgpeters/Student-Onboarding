#ifndef STUDENT_INTAKE_APP_H
#define STUDENT_INTAKE_APP_H

#include <Wt/WApplication.h>
#include <Wt/WContainerWidget.h>
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
#include "api/ClassroomService.h"
#include "api/InstructorService.h"
#include "models/Instructor.h"

// Forward declaration
namespace StudentIntake {
namespace Classroom {
    class ClassroomWidget;
    class AssessmentReportWidget;
}
namespace Instructor {
    class InstructorDashboardWidget;
    class StudentProgressViewWidget;
    class SchedulingWidget;
    class FeedbackWidget;
    class ValidationWidget;
}
}

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
        Completion,
        Classroom,  // Online course learning environment
        InstructorDashboard,  // Instructor main view
        InstructorStudents,   // View student progress
        InstructorSchedule,   // Session scheduling
        InstructorFeedback,   // Feedback management
        InstructorValidation  // Skill validation
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
    void hideAllViews();
    void showLogin();
    void showRegister();
    void showCurriculumSelection();
    void showDashboard();
    void showForms();
    void showCompletion();
    void showClassroom();
    void showInstructorDashboard();
    void showInstructorStudents();
    void showInstructorSchedule();
    void showInstructorFeedback();
    void showInstructorValidation();

    // Event handlers
    void handleLoginSuccess();
    void handleRegistrationSuccess();
    void handleLogout();
    void handleCurriculumSelected(const Models::Curriculum& curriculum);
    void handleChangeProgram();
    void handleFormCompleted(const std::string& formId);
    void handleAllFormsCompleted();
    void handleEnterClassroom();
    void handleClassroomBack();
    void handleCourseCompleted();

    // Instructor event handlers
    void handleInstructorLogin();
    void handleInstructorBack();
    void handleViewStudents();
    void handleViewSchedule();
    void handleViewFeedback();
    void handleViewValidations();
    void handleStudentSelected(int studentId);
    void handleScheduleSession();
    void handleAddFeedback(int studentId);

    // Configuration
    AppConfig& config_;

    // State
    AppState currentState_;

    // Services
    std::shared_ptr<Api::ApiClient> apiClient_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::shared_ptr<Api::ClassroomService> classroomService_;
    std::shared_ptr<Api::InstructorService> instructorService_;
    std::shared_ptr<Auth::AuthManager> authManager_;
    std::shared_ptr<Curriculum::CurriculumManager> curriculumManager_;
    std::shared_ptr<Forms::FormFactory> formFactory_;
    std::shared_ptr<Session::StudentSession> session_;

    // Instructor state
    Models::Instructor currentInstructor_;
    bool isInstructorMode_;

    // UI Components
    Wt::WContainerWidget* mainContainer_;
    Widgets::NavigationWidget* navigationWidget_;
    Wt::WContainerWidget* contentContainer_;  // Changed from WStackedWidget

    // Views
    Auth::LoginWidget* loginWidget_;
    Auth::RegisterWidget* registerWidget_;
    Curriculum::CurriculumSelector* curriculumSelector_;
    Widgets::DashboardWidget* dashboardWidget_;
    Wt::WContainerWidget* formsView_;
    Wt::WText* programHeaderText_;
    Widgets::ProgressWidget* progressWidget_;
    Widgets::FormContainer* formContainer_;
    Wt::WContainerWidget* completionView_;
    Classroom::ClassroomWidget* classroomWidget_;
    Classroom::AssessmentReportWidget* assessmentReportWidget_;

    // Instructor views
    Instructor::InstructorDashboardWidget* instructorDashboardWidget_;
    Instructor::StudentProgressViewWidget* studentProgressWidget_;
    Instructor::SchedulingWidget* schedulingWidget_;
    Instructor::FeedbackWidget* feedbackWidget_;
    Instructor::ValidationWidget* validationWidget_;
};

} // namespace App
} // namespace StudentIntake

#endif // STUDENT_INTAKE_APP_H
