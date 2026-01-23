#ifndef FEEDBACK_WIDGET_H
#define FEEDBACK_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WDialog.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/InstructorService.h"
#include "models/TrainingSession.h"

namespace StudentIntake {
namespace Instructor {

/**
 * @brief Widget for managing student feedback
 *
 * Allows instructors to:
 * - View all feedback given
 * - Create new feedback (progress, skills, behavior, etc.)
 * - Edit existing feedback
 * - Track follow-ups
 */
class FeedbackWidget : public Wt::WContainerWidget {
public:
    FeedbackWidget();
    ~FeedbackWidget() = default;

    void setInstructorService(std::shared_ptr<Api::InstructorService> service) { instructorService_ = service; }
    void setInstructorId(int instructorId) { instructorId_ = instructorId; }

    /**
     * @brief Load and display feedback
     */
    void loadFeedback();

    /**
     * @brief Show dialog to add feedback for a student
     */
    void showAddFeedbackDialog(int studentId = 0);

    /**
     * @brief Edit existing feedback
     */
    void editFeedback(const std::string& feedbackId);

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }

private:
    void setupUI();
    void createFeedbackTable();
    void updateFeedbackTable();
    void createFeedbackDialog();
    void saveFeedback();
    void deleteFeedback(const std::string& feedbackId);
    void completeFollowUp(const std::string& feedbackId);
    void populateStudentCombo();
    void populateFeedbackTypes();

    std::shared_ptr<Api::InstructorService> instructorService_;
    int instructorId_;
    std::string editingFeedbackId_;

    // Main UI
    Wt::WContainerWidget* headerSection_;
    Wt::WText* titleText_;
    Wt::WPushButton* backButton_;
    Wt::WPushButton* addFeedbackButton_;

    // Filter section
    Wt::WContainerWidget* filterSection_;
    Wt::WComboBox* typeFilter_;
    Wt::WCheckBox* followUpFilter_;

    // Feedback table
    Wt::WTable* feedbackTable_;
    Wt::WText* noFeedbackText_;

    // Feedback dialog
    Wt::WDialog* feedbackDialog_;
    Wt::WComboBox* studentCombo_;
    Wt::WComboBox* feedbackTypeCombo_;
    Wt::WLineEdit* subjectEdit_;
    Wt::WTextArea* feedbackTextEdit_;
    Wt::WSpinBox* ratingEdit_;
    Wt::WTextArea* strengthsEdit_;
    Wt::WTextArea* improvementEdit_;
    Wt::WTextArea* actionsEdit_;
    Wt::WCheckBox* visibleToStudentCheck_;
    Wt::WCheckBox* requiresFollowUpCheck_;
    Wt::WPushButton* saveButton_;
    Wt::WPushButton* dialogCancelButton_;

    // Signals
    Wt::Signal<> backClicked_;
};

} // namespace Instructor
} // namespace StudentIntake

#endif // FEEDBACK_WIDGET_H
