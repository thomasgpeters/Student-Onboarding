#include "ActivityLog.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace StudentIntake {
namespace Models {

ActivityLog::ActivityLog()
    : id_(0)
    , actorType_(ActorType::System)
    , actorId_(0)
    , actionCategory_(ActivityCategory::System)
    , severity_(ActivitySeverity::Info)
    , details_(nlohmann::json::object()) {
}

// =============================================================================
// Display helpers
// =============================================================================

std::string ActivityLog::getActorTypeString() const {
    return actorTypeToString(actorType_);
}

std::string ActivityLog::getCategoryString() const {
    return categoryToString(actionCategory_);
}

std::string ActivityLog::getSeverityString() const {
    return severityToString(severity_);
}

std::string ActivityLog::getRelativeTime() const {
    if (createdAt_.empty()) {
        return "Unknown";
    }

    // Parse ISO 8601 timestamp
    std::tm tm = {};
    std::istringstream ss(createdAt_);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        // Try alternative format
        ss.clear();
        ss.str(createdAt_);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            return createdAt_;
        }
    }

    std::time_t activityTime = std::mktime(&tm);
    std::time_t now = std::time(nullptr);
    double diffSeconds = std::difftime(now, activityTime);

    if (diffSeconds < 60) {
        return "Just now";
    } else if (diffSeconds < 3600) {
        int minutes = static_cast<int>(diffSeconds / 60);
        return std::to_string(minutes) + (minutes == 1 ? " minute ago" : " minutes ago");
    } else if (diffSeconds < 86400) {
        int hours = static_cast<int>(diffSeconds / 3600);
        return std::to_string(hours) + (hours == 1 ? " hour ago" : " hours ago");
    } else if (diffSeconds < 604800) {
        int days = static_cast<int>(diffSeconds / 86400);
        return std::to_string(days) + (days == 1 ? " day ago" : " days ago");
    } else {
        return getFormattedTime();
    }
}

std::string ActivityLog::getFormattedTime() const {
    if (createdAt_.empty()) {
        return "Unknown";
    }

    // Parse ISO 8601 timestamp
    std::tm tm = {};
    std::istringstream ss(createdAt_);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail()) {
        ss.clear();
        ss.str(createdAt_);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            return createdAt_;
        }
    }

    std::ostringstream out;
    out << std::put_time(&tm, "%b %d, %Y %I:%M %p");
    return out.str();
}

std::string ActivityLog::getIconClass() const {
    switch (actionCategory_) {
        case ActivityCategory::Authentication:
            return "activity-icon auth";
        case ActivityCategory::Forms:
            return "activity-icon forms";
        case ActivityCategory::Profile:
            return "activity-icon profile";
        case ActivityCategory::Admin:
            return "activity-icon admin";
        case ActivityCategory::System:
        default:
            return "activity-icon system";
    }
}

std::string ActivityLog::getIcon() const {
    // Return appropriate icon based on action type
    if (actionType_ == "login_success" || actionType_ == "login_failed") {
        return "&#128274;"; // Lock icon
    } else if (actionType_ == "logout") {
        return "&#128275;"; // Unlock icon
    } else if (actionType_ == "form_submitted") {
        return "&#128196;"; // Document icon
    } else if (actionType_ == "form_approved") {
        return "&#9989;"; // Check mark
    } else if (actionType_ == "form_rejected") {
        return "&#10060;"; // X mark
    } else if (actionType_ == "student_registered") {
        return "&#128100;"; // Person icon
    } else if (actionType_ == "user_created") {
        return "&#128101;"; // People icon
    } else if (actionType_ == "access_revoked") {
        return "&#128683;"; // No entry icon
    } else if (actionType_ == "access_restored") {
        return "&#9989;"; // Check mark
    } else if (actionType_ == "curriculum_selected" || actionType_ == "curriculum_created") {
        return "&#128218;"; // Book icon
    } else if (actionType_ == "settings_updated") {
        return "&#9881;"; // Gear icon
    }

    // Default icons by category
    switch (actionCategory_) {
        case ActivityCategory::Authentication:
            return "&#128274;";
        case ActivityCategory::Forms:
            return "&#128196;";
        case ActivityCategory::Profile:
            return "&#128100;";
        case ActivityCategory::Admin:
            return "&#128736;";
        case ActivityCategory::System:
        default:
            return "&#9881;";
    }
}

// =============================================================================
// Serialization
// =============================================================================

nlohmann::json ActivityLog::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["actor_type"] = actorTypeToString(actorType_);
    j["actor_id"] = actorId_;
    j["actor_name"] = actorName_;
    j["actor_email"] = actorEmail_;
    j["action_type"] = actionType_;
    j["action_category"] = categoryToString(actionCategory_);
    j["description"] = description_;
    j["entity_type"] = entityType_;
    j["entity_id"] = entityId_;
    j["entity_name"] = entityName_;
    j["details"] = details_;
    j["severity"] = severityToString(severity_);
    j["ip_address"] = ipAddress_;
    j["user_agent"] = userAgent_;
    j["session_id"] = sessionId_;
    j["created_at"] = createdAt_;
    return j;
}

ActivityLog ActivityLog::fromJson(const nlohmann::json& json) {
    ActivityLog activity;

    // Handle both direct JSON and JSON:API format
    const nlohmann::json* data = &json;
    if (json.contains("data")) {
        if (json["data"].is_object() && json["data"].contains("attributes")) {
            // JSON:API single resource format
            data = &json["data"]["attributes"];
            if (json["data"].contains("id")) {
                auto idVal = json["data"]["id"];
                if (idVal.is_string()) {
                    activity.setId(std::stoi(idVal.get<std::string>()));
                } else if (idVal.is_number()) {
                    activity.setId(idVal.get<int>());
                }
            }
        } else {
            data = &json["data"];
        }
    }

    const nlohmann::json& j = *data;

    if (j.contains("id") && !j["id"].is_null()) {
        if (j["id"].is_string()) {
            activity.setId(std::stoi(j["id"].get<std::string>()));
        } else {
            activity.setId(j["id"].get<int>());
        }
    }

    if (j.contains("actor_type") && !j["actor_type"].is_null()) {
        activity.setActorType(actorTypeFromString(j["actor_type"].get<std::string>()));
    }

    if (j.contains("actor_id") && !j["actor_id"].is_null()) {
        if (j["actor_id"].is_string()) {
            activity.setActorId(std::stoi(j["actor_id"].get<std::string>()));
        } else {
            activity.setActorId(j["actor_id"].get<int>());
        }
    }

    if (j.contains("actor_name") && !j["actor_name"].is_null()) {
        activity.setActorName(j["actor_name"].get<std::string>());
    }

    if (j.contains("actor_email") && !j["actor_email"].is_null()) {
        activity.setActorEmail(j["actor_email"].get<std::string>());
    }

    if (j.contains("action_type") && !j["action_type"].is_null()) {
        activity.setActionType(j["action_type"].get<std::string>());
    }

    if (j.contains("action_category") && !j["action_category"].is_null()) {
        activity.setActionCategory(categoryFromString(j["action_category"].get<std::string>()));
    }

    if (j.contains("description") && !j["description"].is_null()) {
        activity.setDescription(j["description"].get<std::string>());
    }

    if (j.contains("entity_type") && !j["entity_type"].is_null()) {
        activity.setEntityType(j["entity_type"].get<std::string>());
    }

    if (j.contains("entity_id") && !j["entity_id"].is_null()) {
        if (j["entity_id"].is_string()) {
            activity.setEntityId(j["entity_id"].get<std::string>());
        } else {
            activity.setEntityId(std::to_string(j["entity_id"].get<int>()));
        }
    }

    if (j.contains("entity_name") && !j["entity_name"].is_null()) {
        activity.setEntityName(j["entity_name"].get<std::string>());
    }

    if (j.contains("details") && !j["details"].is_null()) {
        activity.setDetails(j["details"]);
    }

    if (j.contains("severity") && !j["severity"].is_null()) {
        activity.setSeverity(severityFromString(j["severity"].get<std::string>()));
    }

    if (j.contains("ip_address") && !j["ip_address"].is_null()) {
        activity.setIpAddress(j["ip_address"].get<std::string>());
    }

    if (j.contains("user_agent") && !j["user_agent"].is_null()) {
        activity.setUserAgent(j["user_agent"].get<std::string>());
    }

    if (j.contains("session_id") && !j["session_id"].is_null()) {
        activity.setSessionId(j["session_id"].get<std::string>());
    }

    if (j.contains("created_at") && !j["created_at"].is_null()) {
        activity.setCreatedAt(j["created_at"].get<std::string>());
    }

    return activity;
}

// =============================================================================
// Static conversion helpers
// =============================================================================

ActorType ActivityLog::actorTypeFromString(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "student") return ActorType::Student;
    if (lower == "instructor") return ActorType::Instructor;
    if (lower == "admin") return ActorType::Admin;
    return ActorType::System;
}

std::string ActivityLog::actorTypeToString(ActorType type) {
    switch (type) {
        case ActorType::Student: return "student";
        case ActorType::Instructor: return "instructor";
        case ActorType::Admin: return "admin";
        case ActorType::System:
        default: return "system";
    }
}

ActivityCategory ActivityLog::categoryFromString(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "authentication") return ActivityCategory::Authentication;
    if (lower == "forms") return ActivityCategory::Forms;
    if (lower == "profile") return ActivityCategory::Profile;
    if (lower == "admin") return ActivityCategory::Admin;
    return ActivityCategory::System;
}

std::string ActivityLog::categoryToString(ActivityCategory cat) {
    switch (cat) {
        case ActivityCategory::Authentication: return "authentication";
        case ActivityCategory::Forms: return "forms";
        case ActivityCategory::Profile: return "profile";
        case ActivityCategory::Admin: return "admin";
        case ActivityCategory::System:
        default: return "system";
    }
}

ActivitySeverity ActivityLog::severityFromString(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "success") return ActivitySeverity::Success;
    if (lower == "warning") return ActivitySeverity::Warning;
    if (lower == "error") return ActivitySeverity::Error;
    return ActivitySeverity::Info;
}

std::string ActivityLog::severityToString(ActivitySeverity sev) {
    switch (sev) {
        case ActivitySeverity::Success: return "success";
        case ActivitySeverity::Warning: return "warning";
        case ActivitySeverity::Error: return "error";
        case ActivitySeverity::Info:
        default: return "info";
    }
}

// =============================================================================
// Builder methods for common activity types
// =============================================================================

ActivityLog ActivityLog::createLoginActivity(
    ActorType actorType, int actorId, const std::string& actorName,
    const std::string& actorEmail, bool success,
    const std::string& ipAddress, const std::string& failureReason) {

    ActivityLog activity;
    activity.setActorType(actorType);
    activity.setActorId(actorId);
    activity.setActorName(actorName);
    activity.setActorEmail(actorEmail);
    activity.setActionType(success ? "login_success" : "login_failed");
    activity.setActionCategory(ActivityCategory::Authentication);
    activity.setEntityType("app_user");
    activity.setIpAddress(ipAddress);

    if (success) {
        activity.setDescription(actorName + " logged in");
        activity.setSeverity(ActivitySeverity::Success);
    } else {
        activity.setDescription("Failed login attempt for " + actorEmail);
        activity.setSeverity(ActivitySeverity::Warning);
        if (!failureReason.empty()) {
            nlohmann::json details;
            details["reason"] = failureReason;
            activity.setDetails(details);
        }
    }

    return activity;
}

ActivityLog ActivityLog::createLogoutActivity(
    ActorType actorType, int actorId, const std::string& actorName,
    const std::string& actorEmail) {

    ActivityLog activity;
    activity.setActorType(actorType);
    activity.setActorId(actorId);
    activity.setActorName(actorName);
    activity.setActorEmail(actorEmail);
    activity.setActionType("logout");
    activity.setActionCategory(ActivityCategory::Authentication);
    activity.setDescription(actorName + " logged out");
    activity.setSeverity(ActivitySeverity::Info);

    return activity;
}

ActivityLog ActivityLog::createFormSubmittedActivity(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& formId, const std::string& formName) {

    ActivityLog activity;
    activity.setActorType(ActorType::Student);
    activity.setActorId(studentId);
    activity.setActorName(studentName);
    activity.setActorEmail(studentEmail);
    activity.setActionType("form_submitted");
    activity.setActionCategory(ActivityCategory::Forms);
    activity.setDescription(studentName + " submitted " + formName + " form");
    activity.setEntityType("form_submission");
    activity.setEntityId(formId);
    activity.setEntityName(formName);
    activity.setSeverity(ActivitySeverity::Success);

    return activity;
}

ActivityLog ActivityLog::createFormReviewActivity(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int studentId, const std::string& studentName,
    const std::string& formId, const std::string& formName, bool approved,
    const std::string& reason) {

    ActivityLog activity;
    activity.setActorType(ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType(approved ? "form_approved" : "form_rejected");
    activity.setActionCategory(ActivityCategory::Admin);
    activity.setEntityType("form_submission");
    activity.setEntityId(formId);
    activity.setEntityName(formName);
    activity.setSeverity(approved ? ActivitySeverity::Success : ActivitySeverity::Warning);

    std::string action = approved ? "approved" : "rejected";
    activity.setDescription(adminName + " " + action + " " + formName + " for " + studentName);

    nlohmann::json details;
    details["student_name"] = studentName;
    details["student_id"] = studentId;
    if (!reason.empty()) {
        details["reason"] = reason;
    }
    activity.setDetails(details);

    return activity;
}

ActivityLog ActivityLog::createStudentRegisteredActivity(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& ipAddress) {

    ActivityLog activity;
    activity.setActorType(ActorType::Student);
    activity.setActorId(studentId);
    activity.setActorName(studentName);
    activity.setActorEmail(studentEmail);
    activity.setActionType("student_registered");
    activity.setActionCategory(ActivityCategory::Profile);
    activity.setDescription("New student registered: " + studentName);
    activity.setEntityType("student");
    activity.setEntityId(std::to_string(studentId));
    activity.setEntityName(studentName);
    activity.setSeverity(ActivitySeverity::Success);
    activity.setIpAddress(ipAddress);

    return activity;
}

ActivityLog ActivityLog::createAccessChangeActivity(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int studentId, const std::string& studentName, bool revoked) {

    ActivityLog activity;
    activity.setActorType(ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType(revoked ? "access_revoked" : "access_restored");
    activity.setActionCategory(ActivityCategory::Admin);
    activity.setEntityType("student");
    activity.setEntityId(std::to_string(studentId));
    activity.setEntityName(studentName);
    activity.setSeverity(revoked ? ActivitySeverity::Warning : ActivitySeverity::Success);

    std::string action = revoked ? "revoked access for" : "restored access for";
    activity.setDescription(adminName + " " + action + " " + studentName);

    return activity;
}

ActivityLog ActivityLog::createUserCreatedActivity(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int newUserId, const std::string& newUserName, const std::string& newUserEmail,
    const std::string& role) {

    ActivityLog activity;
    activity.setActorType(ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType("user_created");
    activity.setActionCategory(ActivityCategory::Admin);
    activity.setDescription(adminName + " created user: " + newUserName);
    activity.setEntityType("app_user");
    activity.setEntityId(std::to_string(newUserId));
    activity.setEntityName(newUserName);
    activity.setSeverity(ActivitySeverity::Success);

    nlohmann::json details;
    details["new_user_email"] = newUserEmail;
    details["role"] = role;
    activity.setDetails(details);

    return activity;
}

ActivityLog ActivityLog::createCurriculumSelectedActivity(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& curriculumId, const std::string& curriculumName) {

    ActivityLog activity;
    activity.setActorType(ActorType::Student);
    activity.setActorId(studentId);
    activity.setActorName(studentName);
    activity.setActorEmail(studentEmail);
    activity.setActionType("curriculum_selected");
    activity.setActionCategory(ActivityCategory::Profile);
    activity.setDescription(studentName + " selected " + curriculumName);
    activity.setEntityType("curriculum");
    activity.setEntityId(curriculumId);
    activity.setEntityName(curriculumName);
    activity.setSeverity(ActivitySeverity::Info);

    return activity;
}

} // namespace Models
} // namespace StudentIntake
