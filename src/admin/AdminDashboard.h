#ifndef ADMIN_DASHBOARD_H
#define ADMIN_DASHBOARD_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include "admin/models/AdminSession.h"
#include "api/FormSubmissionService.h"

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

    // Refresh dashboard data
    void refresh();

    // Signals for navigation
    Wt::Signal<>& viewStudentsClicked() { return viewStudentsClicked_; }
    Wt::Signal<>& viewFormsClicked() { return viewFormsClicked_; }
    Wt::Signal<>& viewCurriculumClicked() { return viewCurriculumClicked_; }

private:
    void setupUI();
    void loadStatistics();
    void updateDisplay();

    std::shared_ptr<Models::AdminSession> session_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;

    // UI elements
    Wt::WText* welcomeText_;
    Wt::WText* studentCountText_;
    Wt::WText* pendingFormsText_;
    Wt::WText* programCountText_;
    Wt::WContainerWidget* activityContainer_;
    Wt::WContainerWidget* quickActionsContainer_;

    // Statistics
    int totalStudents_;
    int pendingForms_;
    int activePrograms_;

    // Signals
    Wt::Signal<> viewStudentsClicked_;
    Wt::Signal<> viewFormsClicked_;
    Wt::Signal<> viewCurriculumClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_DASHBOARD_H
