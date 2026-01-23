#ifndef ASSESSMENT_WIDGET_H
#define ASSESSMENT_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WRadioButton.h>
#include <Wt/WCheckBox.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WSignal.h>
#include <Wt/WTimer.h>
#include <memory>
#include <vector>
#include <map>
#include "api/ClassroomService.h"
#include "models/Assessment.h"

namespace StudentIntake {
namespace Classroom {

/**
 * @brief Widget for taking assessments (quizzes and exams)
 *
 * This widget handles:
 * - Displaying questions one at a time or all at once
 * - Multiple choice, true/false, and multiple select questions
 * - Time limits and countdown timer
 * - Saving answers and submitting the assessment
 * - Showing results after submission
 */
class AssessmentWidget : public Wt::WContainerWidget {
public:
    AssessmentWidget();
    ~AssessmentWidget();

    void setClassroomService(std::shared_ptr<Api::ClassroomService> service) { classroomService_ = service; }
    void setAssessment(const Models::Assessment& assessment);
    void setStudentId(int studentId) { studentId_ = studentId; }
    void setEnrollmentId(int enrollmentId) { enrollmentId_ = enrollmentId; }

    /**
     * @brief Start the assessment (creates an attempt)
     */
    void startAssessment();

    /**
     * @brief Resume an existing attempt
     */
    void resumeAttempt(const std::string& attemptId);

    // Signals
    Wt::Signal<int, double, bool>& assessmentCompleted() { return assessmentCompleted_; }
    Wt::Signal<>& backClicked() { return backClicked_; }

private:
    void setupUI();
    void setupInstructions();
    void setupQuestionArea();
    void setupResultsArea();

    void loadQuestions();
    void displayQuestion(int questionIndex);
    void displayAllQuestions();
    void saveCurrentAnswer();

    void onNextQuestion();
    void onPreviousQuestion();
    void onSubmitAssessment();
    void onTimerTick();

    void showInstructions();
    void showQuestions();
    void showResults();

    void updateTimer();
    void updateNavigationButtons();
    void updateProgressIndicator();

    std::shared_ptr<Api::ClassroomService> classroomService_;
    Models::Assessment currentAssessment_;
    Models::StudentAssessmentAttempt currentAttempt_;
    std::vector<Models::AssessmentQuestion> questions_;

    int studentId_;
    int enrollmentId_;
    int currentQuestionIndex_;

    // Answers map: question_id -> answer(s)
    std::map<int, std::string> singleAnswers_;
    std::map<int, std::vector<std::string>> multipleAnswers_;

    // Timer
    std::unique_ptr<Wt::WTimer> timer_;
    int remainingSeconds_;
    bool isTimedAssessment_;

    // UI sections
    Wt::WContainerWidget* instructionsSection_;
    Wt::WContainerWidget* questionsSection_;
    Wt::WContainerWidget* resultsSection_;

    // Instructions UI
    Wt::WText* assessmentTitle_;
    Wt::WText* assessmentInstructions_;
    Wt::WText* assessmentInfo_;
    Wt::WPushButton* startButton_;
    Wt::WPushButton* backButton_;

    // Questions UI
    Wt::WText* timerDisplay_;
    Wt::WText* progressText_;
    Wt::WContainerWidget* questionContainer_;
    Wt::WText* questionText_;
    Wt::WContainerWidget* answersContainer_;
    Wt::WPushButton* prevButton_;
    Wt::WPushButton* nextButton_;
    Wt::WPushButton* submitButton_;

    // Current question answer widgets
    std::shared_ptr<Wt::WButtonGroup> radioGroup_;
    std::vector<Wt::WCheckBox*> checkboxes_;

    // Results UI
    Wt::WText* resultTitle_;
    Wt::WText* scoreText_;
    Wt::WText* passFailText_;
    Wt::WContainerWidget* reviewContainer_;
    Wt::WPushButton* finishButton_;

    // Signals
    Wt::Signal<int, double, bool> assessmentCompleted_;
    Wt::Signal<> backClicked_;
};

} // namespace Classroom
} // namespace StudentIntake

#endif // ASSESSMENT_WIDGET_H
