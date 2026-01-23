#ifndef STUDENT_PROGRESS_VIEW_WIDGET_H
#define STUDENT_PROGRESS_VIEW_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WProgressBar.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/InstructorService.h"
#include "models/Instructor.h"

namespace StudentIntake {
namespace Instructor {

/**
 * @brief Widget for viewing and tracking student progress
 *
 * Allows instructors to:
 * - View all assigned students
 * - See detailed progress for each student
 * - Track module completion and skill development
 * - View time spent in training
 */
class StudentProgressViewWidget : public Wt::WContainerWidget {
public:
    StudentProgressViewWidget();
    ~StudentProgressViewWidget() = default;

    void setInstructorService(std::shared_ptr<Api::InstructorService> service) { instructorService_ = service; }
    void setInstructorId(int instructorId) { instructorId_ = instructorId; }

    /**
     * @brief Load and display all assigned students
     */
    void loadStudents();

    /**
     * @brief Load detailed progress for a specific student
     */
    void loadStudentDetail(int studentId);

    /**
     * @brief Go back to student list
     */
    void showStudentList();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<int>& addFeedbackClicked() { return addFeedbackClicked_; }
    Wt::Signal<int>& scheduleSessionClicked() { return scheduleSessionClicked_; }
    Wt::Signal<int>& validateSkillClicked() { return validateSkillClicked_; }

private:
    void setupUI();
    void createStudentListView();
    void createStudentDetailView();
    void updateStudentList();
    void updateStudentDetail();
    void createProgressCard(Wt::WContainerWidget* parent, const std::string& title,
                           double progress, const std::string& details);

    std::shared_ptr<Api::InstructorService> instructorService_;
    int instructorId_;
    int selectedStudentId_;
    Api::StudentProgressSummary selectedStudent_;

    // Main views
    Wt::WContainerWidget* listView_;
    Wt::WContainerWidget* detailView_;

    // List view widgets
    Wt::WText* listTitle_;
    Wt::WTable* studentsTable_;
    Wt::WText* noStudentsText_;
    Wt::WPushButton* listBackButton_;

    // Detail view widgets
    Wt::WText* studentName_;
    Wt::WText* studentEmail_;
    Wt::WContainerWidget* progressOverview_;
    Wt::WProgressBar* overallProgressBar_;
    Wt::WText* progressText_;

    Wt::WContainerWidget* moduleProgressSection_;
    Wt::WTable* moduleProgressTable_;

    Wt::WContainerWidget* skillProgressSection_;
    Wt::WTable* skillProgressTable_;

    Wt::WContainerWidget* timeTrackingSection_;
    Wt::WText* totalHoursText_;
    Wt::WText* practiceHoursText_;
    Wt::WText* classroomHoursText_;

    Wt::WContainerWidget* feedbackSection_;
    Wt::WTable* recentFeedbackTable_;

    Wt::WContainerWidget* detailActions_;
    Wt::WPushButton* detailBackButton_;
    Wt::WPushButton* addFeedbackButton_;
    Wt::WPushButton* scheduleButton_;
    Wt::WPushButton* validateButton_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<int> addFeedbackClicked_;
    Wt::Signal<int> scheduleSessionClicked_;
    Wt::Signal<int> validateSkillClicked_;
};

} // namespace Instructor
} // namespace StudentIntake

#endif // STUDENT_PROGRESS_VIEW_WIDGET_H
