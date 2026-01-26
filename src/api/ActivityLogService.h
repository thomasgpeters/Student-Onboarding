#ifndef ACTIVITY_LOG_SERVICE_H
#define ACTIVITY_LOG_SERVICE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "ApiClient.h"
#include "models/ActivityLog.h"

namespace StudentIntake {
namespace Api {

/**
 * @brief Filter options for querying activities
 */
struct ActivityFilter {
    std::string actorType;       // Filter by actor type: student, instructor, admin, system
    std::string actionCategory;  // Filter by category: authentication, forms, profile, admin, system
    std::string actionType;      // Filter by specific action type
    std::string entityType;      // Filter by entity type
    std::string entityId;        // Filter by specific entity
    int actorId = 0;             // Filter by specific actor
    int limit = 10;              // Maximum records to return (default 10)
    int offset = 0;              // Pagination offset
    std::string sortBy = "created_at";  // Sort field
    bool sortDesc = true;        // Sort direction (default: newest first)

    /**
     * @brief Build query string for API request
     */
    std::string toQueryString() const;
};

/**
 * @brief Result of logging an activity
 */
struct ActivityLogResult {
    bool success;
    int activityId;
    std::string message;
    std::string errorMessage;
};

/**
 * @brief Callback for async activity operations
 */
using ActivityCallback = std::function<void(const ActivityLogResult&)>;
using ActivitiesCallback = std::function<void(const std::vector<Models::ActivityLog>&, bool success)>;

/**
 * @brief Service for logging and querying activity/audit trail events
 *
 * Communicates with ApiLogicServer backend using JSON:API format to:
 * - Log new activities (authentication, form submissions, admin actions)
 * - Query recent activities for dashboard display
 * - Retrieve activity details for drill-down views
 */
class ActivityLogService {
public:
    ActivityLogService();
    explicit ActivityLogService(std::shared_ptr<ApiClient> apiClient);
    ~ActivityLogService();

    // =========================================================================
    // Configuration
    // =========================================================================

    void setApiClient(std::shared_ptr<ApiClient> client) { apiClient_ = client; }
    std::shared_ptr<ApiClient> getApiClient() const { return apiClient_; }

    // =========================================================================
    // Query Activities
    // =========================================================================

    /**
     * @brief Get recent activities for dashboard display
     * @param limit Maximum number of activities to return (default 10)
     * @return Vector of recent activities, sorted by time descending
     */
    std::vector<Models::ActivityLog> getRecentActivities(int limit = 10);

    /**
     * @brief Get activities with filtering options
     * @param filter Filter criteria
     * @return Vector of matching activities
     */
    std::vector<Models::ActivityLog> getActivities(const ActivityFilter& filter);

    /**
     * @brief Get a single activity by ID
     * @param activityId The activity ID
     * @return The activity, or empty ActivityLog if not found
     */
    Models::ActivityLog getActivity(int activityId);

    /**
     * @brief Get activities for a specific user
     * @param userId The user's app_user ID
     * @param limit Maximum number to return
     * @return Vector of activities for this user
     */
    std::vector<Models::ActivityLog> getActivitiesForUser(int userId, int limit = 20);

    /**
     * @brief Get activities for a specific entity
     * @param entityType The entity type (student, form_submission, curriculum, etc.)
     * @param entityId The entity ID
     * @param limit Maximum number to return
     * @return Vector of activities for this entity
     */
    std::vector<Models::ActivityLog> getActivitiesForEntity(
        const std::string& entityType, const std::string& entityId, int limit = 20);

    /**
     * @brief Get total count of activities (for pagination)
     * @param filter Optional filter criteria
     * @return Total count
     */
    int getActivityCount(const ActivityFilter& filter = ActivityFilter());

    // =========================================================================
    // Async Query Methods
    // =========================================================================

    /**
     * @brief Get recent activities asynchronously
     */
    void getRecentActivitiesAsync(int limit, ActivitiesCallback callback);

    /**
     * @brief Get activities with filter asynchronously
     */
    void getActivitiesAsync(const ActivityFilter& filter, ActivitiesCallback callback);

    // =========================================================================
    // Log Activities
    // =========================================================================

    /**
     * @brief Log a new activity
     * @param activity The activity to log
     * @return Result with success status and new activity ID
     */
    ActivityLogResult logActivity(const Models::ActivityLog& activity);

    /**
     * @brief Log an activity asynchronously (fire-and-forget)
     * @param activity The activity to log
     * @param callback Optional callback for result
     */
    void logActivityAsync(const Models::ActivityLog& activity,
                          ActivityCallback callback = nullptr);

    // =========================================================================
    // Convenience Methods for Common Activities
    // =========================================================================

    /**
     * @brief Log a login event
     */
    ActivityLogResult logLogin(
        int userId, const std::string& userName, const std::string& email,
        const std::string& actorType, const std::string& ipAddress,
        bool success, const std::string& failureReason = "");

    /**
     * @brief Log a logout event
     */
    ActivityLogResult logLogout(
        int userId, const std::string& userName, const std::string& email,
        const std::string& actorType);

    /**
     * @brief Log a form submission event
     */
    ActivityLogResult logFormSubmission(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& formId, const std::string& formName);

    /**
     * @brief Log a form approval/rejection event
     */
    ActivityLogResult logFormReview(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int studentId, const std::string& studentName,
        int formSubmissionId, const std::string& formName,
        bool approved, const std::string& reason = "");

    /**
     * @brief Log a student registration event
     */
    ActivityLogResult logStudentRegistration(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& ipAddress = "");

    /**
     * @brief Log a profile update event
     */
    ActivityLogResult logProfileUpdate(
        int userId, const std::string& userName, const std::string& userEmail,
        const std::string& actorType, const std::string& fieldChanged = "");

    /**
     * @brief Log an access change event (revoke/restore)
     */
    ActivityLogResult logAccessChange(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int studentId, const std::string& studentName, bool revoked);

    /**
     * @brief Log a user creation event
     */
    ActivityLogResult logUserCreated(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int newUserId, const std::string& newUserName, const std::string& newUserEmail,
        const std::string& role);

    /**
     * @brief Log a curriculum selection event
     */
    ActivityLogResult logCurriculumSelected(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& curriculumId, const std::string& curriculumName);

    /**
     * @brief Log a curriculum creation/update event
     */
    ActivityLogResult logCurriculumChange(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        const std::string& curriculumId, const std::string& curriculumName,
        bool isNew);

    /**
     * @brief Log a settings update event
     */
    ActivityLogResult logSettingsUpdate(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        const std::string& settingName = "");

    /**
     * @brief Log a generic admin action
     */
    ActivityLogResult logAdminAction(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        const std::string& actionType, const std::string& description,
        const std::string& entityType = "", const std::string& entityId = "",
        const std::string& entityName = "");

private:
    std::shared_ptr<ApiClient> apiClient_;

    /**
     * @brief Parse activity from JSON:API response
     */
    Models::ActivityLog parseActivityFromJson(const nlohmann::json& json);

    /**
     * @brief Parse multiple activities from JSON:API response
     */
    std::vector<Models::ActivityLog> parseActivitiesFromJson(const nlohmann::json& json);

    /**
     * @brief Prepare JSON:API payload for creating an activity
     */
    nlohmann::json preparePayload(const Models::ActivityLog& activity);

    /**
     * @brief Get current timestamp in ISO 8601 format
     */
    std::string getCurrentTimestamp();
};

} // namespace Api
} // namespace StudentIntake

#endif // ACTIVITY_LOG_SERVICE_H
