#include "AssessmentWidget.h"
#include <Wt/WBreak.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>

namespace StudentIntake {
namespace Classroom {

AssessmentWidget::AssessmentWidget()
    : studentId_(0)
    , enrollmentId_(0)
    , currentQuestionIndex_(0)
    , remainingSeconds_(0)
    , isTimedAssessment_(false)
    , instructionsSection_(nullptr)
    , questionsSection_(nullptr)
    , resultsSection_(nullptr)
    , assessmentTitle_(nullptr)
    , assessmentInstructions_(nullptr)
    , assessmentInfo_(nullptr)
    , startButton_(nullptr)
    , backButton_(nullptr)
    , timerDisplay_(nullptr)
    , progressText_(nullptr)
    , questionContainer_(nullptr)
    , questionText_(nullptr)
    , answersContainer_(nullptr)
    , prevButton_(nullptr)
    , nextButton_(nullptr)
    , submitButton_(nullptr)
    , resultTitle_(nullptr)
    , scoreText_(nullptr)
    , passFailText_(nullptr)
    , reviewContainer_(nullptr)
    , finishButton_(nullptr) {
    setupUI();
}

AssessmentWidget::~AssessmentWidget() {
    if (timer_) {
        timer_->stop();
    }
}

void AssessmentWidget::setupUI() {
    addStyleClass("assessment-widget");

    setupInstructions();
    setupQuestionArea();
    setupResultsArea();

    // Show instructions by default
    showInstructions();
}

void AssessmentWidget::setupInstructions() {
    instructionsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    instructionsSection_->addStyleClass("assessment-instructions");

    assessmentTitle_ = instructionsSection_->addWidget(std::make_unique<Wt::WText>());
    assessmentTitle_->addStyleClass("assessment-title");

    assessmentInstructions_ = instructionsSection_->addWidget(std::make_unique<Wt::WText>());
    assessmentInstructions_->addStyleClass("assessment-instructions-text");

    assessmentInfo_ = instructionsSection_->addWidget(std::make_unique<Wt::WText>());
    assessmentInfo_->addStyleClass("assessment-info");

    auto buttonContainer = instructionsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("assessment-buttons");

    backButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    backButton_->addStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this] {
        backClicked_.emit();
    });

    startButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Start Assessment"));
    startButton_->addStyleClass("btn btn-primary");
    startButton_->clicked().connect([this] {
        loadQuestions();
        showQuestions();
    });
}

void AssessmentWidget::setupQuestionArea() {
    questionsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    questionsSection_->addStyleClass("assessment-questions");

    // Header with timer and progress
    auto header = questionsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("questions-header");

    progressText_ = header->addWidget(std::make_unique<Wt::WText>());
    progressText_->addStyleClass("progress-text");

    timerDisplay_ = header->addWidget(std::make_unique<Wt::WText>());
    timerDisplay_->addStyleClass("timer-display");

    // Question container
    questionContainer_ = questionsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    questionContainer_->addStyleClass("question-container");

    questionText_ = questionContainer_->addWidget(std::make_unique<Wt::WText>());
    questionText_->addStyleClass("question-text");

    answersContainer_ = questionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    answersContainer_->addStyleClass("answers-container");

    // Navigation buttons
    auto navButtons = questionsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    navButtons->addStyleClass("nav-buttons");

    prevButton_ = navButtons->addWidget(std::make_unique<Wt::WPushButton>("Previous"));
    prevButton_->addStyleClass("btn btn-secondary");
    prevButton_->clicked().connect([this] {
        onPreviousQuestion();
    });

    nextButton_ = navButtons->addWidget(std::make_unique<Wt::WPushButton>("Next"));
    nextButton_->addStyleClass("btn btn-primary");
    nextButton_->clicked().connect([this] {
        onNextQuestion();
    });

    submitButton_ = navButtons->addWidget(std::make_unique<Wt::WPushButton>("Submit Assessment"));
    submitButton_->addStyleClass("btn btn-success");
    submitButton_->clicked().connect([this] {
        onSubmitAssessment();
    });
}

void AssessmentWidget::setupResultsArea() {
    resultsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    resultsSection_->addStyleClass("assessment-results");

    resultTitle_ = resultsSection_->addWidget(std::make_unique<Wt::WText>("Assessment Complete"));
    resultTitle_->addStyleClass("result-title");

    scoreText_ = resultsSection_->addWidget(std::make_unique<Wt::WText>());
    scoreText_->addStyleClass("score-text");

    passFailText_ = resultsSection_->addWidget(std::make_unique<Wt::WText>());
    passFailText_->addStyleClass("pass-fail-text");

    reviewContainer_ = resultsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    reviewContainer_->addStyleClass("review-container");

    finishButton_ = resultsSection_->addWidget(std::make_unique<Wt::WPushButton>("Continue"));
    finishButton_->addStyleClass("btn btn-primary");
    finishButton_->clicked().connect([this] {
        assessmentCompleted_.emit(
            std::stoi(currentAssessment_.getId()),
            currentAttempt_.getScore(),
            currentAttempt_.hasPassed()
        );
    });
}

void AssessmentWidget::setAssessment(const Models::Assessment& assessment) {
    currentAssessment_ = assessment;

    // Update instructions display
    assessmentTitle_->setText(assessment.getTitle());

    if (!assessment.getInstructions().empty()) {
        assessmentInstructions_->setText(assessment.getInstructions());
    } else {
        assessmentInstructions_->setText("Please read each question carefully and select the best answer.");
    }

    // Build info text
    std::ostringstream infoStream;
    infoStream << "<ul class='assessment-info-list'>";
    infoStream << "<li>Questions: " << assessment.getQuestionCount() << "</li>";
    infoStream << "<li>Passing Score: " << assessment.getPassingScore() << "%</li>";
    if (assessment.hasTimeLimit()) {
        infoStream << "<li>Time Limit: " << assessment.getTimeLimitMinutes() << " minutes</li>";
    }
    infoStream << "<li>Attempts Allowed: " << assessment.getMaxAttempts() << "</li>";
    infoStream << "</ul>";
    assessmentInfo_->setText(infoStream.str());
    assessmentInfo_->setTextFormat(Wt::TextFormat::XHTML);

    isTimedAssessment_ = assessment.hasTimeLimit();
    remainingSeconds_ = assessment.getTimeLimitMinutes() * 60;
}

void AssessmentWidget::startAssessment() {
    if (!classroomService_) {
        return;
    }

    // Check remaining attempts
    if (!classroomService_->canAttemptAssessment(studentId_, std::stoi(currentAssessment_.getId()))) {
        // Show message about no attempts remaining
        assessmentInstructions_->setText("You have used all available attempts for this assessment.");
        startButton_->hide();
        return;
    }

    // Create new attempt
    auto result = classroomService_->startAssessmentAttempt(
        studentId_,
        std::stoi(currentAssessment_.getId()),
        enrollmentId_
    );

    if (result.success) {
        currentAttempt_ = classroomService_->getCurrentAttempt(
            studentId_,
            std::stoi(currentAssessment_.getId())
        );
    }

    showInstructions();
}

void AssessmentWidget::resumeAttempt(const std::string& attemptId) {
    if (!classroomService_) {
        return;
    }

    currentAttempt_ = classroomService_->getAttemptDetails(attemptId);

    // Load existing answers
    auto answers = classroomService_->getAttemptAnswers(attemptId);
    for (const auto& answer : answers) {
        if (!answer.getAnswersGiven().empty()) {
            multipleAnswers_[answer.getQuestionId()] = answer.getAnswersGiven();
        } else {
            singleAnswers_[answer.getQuestionId()] = answer.getAnswerGiven();
        }
    }

    loadQuestions();
    showQuestions();
}

void AssessmentWidget::loadQuestions() {
    if (!classroomService_) {
        return;
    }

    questions_ = classroomService_->getAssessmentQuestions(currentAssessment_.getId());

    // Shuffle questions if enabled
    if (currentAssessment_.shufflesQuestions() && !questions_.empty()) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(questions_.begin(), questions_.end(), g);
    }

    currentQuestionIndex_ = 0;
}

void AssessmentWidget::displayQuestion(int questionIndex) {
    if (questionIndex < 0 || questionIndex >= static_cast<int>(questions_.size())) {
        return;
    }

    currentQuestionIndex_ = questionIndex;
    const auto& question = questions_[questionIndex];

    // Update question text
    questionText_->setText("Question " + std::to_string(questionIndex + 1) + ": " + question.getQuestionText());

    // Clear previous answers
    answersContainer_->clear();
    radioGroup_.reset();
    checkboxes_.clear();

    // Get answer options
    auto options = question.getAnswerOptions();

    // Shuffle options if enabled
    if (currentAssessment_.shufflesAnswers() && !options.empty()) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(options.begin(), options.end(), g);
    }

    // Create answer inputs based on question type
    switch (question.getQuestionType()) {
        case Models::QuestionType::MultipleChoice:
        case Models::QuestionType::TrueFalse: {
            radioGroup_ = std::make_shared<Wt::WButtonGroup>();

            for (const auto& option : options) {
                auto radio = answersContainer_->addWidget(
                    std::make_unique<Wt::WRadioButton>(option.text));
                radio->addStyleClass("answer-option");
                radioGroup_->addButton(radio);

                // Restore previous answer
                int qId = std::stoi(question.getId());
                if (singleAnswers_.count(qId) && singleAnswers_[qId] == option.id) {
                    radio->setChecked(true);
                }
            }
            break;
        }

        case Models::QuestionType::MultipleSelect: {
            for (const auto& option : options) {
                auto checkbox = answersContainer_->addWidget(
                    std::make_unique<Wt::WCheckBox>(option.text));
                checkbox->addStyleClass("answer-option");
                checkboxes_.push_back(checkbox);

                // Store option ID as object name for retrieval
                checkbox->setObjectName(option.id);

                // Restore previous answers
                int qId = std::stoi(question.getId());
                if (multipleAnswers_.count(qId)) {
                    auto& prevAnswers = multipleAnswers_[qId];
                    if (std::find(prevAnswers.begin(), prevAnswers.end(), option.id) != prevAnswers.end()) {
                        checkbox->setChecked(true);
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    updateProgressIndicator();
    updateNavigationButtons();
}

void AssessmentWidget::displayAllQuestions() {
    // For displaying all questions at once (alternative UI)
    questionContainer_->clear();

    for (size_t i = 0; i < questions_.size(); ++i) {
        auto qContainer = questionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        qContainer->addStyleClass("question-block");

        auto qText = qContainer->addWidget(std::make_unique<Wt::WText>(
            "Question " + std::to_string(i + 1) + ": " + questions_[i].getQuestionText()));
        qText->addStyleClass("question-text");

        // Add answers...
    }
}

void AssessmentWidget::saveCurrentAnswer() {
    if (currentQuestionIndex_ < 0 || currentQuestionIndex_ >= static_cast<int>(questions_.size())) {
        return;
    }

    const auto& question = questions_[currentQuestionIndex_];
    int questionId = std::stoi(question.getId());

    switch (question.getQuestionType()) {
        case Models::QuestionType::MultipleChoice:
        case Models::QuestionType::TrueFalse: {
            if (radioGroup_) {
                auto checkedButton = radioGroup_->checkedButton();
                if (checkedButton) {
                    // Find which option was selected
                    auto options = question.getAnswerOptions();
                    int idx = radioGroup_->id(checkedButton);
                    if (idx >= 0 && idx < static_cast<int>(options.size())) {
                        singleAnswers_[questionId] = options[idx].id;
                    }
                }
            }
            break;
        }

        case Models::QuestionType::MultipleSelect: {
            std::vector<std::string> selectedAnswers;
            for (auto* checkbox : checkboxes_) {
                if (checkbox->isChecked()) {
                    selectedAnswers.push_back(checkbox->objectName());
                }
            }
            multipleAnswers_[questionId] = selectedAnswers;
            break;
        }

        default:
            break;
    }
}

void AssessmentWidget::onNextQuestion() {
    saveCurrentAnswer();

    if (currentQuestionIndex_ < static_cast<int>(questions_.size()) - 1) {
        displayQuestion(currentQuestionIndex_ + 1);
    }
}

void AssessmentWidget::onPreviousQuestion() {
    saveCurrentAnswer();

    if (currentQuestionIndex_ > 0) {
        displayQuestion(currentQuestionIndex_ - 1);
    }
}

void AssessmentWidget::onSubmitAssessment() {
    saveCurrentAnswer();

    if (timer_) {
        timer_->stop();
    }

    if (!classroomService_) {
        return;
    }

    // Submit all answers
    for (const auto& question : questions_) {
        int questionId = std::stoi(question.getId());

        if (question.getQuestionType() == Models::QuestionType::MultipleSelect) {
            if (multipleAnswers_.count(questionId)) {
                classroomService_->submitAnswer(
                    std::stoi(currentAttempt_.getId()),
                    questionId,
                    "",
                    multipleAnswers_[questionId]
                );
            }
        } else {
            if (singleAnswers_.count(questionId)) {
                classroomService_->submitAnswer(
                    std::stoi(currentAttempt_.getId()),
                    questionId,
                    singleAnswers_[questionId]
                );
            }
        }
    }

    // Submit the assessment
    auto result = classroomService_->submitAssessment(currentAttempt_.getId());

    // Reload attempt to get results
    currentAttempt_ = classroomService_->getAttemptDetails(currentAttempt_.getId());

    showResults();
}

void AssessmentWidget::onTimerTick() {
    remainingSeconds_--;
    updateTimer();

    if (remainingSeconds_ <= 0) {
        // Auto-submit
        onSubmitAssessment();
    }
}

void AssessmentWidget::showInstructions() {
    instructionsSection_->show();
    questionsSection_->hide();
    resultsSection_->hide();
}

void AssessmentWidget::showQuestions() {
    instructionsSection_->hide();
    questionsSection_->show();
    resultsSection_->hide();

    // Start timer if timed
    if (isTimedAssessment_) {
        timer_ = std::make_unique<Wt::WTimer>();
        timer_->setInterval(std::chrono::seconds(1));
        timer_->timeout().connect([this] {
            onTimerTick();
        });
        timer_->start();
        timerDisplay_->show();
    } else {
        timerDisplay_->hide();
    }

    displayQuestion(0);
    updateTimer();
}

void AssessmentWidget::showResults() {
    instructionsSection_->hide();
    questionsSection_->hide();
    resultsSection_->show();

    // Update results display
    std::ostringstream scoreStream;
    scoreStream << std::fixed << std::setprecision(1);
    scoreStream << "Your Score: " << currentAttempt_.getScore() << "%";
    scoreStream << " (" << currentAttempt_.getCorrectAnswers() << "/" << currentAttempt_.getTotalQuestions() << " correct)";
    scoreText_->setText(scoreStream.str());

    if (currentAttempt_.hasPassed()) {
        passFailText_->setText("Congratulations! You passed the assessment.");
        passFailText_->addStyleClass("passed");
        passFailText_->removeStyleClass("failed");
    } else {
        passFailText_->setText("You did not pass. Please review the material and try again.");
        passFailText_->addStyleClass("failed");
        passFailText_->removeStyleClass("passed");
    }

    // Show review if enabled
    reviewContainer_->clear();
    if (currentAssessment_.allowsReview()) {
        auto reviewTitle = reviewContainer_->addWidget(std::make_unique<Wt::WText>("Review Your Answers"));
        reviewTitle->addStyleClass("review-title");

        auto answers = classroomService_->getAttemptAnswers(currentAttempt_.getId());

        for (size_t i = 0; i < questions_.size(); ++i) {
            auto qReview = reviewContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
            qReview->addStyleClass("question-review");

            auto qText = qReview->addWidget(std::make_unique<Wt::WText>(
                "Q" + std::to_string(i + 1) + ": " + questions_[i].getQuestionText()));
            qText->addStyleClass("review-question");

            // Find answer for this question
            for (const auto& answer : answers) {
                if (answer.getQuestionId() == std::stoi(questions_[i].getId())) {
                    std::string answerText = answer.getAnswerGiven();
                    if (answerText.empty() && !answer.getAnswersGiven().empty()) {
                        for (const auto& a : answer.getAnswersGiven()) {
                            if (!answerText.empty()) answerText += ", ";
                            answerText += a;
                        }
                    }

                    auto aText = qReview->addWidget(std::make_unique<Wt::WText>(
                        "Your answer: " + answerText));
                    aText->addStyleClass(answer.isCorrect() ? "answer-correct" : "answer-incorrect");

                    // Show correct answer if enabled
                    if (currentAssessment_.showsCorrectAnswers() && !answer.isCorrect()) {
                        auto correctText = qReview->addWidget(std::make_unique<Wt::WText>(
                            "Correct answer: " + questions_[i].getCorrectAnswer()));
                        correctText->addStyleClass("correct-answer");
                    }
                    break;
                }
            }
        }
    }
}

void AssessmentWidget::updateTimer() {
    if (!isTimedAssessment_ || !timerDisplay_) {
        return;
    }

    int minutes = remainingSeconds_ / 60;
    int seconds = remainingSeconds_ % 60;

    std::ostringstream timerStream;
    timerStream << "Time Remaining: "
                << std::setfill('0') << std::setw(2) << minutes << ":"
                << std::setfill('0') << std::setw(2) << seconds;

    timerDisplay_->setText(timerStream.str());

    // Warning colors
    if (remainingSeconds_ <= 60) {
        timerDisplay_->addStyleClass("timer-warning");
    } else if (remainingSeconds_ <= 300) {
        timerDisplay_->addStyleClass("timer-caution");
    }
}

void AssessmentWidget::updateNavigationButtons() {
    bool isFirst = (currentQuestionIndex_ == 0);
    bool isLast = (currentQuestionIndex_ == static_cast<int>(questions_.size()) - 1);

    prevButton_->setEnabled(!isFirst);
    nextButton_->setEnabled(!isLast);
    submitButton_->setEnabled(isLast);

    // Show/hide buttons appropriately
    if (isLast) {
        nextButton_->hide();
        submitButton_->show();
    } else {
        nextButton_->show();
        submitButton_->hide();
    }
}

void AssessmentWidget::updateProgressIndicator() {
    if (!progressText_) {
        return;
    }

    progressText_->setText("Question " + std::to_string(currentQuestionIndex_ + 1) +
                           " of " + std::to_string(questions_.size()));
}

} // namespace Classroom
} // namespace StudentIntake
