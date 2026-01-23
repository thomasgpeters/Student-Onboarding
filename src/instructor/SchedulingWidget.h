#ifndef SCHEDULING_WIDGET_H
#define SCHEDULING_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WDialog.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/InstructorService.h"
#include "models/TrainingSession.h"

namespace StudentIntake {
namespace Instructor {

/**
 * @brief Widget for scheduling and managing training sessions
 *
 * Allows instructors to:
 * - View upcoming sessions
 * - Schedule new sessions (skills practice, pre-trip, road tests, etc.)
 * - Reschedule or cancel sessions
 * - Mark sessions as complete
 */
class SchedulingWidget : public Wt::WContainerWidget {
public:
    SchedulingWidget();
    ~SchedulingWidget() = default;

    void setInstructorService(std::shared_ptr<Api::InstructorService> service) { instructorService_ = service; }
    void setInstructorId(int instructorId) { instructorId_ = instructorId; }

    /**
     * @brief Load and display sessions
     */
    void loadSessions();

    /**
     * @brief Open dialog to schedule new session
     */
    void showScheduleDialog(int studentId = 0);

    /**
     * @brief Edit existing session
     */
    void editSession(const std::string& sessionId);

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<std::string>& sessionCompleted() { return sessionCompleted_; }

private:
    void setupUI();
    void createSessionsTable();
    void updateSessionsTable();
    void createScheduleDialog();
    void saveSession();
    void cancelSession(const std::string& sessionId);
    void startSession(const std::string& sessionId);
    void completeSession(const std::string& sessionId);
    void populateStudentCombo();
    void populateSessionTypes();

    std::shared_ptr<Api::InstructorService> instructorService_;
    int instructorId_;
    std::string editingSessionId_;

    // Main UI
    Wt::WContainerWidget* headerSection_;
    Wt::WText* titleText_;
    Wt::WPushButton* backButton_;
    Wt::WPushButton* newSessionButton_;

    // Filter/view controls
    Wt::WContainerWidget* filterSection_;
    Wt::WComboBox* viewFilter_;
    Wt::WDateEdit* startDateFilter_;
    Wt::WDateEdit* endDateFilter_;

    // Sessions table
    Wt::WTable* sessionsTable_;
    Wt::WText* noSessionsText_;

    // Schedule dialog
    Wt::WDialog* scheduleDialog_;
    Wt::WComboBox* studentCombo_;
    Wt::WComboBox* sessionTypeCombo_;
    Wt::WLineEdit* titleEdit_;
    Wt::WDateEdit* dateEdit_;
    Wt::WTimeEdit* startTimeEdit_;
    Wt::WTimeEdit* endTimeEdit_;
    Wt::WLineEdit* locationEdit_;
    Wt::WTextArea* notesEdit_;
    Wt::WPushButton* saveButton_;
    Wt::WPushButton* dialogCancelButton_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<std::string> sessionCompleted_;
};

} // namespace Instructor
} // namespace StudentIntake

#endif // SCHEDULING_WIDGET_H
