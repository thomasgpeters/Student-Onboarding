#ifndef INSTRUCTOR_DASHBOARD_WIDGET_H
#define INSTRUCTOR_DASHBOARD_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/InstructorService.h"
#include "models/Instructor.h"

namespace StudentIntake {
namespace Instructor {

/**
 * @brief Main dashboard widget for instructors
 *
 * Provides an overview of instructor activities including:
 * - Statistics summary (active students, upcoming sessions, etc.)
 * - Today's schedule
 * - Students needing attention
 * - Quick actions
 */
class InstructorDashboardWidget : public Wt::WContainerWidget {
public:
    InstructorDashboardWidget();
    ~InstructorDashboardWidget() = default;

    void setInstructorService(std::shared_ptr<Api::InstructorService> service) { instructorService_ = service; }
    void setInstructor(const Models::Instructor& instructor);

    /**
     * @brief Refresh dashboard data
     */
    void refresh();

    // Signals for navigation
    Wt::Signal<>& viewStudentsClicked() { return viewStudentsClicked_; }
    Wt::Signal<>& viewScheduleClicked() { return viewScheduleClicked_; }
    Wt::Signal<>& viewFeedbackClicked() { return viewFeedbackClicked_; }
    Wt::Signal<>& viewValidationsClicked() { return viewValidationsClicked_; }
    Wt::Signal<int>& studentSelected() { return studentSelected_; }
    Wt::Signal<std::string>& sessionSelected() { return sessionSelected_; }
    Wt::Signal<>& scheduleSessionClicked() { return scheduleSessionClicked_; }
    Wt::Signal<>& addFeedbackClicked() { return addFeedbackClicked_; }
    Wt::Signal<>& backClicked() { return backClicked_; }

private:
    void setupUI();
    void updateDashboard();
    void createStatsSection();
    void createTodayScheduleSection();
    void createStudentsSection();
    void createQuickActionsSection();
    void updateStats();
    void updateTodaySchedule();
    void updateStudentsList();

    std::shared_ptr<Api::InstructorService> instructorService_;
    Models::Instructor instructor_;

    // Main layout
    Wt::WContainerWidget* headerSection_;
    Wt::WContainerWidget* statsSection_;
    Wt::WContainerWidget* contentSection_;

    // Header widgets
    Wt::WText* welcomeText_;
    Wt::WText* instructorTypeText_;
    Wt::WPushButton* backButton_;

    // Stats widgets
    Wt::WContainerWidget* statsGrid_;
    Wt::WText* activeStudentsValue_;
    Wt::WText* sessionsTodayValue_;
    Wt::WText* pendingFeedbackValue_;
    Wt::WText* avgProgressValue_;

    // Today's schedule section
    Wt::WContainerWidget* scheduleSection_;
    Wt::WText* scheduleTitle_;
    Wt::WTable* scheduleTable_;
    Wt::WText* noSessionsText_;

    // Students section
    Wt::WContainerWidget* studentsSection_;
    Wt::WText* studentsTitle_;
    Wt::WTable* studentsTable_;
    Wt::WText* noStudentsText_;

    // Quick actions section
    Wt::WContainerWidget* actionsSection_;
    Wt::WPushButton* scheduleSessionButton_;
    Wt::WPushButton* addFeedbackButton_;
    Wt::WPushButton* viewAllStudentsButton_;
    Wt::WPushButton* viewScheduleButton_;

    // Signals
    Wt::Signal<> viewStudentsClicked_;
    Wt::Signal<> viewScheduleClicked_;
    Wt::Signal<> viewFeedbackClicked_;
    Wt::Signal<> viewValidationsClicked_;
    Wt::Signal<int> studentSelected_;
    Wt::Signal<std::string> sessionSelected_;
    Wt::Signal<> scheduleSessionClicked_;
    Wt::Signal<> addFeedbackClicked_;
    Wt::Signal<> backClicked_;
};

} // namespace Instructor
} // namespace StudentIntake

#endif // INSTRUCTOR_DASHBOARD_WIDGET_H
