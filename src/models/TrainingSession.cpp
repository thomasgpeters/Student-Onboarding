#include "TrainingSession.h"

namespace StudentIntake {
namespace Models {

// ==================== ScheduledSession ====================

ScheduledSession::ScheduledSession()
    : instructorId_(0)
    , studentId_(0)
    , enrollmentId_(0)
    , sessionType_(SessionType::Classroom)
    , durationMinutes_(60)
    , status_(SessionStatus::Scheduled)
    , rescheduleCount_(0)
{
}

std::string ScheduledSession::getSessionTypeString() const {
    return sessionTypeToString(sessionType_);
}

std::string ScheduledSession::getStatusString() const {
    return sessionStatusToString(status_);
}

void ScheduledSession::setSessionTypeFromString(const std::string& type) {
    sessionType_ = stringToSessionType(type);
}

void ScheduledSession::setStatusFromString(const std::string& status) {
    status_ = stringToSessionStatus(status);
}

SessionType ScheduledSession::stringToSessionType(const std::string& type) {
    if (type == "orientation") return SessionType::Orientation;
    if (type == "classroom") return SessionType::Classroom;
    if (type == "pre_trip_inspection") return SessionType::PreTripInspection;
    if (type == "basic_control") return SessionType::BasicControl;
    if (type == "range_practice") return SessionType::RangePractice;
    if (type == "road_practice") return SessionType::RoadPractice;
    if (type == "skills_test") return SessionType::SkillsTest;
    if (type == "road_test") return SessionType::RoadTest;
    if (type == "final_exam") return SessionType::FinalExam;
    if (type == "remedial") return SessionType::Remedial;
    if (type == "evaluation") return SessionType::Evaluation;
    return SessionType::Other;
}

std::string ScheduledSession::sessionTypeToString(SessionType type) {
    switch (type) {
        case SessionType::Orientation: return "orientation";
        case SessionType::Classroom: return "classroom";
        case SessionType::PreTripInspection: return "pre_trip_inspection";
        case SessionType::BasicControl: return "basic_control";
        case SessionType::RangePractice: return "range_practice";
        case SessionType::RoadPractice: return "road_practice";
        case SessionType::SkillsTest: return "skills_test";
        case SessionType::RoadTest: return "road_test";
        case SessionType::FinalExam: return "final_exam";
        case SessionType::Remedial: return "remedial";
        case SessionType::Evaluation: return "evaluation";
        case SessionType::Other: return "other";
    }
    return "other";
}

SessionStatus ScheduledSession::stringToSessionStatus(const std::string& status) {
    if (status == "scheduled") return SessionStatus::Scheduled;
    if (status == "confirmed") return SessionStatus::Confirmed;
    if (status == "in_progress") return SessionStatus::InProgress;
    if (status == "completed") return SessionStatus::Completed;
    if (status == "cancelled") return SessionStatus::Cancelled;
    if (status == "no_show") return SessionStatus::NoShow;
    if (status == "rescheduled") return SessionStatus::Rescheduled;
    return SessionStatus::Scheduled;
}

std::string ScheduledSession::sessionStatusToString(SessionStatus status) {
    switch (status) {
        case SessionStatus::Scheduled: return "scheduled";
        case SessionStatus::Confirmed: return "confirmed";
        case SessionStatus::InProgress: return "in_progress";
        case SessionStatus::Completed: return "completed";
        case SessionStatus::Cancelled: return "cancelled";
        case SessionStatus::NoShow: return "no_show";
        case SessionStatus::Rescheduled: return "rescheduled";
    }
    return "scheduled";
}

bool ScheduledSession::isUpcoming() const {
    return status_ == SessionStatus::Scheduled || status_ == SessionStatus::Confirmed;
}

bool ScheduledSession::canCancel() const {
    return status_ == SessionStatus::Scheduled || status_ == SessionStatus::Confirmed;
}

bool ScheduledSession::canReschedule() const {
    return status_ == SessionStatus::Scheduled ||
           status_ == SessionStatus::Confirmed ||
           status_ == SessionStatus::Rescheduled;
}

std::string ScheduledSession::getDisplayName() const {
    if (!title_.empty()) {
        return title_;
    }
    // Generate display name from session type
    switch (sessionType_) {
        case SessionType::Orientation: return "Orientation Session";
        case SessionType::Classroom: return "Classroom Session";
        case SessionType::PreTripInspection: return "Pre-Trip Inspection Training";
        case SessionType::BasicControl: return "Basic Control Skills";
        case SessionType::RangePractice: return "Range Practice";
        case SessionType::RoadPractice: return "Road Practice";
        case SessionType::SkillsTest: return "Skills Test";
        case SessionType::RoadTest: return "Road Test";
        case SessionType::FinalExam: return "Final Exam";
        case SessionType::Remedial: return "Remedial Training";
        case SessionType::Evaluation: return "Evaluation";
        case SessionType::Other: return "Training Session";
    }
    return "Training Session";
}

nlohmann::json ScheduledSession::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["instructor_id"] = instructorId_;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    j["session_type"] = sessionTypeToString(sessionType_);
    j["title"] = title_;
    j["description"] = description_;
    j["scheduled_date"] = scheduledDate_;
    j["start_time"] = startTime_;
    j["end_time"] = endTime_;
    j["duration_minutes"] = durationMinutes_;
    j["location_type"] = locationType_;
    j["location_name"] = locationName_;
    j["location_address"] = locationAddress_;
    j["vehicle_info"] = vehicleInfo_;
    j["status"] = sessionStatusToString(status_);
    j["actual_start_time"] = actualStartTime_;
    j["actual_end_time"] = actualEndTime_;
    j["completed_at"] = completedAt_;
    j["cancellation_reason"] = cancellationReason_;
    j["reschedule_count"] = rescheduleCount_;
    j["notes"] = notes_;
    return j;
}

ScheduledSession ScheduledSession::fromJson(const nlohmann::json& json) {
    ScheduledSession session;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            session.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            session.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("instructor_id") && json["instructor_id"].is_number()) {
        session.instructorId_ = json["instructor_id"].get<int>();
    }
    if (json.contains("student_id") && json["student_id"].is_number()) {
        session.studentId_ = json["student_id"].get<int>();
    }
    if (json.contains("enrollment_id") && json["enrollment_id"].is_number()) {
        session.enrollmentId_ = json["enrollment_id"].get<int>();
    }
    if (json.contains("session_type") && json["session_type"].is_string()) {
        session.setSessionTypeFromString(json["session_type"].get<std::string>());
    }
    if (json.contains("title") && json["title"].is_string()) {
        session.title_ = json["title"].get<std::string>();
    }
    if (json.contains("description") && json["description"].is_string()) {
        session.description_ = json["description"].get<std::string>();
    }
    if (json.contains("scheduled_date") && json["scheduled_date"].is_string()) {
        session.scheduledDate_ = json["scheduled_date"].get<std::string>();
    }
    if (json.contains("start_time") && json["start_time"].is_string()) {
        session.startTime_ = json["start_time"].get<std::string>();
    }
    if (json.contains("end_time") && json["end_time"].is_string()) {
        session.endTime_ = json["end_time"].get<std::string>();
    }
    if (json.contains("duration_minutes") && json["duration_minutes"].is_number()) {
        session.durationMinutes_ = json["duration_minutes"].get<int>();
    }
    if (json.contains("location_type") && json["location_type"].is_string()) {
        session.locationType_ = json["location_type"].get<std::string>();
    }
    if (json.contains("location_name") && json["location_name"].is_string()) {
        session.locationName_ = json["location_name"].get<std::string>();
    }
    if (json.contains("location_address") && json["location_address"].is_string()) {
        session.locationAddress_ = json["location_address"].get<std::string>();
    }
    if (json.contains("vehicle_info") && json["vehicle_info"].is_string()) {
        session.vehicleInfo_ = json["vehicle_info"].get<std::string>();
    }
    if (json.contains("status") && json["status"].is_string()) {
        session.setStatusFromString(json["status"].get<std::string>());
    }
    if (json.contains("actual_start_time") && json["actual_start_time"].is_string()) {
        session.actualStartTime_ = json["actual_start_time"].get<std::string>();
    }
    if (json.contains("actual_end_time") && json["actual_end_time"].is_string()) {
        session.actualEndTime_ = json["actual_end_time"].get<std::string>();
    }
    if (json.contains("completed_at") && json["completed_at"].is_string()) {
        session.completedAt_ = json["completed_at"].get<std::string>();
    }
    if (json.contains("cancellation_reason") && json["cancellation_reason"].is_string()) {
        session.cancellationReason_ = json["cancellation_reason"].get<std::string>();
    }
    if (json.contains("reschedule_count") && json["reschedule_count"].is_number()) {
        session.rescheduleCount_ = json["reschedule_count"].get<int>();
    }
    if (json.contains("notes") && json["notes"].is_string()) {
        session.notes_ = json["notes"].get<std::string>();
    }

    return session;
}

// ==================== StudentFeedback ====================

StudentFeedback::StudentFeedback()
    : instructorId_(0)
    , studentId_(0)
    , enrollmentId_(0)
    , sessionId_(0)
    , feedbackType_(FeedbackType::General)
    , performanceRating_(0)
    , ratingScale_(5)
    , isPrivate_(false)
    , visibleToStudent_(true)
    , acknowledgedByStudent_(false)
    , requiresFollowUp_(false)
    , followUpCompleted_(false)
{
}

std::string StudentFeedback::getFeedbackTypeString() const {
    return feedbackTypeToString(feedbackType_);
}

void StudentFeedback::setFeedbackTypeFromString(const std::string& type) {
    feedbackType_ = stringToFeedbackType(type);
}

FeedbackType StudentFeedback::stringToFeedbackType(const std::string& type) {
    if (type == "progress_update") return FeedbackType::ProgressUpdate;
    if (type == "session_review") return FeedbackType::SessionReview;
    if (type == "skill_assessment") return FeedbackType::SkillAssessment;
    if (type == "behavior") return FeedbackType::Behavior;
    if (type == "safety_concern") return FeedbackType::SafetyConcern;
    if (type == "commendation") return FeedbackType::Commendation;
    if (type == "general") return FeedbackType::General;
    return FeedbackType::Other;
}

std::string StudentFeedback::feedbackTypeToString(FeedbackType type) {
    switch (type) {
        case FeedbackType::ProgressUpdate: return "progress_update";
        case FeedbackType::SessionReview: return "session_review";
        case FeedbackType::SkillAssessment: return "skill_assessment";
        case FeedbackType::Behavior: return "behavior";
        case FeedbackType::SafetyConcern: return "safety_concern";
        case FeedbackType::Commendation: return "commendation";
        case FeedbackType::General: return "general";
        case FeedbackType::Other: return "other";
    }
    return "other";
}

double StudentFeedback::getRatingPercentage() const {
    if (ratingScale_ <= 0) return 0.0;
    return (static_cast<double>(performanceRating_) / static_cast<double>(ratingScale_)) * 100.0;
}

nlohmann::json StudentFeedback::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["instructor_id"] = instructorId_;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    j["session_id"] = sessionId_;
    j["feedback_type"] = feedbackTypeToString(feedbackType_);
    j["feedback_category"] = feedbackCategory_;
    j["subject"] = subject_;
    j["feedback_text"] = feedbackText_;
    j["performance_rating"] = performanceRating_;
    j["rating_scale"] = ratingScale_;
    j["strengths"] = strengths_;
    j["areas_for_improvement"] = areasForImprovement_;
    j["recommended_actions"] = recommendedActions_;
    j["is_private"] = isPrivate_;
    j["visible_to_student"] = visibleToStudent_;
    j["acknowledged_by_student"] = acknowledgedByStudent_;
    j["acknowledged_at"] = acknowledgedAt_;
    j["requires_follow_up"] = requiresFollowUp_;
    j["follow_up_date"] = followUpDate_;
    j["follow_up_completed"] = followUpCompleted_;
    j["created_at"] = createdAt_;
    return j;
}

StudentFeedback StudentFeedback::fromJson(const nlohmann::json& json) {
    StudentFeedback feedback;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            feedback.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            feedback.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("instructor_id") && json["instructor_id"].is_number()) {
        feedback.instructorId_ = json["instructor_id"].get<int>();
    }
    if (json.contains("student_id") && json["student_id"].is_number()) {
        feedback.studentId_ = json["student_id"].get<int>();
    }
    if (json.contains("enrollment_id") && json["enrollment_id"].is_number()) {
        feedback.enrollmentId_ = json["enrollment_id"].get<int>();
    }
    if (json.contains("session_id") && json["session_id"].is_number()) {
        feedback.sessionId_ = json["session_id"].get<int>();
    }
    if (json.contains("feedback_type") && json["feedback_type"].is_string()) {
        feedback.setFeedbackTypeFromString(json["feedback_type"].get<std::string>());
    }
    if (json.contains("feedback_category") && json["feedback_category"].is_string()) {
        feedback.feedbackCategory_ = json["feedback_category"].get<std::string>();
    }
    if (json.contains("subject") && json["subject"].is_string()) {
        feedback.subject_ = json["subject"].get<std::string>();
    }
    if (json.contains("feedback_text") && json["feedback_text"].is_string()) {
        feedback.feedbackText_ = json["feedback_text"].get<std::string>();
    }
    if (json.contains("performance_rating") && json["performance_rating"].is_number()) {
        feedback.performanceRating_ = json["performance_rating"].get<int>();
    }
    if (json.contains("rating_scale") && json["rating_scale"].is_number()) {
        feedback.ratingScale_ = json["rating_scale"].get<int>();
    }
    if (json.contains("strengths") && json["strengths"].is_string()) {
        feedback.strengths_ = json["strengths"].get<std::string>();
    }
    if (json.contains("areas_for_improvement") && json["areas_for_improvement"].is_string()) {
        feedback.areasForImprovement_ = json["areas_for_improvement"].get<std::string>();
    }
    if (json.contains("recommended_actions") && json["recommended_actions"].is_string()) {
        feedback.recommendedActions_ = json["recommended_actions"].get<std::string>();
    }
    if (json.contains("is_private") && json["is_private"].is_boolean()) {
        feedback.isPrivate_ = json["is_private"].get<bool>();
    }
    if (json.contains("visible_to_student") && json["visible_to_student"].is_boolean()) {
        feedback.visibleToStudent_ = json["visible_to_student"].get<bool>();
    }
    if (json.contains("acknowledged_by_student") && json["acknowledged_by_student"].is_boolean()) {
        feedback.acknowledgedByStudent_ = json["acknowledged_by_student"].get<bool>();
    }
    if (json.contains("acknowledged_at") && json["acknowledged_at"].is_string()) {
        feedback.acknowledgedAt_ = json["acknowledged_at"].get<std::string>();
    }
    if (json.contains("requires_follow_up") && json["requires_follow_up"].is_boolean()) {
        feedback.requiresFollowUp_ = json["requires_follow_up"].get<bool>();
    }
    if (json.contains("follow_up_date") && json["follow_up_date"].is_string()) {
        feedback.followUpDate_ = json["follow_up_date"].get<std::string>();
    }
    if (json.contains("follow_up_completed") && json["follow_up_completed"].is_boolean()) {
        feedback.followUpCompleted_ = json["follow_up_completed"].get<bool>();
    }
    if (json.contains("created_at") && json["created_at"].is_string()) {
        feedback.createdAt_ = json["created_at"].get<std::string>();
    }

    return feedback;
}

// ==================== SkillValidation ====================

SkillValidation::SkillValidation()
    : instructorId_(0)
    , studentId_(0)
    , enrollmentId_(0)
    , skillItemId_(0)
    , sessionId_(0)
    , result_(ValidationResult::Incomplete)
    , attemptNumber_(1)
    , score_(0.0)
    , maxScore_(100.0)
    , pointsEarned_(0.0)
    , criticalError_(false)
    , isOfficialTest_(false)
{
}

std::string SkillValidation::getResultString() const {
    return validationResultToString(result_);
}

void SkillValidation::setResultFromString(const std::string& result) {
    result_ = stringToValidationResult(result);
}

ValidationResult SkillValidation::stringToValidationResult(const std::string& result) {
    if (result == "pass") return ValidationResult::Pass;
    if (result == "fail") return ValidationResult::Fail;
    if (result == "needs_practice") return ValidationResult::NeedsPractice;
    if (result == "deferred") return ValidationResult::Deferred;
    return ValidationResult::Incomplete;
}

std::string SkillValidation::validationResultToString(ValidationResult result) {
    switch (result) {
        case ValidationResult::Pass: return "pass";
        case ValidationResult::Fail: return "fail";
        case ValidationResult::NeedsPractice: return "needs_practice";
        case ValidationResult::Deferred: return "deferred";
        case ValidationResult::Incomplete: return "incomplete";
    }
    return "incomplete";
}

double SkillValidation::getScorePercentage() const {
    if (maxScore_ <= 0.0) return 0.0;
    return (score_ / maxScore_) * 100.0;
}

nlohmann::json SkillValidation::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["instructor_id"] = instructorId_;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    j["skill_item_id"] = skillItemId_;
    j["session_id"] = sessionId_;
    j["validation_type"] = validationType_;
    j["result"] = validationResultToString(result_);
    j["attempt_number"] = attemptNumber_;
    j["score"] = score_;
    j["max_score"] = maxScore_;
    j["points_earned"] = pointsEarned_;
    j["errors_noted"] = errorsNoted_;
    j["critical_error"] = criticalError_;
    j["validated_at"] = validatedAt_;
    j["is_official_test"] = isOfficialTest_;
    j["witness_name"] = witnessName_;
    j["notes"] = notes_;
    return j;
}

SkillValidation SkillValidation::fromJson(const nlohmann::json& json) {
    SkillValidation validation;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            validation.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            validation.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("instructor_id") && json["instructor_id"].is_number()) {
        validation.instructorId_ = json["instructor_id"].get<int>();
    }
    if (json.contains("student_id") && json["student_id"].is_number()) {
        validation.studentId_ = json["student_id"].get<int>();
    }
    if (json.contains("enrollment_id") && json["enrollment_id"].is_number()) {
        validation.enrollmentId_ = json["enrollment_id"].get<int>();
    }
    if (json.contains("skill_item_id") && json["skill_item_id"].is_number()) {
        validation.skillItemId_ = json["skill_item_id"].get<int>();
    }
    if (json.contains("session_id") && json["session_id"].is_number()) {
        validation.sessionId_ = json["session_id"].get<int>();
    }
    if (json.contains("validation_type") && json["validation_type"].is_string()) {
        validation.validationType_ = json["validation_type"].get<std::string>();
    }
    if (json.contains("result") && json["result"].is_string()) {
        validation.setResultFromString(json["result"].get<std::string>());
    }
    if (json.contains("attempt_number") && json["attempt_number"].is_number()) {
        validation.attemptNumber_ = json["attempt_number"].get<int>();
    }
    if (json.contains("score") && json["score"].is_number()) {
        validation.score_ = json["score"].get<double>();
    }
    if (json.contains("max_score") && json["max_score"].is_number()) {
        validation.maxScore_ = json["max_score"].get<double>();
    }
    if (json.contains("points_earned") && json["points_earned"].is_number()) {
        validation.pointsEarned_ = json["points_earned"].get<double>();
    }
    if (json.contains("errors_noted") && json["errors_noted"].is_string()) {
        validation.errorsNoted_ = json["errors_noted"].get<std::string>();
    }
    if (json.contains("critical_error") && json["critical_error"].is_boolean()) {
        validation.criticalError_ = json["critical_error"].get<bool>();
    }
    if (json.contains("validated_at") && json["validated_at"].is_string()) {
        validation.validatedAt_ = json["validated_at"].get<std::string>();
    }
    if (json.contains("is_official_test") && json["is_official_test"].is_boolean()) {
        validation.isOfficialTest_ = json["is_official_test"].get<bool>();
    }
    if (json.contains("witness_name") && json["witness_name"].is_string()) {
        validation.witnessName_ = json["witness_name"].get<std::string>();
    }
    if (json.contains("notes") && json["notes"].is_string()) {
        validation.notes_ = json["notes"].get<std::string>();
    }

    return validation;
}

// ==================== SkillCategory ====================

SkillCategory::SkillCategory()
    : minimumPracticeHours_(0.0)
    , requiresValidation_(true)
    , displayOrder_(0)
{
}

nlohmann::json SkillCategory::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["code"] = code_;
    j["name"] = name_;
    j["description"] = description_;
    j["category_type"] = categoryType_;
    j["minimum_practice_hours"] = minimumPracticeHours_;
    j["requires_validation"] = requiresValidation_;
    j["display_order"] = displayOrder_;
    return j;
}

SkillCategory SkillCategory::fromJson(const nlohmann::json& json) {
    SkillCategory category;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            category.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            category.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("code") && json["code"].is_string()) {
        category.code_ = json["code"].get<std::string>();
    }
    if (json.contains("name") && json["name"].is_string()) {
        category.name_ = json["name"].get<std::string>();
    }
    if (json.contains("description") && json["description"].is_string()) {
        category.description_ = json["description"].get<std::string>();
    }
    if (json.contains("category_type") && json["category_type"].is_string()) {
        category.categoryType_ = json["category_type"].get<std::string>();
    }
    if (json.contains("minimum_practice_hours") && json["minimum_practice_hours"].is_number()) {
        category.minimumPracticeHours_ = json["minimum_practice_hours"].get<double>();
    }
    if (json.contains("requires_validation") && json["requires_validation"].is_boolean()) {
        category.requiresValidation_ = json["requires_validation"].get<bool>();
    }
    if (json.contains("display_order") && json["display_order"].is_number()) {
        category.displayOrder_ = json["display_order"].get<int>();
    }

    return category;
}

// ==================== SkillItem ====================

SkillItem::SkillItem()
    : categoryId_(0)
    , pointValue_(10)
    , requiresDemonstration_(true)
    , minimumSuccessfulAttempts_(1)
    , displayOrder_(0)
{
}

nlohmann::json SkillItem::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["category_id"] = categoryId_;
    j["code"] = code_;
    j["name"] = name_;
    j["description"] = description_;
    j["passing_criteria"] = passingCriteria_;
    j["critical_errors"] = criticalErrors_;
    j["point_value"] = pointValue_;
    j["requires_demonstration"] = requiresDemonstration_;
    j["minimum_successful_attempts"] = minimumSuccessfulAttempts_;
    j["display_order"] = displayOrder_;
    return j;
}

SkillItem SkillItem::fromJson(const nlohmann::json& json) {
    SkillItem item;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            item.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            item.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("category_id") && json["category_id"].is_number()) {
        item.categoryId_ = json["category_id"].get<int>();
    }
    if (json.contains("code") && json["code"].is_string()) {
        item.code_ = json["code"].get<std::string>();
    }
    if (json.contains("name") && json["name"].is_string()) {
        item.name_ = json["name"].get<std::string>();
    }
    if (json.contains("description") && json["description"].is_string()) {
        item.description_ = json["description"].get<std::string>();
    }
    if (json.contains("passing_criteria") && json["passing_criteria"].is_string()) {
        item.passingCriteria_ = json["passing_criteria"].get<std::string>();
    }
    if (json.contains("critical_errors") && json["critical_errors"].is_string()) {
        item.criticalErrors_ = json["critical_errors"].get<std::string>();
    }
    if (json.contains("point_value") && json["point_value"].is_number()) {
        item.pointValue_ = json["point_value"].get<int>();
    }
    if (json.contains("requires_demonstration") && json["requires_demonstration"].is_boolean()) {
        item.requiresDemonstration_ = json["requires_demonstration"].get<bool>();
    }
    if (json.contains("minimum_successful_attempts") && json["minimum_successful_attempts"].is_number()) {
        item.minimumSuccessfulAttempts_ = json["minimum_successful_attempts"].get<int>();
    }
    if (json.contains("display_order") && json["display_order"].is_number()) {
        item.displayOrder_ = json["display_order"].get<int>();
    }

    return item;
}

// ==================== StudentSkillProgress ====================

StudentSkillProgress::StudentSkillProgress()
    : studentId_(0)
    , enrollmentId_(0)
    , skillItemId_(0)
    , status_("not_started")
    , practiceCount_(0)
    , successfulCount_(0)
    , failedCount_(0)
    , bestScore_(0.0)
    , isValidated_(false)
    , practiceHours_(0.0)
{
}

double StudentSkillProgress::getSuccessRate() const {
    if (practiceCount_ <= 0) return 0.0;
    return (static_cast<double>(successfulCount_) / static_cast<double>(practiceCount_)) * 100.0;
}

nlohmann::json StudentSkillProgress::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    j["skill_item_id"] = skillItemId_;
    j["status"] = status_;
    j["practice_count"] = practiceCount_;
    j["successful_count"] = successfulCount_;
    j["failed_count"] = failedCount_;
    j["best_score"] = bestScore_;
    j["is_validated"] = isValidated_;
    j["validated_at"] = validatedAt_;
    j["practice_hours"] = practiceHours_;
    return j;
}

StudentSkillProgress StudentSkillProgress::fromJson(const nlohmann::json& json) {
    StudentSkillProgress progress;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            progress.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            progress.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (json.contains("student_id") && json["student_id"].is_number()) {
        progress.studentId_ = json["student_id"].get<int>();
    }
    if (json.contains("enrollment_id") && json["enrollment_id"].is_number()) {
        progress.enrollmentId_ = json["enrollment_id"].get<int>();
    }
    if (json.contains("skill_item_id") && json["skill_item_id"].is_number()) {
        progress.skillItemId_ = json["skill_item_id"].get<int>();
    }
    if (json.contains("status") && json["status"].is_string()) {
        progress.status_ = json["status"].get<std::string>();
    }
    if (json.contains("practice_count") && json["practice_count"].is_number()) {
        progress.practiceCount_ = json["practice_count"].get<int>();
    }
    if (json.contains("successful_count") && json["successful_count"].is_number()) {
        progress.successfulCount_ = json["successful_count"].get<int>();
    }
    if (json.contains("failed_count") && json["failed_count"].is_number()) {
        progress.failedCount_ = json["failed_count"].get<int>();
    }
    if (json.contains("best_score") && json["best_score"].is_number()) {
        progress.bestScore_ = json["best_score"].get<double>();
    }
    if (json.contains("is_validated") && json["is_validated"].is_boolean()) {
        progress.isValidated_ = json["is_validated"].get<bool>();
    }
    if (json.contains("validated_at") && json["validated_at"].is_string()) {
        progress.validatedAt_ = json["validated_at"].get<std::string>();
    }
    if (json.contains("practice_hours") && json["practice_hours"].is_number()) {
        progress.practiceHours_ = json["practice_hours"].get<double>();
    }

    return progress;
}

} // namespace Models
} // namespace StudentIntake
