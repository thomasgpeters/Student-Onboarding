#include "Assessment.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Models {

// =============================================================================
// AnswerOption Implementation
// =============================================================================

nlohmann::json AnswerOption::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["text"] = text;
    j["order"] = order;
    return j;
}

AnswerOption AnswerOption::fromJson(const nlohmann::json& json) {
    AnswerOption option;
    if (json.contains("id") && !json["id"].is_null())
        option.id = json["id"].get<std::string>();
    if (json.contains("text") && !json["text"].is_null())
        option.text = json["text"].get<std::string>();
    if (json.contains("order") && !json["order"].is_null())
        option.order = json["order"].get<int>();
    return option;
}

// =============================================================================
// Assessment Implementation
// =============================================================================

Assessment::Assessment()
    : id_("")
    , courseId_(0)
    , moduleId_(0)
    , code_("")
    , title_("")
    , description_("")
    , instructions_("")
    , assessmentType_(AssessmentType::Quiz)
    , questionCount_(0)
    , passingScore_(70)
    , timeLimitMinutes_(0)
    , maxAttempts_(3)
    , allowReview_(true)
    , shuffleQuestions_(true)
    , shuffleAnswers_(true)
    , showCorrectAnswers_(false)
    , isActive_(true)
    , availableFrom_("")
    , availableUntil_("") {
}

Assessment::Assessment(const std::string& id, const std::string& title)
    : id_(id)
    , courseId_(0)
    , moduleId_(0)
    , code_("")
    , title_(title)
    , description_("")
    , instructions_("")
    , assessmentType_(AssessmentType::Quiz)
    , questionCount_(0)
    , passingScore_(70)
    , timeLimitMinutes_(0)
    , maxAttempts_(3)
    , allowReview_(true)
    , shuffleQuestions_(true)
    , shuffleAnswers_(true)
    , showCorrectAnswers_(false)
    , isActive_(true)
    , availableFrom_("")
    , availableUntil_("") {
}

std::string Assessment::getAssessmentTypeString() const {
    return assessmentTypeToString(assessmentType_);
}

void Assessment::setAssessmentTypeFromString(const std::string& type) {
    assessmentType_ = stringToAssessmentType(type);
}

AssessmentType Assessment::stringToAssessmentType(const std::string& type) {
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "quiz") return AssessmentType::Quiz;
    if (lower == "module_exam") return AssessmentType::ModuleExam;
    if (lower == "final_exam") return AssessmentType::FinalExam;
    if (lower == "practice") return AssessmentType::Practice;
    return AssessmentType::Quiz;  // Default
}

std::string Assessment::assessmentTypeToString(AssessmentType type) {
    switch (type) {
        case AssessmentType::Quiz: return "quiz";
        case AssessmentType::ModuleExam: return "module_exam";
        case AssessmentType::FinalExam: return "final_exam";
        case AssessmentType::Practice: return "practice";
        default: return "quiz";
    }
}

nlohmann::json Assessment::toJson() const {
    nlohmann::json j;
    if (courseId_ > 0) j["course_id"] = courseId_;
    if (moduleId_ > 0) j["module_id"] = moduleId_;
    j["code"] = code_;
    j["title"] = title_;
    j["description"] = description_;
    j["instructions"] = instructions_;
    j["assessment_type"] = assessmentTypeToString(assessmentType_);
    j["question_count"] = questionCount_;
    j["passing_score"] = passingScore_;
    j["time_limit_minutes"] = timeLimitMinutes_;
    j["max_attempts"] = maxAttempts_;
    j["allow_review"] = allowReview_;
    j["shuffle_questions"] = shuffleQuestions_;
    j["shuffle_answers"] = shuffleAnswers_;
    j["show_correct_answers"] = showCorrectAnswers_;
    j["is_active"] = isActive_;
    if (!availableFrom_.empty()) j["available_from"] = availableFrom_;
    if (!availableUntil_.empty()) j["available_until"] = availableUntil_;
    return j;
}

Assessment Assessment::fromJson(const nlohmann::json& json) {
    Assessment assessment;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            assessment.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            assessment.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer IDs
    auto parseIntField = [&attrs](const std::string& field) -> int {
        if (attrs.contains(field) && !attrs[field].is_null()) {
            if (attrs[field].is_number())
                return attrs[field].get<int>();
            else if (attrs[field].is_string())
                return std::stoi(attrs[field].get<std::string>());
        }
        return 0;
    };

    assessment.courseId_ = parseIntField("course_id");
    assessment.moduleId_ = parseIntField("module_id");
    assessment.questionCount_ = parseIntField("question_count");
    assessment.passingScore_ = parseIntField("passing_score");
    assessment.timeLimitMinutes_ = parseIntField("time_limit_minutes");
    assessment.maxAttempts_ = parseIntField("max_attempts");

    // String fields
    if (attrs.contains("code") && !attrs["code"].is_null())
        assessment.code_ = attrs["code"].get<std::string>();
    if (attrs.contains("title") && !attrs["title"].is_null())
        assessment.title_ = attrs["title"].get<std::string>();
    if (attrs.contains("description") && !attrs["description"].is_null())
        assessment.description_ = attrs["description"].get<std::string>();
    if (attrs.contains("instructions") && !attrs["instructions"].is_null())
        assessment.instructions_ = attrs["instructions"].get<std::string>();
    if (attrs.contains("assessment_type") && !attrs["assessment_type"].is_null())
        assessment.assessmentType_ = stringToAssessmentType(attrs["assessment_type"].get<std::string>());

    // Boolean fields
    if (attrs.contains("allow_review") && !attrs["allow_review"].is_null())
        assessment.allowReview_ = attrs["allow_review"].get<bool>();
    if (attrs.contains("shuffle_questions") && !attrs["shuffle_questions"].is_null())
        assessment.shuffleQuestions_ = attrs["shuffle_questions"].get<bool>();
    if (attrs.contains("shuffle_answers") && !attrs["shuffle_answers"].is_null())
        assessment.shuffleAnswers_ = attrs["shuffle_answers"].get<bool>();
    if (attrs.contains("show_correct_answers") && !attrs["show_correct_answers"].is_null())
        assessment.showCorrectAnswers_ = attrs["show_correct_answers"].get<bool>();
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        assessment.isActive_ = attrs["is_active"].get<bool>();

    // Timestamp fields
    if (attrs.contains("available_from") && !attrs["available_from"].is_null())
        assessment.availableFrom_ = attrs["available_from"].get<std::string>();
    if (attrs.contains("available_until") && !attrs["available_until"].is_null())
        assessment.availableUntil_ = attrs["available_until"].get<std::string>();

    return assessment;
}

// =============================================================================
// AssessmentQuestion Implementation
// =============================================================================

AssessmentQuestion::AssessmentQuestion()
    : id_("")
    , assessmentId_(0)
    , questionOrder_(0)
    , questionText_("")
    , questionType_(QuestionType::MultipleChoice)
    , correctAnswer_("")
    , explanation_("")
    , points_(1)
    , isActive_(true) {
}

AssessmentQuestion::AssessmentQuestion(const std::string& id, const std::string& questionText)
    : id_(id)
    , assessmentId_(0)
    , questionOrder_(0)
    , questionText_(questionText)
    , questionType_(QuestionType::MultipleChoice)
    , correctAnswer_("")
    , explanation_("")
    , points_(1)
    , isActive_(true) {
}

std::string AssessmentQuestion::getQuestionTypeString() const {
    return questionTypeToString(questionType_);
}

void AssessmentQuestion::setQuestionTypeFromString(const std::string& type) {
    questionType_ = stringToQuestionType(type);
}

void AssessmentQuestion::addAnswerOption(const AnswerOption& option) {
    answerOptions_.push_back(option);
}

QuestionType AssessmentQuestion::stringToQuestionType(const std::string& type) {
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "multiple_choice") return QuestionType::MultipleChoice;
    if (lower == "true_false") return QuestionType::TrueFalse;
    if (lower == "multiple_select") return QuestionType::MultipleSelect;
    if (lower == "short_answer") return QuestionType::ShortAnswer;
    return QuestionType::MultipleChoice;  // Default
}

std::string AssessmentQuestion::questionTypeToString(QuestionType type) {
    switch (type) {
        case QuestionType::MultipleChoice: return "multiple_choice";
        case QuestionType::TrueFalse: return "true_false";
        case QuestionType::MultipleSelect: return "multiple_select";
        case QuestionType::ShortAnswer: return "short_answer";
        default: return "multiple_choice";
    }
}

bool AssessmentQuestion::isAnswerCorrect(const std::string& answer) const {
    if (questionType_ == QuestionType::TrueFalse || questionType_ == QuestionType::MultipleChoice) {
        std::string lowerAnswer = answer;
        std::string lowerCorrect = correctAnswer_;
        std::transform(lowerAnswer.begin(), lowerAnswer.end(), lowerAnswer.begin(), ::tolower);
        std::transform(lowerCorrect.begin(), lowerCorrect.end(), lowerCorrect.begin(), ::tolower);
        return lowerAnswer == lowerCorrect;
    }
    return answer == correctAnswer_;
}

bool AssessmentQuestion::areAnswersCorrect(const std::vector<std::string>& answers) const {
    if (answers.size() != correctAnswers_.size()) {
        return false;
    }

    std::vector<std::string> sortedAnswers = answers;
    std::vector<std::string> sortedCorrect = correctAnswers_;
    std::sort(sortedAnswers.begin(), sortedAnswers.end());
    std::sort(sortedCorrect.begin(), sortedCorrect.end());

    return sortedAnswers == sortedCorrect;
}

nlohmann::json AssessmentQuestion::toJson() const {
    nlohmann::json j;
    j["assessment_id"] = assessmentId_;
    j["question_order"] = questionOrder_;
    j["question_text"] = questionText_;
    j["question_type"] = questionTypeToString(questionType_);

    nlohmann::json options = nlohmann::json::array();
    for (const auto& opt : answerOptions_) {
        options.push_back(opt.toJson());
    }
    j["answer_options"] = options;

    j["correct_answer"] = correctAnswer_;
    j["correct_answers"] = correctAnswers_;
    j["explanation"] = explanation_;
    j["points"] = points_;
    j["is_active"] = isActive_;
    return j;
}

AssessmentQuestion AssessmentQuestion::fromJson(const nlohmann::json& json) {
    AssessmentQuestion question;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            question.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            question.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer fields
    if (attrs.contains("assessment_id") && !attrs["assessment_id"].is_null()) {
        if (attrs["assessment_id"].is_number())
            question.assessmentId_ = attrs["assessment_id"].get<int>();
        else if (attrs["assessment_id"].is_string())
            question.assessmentId_ = std::stoi(attrs["assessment_id"].get<std::string>());
    }

    if (attrs.contains("question_order") && !attrs["question_order"].is_null())
        question.questionOrder_ = attrs["question_order"].get<int>();
    if (attrs.contains("points") && !attrs["points"].is_null())
        question.points_ = attrs["points"].get<int>();

    // String fields
    if (attrs.contains("question_text") && !attrs["question_text"].is_null())
        question.questionText_ = attrs["question_text"].get<std::string>();
    if (attrs.contains("question_type") && !attrs["question_type"].is_null())
        question.questionType_ = stringToQuestionType(attrs["question_type"].get<std::string>());
    if (attrs.contains("correct_answer") && !attrs["correct_answer"].is_null())
        question.correctAnswer_ = attrs["correct_answer"].get<std::string>();
    if (attrs.contains("explanation") && !attrs["explanation"].is_null())
        question.explanation_ = attrs["explanation"].get<std::string>();

    // Array fields
    if (attrs.contains("answer_options") && !attrs["answer_options"].is_null()) {
        for (const auto& opt : attrs["answer_options"]) {
            question.answerOptions_.push_back(AnswerOption::fromJson(opt));
        }
    }
    if (attrs.contains("correct_answers") && !attrs["correct_answers"].is_null()) {
        question.correctAnswers_ = attrs["correct_answers"].get<std::vector<std::string>>();
    }

    // Boolean fields
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        question.isActive_ = attrs["is_active"].get<bool>();

    return question;
}

// =============================================================================
// StudentAssessmentAttempt Implementation
// =============================================================================

StudentAssessmentAttempt::StudentAssessmentAttempt()
    : id_("")
    , studentId_(0)
    , assessmentId_(0)
    , enrollmentId_(0)
    , attemptNumber_(1)
    , startedAt_("")
    , submittedAt_("")
    , timeSpentSeconds_(0)
    , status_(AttemptStatus::InProgress)
    , totalQuestions_(0)
    , correctAnswers_(0)
    , score_(0.0)
    , passed_(false)
    , ipAddress_("")
    , userAgent_("") {
}

std::string StudentAssessmentAttempt::getStatusString() const {
    return attemptStatusToString(status_);
}

void StudentAssessmentAttempt::setStatusFromString(const std::string& status) {
    status_ = stringToAttemptStatus(status);
}

AttemptStatus StudentAssessmentAttempt::stringToAttemptStatus(const std::string& status) {
    std::string lower = status;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "in_progress") return AttemptStatus::InProgress;
    if (lower == "submitted") return AttemptStatus::Submitted;
    if (lower == "graded") return AttemptStatus::Graded;
    if (lower == "expired") return AttemptStatus::Expired;
    if (lower == "abandoned") return AttemptStatus::Abandoned;
    return AttemptStatus::InProgress;  // Default
}

std::string StudentAssessmentAttempt::attemptStatusToString(AttemptStatus status) {
    switch (status) {
        case AttemptStatus::InProgress: return "in_progress";
        case AttemptStatus::Submitted: return "submitted";
        case AttemptStatus::Graded: return "graded";
        case AttemptStatus::Expired: return "expired";
        case AttemptStatus::Abandoned: return "abandoned";
        default: return "in_progress";
    }
}

std::string StudentAssessmentAttempt::getFormattedTimeSpent() const {
    int hours = timeSpentSeconds_ / 3600;
    int minutes = (timeSpentSeconds_ % 3600) / 60;
    int seconds = timeSpentSeconds_ % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

nlohmann::json StudentAssessmentAttempt::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["assessment_id"] = assessmentId_;
    j["enrollment_id"] = enrollmentId_;
    j["attempt_number"] = attemptNumber_;
    j["started_at"] = startedAt_;
    if (!submittedAt_.empty()) j["submitted_at"] = submittedAt_;
    j["time_spent_seconds"] = timeSpentSeconds_;
    j["status"] = attemptStatusToString(status_);
    j["total_questions"] = totalQuestions_;
    j["correct_answers"] = correctAnswers_;
    j["score"] = score_;
    j["passed"] = passed_;
    if (!ipAddress_.empty()) j["ip_address"] = ipAddress_;
    if (!userAgent_.empty()) j["user_agent"] = userAgent_;
    return j;
}

StudentAssessmentAttempt StudentAssessmentAttempt::fromJson(const nlohmann::json& json) {
    StudentAssessmentAttempt attempt;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            attempt.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            attempt.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer fields
    auto parseIntField = [&attrs](const std::string& field) -> int {
        if (attrs.contains(field) && !attrs[field].is_null()) {
            if (attrs[field].is_number())
                return attrs[field].get<int>();
            else if (attrs[field].is_string())
                return std::stoi(attrs[field].get<std::string>());
        }
        return 0;
    };

    attempt.studentId_ = parseIntField("student_id");
    attempt.assessmentId_ = parseIntField("assessment_id");
    attempt.enrollmentId_ = parseIntField("enrollment_id");
    attempt.attemptNumber_ = parseIntField("attempt_number");
    attempt.timeSpentSeconds_ = parseIntField("time_spent_seconds");
    attempt.totalQuestions_ = parseIntField("total_questions");
    attempt.correctAnswers_ = parseIntField("correct_answers");

    // Default attempt number to 1
    if (attempt.attemptNumber_ == 0) attempt.attemptNumber_ = 1;

    // Double fields
    if (attrs.contains("score") && !attrs["score"].is_null())
        attempt.score_ = attrs["score"].get<double>();

    // Boolean fields
    if (attrs.contains("passed") && !attrs["passed"].is_null())
        attempt.passed_ = attrs["passed"].get<bool>();

    // Status
    if (attrs.contains("status") && !attrs["status"].is_null())
        attempt.status_ = stringToAttemptStatus(attrs["status"].get<std::string>());

    // String fields
    if (attrs.contains("started_at") && !attrs["started_at"].is_null())
        attempt.startedAt_ = attrs["started_at"].get<std::string>();
    if (attrs.contains("submitted_at") && !attrs["submitted_at"].is_null())
        attempt.submittedAt_ = attrs["submitted_at"].get<std::string>();
    if (attrs.contains("ip_address") && !attrs["ip_address"].is_null())
        attempt.ipAddress_ = attrs["ip_address"].get<std::string>();
    if (attrs.contains("user_agent") && !attrs["user_agent"].is_null())
        attempt.userAgent_ = attrs["user_agent"].get<std::string>();

    return attempt;
}

// =============================================================================
// StudentAssessmentAnswer Implementation
// =============================================================================

StudentAssessmentAnswer::StudentAssessmentAnswer()
    : id_("")
    , attemptId_(0)
    , questionId_(0)
    , answerGiven_("")
    , isCorrect_(false)
    , pointsEarned_(0.0)
    , answeredAt_("")
    , timeSpentSeconds_(0) {
}

nlohmann::json StudentAssessmentAnswer::toJson() const {
    nlohmann::json j;
    j["attempt_id"] = attemptId_;
    j["question_id"] = questionId_;
    j["answer_given"] = answerGiven_;
    j["answers_given"] = answersGiven_;
    j["is_correct"] = isCorrect_;
    j["points_earned"] = pointsEarned_;
    if (!answeredAt_.empty()) j["answered_at"] = answeredAt_;
    j["time_spent_seconds"] = timeSpentSeconds_;
    return j;
}

StudentAssessmentAnswer StudentAssessmentAnswer::fromJson(const nlohmann::json& json) {
    StudentAssessmentAnswer answer;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            answer.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            answer.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer fields
    if (attrs.contains("attempt_id") && !attrs["attempt_id"].is_null()) {
        if (attrs["attempt_id"].is_number())
            answer.attemptId_ = attrs["attempt_id"].get<int>();
        else if (attrs["attempt_id"].is_string())
            answer.attemptId_ = std::stoi(attrs["attempt_id"].get<std::string>());
    }

    if (attrs.contains("question_id") && !attrs["question_id"].is_null()) {
        if (attrs["question_id"].is_number())
            answer.questionId_ = attrs["question_id"].get<int>();
        else if (attrs["question_id"].is_string())
            answer.questionId_ = std::stoi(attrs["question_id"].get<std::string>());
    }

    if (attrs.contains("time_spent_seconds") && !attrs["time_spent_seconds"].is_null())
        answer.timeSpentSeconds_ = attrs["time_spent_seconds"].get<int>();

    // String/array fields
    if (attrs.contains("answer_given") && !attrs["answer_given"].is_null())
        answer.answerGiven_ = attrs["answer_given"].get<std::string>();
    if (attrs.contains("answers_given") && !attrs["answers_given"].is_null())
        answer.answersGiven_ = attrs["answers_given"].get<std::vector<std::string>>();

    // Boolean/double fields
    if (attrs.contains("is_correct") && !attrs["is_correct"].is_null())
        answer.isCorrect_ = attrs["is_correct"].get<bool>();
    if (attrs.contains("points_earned") && !attrs["points_earned"].is_null())
        answer.pointsEarned_ = attrs["points_earned"].get<double>();

    // Timestamp
    if (attrs.contains("answered_at") && !attrs["answered_at"].is_null())
        answer.answeredAt_ = attrs["answered_at"].get<std::string>();

    return answer;
}

// =============================================================================
// ModuleScoreEntry Implementation
// =============================================================================

nlohmann::json ModuleScoreEntry::toJson() const {
    nlohmann::json j;
    j["module_id"] = moduleId;
    j["module_title"] = moduleTitle;
    j["module_number"] = moduleNumber;
    j["score"] = score;
    j["time_spent_seconds"] = timeSpentSeconds;
    j["completed"] = completed;
    return j;
}

ModuleScoreEntry ModuleScoreEntry::fromJson(const nlohmann::json& json) {
    ModuleScoreEntry entry;
    if (json.contains("module_id") && !json["module_id"].is_null())
        entry.moduleId = json["module_id"].get<int>();
    if (json.contains("module_title") && !json["module_title"].is_null())
        entry.moduleTitle = json["module_title"].get<std::string>();
    if (json.contains("module_number") && !json["module_number"].is_null())
        entry.moduleNumber = json["module_number"].get<int>();
    if (json.contains("score") && !json["score"].is_null())
        entry.score = json["score"].get<double>();
    if (json.contains("time_spent_seconds") && !json["time_spent_seconds"].is_null())
        entry.timeSpentSeconds = json["time_spent_seconds"].get<int>();
    if (json.contains("completed") && !json["completed"].is_null())
        entry.completed = json["completed"].get<bool>();
    return entry;
}

// =============================================================================
// AssessmentReport Implementation
// =============================================================================

AssessmentReport::AssessmentReport()
    : id_("")
    , studentId_(0)
    , enrollmentId_(0)
    , courseId_(0)
    , reportNumber_("")
    , reportType_("completion")
    , studentName_("")
    , studentEmail_("")
    , courseName_("")
    , totalModules_(0)
    , modulesCompleted_(0)
    , totalTimeHours_(0.0)
    , overallScore_(0.0)
    , finalExamScore_(0.0)
    , averageQuizScore_(0.0)
    , passed_(false)
    , completionDate_("")
    , certificateIssued_(false)
    , certificateNumber_("")
    , certificateIssuedAt_("")
    , generatedAt_("") {
}

void AssessmentReport::addModuleScore(const ModuleScoreEntry& score) {
    moduleScores_.push_back(score);
}

double AssessmentReport::getCompletionPercentage() const {
    if (totalModules_ == 0) return 0.0;
    return (static_cast<double>(modulesCompleted_) / totalModules_) * 100.0;
}

std::string AssessmentReport::getFormattedTotalTime() const {
    int totalMinutes = static_cast<int>(totalTimeHours_ * 60);
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;

    std::ostringstream oss;
    oss << hours << "h " << minutes << "m";
    return oss.str();
}

nlohmann::json AssessmentReport::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    j["course_id"] = courseId_;
    j["report_number"] = reportNumber_;
    j["report_type"] = reportType_;
    j["student_name"] = studentName_;
    j["student_email"] = studentEmail_;
    j["course_name"] = courseName_;
    j["total_modules"] = totalModules_;
    j["modules_completed"] = modulesCompleted_;
    j["total_time_hours"] = totalTimeHours_;
    j["overall_score"] = overallScore_;
    j["final_exam_score"] = finalExamScore_;
    j["average_quiz_score"] = averageQuizScore_;
    j["passed"] = passed_;
    j["completion_date"] = completionDate_;
    j["certificate_issued"] = certificateIssued_;
    if (!certificateNumber_.empty()) j["certificate_number"] = certificateNumber_;
    if (!certificateIssuedAt_.empty()) j["certificate_issued_at"] = certificateIssuedAt_;

    nlohmann::json scores = nlohmann::json::array();
    for (const auto& score : moduleScores_) {
        scores.push_back(score.toJson());
    }
    j["module_scores"] = scores;

    if (!generatedAt_.empty()) j["generated_at"] = generatedAt_;
    return j;
}

AssessmentReport AssessmentReport::fromJson(const nlohmann::json& json) {
    AssessmentReport report;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            report.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            report.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer fields
    auto parseIntField = [&attrs](const std::string& field) -> int {
        if (attrs.contains(field) && !attrs[field].is_null()) {
            if (attrs[field].is_number())
                return attrs[field].get<int>();
            else if (attrs[field].is_string())
                return std::stoi(attrs[field].get<std::string>());
        }
        return 0;
    };

    report.studentId_ = parseIntField("student_id");
    report.enrollmentId_ = parseIntField("enrollment_id");
    report.courseId_ = parseIntField("course_id");
    report.totalModules_ = parseIntField("total_modules");
    report.modulesCompleted_ = parseIntField("modules_completed");

    // Double fields
    if (attrs.contains("total_time_hours") && !attrs["total_time_hours"].is_null())
        report.totalTimeHours_ = attrs["total_time_hours"].get<double>();
    if (attrs.contains("overall_score") && !attrs["overall_score"].is_null())
        report.overallScore_ = attrs["overall_score"].get<double>();
    if (attrs.contains("final_exam_score") && !attrs["final_exam_score"].is_null())
        report.finalExamScore_ = attrs["final_exam_score"].get<double>();
    if (attrs.contains("average_quiz_score") && !attrs["average_quiz_score"].is_null())
        report.averageQuizScore_ = attrs["average_quiz_score"].get<double>();

    // Boolean fields
    if (attrs.contains("passed") && !attrs["passed"].is_null())
        report.passed_ = attrs["passed"].get<bool>();
    if (attrs.contains("certificate_issued") && !attrs["certificate_issued"].is_null())
        report.certificateIssued_ = attrs["certificate_issued"].get<bool>();

    // String fields
    if (attrs.contains("report_number") && !attrs["report_number"].is_null())
        report.reportNumber_ = attrs["report_number"].get<std::string>();
    if (attrs.contains("report_type") && !attrs["report_type"].is_null())
        report.reportType_ = attrs["report_type"].get<std::string>();
    if (attrs.contains("student_name") && !attrs["student_name"].is_null())
        report.studentName_ = attrs["student_name"].get<std::string>();
    if (attrs.contains("student_email") && !attrs["student_email"].is_null())
        report.studentEmail_ = attrs["student_email"].get<std::string>();
    if (attrs.contains("course_name") && !attrs["course_name"].is_null())
        report.courseName_ = attrs["course_name"].get<std::string>();
    if (attrs.contains("completion_date") && !attrs["completion_date"].is_null())
        report.completionDate_ = attrs["completion_date"].get<std::string>();
    if (attrs.contains("certificate_number") && !attrs["certificate_number"].is_null())
        report.certificateNumber_ = attrs["certificate_number"].get<std::string>();
    if (attrs.contains("certificate_issued_at") && !attrs["certificate_issued_at"].is_null())
        report.certificateIssuedAt_ = attrs["certificate_issued_at"].get<std::string>();
    if (attrs.contains("generated_at") && !attrs["generated_at"].is_null())
        report.generatedAt_ = attrs["generated_at"].get<std::string>();

    // Module scores array
    if (attrs.contains("module_scores") && !attrs["module_scores"].is_null()) {
        for (const auto& score : attrs["module_scores"]) {
            report.moduleScores_.push_back(ModuleScoreEntry::fromJson(score));
        }
    }

    return report;
}

} // namespace Models
} // namespace StudentIntake
