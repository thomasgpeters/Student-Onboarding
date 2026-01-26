#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include "admin/models/AdminSession.h"
#include "api/FormSubmissionService.h"
#include "api/ActivityLogService.h"

namespace StudentIntake {
namespace Admin {
    class ActivityListWidget;  // Forward declaration
}
}

namespace StudentIntake {
namespace Admin {

/**
 * @brief Admin dashboard home page showing overview statistics
 */
class AdminDashboard : public Wt::WContainerWidget {
public:
    AdminDashboard();
    ~AdminDashboard();

    void setSession(std::shared_ptr<Models::AdminSession> session) { session_ = session; }
    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) { apiService_ = apiService; }
    void setActivityService(std::shared_ptr<Api::ActivityLogService> activityService) { activityService_ = activityService; }

    // Refresh dashboard data
    void refresh();

    // Signals for navigation
    Wt::Signal<>& viewStudentsClicked() { return viewStudentsClicked_; }
    Wt::Signal<>& viewFormsClicked() { return viewFormsClicked_; }
    Wt::Signal<>& viewCurriculumClicked() { return viewCurriculumClicked_; }
    Wt::Signal<>& viewSettingsClicked() { return viewSettingsClicked_; }
    Wt::Signal<>& viewTodaysStudentsClicked() { return viewTodaysStudentsClicked_; }
    Wt::Signal<>& viewActivityLogClicked() { return viewActivityLogClicked_; }
    Wt::Signal<int>& activityDetailClicked() { return activityDetailClicked_; }

private:
    void setupUI();
    void loadStatistics();
    void updateDisplay();

    std::shared_ptr<Models::AdminSession> session_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::shared_ptr<Api::ActivityLogService> activityService_;

    // UI elements
    Wt::WText* welcomeText_;
    Wt::WText* studentCountText_;
    Wt::WText* todaysStudentsText_;
    Wt::WText* completedOnboardingText_;
    Wt::WText* programCountText_;
    Wt::WContainerWidget* activityContainer_;
    Wt::WContainerWidget* quickActionsContainer_;
    ActivityListWidget* activityListWidget_;

    // Statistics
    int totalStudents_;
    int todaysStudents_;
    int completedOnboarding_;
    int activePrograms_;

    // Signals
    Wt::Signal<> viewStudentsClicked_;
    Wt::Signal<> viewFormsClicked_;
    Wt::Signal<> viewCurriculumClicked_;
    Wt::Signal<> viewSettingsClicked_;
    Wt::Signal<> viewTodaysStudentsClicked_;
    Wt::Signal<> viewActivityLogClicked_;
    Wt::Signal<int> activityDetailClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_DASHBOARD_H
