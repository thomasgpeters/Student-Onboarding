#include "StudentProgress.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Models {

// =============================================================================
// StudentCourseEnrollment Implementation
// =============================================================================

StudentCourseEnrollment::StudentCourseEnrollment()
    : id_("")
    , studentId_(0)
    , courseId_(0)
    , enrolledAt_("")
    , enrollmentStatus_(EnrollmentStatus::Active)
    , currentModuleId_(0)
    , progressPercentage_(0.0)
    , totalTimeSpent_(0)
    , startedAt_("")
    , completedAt_("")
    , completionStatus_(CompletionStatus::NotStarted)
    , finalScore_(0.0)
    , passed_(false) {
}

std::string StudentCourseEnrollment::getEnrollmentStatusString() const {
    return enrollmentStatusToString(enrollmentStatus_);
}

void StudentCourseEnrollment::setEnrollmentStatusFromString(const std::string& status) {
    enrollmentStatus_ = stringToEnrollmentStatus(status);
}

std::string StudentCourseEnrollment::getCompletionStatusString() const {
    return completionStatusToString(completionStatus_);
}

void StudentCourseEnrollment::setCompletionStatusFromString(const std::string& status) {
    completionStatus_ = stringToCompletionStatus(status);
}

EnrollmentStatus StudentCourseEnrollment::stringToEnrollmentStatus(const std::string& status) {
    std::string lower = status;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "active") return EnrollmentStatus::Active;
    if (lower == "paused") return EnrollmentStatus::Paused;
    if (lower == "withdrawn") return EnrollmentStatus::Withdrawn;
    if (lower == "completed") return EnrollmentStatus::Completed;
    if (lower == "expired") return EnrollmentStatus::Expired;
    return EnrollmentStatus::Active;  // Default
}

std::string StudentCourseEnrollment::enrollmentStatusToString(EnrollmentStatus status) {
    switch (status) {
        case EnrollmentStatus::Active: return "active";
        case EnrollmentStatus::Paused: return "paused";
        case EnrollmentStatus::Withdrawn: return "withdrawn";
        case EnrollmentStatus::Completed: return "completed";
        case EnrollmentStatus::Expired: return "expired";
        default: return "active";
    }
}

CompletionStatus StudentCourseEnrollment::stringToCompletionStatus(const std::string& status) {
    std::string lower = status;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "not_started") return CompletionStatus::NotStarted;
    if (lower == "in_progress") return CompletionStatus::InProgress;
    if (lower == "completed") return CompletionStatus::Completed;
    if (lower == "failed") return CompletionStatus::Failed;
    return CompletionStatus::NotStarted;  // Default
}

std::string StudentCourseEnrollment::completionStatusToString(CompletionStatus status) {
    switch (status) {
        case CompletionStatus::NotStarted: return "not_started";
        case CompletionStatus::InProgress: return "in_progress";
        case CompletionStatus::Completed: return "completed";
        case CompletionStatus::Failed: return "failed";
        default: return "not_started";
    }
}

std::string StudentCourseEnrollment::getFormattedTimeSpent() const {
    int hours = totalTimeSpent_ / 3600;
    int minutes = (totalTimeSpent_ % 3600) / 60;
    int seconds = totalTimeSpent_ % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

nlohmann::json StudentCourseEnrollment::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["course_id"] = courseId_;
    j["enrollment_status"] = enrollmentStatusToString(enrollmentStatus_);
    j["current_module_id"] = currentModuleId_;
    j["progress_percentage"] = progressPercentage_;
    j["total_time_spent"] = totalTimeSpent_;
    j["completion_status"] = completionStatusToString(completionStatus_);
    j["final_score"] = finalScore_;
    j["passed"] = passed_;
    if (!enrolledAt_.empty()) j["enrolled_at"] = enrolledAt_;
    if (!startedAt_.empty()) j["started_at"] = startedAt_;
    if (!completedAt_.empty()) j["completed_at"] = completedAt_;
    return j;
}

StudentCourseEnrollment StudentCourseEnrollment::fromJson(const nlohmann::json& json) {
    StudentCourseEnrollment enrollment;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            enrollment.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            enrollment.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer IDs
    if (attrs.contains("student_id") && !attrs["student_id"].is_null()) {
        if (attrs["student_id"].is_number())
            enrollment.studentId_ = attrs["student_id"].get<int>();
        else if (attrs["student_id"].is_string())
            enrollment.studentId_ = std::stoi(attrs["student_id"].get<std::string>());
    }

    if (attrs.contains("course_id") && !attrs["course_id"].is_null()) {
        if (attrs["course_id"].is_number())
            enrollment.courseId_ = attrs["course_id"].get<int>();
        else if (attrs["course_id"].is_string())
            enrollment.courseId_ = std::stoi(attrs["course_id"].get<std::string>());
    }

    if (attrs.contains("current_module_id") && !attrs["current_module_id"].is_null()) {
        if (attrs["current_module_id"].is_number())
            enrollment.currentModuleId_ = attrs["current_module_id"].get<int>();
        else if (attrs["current_module_id"].is_string())
            enrollment.currentModuleId_ = std::stoi(attrs["current_module_id"].get<std::string>());
    }

    // Status fields
    if (attrs.contains("enrollment_status") && !attrs["enrollment_status"].is_null())
        enrollment.enrollmentStatus_ = stringToEnrollmentStatus(attrs["enrollment_status"].get<std::string>());
    if (attrs.contains("completion_status") && !attrs["completion_status"].is_null())
        enrollment.completionStatus_ = stringToCompletionStatus(attrs["completion_status"].get<std::string>());

    // Numeric fields
    if (attrs.contains("progress_percentage") && !attrs["progress_percentage"].is_null())
        enrollment.progressPercentage_ = attrs["progress_percentage"].get<double>();
    if (attrs.contains("total_time_spent") && !attrs["total_time_spent"].is_null())
        enrollment.totalTimeSpent_ = attrs["total_time_spent"].get<int>();
    if (attrs.contains("final_score") && !attrs["final_score"].is_null())
        enrollment.finalScore_ = attrs["final_score"].get<double>();
    if (attrs.contains("passed") && !attrs["passed"].is_null())
        enrollment.passed_ = attrs["passed"].get<bool>();

    // Timestamp fields
    if (attrs.contains("enrolled_at") && !attrs["enrolled_at"].is_null())
        enrollment.enrolledAt_ = attrs["enrolled_at"].get<std::string>();
    if (attrs.contains("started_at") && !attrs["started_at"].is_null())
        enrollment.startedAt_ = attrs["started_at"].get<std::string>();
    if (attrs.contains("completed_at") && !attrs["completed_at"].is_null())
        enrollment.completedAt_ = attrs["completed_at"].get<std::string>();

    return enrollment;
}

// =============================================================================
// StudentModuleProgress Implementation
// =============================================================================

StudentModuleProgress::StudentModuleProgress()
    : id_("")
    , studentId_(0)
    , moduleId_(0)
    , enrollmentId_(0)
    , status_(ProgressStatus::NotStarted)
    , progressPercentage_(0.0)
    , timeSpent_(0)
    , attempts_(0)
    , bestScore_(0.0)
    , lastScore_(0.0)
    , startedAt_("")
    , completedAt_("")
    , lastAccessedAt_("") {
}

std::string StudentModuleProgress::getStatusString() const {
    return progressStatusToString(status_);
}

void StudentModuleProgress::setStatusFromString(const std::string& status) {
    status_ = stringToProgressStatus(status);
}

void StudentModuleProgress::addCompletedContentId(int id) {
    if (!isContentCompleted(id)) {
        completedContentIds_.push_back(id);
    }
}

bool StudentModuleProgress::isContentCompleted(int contentId) const {
    return std::find(completedContentIds_.begin(), completedContentIds_.end(), contentId)
           != completedContentIds_.end();
}

ProgressStatus StudentModuleProgress::stringToProgressStatus(const std::string& status) {
    std::string lower = status;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "not_started") return ProgressStatus::NotStarted;
    if (lower == "in_progress") return ProgressStatus::InProgress;
    if (lower == "completed") return ProgressStatus::Completed;
    if (lower == "failed") return ProgressStatus::Failed;
    if (lower == "locked") return ProgressStatus::Locked;
    return ProgressStatus::NotStarted;  // Default
}

std::string StudentModuleProgress::progressStatusToString(ProgressStatus status) {
    switch (status) {
        case ProgressStatus::NotStarted: return "not_started";
        case ProgressStatus::InProgress: return "in_progress";
        case ProgressStatus::Completed: return "completed";
        case ProgressStatus::Failed: return "failed";
        case ProgressStatus::Locked: return "locked";
        default: return "not_started";
    }
}

nlohmann::json StudentModuleProgress::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["module_id"] = moduleId_;
    j["enrollment_id"] = enrollmentId_;
    j["status"] = progressStatusToString(status_);
    j["progress_percentage"] = progressPercentage_;
    j["time_spent"] = timeSpent_;
    j["completed_content_ids"] = completedContentIds_;
    j["attempts"] = attempts_;
    j["best_score"] = bestScore_;
    j["last_score"] = lastScore_;
    if (!startedAt_.empty()) j["started_at"] = startedAt_;
    if (!completedAt_.empty()) j["completed_at"] = completedAt_;
    if (!lastAccessedAt_.empty()) j["last_accessed_at"] = lastAccessedAt_;
    return j;
}

StudentModuleProgress StudentModuleProgress::fromJson(const nlohmann::json& json) {
    StudentModuleProgress progress;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            progress.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            progress.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer IDs
    if (attrs.contains("student_id") && !attrs["student_id"].is_null()) {
        if (attrs["student_id"].is_number())
            progress.studentId_ = attrs["student_id"].get<int>();
        else if (attrs["student_id"].is_string())
            progress.studentId_ = std::stoi(attrs["student_id"].get<std::string>());
    }

    if (attrs.contains("module_id") && !attrs["module_id"].is_null()) {
        if (attrs["module_id"].is_number())
            progress.moduleId_ = attrs["module_id"].get<int>();
        else if (attrs["module_id"].is_string())
            progress.moduleId_ = std::stoi(attrs["module_id"].get<std::string>());
    }

    if (attrs.contains("enrollment_id") && !attrs["enrollment_id"].is_null()) {
        if (attrs["enrollment_id"].is_number())
            progress.enrollmentId_ = attrs["enrollment_id"].get<int>();
        else if (attrs["enrollment_id"].is_string())
            progress.enrollmentId_ = std::stoi(attrs["enrollment_id"].get<std::string>());
    }

    // Status
    if (attrs.contains("status") && !attrs["status"].is_null())
        progress.status_ = stringToProgressStatus(attrs["status"].get<std::string>());

    // Numeric fields
    if (attrs.contains("progress_percentage") && !attrs["progress_percentage"].is_null())
        progress.progressPercentage_ = attrs["progress_percentage"].get<double>();
    if (attrs.contains("time_spent") && !attrs["time_spent"].is_null())
        progress.timeSpent_ = attrs["time_spent"].get<int>();
    if (attrs.contains("attempts") && !attrs["attempts"].is_null())
        progress.attempts_ = attrs["attempts"].get<int>();
    if (attrs.contains("best_score") && !attrs["best_score"].is_null())
        progress.bestScore_ = attrs["best_score"].get<double>();
    if (attrs.contains("last_score") && !attrs["last_score"].is_null())
        progress.lastScore_ = attrs["last_score"].get<double>();

    // Completed content IDs
    if (attrs.contains("completed_content_ids") && !attrs["completed_content_ids"].is_null()) {
        progress.completedContentIds_ = attrs["completed_content_ids"].get<std::vector<int>>();
    }

    // Timestamp fields
    if (attrs.contains("started_at") && !attrs["started_at"].is_null())
        progress.startedAt_ = attrs["started_at"].get<std::string>();
    if (attrs.contains("completed_at") && !attrs["completed_at"].is_null())
        progress.completedAt_ = attrs["completed_at"].get<std::string>();
    if (attrs.contains("last_accessed_at") && !attrs["last_accessed_at"].is_null())
        progress.lastAccessedAt_ = attrs["last_accessed_at"].get<std::string>();

    return progress;
}

// =============================================================================
// StudentContentProgress Implementation
// =============================================================================

StudentContentProgress::StudentContentProgress()
    : id_("")
    , studentId_(0)
    , contentId_(0)
    , moduleProgressId_(0)
    , status_("not_started")
    , timeSpent_(0)
    , lastPosition_(0)
    , viewCount_(0)
    , completed_(false)
    , completedAt_("") {
}

nlohmann::json StudentContentProgress::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["content_id"] = contentId_;
    j["module_progress_id"] = moduleProgressId_;
    j["status"] = status_;
    j["time_spent"] = timeSpent_;
    j["last_position"] = lastPosition_;
    j["view_count"] = viewCount_;
    j["completed"] = completed_;
    if (!completedAt_.empty()) j["completed_at"] = completedAt_;
    return j;
}

StudentContentProgress StudentContentProgress::fromJson(const nlohmann::json& json) {
    StudentContentProgress progress;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            progress.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            progress.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Integer IDs
    if (attrs.contains("student_id") && !attrs["student_id"].is_null()) {
        if (attrs["student_id"].is_number())
            progress.studentId_ = attrs["student_id"].get<int>();
        else if (attrs["student_id"].is_string())
            progress.studentId_ = std::stoi(attrs["student_id"].get<std::string>());
    }

    if (attrs.contains("content_id") && !attrs["content_id"].is_null()) {
        if (attrs["content_id"].is_number())
            progress.contentId_ = attrs["content_id"].get<int>();
        else if (attrs["content_id"].is_string())
            progress.contentId_ = std::stoi(attrs["content_id"].get<std::string>());
    }

    if (attrs.contains("module_progress_id") && !attrs["module_progress_id"].is_null()) {
        if (attrs["module_progress_id"].is_number())
            progress.moduleProgressId_ = attrs["module_progress_id"].get<int>();
        else if (attrs["module_progress_id"].is_string())
            progress.moduleProgressId_ = std::stoi(attrs["module_progress_id"].get<std::string>());
    }

    // String/numeric fields
    if (attrs.contains("status") && !attrs["status"].is_null())
        progress.status_ = attrs["status"].get<std::string>();
    if (attrs.contains("time_spent") && !attrs["time_spent"].is_null())
        progress.timeSpent_ = attrs["time_spent"].get<int>();
    if (attrs.contains("last_position") && !attrs["last_position"].is_null())
        progress.lastPosition_ = attrs["last_position"].get<int>();
    if (attrs.contains("view_count") && !attrs["view_count"].is_null())
        progress.viewCount_ = attrs["view_count"].get<int>();
    if (attrs.contains("completed") && !attrs["completed"].is_null())
        progress.completed_ = attrs["completed"].get<bool>();
    if (attrs.contains("completed_at") && !attrs["completed_at"].is_null())
        progress.completedAt_ = attrs["completed_at"].get<std::string>();

    return progress;
}

// =============================================================================
// StudentTimeLog Implementation
// =============================================================================

StudentTimeLog::StudentTimeLog()
    : id_("")
    , studentId_(0)
    , enrollmentId_(0)
    , courseId_(0)
    , moduleId_(0)
    , contentId_(0)
    , sessionStart_("")
    , sessionEnd_("")
    , durationSeconds_(0)
    , activityType_(ActivityType::Reading)
    , isValidated_(false)
    , validationMethod_("")
    , ipAddress_("")
    , userAgent_("") {
}

std::string StudentTimeLog::getActivityTypeString() const {
    return activityTypeToString(activityType_);
}

void StudentTimeLog::setActivityTypeFromString(const std::string& type) {
    activityType_ = stringToActivityType(type);
}

ActivityType StudentTimeLog::stringToActivityType(const std::string& type) {
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "reading") return ActivityType::Reading;
    if (lower == "video") return ActivityType::Video;
    if (lower == "quiz") return ActivityType::Quiz;
    if (lower == "review") return ActivityType::Review;
    if (lower == "navigation") return ActivityType::Navigation;
    if (lower == "idle") return ActivityType::Idle;
    return ActivityType::Reading;  // Default
}

std::string StudentTimeLog::activityTypeToString(ActivityType type) {
    switch (type) {
        case ActivityType::Reading: return "reading";
        case ActivityType::Video: return "video";
        case ActivityType::Quiz: return "quiz";
        case ActivityType::Review: return "review";
        case ActivityType::Navigation: return "navigation";
        case ActivityType::Idle: return "idle";
        default: return "reading";
    }
}

std::string StudentTimeLog::getFormattedDuration() const {
    int hours = durationSeconds_ / 3600;
    int minutes = (durationSeconds_ % 3600) / 60;
    int seconds = durationSeconds_ % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
}

nlohmann::json StudentTimeLog::toJson() const {
    nlohmann::json j;
    j["student_id"] = studentId_;
    j["enrollment_id"] = enrollmentId_;
    if (courseId_ > 0) j["course_id"] = courseId_;
    if (moduleId_ > 0) j["module_id"] = moduleId_;
    if (contentId_ > 0) j["content_id"] = contentId_;
    j["session_start"] = sessionStart_;
    if (!sessionEnd_.empty()) j["session_end"] = sessionEnd_;
    j["duration_seconds"] = durationSeconds_;
    j["activity_type"] = activityTypeToString(activityType_);
    j["is_validated"] = isValidated_;
    if (!validationMethod_.empty()) j["validation_method"] = validationMethod_;
    if (!ipAddress_.empty()) j["ip_address"] = ipAddress_;
    if (!userAgent_.empty()) j["user_agent"] = userAgent_;
    return j;
}

StudentTimeLog StudentTimeLog::fromJson(const nlohmann::json& json) {
    StudentTimeLog log;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            log.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            log.id_ = std::to_string(json["id"].get<int>());
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

    log.studentId_ = parseIntField("student_id");
    log.enrollmentId_ = parseIntField("enrollment_id");
    log.courseId_ = parseIntField("course_id");
    log.moduleId_ = parseIntField("module_id");
    log.contentId_ = parseIntField("content_id");
    log.durationSeconds_ = parseIntField("duration_seconds");

    // String fields
    if (attrs.contains("session_start") && !attrs["session_start"].is_null())
        log.sessionStart_ = attrs["session_start"].get<std::string>();
    if (attrs.contains("session_end") && !attrs["session_end"].is_null())
        log.sessionEnd_ = attrs["session_end"].get<std::string>();
    if (attrs.contains("activity_type") && !attrs["activity_type"].is_null())
        log.activityType_ = stringToActivityType(attrs["activity_type"].get<std::string>());
    if (attrs.contains("is_validated") && !attrs["is_validated"].is_null())
        log.isValidated_ = attrs["is_validated"].get<bool>();
    if (attrs.contains("validation_method") && !attrs["validation_method"].is_null())
        log.validationMethod_ = attrs["validation_method"].get<std::string>();
    if (attrs.contains("ip_address") && !attrs["ip_address"].is_null())
        log.ipAddress_ = attrs["ip_address"].get<std::string>();
    if (attrs.contains("user_agent") && !attrs["user_agent"].is_null())
        log.userAgent_ = attrs["user_agent"].get<std::string>();

    return log;
}

} // namespace Models
} // namespace StudentIntake
