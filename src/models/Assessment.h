#ifndef ASSESSMENT_H
#define ASSESSMENT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Assessment type enumeration
 */
enum class AssessmentType {
    Quiz,
    ModuleExam,
    FinalExam,
    Practice
};

/**
 * @brief Question type enumeration
 */
enum class QuestionType {
    MultipleChoice,
    TrueFalse,
    MultipleSelect,
    ShortAnswer
};

/**
 * @brief Assessment attempt status enumeration
 */
enum class AttemptStatus {
    InProgress,
    Submitted,
    Graded,
    Expired,
    Abandoned
};

/**
 * @brief Represents an answer option for a question
 */
struct AnswerOption {
    std::string id;
    std::string text;
    int order;

    nlohmann::json toJson() const;
    static AnswerOption fromJson(const nlohmann::json& json);
};

/**
 * @brief Represents an assessment (quiz or exam)
 *
 * Assessments are used to test student knowledge after completing
 * reading assignments and videos. The final exam consists of 25 questions.
 */
class Assessment {
public:
    Assessment();
    Assessment(const std::string& id, const std::string& title);

    // Getters
    std::string getId() const { return id_; }
    int getCourseId() const { return courseId_; }
    int getModuleId() const { return moduleId_; }
    std::string getCode() const { return code_; }
    std::string getTitle() const { return title_; }
    std::string getDescription() const { return description_; }
    std::string getInstructions() const { return instructions_; }
    AssessmentType getAssessmentType() const { return assessmentType_; }
    std::string getAssessmentTypeString() const;
    int getQuestionCount() const { return questionCount_; }
    int getPassingScore() const { return passingScore_; }
    int getTimeLimitMinutes() const { return timeLimitMinutes_; }
    int getMaxAttempts() const { return maxAttempts_; }
    bool allowsReview() const { return allowReview_; }
    bool shufflesQuestions() const { return shuffleQuestions_; }
    bool shufflesAnswers() const { return shuffleAnswers_; }
    bool showsCorrectAnswers() const { return showCorrectAnswers_; }
    bool isActive() const { return isActive_; }
    std::string getAvailableFrom() const { return availableFrom_; }
    std::string getAvailableUntil() const { return availableUntil_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setModuleId(int id) { moduleId_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setTitle(const std::string& title) { title_ = title; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setInstructions(const std::string& instructions) { instructions_ = instructions; }
    void setAssessmentType(AssessmentType type) { assessmentType_ = type; }
    void setAssessmentTypeFromString(const std::string& type);
    void setQuestionCount(int count) { questionCount_ = count; }
    void setPassingScore(int score) { passingScore_ = score; }
    void setTimeLimitMinutes(int minutes) { timeLimitMinutes_ = minutes; }
    void setMaxAttempts(int attempts) { maxAttempts_ = attempts; }
    void setAllowReview(bool allow) { allowReview_ = allow; }
    void setShuffleQuestions(bool shuffle) { shuffleQuestions_ = shuffle; }
    void setShuffleAnswers(bool shuffle) { shuffleAnswers_ = shuffle; }
    void setShowCorrectAnswers(bool show) { showCorrectAnswers_ = show; }
    void setActive(bool active) { isActive_ = active; }
    void setAvailableFrom(const std::string& timestamp) { availableFrom_ = timestamp; }
    void setAvailableUntil(const std::string& timestamp) { availableUntil_ = timestamp; }

    // Serialization
    nlohmann::json toJson() const;
    static Assessment fromJson(const nlohmann::json& json);

    // Helper methods
    static AssessmentType stringToAssessmentType(const std::string& type);
    static std::string assessmentTypeToString(AssessmentType type);

    // Utility
    bool hasTimeLimit() const { return timeLimitMinutes_ > 0; }
    bool isFinalExam() const { return assessmentType_ == AssessmentType::FinalExam; }

private:
    std::string id_;
    int courseId_;
    int moduleId_;
    std::string code_;
    std::string title_;
    std::string description_;
    std::string instructions_;
    AssessmentType assessmentType_;
    int questionCount_;
    int passingScore_;
    int timeLimitMinutes_;
    int maxAttempts_;
    bool allowReview_;
    bool shuffleQuestions_;
    bool shuffleAnswers_;
    bool showCorrectAnswers_;
    bool isActive_;
    std::string availableFrom_;
    std::string availableUntil_;
};

/**
 * @brief Represents a question within an assessment
 */
class AssessmentQuestion {
public:
    AssessmentQuestion();
    AssessmentQuestion(const std::string& id, const std::string& questionText);

    // Getters
    std::string getId() const { return id_; }
    int getAssessmentId() const { return assessmentId_; }
    int getQuestionOrder() const { return questionOrder_; }
    std::string getQuestionText() const { return questionText_; }
    QuestionType getQuestionType() const { return questionType_; }
    std::string getQuestionTypeString() const;
    std::vector<AnswerOption> getAnswerOptions() const { return answerOptions_; }
    std::string getCorrectAnswer() const { return correctAnswer_; }
    std::vector<std::string> getCorrectAnswers() const { return correctAnswers_; }
    std::string getExplanation() const { return explanation_; }
    int getPoints() const { return points_; }
    bool isActive() const { return isActive_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setAssessmentId(int id) { assessmentId_ = id; }
    void setQuestionOrder(int order) { questionOrder_ = order; }
    void setQuestionText(const std::string& text) { questionText_ = text; }
    void setQuestionType(QuestionType type) { questionType_ = type; }
    void setQuestionTypeFromString(const std::string& type);
    void setAnswerOptions(const std::vector<AnswerOption>& options) { answerOptions_ = options; }
    void addAnswerOption(const AnswerOption& option);
    void setCorrectAnswer(const std::string& answer) { correctAnswer_ = answer; }
    void setCorrectAnswers(const std::vector<std::string>& answers) { correctAnswers_ = answers; }
    void setExplanation(const std::string& explanation) { explanation_ = explanation; }
    void setPoints(int points) { points_ = points; }
    void setActive(bool active) { isActive_ = active; }

    // Serialization
    nlohmann::json toJson() const;
    static AssessmentQuestion fromJson(const nlohmann::json& json);

    // Helper methods
    static QuestionType stringToQuestionType(const std::string& type);
    static std::string questionTypeToString(QuestionType type);

    // Validation
    bool isAnswerCorrect(const std::string& answer) const;
    bool areAnswersCorrect(const std::vector<std::string>& answers) const;

private:
    std::string id_;
    int assessmentId_;
    int questionOrder_;
    std::string questionText_;
    QuestionType questionType_;
    std::vector<AnswerOption> answerOptions_;
    std::string correctAnswer_;
    std::vector<std::string> correctAnswers_;
    std::string explanation_;
    int points_;
    bool isActive_;
};

/**
 * @brief Represents a student's attempt at an assessment
 */
class StudentAssessmentAttempt {
public:
    StudentAssessmentAttempt();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getAssessmentId() const { return assessmentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getAttemptNumber() const { return attemptNumber_; }
    std::string getStartedAt() const { return startedAt_; }
    std::string getSubmittedAt() const { return submittedAt_; }
    int getTimeSpentSeconds() const { return timeSpentSeconds_; }
    AttemptStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    int getTotalQuestions() const { return totalQuestions_; }
    int getCorrectAnswers() const { return correctAnswers_; }
    double getScore() const { return score_; }
    bool hasPassed() const { return passed_; }
    std::string getIpAddress() const { return ipAddress_; }
    std::string getUserAgent() const { return userAgent_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setAssessmentId(int id) { assessmentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setAttemptNumber(int number) { attemptNumber_ = number; }
    void setStartedAt(const std::string& timestamp) { startedAt_ = timestamp; }
    void setSubmittedAt(const std::string& timestamp) { submittedAt_ = timestamp; }
    void setTimeSpentSeconds(int seconds) { timeSpentSeconds_ = seconds; }
    void setStatus(AttemptStatus status) { status_ = status; }
    void setStatusFromString(const std::string& status);
    void setTotalQuestions(int count) { totalQuestions_ = count; }
    void setCorrectAnswers(int count) { correctAnswers_ = count; }
    void setScore(double score) { score_ = score; }
    void setPassed(bool passed) { passed_ = passed; }
    void setIpAddress(const std::string& ip) { ipAddress_ = ip; }
    void setUserAgent(const std::string& agent) { userAgent_ = agent; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentAssessmentAttempt fromJson(const nlohmann::json& json);

    // Helper methods
    static AttemptStatus stringToAttemptStatus(const std::string& status);
    static std::string attemptStatusToString(AttemptStatus status);

    // Utility
    std::string getFormattedTimeSpent() const;
    bool isInProgress() const { return status_ == AttemptStatus::InProgress; }
    bool isSubmitted() const { return status_ == AttemptStatus::Submitted || status_ == AttemptStatus::Graded; }

private:
    std::string id_;
    int studentId_;
    int assessmentId_;
    int enrollmentId_;
    int attemptNumber_;
    std::string startedAt_;
    std::string submittedAt_;
    int timeSpentSeconds_;
    AttemptStatus status_;
    int totalQuestions_;
    int correctAnswers_;
    double score_;
    bool passed_;
    std::string ipAddress_;
    std::string userAgent_;
};

/**
 * @brief Represents a student's answer to a question
 */
class StudentAssessmentAnswer {
public:
    StudentAssessmentAnswer();

    // Getters
    std::string getId() const { return id_; }
    int getAttemptId() const { return attemptId_; }
    int getQuestionId() const { return questionId_; }
    std::string getAnswerGiven() const { return answerGiven_; }
    std::vector<std::string> getAnswersGiven() const { return answersGiven_; }
    bool isCorrect() const { return isCorrect_; }
    double getPointsEarned() const { return pointsEarned_; }
    std::string getAnsweredAt() const { return answeredAt_; }
    int getTimeSpentSeconds() const { return timeSpentSeconds_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setAttemptId(int id) { attemptId_ = id; }
    void setQuestionId(int id) { questionId_ = id; }
    void setAnswerGiven(const std::string& answer) { answerGiven_ = answer; }
    void setAnswersGiven(const std::vector<std::string>& answers) { answersGiven_ = answers; }
    void setCorrect(bool correct) { isCorrect_ = correct; }
    void setPointsEarned(double points) { pointsEarned_ = points; }
    void setAnsweredAt(const std::string& timestamp) { answeredAt_ = timestamp; }
    void setTimeSpentSeconds(int seconds) { timeSpentSeconds_ = seconds; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentAssessmentAnswer fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int attemptId_;
    int questionId_;
    std::string answerGiven_;
    std::vector<std::string> answersGiven_;
    bool isCorrect_;
    double pointsEarned_;
    std::string answeredAt_;
    int timeSpentSeconds_;
};

/**
 * @brief Module score breakdown for assessment reports
 */
struct ModuleScoreEntry {
    int moduleId;
    std::string moduleTitle;
    int moduleNumber;
    double score;
    int timeSpentSeconds;
    bool completed;

    nlohmann::json toJson() const;
    static ModuleScoreEntry fromJson(const nlohmann::json& json);
};

/**
 * @brief Represents a final assessment report for course completion
 */
class AssessmentReport {
public:
    AssessmentReport();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getCourseId() const { return courseId_; }
    std::string getReportNumber() const { return reportNumber_; }
    std::string getReportType() const { return reportType_; }
    std::string getStudentName() const { return studentName_; }
    std::string getStudentEmail() const { return studentEmail_; }
    std::string getCourseName() const { return courseName_; }
    int getTotalModules() const { return totalModules_; }
    int getModulesCompleted() const { return modulesCompleted_; }
    double getTotalTimeHours() const { return totalTimeHours_; }
    double getOverallScore() const { return overallScore_; }
    double getFinalExamScore() const { return finalExamScore_; }
    double getAverageQuizScore() const { return averageQuizScore_; }
    bool hasPassed() const { return passed_; }
    std::string getCompletionDate() const { return completionDate_; }
    bool isCertificateIssued() const { return certificateIssued_; }
    std::string getCertificateNumber() const { return certificateNumber_; }
    std::string getCertificateIssuedAt() const { return certificateIssuedAt_; }
    std::vector<ModuleScoreEntry> getModuleScores() const { return moduleScores_; }
    std::string getGeneratedAt() const { return generatedAt_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setReportNumber(const std::string& number) { reportNumber_ = number; }
    void setReportType(const std::string& type) { reportType_ = type; }
    void setStudentName(const std::string& name) { studentName_ = name; }
    void setStudentEmail(const std::string& email) { studentEmail_ = email; }
    void setCourseName(const std::string& name) { courseName_ = name; }
    void setTotalModules(int count) { totalModules_ = count; }
    void setModulesCompleted(int count) { modulesCompleted_ = count; }
    void setTotalTimeHours(double hours) { totalTimeHours_ = hours; }
    void setOverallScore(double score) { overallScore_ = score; }
    void setFinalExamScore(double score) { finalExamScore_ = score; }
    void setAverageQuizScore(double score) { averageQuizScore_ = score; }
    void setPassed(bool passed) { passed_ = passed; }
    void setCompletionDate(const std::string& date) { completionDate_ = date; }
    void setCertificateIssued(bool issued) { certificateIssued_ = issued; }
    void setCertificateNumber(const std::string& number) { certificateNumber_ = number; }
    void setCertificateIssuedAt(const std::string& timestamp) { certificateIssuedAt_ = timestamp; }
    void setModuleScores(const std::vector<ModuleScoreEntry>& scores) { moduleScores_ = scores; }
    void addModuleScore(const ModuleScoreEntry& score);
    void setGeneratedAt(const std::string& timestamp) { generatedAt_ = timestamp; }

    // Serialization
    nlohmann::json toJson() const;
    static AssessmentReport fromJson(const nlohmann::json& json);

    // Utility
    double getCompletionPercentage() const;
    std::string getFormattedTotalTime() const;

private:
    std::string id_;
    int studentId_;
    int enrollmentId_;
    int courseId_;
    std::string reportNumber_;
    std::string reportType_;
    std::string studentName_;
    std::string studentEmail_;
    std::string courseName_;
    int totalModules_;
    int modulesCompleted_;
    double totalTimeHours_;
    double overallScore_;
    double finalExamScore_;
    double averageQuizScore_;
    bool passed_;
    std::string completionDate_;
    bool certificateIssued_;
    std::string certificateNumber_;
    std::string certificateIssuedAt_;
    std::vector<ModuleScoreEntry> moduleScores_;
    std::string generatedAt_;
};

} // namespace Models
} // namespace StudentIntake

#endif // ASSESSMENT_H
