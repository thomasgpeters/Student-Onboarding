#include "ActivityLogService.h"
#include "utils/Logger.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace StudentIntake {
namespace Api {

// =============================================================================
// ActivityFilter implementation
// =============================================================================

std::string ActivityFilter::toQueryString() const {
    std::ostringstream qs;
    bool first = true;

    auto addParam = [&](const std::string& key, const std::string& value) {
        if (!value.empty()) {
            qs << (first ? "?" : "&") << key << "=" << value;
            first = false;
        }
    };

    auto addIntParam = [&](const std::string& key, int value) {
        if (value > 0) {
            qs << (first ? "?" : "&") << key << "=" << value;
            first = false;
        }
    };

    // JSON:API filter format
    if (!actorType.empty()) {
        addParam("filter[actor_type]", actorType);
    }
    if (!actionCategory.empty()) {
        addParam("filter[action_category]", actionCategory);
    }
    if (!actionType.empty()) {
        addParam("filter[action_type]", actionType);
    }
    if (!entityType.empty()) {
        addParam("filter[entity_type]", entityType);
    }
    if (!entityId.empty()) {
        addParam("filter[entity_id]", entityId);
    }
    if (actorId > 0) {
        addIntParam("filter[actor_id]", actorId);
    }

    // Pagination
    addIntParam("page[limit]", limit);
    if (offset > 0) {
        addIntParam("page[offset]", offset);
    }

    // Sorting (JSON:API format: sort=-created_at for descending)
    if (!sortBy.empty()) {
        std::string sortParam = sortDesc ? "-" + sortBy : sortBy;
        addParam("sort", sortParam);
    }

    return qs.str();
}

// =============================================================================
// ActivityLogService implementation
// =============================================================================

ActivityLogService::ActivityLogService()
    : apiClient_(nullptr) {
}

ActivityLogService::ActivityLogService(std::shared_ptr<ApiClient> apiClient)
    : apiClient_(apiClient) {
}

ActivityLogService::~ActivityLogService() {
}

// =============================================================================
// Query Activities
// =============================================================================

std::vector<Models::ActivityLog> ActivityLogService::getRecentActivities(int limit) {
    ActivityFilter filter;
    filter.limit = limit;
    filter.sortBy = "created_at";
    filter.sortDesc = true;
    return getActivities(filter);
}

std::vector<Models::ActivityLog> ActivityLogService::getActivities(const ActivityFilter& filter) {
    std::vector<Models::ActivityLog> activities;

    if (!apiClient_) {
        LOG_ERROR("ActivityLogService", "API client not set");
        return activities;
    }

    std::string endpoint = "/ActivityLog" + filter.toQueryString();
    LOG_DEBUG("ActivityLogService", "GET " << endpoint);

    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        try {
            nlohmann::json json = response.getJson();
            activities = parseActivitiesFromJson(json);
            LOG_DEBUG("ActivityLogService", "Retrieved " << activities.size() << " activities");
        } catch (const std::exception& e) {
            LOG_ERROR("ActivityLogService", "Error parsing activities: " << e.what());
        }
    } else {
        LOG_ERROR("ActivityLogService", "Failed to get activities: " << response.errorMessage);
    }

    return activities;
}

Models::ActivityLog ActivityLogService::getActivity(int activityId) {
    Models::ActivityLog activity;

    if (!apiClient_) {
        LOG_ERROR("ActivityLogService", "API client not set");
        return activity;
    }

    std::string endpoint = "/ActivityLog/" + std::to_string(activityId);
    LOG_DEBUG("ActivityLogService", "GET " << endpoint);

    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        try {
            nlohmann::json json = response.getJson();
            activity = parseActivityFromJson(json);
        } catch (const std::exception& e) {
            LOG_ERROR("ActivityLogService", "Error parsing activity: " << e.what());
        }
    } else {
        LOG_ERROR("ActivityLogService", "Failed to get activity: " << response.errorMessage);
    }

    return activity;
}

std::vector<Models::ActivityLog> ActivityLogService::getActivitiesForUser(int userId, int limit) {
    ActivityFilter filter;
    filter.actorId = userId;
    filter.limit = limit;
    return getActivities(filter);
}

std::vector<Models::ActivityLog> ActivityLogService::getActivitiesForEntity(
    const std::string& entityType, const std::string& entityId, int limit) {
    ActivityFilter filter;
    filter.entityType = entityType;
    filter.entityId = entityId;
    filter.limit = limit;
    return getActivities(filter);
}

int ActivityLogService::getActivityCount(const ActivityFilter& filter) {
    if (!apiClient_) {
        return 0;
    }

    // Use a minimal query to get count
    ActivityFilter countFilter = filter;
    countFilter.limit = 1;
    countFilter.offset = 0;

    std::string endpoint = "/ActivityLog" + countFilter.toQueryString();
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        try {
            nlohmann::json json = response.getJson();
            if (json.contains("meta") && json["meta"].contains("total")) {
                return json["meta"]["total"].get<int>();
            }
        } catch (const std::exception& e) {
            LOG_ERROR("ActivityLogService", "Error getting count: " << e.what());
        }
    }

    return 0;
}

// =============================================================================
// Async Query Methods
// =============================================================================

void ActivityLogService::getRecentActivitiesAsync(int limit, ActivitiesCallback callback) {
    if (!apiClient_ || !callback) {
        if (callback) {
            callback({}, false);
        }
        return;
    }

    ActivityFilter filter;
    filter.limit = limit;
    std::string endpoint = "/ActivityLog" + filter.toQueryString();

    apiClient_->getAsync(endpoint, [this, callback](const ApiResponse& response) {
        if (response.isSuccess()) {
            try {
                nlohmann::json json = response.getJson();
                auto activities = parseActivitiesFromJson(json);
                callback(activities, true);
            } catch (const std::exception& e) {
                LOG_ERROR("ActivityLogService", "Error parsing async activities: " << e.what());
                callback({}, false);
            }
        } else {
            callback({}, false);
        }
    });
}

void ActivityLogService::getActivitiesAsync(const ActivityFilter& filter, ActivitiesCallback callback) {
    if (!apiClient_ || !callback) {
        if (callback) {
            callback({}, false);
        }
        return;
    }

    std::string endpoint = "/ActivityLog" + filter.toQueryString();

    apiClient_->getAsync(endpoint, [this, callback](const ApiResponse& response) {
        if (response.isSuccess()) {
            try {
                nlohmann::json json = response.getJson();
                auto activities = parseActivitiesFromJson(json);
                callback(activities, true);
            } catch (const std::exception& e) {
                LOG_ERROR("ActivityLogService", "Error parsing async activities: " << e.what());
                callback({}, false);
            }
        } else {
            callback({}, false);
        }
    });
}

// =============================================================================
// Log Activities
// =============================================================================

ActivityLogResult ActivityLogService::logActivity(const Models::ActivityLog& activity) {
    ActivityLogResult result;
    result.success = false;
    result.activityId = 0;

    if (!apiClient_) {
        result.errorMessage = "API client not set";
        LOG_ERROR("ActivityLogService", result.errorMessage);
        return result;
    }

    nlohmann::json payload = preparePayload(activity);
    LOG_DEBUG("ActivityLogService", "POST /ActivityLog");

    ApiResponse response = apiClient_->post("/ActivityLog", payload);

    if (response.isSuccess()) {
        result.success = true;
        result.message = "Activity logged successfully";

        try {
            nlohmann::json json = response.getJson();
            if (json.contains("data") && json["data"].contains("id")) {
                auto idVal = json["data"]["id"];
                if (idVal.is_string()) {
                    result.activityId = std::stoi(idVal.get<std::string>());
                } else if (idVal.is_number()) {
                    result.activityId = idVal.get<int>();
                }
            }
        } catch (const std::exception& e) {
            LOG_WARNING("ActivityLogService", "Could not parse activity ID from response");
        }

        LOG_DEBUG("ActivityLogService", "Activity logged with ID: " << result.activityId);
    } else {
        result.errorMessage = response.errorMessage;
        LOG_ERROR("ActivityLogService", "Failed to log activity: " << result.errorMessage);
    }

    return result;
}

void ActivityLogService::logActivityAsync(const Models::ActivityLog& activity,
                                           ActivityCallback callback) {
    if (!apiClient_) {
        if (callback) {
            ActivityLogResult result;
            result.success = false;
            result.errorMessage = "API client not set";
            callback(result);
        }
        return;
    }

    nlohmann::json payload = preparePayload(activity);

    apiClient_->postAsync("/ActivityLog", payload, [callback](const ApiResponse& response) {
        ActivityLogResult result;
        result.success = response.isSuccess();

        if (result.success) {
            result.message = "Activity logged successfully";
            try {
                nlohmann::json json = response.getJson();
                if (json.contains("data") && json["data"].contains("id")) {
                    auto idVal = json["data"]["id"];
                    if (idVal.is_string()) {
                        result.activityId = std::stoi(idVal.get<std::string>());
                    } else if (idVal.is_number()) {
                        result.activityId = idVal.get<int>();
                    }
                }
            } catch (...) {
                // Ignore parsing errors for async logging
            }
        } else {
            result.errorMessage = response.errorMessage;
        }

        if (callback) {
            callback(result);
        }
    });
}

// =============================================================================
// Convenience Methods
// =============================================================================

ActivityLogResult ActivityLogService::logLogin(
    int userId, const std::string& userName, const std::string& email,
    const std::string& actorType, const std::string& ipAddress,
    bool success, const std::string& failureReason) {

    Models::ActorType type = Models::ActivityLog::actorTypeFromString(actorType);
    Models::ActivityLog activity = Models::ActivityLog::createLoginActivity(
        type, userId, userName, email, success, ipAddress, failureReason);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logLogout(
    int userId, const std::string& userName, const std::string& email,
    const std::string& actorType) {

    Models::ActorType type = Models::ActivityLog::actorTypeFromString(actorType);
    Models::ActivityLog activity = Models::ActivityLog::createLogoutActivity(
        type, userId, userName, email);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logFormSubmission(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& formId, const std::string& formName) {

    Models::ActivityLog activity = Models::ActivityLog::createFormSubmittedActivity(
        studentId, studentName, studentEmail, formId, formName);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logFormReview(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int studentId, const std::string& studentName,
    int formSubmissionId, const std::string& formName,
    bool approved, const std::string& reason) {

    Models::ActivityLog activity = Models::ActivityLog::createFormReviewActivity(
        adminId, adminName, adminEmail, studentId, studentName,
        std::to_string(formSubmissionId), formName, approved, reason);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logStudentRegistration(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& ipAddress) {

    Models::ActivityLog activity = Models::ActivityLog::createStudentRegisteredActivity(
        studentId, studentName, studentEmail, ipAddress);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logProfileUpdate(
    int userId, const std::string& userName, const std::string& userEmail,
    const std::string& actorType, const std::string& fieldChanged) {

    Models::ActivityLog activity;
    activity.setActorType(Models::ActivityLog::actorTypeFromString(actorType));
    activity.setActorId(userId);
    activity.setActorName(userName);
    activity.setActorEmail(userEmail);
    activity.setActionType("profile_updated");
    activity.setActionCategory(Models::ActivityCategory::Profile);
    activity.setSeverity(Models::ActivitySeverity::Info);

    std::string desc = userName + " updated their profile";
    if (!fieldChanged.empty()) {
        desc += " (" + fieldChanged + ")";
    }
    activity.setDescription(desc);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logAccessChange(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int studentId, const std::string& studentName, bool revoked) {

    Models::ActivityLog activity = Models::ActivityLog::createAccessChangeActivity(
        adminId, adminName, adminEmail, studentId, studentName, revoked);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logUserCreated(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    int newUserId, const std::string& newUserName, const std::string& newUserEmail,
    const std::string& role) {

    Models::ActivityLog activity = Models::ActivityLog::createUserCreatedActivity(
        adminId, adminName, adminEmail, newUserId, newUserName, newUserEmail, role);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logCurriculumSelected(
    int studentId, const std::string& studentName, const std::string& studentEmail,
    const std::string& curriculumId, const std::string& curriculumName) {

    Models::ActivityLog activity = Models::ActivityLog::createCurriculumSelectedActivity(
        studentId, studentName, studentEmail, curriculumId, curriculumName);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logCurriculumChange(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    const std::string& curriculumId, const std::string& curriculumName,
    bool isNew) {

    Models::ActivityLog activity;
    activity.setActorType(Models::ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType(isNew ? "curriculum_created" : "curriculum_updated");
    activity.setActionCategory(Models::ActivityCategory::Admin);
    activity.setEntityType("curriculum");
    activity.setEntityId(curriculumId);
    activity.setEntityName(curriculumName);
    activity.setSeverity(Models::ActivitySeverity::Success);

    std::string action = isNew ? "created" : "updated";
    activity.setDescription(adminName + " " + action + " program: " + curriculumName);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logSettingsUpdate(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    const std::string& settingName) {

    Models::ActivityLog activity;
    activity.setActorType(Models::ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType("settings_updated");
    activity.setActionCategory(Models::ActivityCategory::Admin);
    activity.setEntityType("institution_settings");
    activity.setEntityName("Institution Settings");
    activity.setSeverity(Models::ActivitySeverity::Info);

    std::string desc = adminName + " updated institution settings";
    if (!settingName.empty()) {
        desc = adminName + " updated setting: " + settingName;
        activity.setEntityId(settingName);
    }
    activity.setDescription(desc);

    return logActivity(activity);
}

ActivityLogResult ActivityLogService::logAdminAction(
    int adminId, const std::string& adminName, const std::string& adminEmail,
    const std::string& actionType, const std::string& description,
    const std::string& entityType, const std::string& entityId,
    const std::string& entityName) {

    Models::ActivityLog activity;
    activity.setActorType(Models::ActorType::Admin);
    activity.setActorId(adminId);
    activity.setActorName(adminName);
    activity.setActorEmail(adminEmail);
    activity.setActionType(actionType);
    activity.setActionCategory(Models::ActivityCategory::Admin);
    activity.setDescription(description);
    activity.setEntityType(entityType);
    activity.setEntityId(entityId);
    activity.setEntityName(entityName);
    activity.setSeverity(Models::ActivitySeverity::Info);

    return logActivity(activity);
}

// =============================================================================
// Private helpers
// =============================================================================

Models::ActivityLog ActivityLogService::parseActivityFromJson(const nlohmann::json& json) {
    return Models::ActivityLog::fromJson(json);
}

std::vector<Models::ActivityLog> ActivityLogService::parseActivitiesFromJson(const nlohmann::json& json) {
    std::vector<Models::ActivityLog> activities;

    try {
        if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                // JSON:API format: data array contains objects with id and attributes
                Models::ActivityLog activity;

                if (item.contains("id")) {
                    auto idVal = item["id"];
                    if (idVal.is_string()) {
                        activity.setId(std::stoi(idVal.get<std::string>()));
                    } else if (idVal.is_number()) {
                        activity.setId(idVal.get<int>());
                    }
                }

                const nlohmann::json* attrs = &item;
                if (item.contains("attributes")) {
                    attrs = &item["attributes"];
                }

                const nlohmann::json& a = *attrs;

                if (a.contains("actor_type") && !a["actor_type"].is_null()) {
                    activity.setActorType(Models::ActivityLog::actorTypeFromString(
                        a["actor_type"].get<std::string>()));
                }
                if (a.contains("actor_id") && !a["actor_id"].is_null()) {
                    if (a["actor_id"].is_number()) {
                        activity.setActorId(a["actor_id"].get<int>());
                    } else if (a["actor_id"].is_string()) {
                        activity.setActorId(std::stoi(a["actor_id"].get<std::string>()));
                    }
                }
                if (a.contains("actor_name") && !a["actor_name"].is_null()) {
                    activity.setActorName(a["actor_name"].get<std::string>());
                }
                if (a.contains("actor_email") && !a["actor_email"].is_null()) {
                    activity.setActorEmail(a["actor_email"].get<std::string>());
                }
                if (a.contains("action_type") && !a["action_type"].is_null()) {
                    activity.setActionType(a["action_type"].get<std::string>());
                }
                if (a.contains("action_category") && !a["action_category"].is_null()) {
                    activity.setActionCategory(Models::ActivityLog::categoryFromString(
                        a["action_category"].get<std::string>()));
                }
                if (a.contains("description") && !a["description"].is_null()) {
                    activity.setDescription(a["description"].get<std::string>());
                }
                if (a.contains("entity_type") && !a["entity_type"].is_null()) {
                    activity.setEntityType(a["entity_type"].get<std::string>());
                }
                if (a.contains("entity_id") && !a["entity_id"].is_null()) {
                    if (a["entity_id"].is_string()) {
                        activity.setEntityId(a["entity_id"].get<std::string>());
                    } else {
                        activity.setEntityId(std::to_string(a["entity_id"].get<int>()));
                    }
                }
                if (a.contains("entity_name") && !a["entity_name"].is_null()) {
                    activity.setEntityName(a["entity_name"].get<std::string>());
                }
                if (a.contains("details") && !a["details"].is_null()) {
                    activity.setDetails(a["details"]);
                }
                if (a.contains("severity") && !a["severity"].is_null()) {
                    activity.setSeverity(Models::ActivityLog::severityFromString(
                        a["severity"].get<std::string>()));
                }
                if (a.contains("ip_address") && !a["ip_address"].is_null()) {
                    activity.setIpAddress(a["ip_address"].get<std::string>());
                }
                if (a.contains("user_agent") && !a["user_agent"].is_null()) {
                    activity.setUserAgent(a["user_agent"].get<std::string>());
                }
                if (a.contains("session_id") && !a["session_id"].is_null()) {
                    activity.setSessionId(a["session_id"].get<std::string>());
                }
                if (a.contains("created_at") && !a["created_at"].is_null()) {
                    activity.setCreatedAt(a["created_at"].get<std::string>());
                }

                activities.push_back(activity);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("ActivityLogService", "Error parsing activities array: " << e.what());
    }

    return activities;
}

nlohmann::json ActivityLogService::preparePayload(const Models::ActivityLog& activity) {
    nlohmann::json payload;
    payload["data"]["type"] = "ActivityLog";

    nlohmann::json& attrs = payload["data"]["attributes"];
    attrs["actor_type"] = Models::ActivityLog::actorTypeToString(activity.getActorType());

    if (activity.getActorId() > 0) {
        attrs["actor_id"] = activity.getActorId();
    }
    if (!activity.getActorName().empty()) {
        attrs["actor_name"] = activity.getActorName();
    }
    if (!activity.getActorEmail().empty()) {
        attrs["actor_email"] = activity.getActorEmail();
    }

    attrs["action_type"] = activity.getActionType();
    attrs["action_category"] = Models::ActivityLog::categoryToString(activity.getActionCategory());
    attrs["description"] = activity.getDescription();

    if (!activity.getEntityType().empty()) {
        attrs["entity_type"] = activity.getEntityType();
    }
    if (!activity.getEntityId().empty()) {
        attrs["entity_id"] = activity.getEntityId();
    }
    if (!activity.getEntityName().empty()) {
        attrs["entity_name"] = activity.getEntityName();
    }

    nlohmann::json details = activity.getDetails();
    if (!details.empty() && !details.is_null()) {
        attrs["details"] = details;
    } else {
        attrs["details"] = nlohmann::json::object();
    }

    attrs["severity"] = Models::ActivityLog::severityToString(activity.getSeverity());

    if (!activity.getIpAddress().empty()) {
        attrs["ip_address"] = activity.getIpAddress();
    }
    if (!activity.getUserAgent().empty()) {
        attrs["user_agent"] = activity.getUserAgent();
    }
    if (!activity.getSessionId().empty()) {
        attrs["session_id"] = activity.getSessionId();
    }

    // Set timestamp if not already set
    if (activity.getCreatedAt().empty()) {
        attrs["created_at"] = getCurrentTimestamp();
    } else {
        attrs["created_at"] = activity.getCreatedAt();
    }

    return payload;
}

std::string ActivityLogService::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

} // namespace Api
} // namespace StudentIntake
