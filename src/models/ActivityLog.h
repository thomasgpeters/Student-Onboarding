#ifndef ACTIVITY_LOG_H
#define ACTIVITY_LOG_H

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Types of actors who can perform activities
 */
enum class ActorType {
    Student,
    Instructor,
    Admin,
    System
};

/**
 * @brief Categories of activities for filtering
 */
enum class ActivityCategory {
    Authentication,
    Forms,
    Profile,
    Admin,
    System
};

/**
 * @brief Severity levels for activities
 */
enum class ActivitySeverity {
    Info,
    Success,
    Warning,
    Error
};

/**
 * @brief Represents an activity log entry in the audit trail
 *
 * Captures "who did what, when" for all user types (Students, Instructors, Admins)
 * Used to display recent activity in the admin dashboard and provide drill-down
 * capability for administrators.
 */
class ActivityLog {
public:
    ActivityLog();
    ~ActivityLog() = default;

    // =========================================================================
    // Getters
    // =========================================================================

    int getId() const { return id_; }
    ActorType getActorType() const { return actorType_; }
    int getActorId() const { return actorId_; }
    std::string getActorName() const { return actorName_; }
    std::string getActorEmail() const { return actorEmail_; }
    std::string getActionType() const { return actionType_; }
    ActivityCategory getActionCategory() const { return actionCategory_; }
    std::string getDescription() const { return description_; }
    std::string getEntityType() const { return entityType_; }
    std::string getEntityId() const { return entityId_; }
    std::string getEntityName() const { return entityName_; }
    nlohmann::json getDetails() const { return details_; }
    ActivitySeverity getSeverity() const { return severity_; }
    std::string getIpAddress() const { return ipAddress_; }
    std::string getUserAgent() const { return userAgent_; }
    std::string getSessionId() const { return sessionId_; }
    std::string getCreatedAt() const { return createdAt_; }

    // =========================================================================
    // Setters
    // =========================================================================

    void setId(int id) { id_ = id; }
    void setActorType(ActorType type) { actorType_ = type; }
    void setActorId(int id) { actorId_ = id; }
    void setActorName(const std::string& name) { actorName_ = name; }
    void setActorEmail(const std::string& email) { actorEmail_ = email; }
    void setActionType(const std::string& type) { actionType_ = type; }
    void setActionCategory(ActivityCategory cat) { actionCategory_ = cat; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setEntityType(const std::string& type) { entityType_ = type; }
    void setEntityId(const std::string& id) { entityId_ = id; }
    void setEntityName(const std::string& name) { entityName_ = name; }
    void setDetails(const nlohmann::json& details) { details_ = details; }
    void setSeverity(ActivitySeverity severity) { severity_ = severity; }
    void setIpAddress(const std::string& ip) { ipAddress_ = ip; }
    void setUserAgent(const std::string& agent) { userAgent_ = agent; }
    void setSessionId(const std::string& id) { sessionId_ = id; }
    void setCreatedAt(const std::string& time) { createdAt_ = time; }

    // =========================================================================
    // Display helpers
    // =========================================================================

    /**
     * @brief Get actor type as display string
     * @return "Student", "Instructor", "Admin", or "System"
     */
    std::string getActorTypeString() const;

    /**
     * @brief Get action category as display string
     * @return "Authentication", "Forms", "Profile", "Admin", or "System"
     */
    std::string getCategoryString() const;

    /**
     * @brief Get severity as display string
     * @return "info", "success", "warning", or "error"
     */
    std::string getSeverityString() const;

    /**
     * @brief Get relative time display
     * @return "Just now", "5 minutes ago", "2 hours ago", "Jan 26, 2026"
     */
    std::string getRelativeTime() const;

    /**
     * @brief Get formatted timestamp
     * @return "Jan 26, 2026 3:45 PM"
     */
    std::string getFormattedTime() const;

    /**
     * @brief Get CSS icon class based on category/action
     * @return CSS class for icon styling
     */
    std::string getIconClass() const;

    /**
     * @brief Get icon character/emoji based on category
     * @return Unicode character or icon name
     */
    std::string getIcon() const;

    // =========================================================================
    // Serialization
    // =========================================================================

    /**
     * @brief Serialize to JSON for API communication
     */
    nlohmann::json toJson() const;

    /**
     * @brief Deserialize from JSON (API response)
     */
    static ActivityLog fromJson(const nlohmann::json& json);

    // =========================================================================
    // Static conversion helpers
    // =========================================================================

    static ActorType actorTypeFromString(const std::string& str);
    static std::string actorTypeToString(ActorType type);
    static ActivityCategory categoryFromString(const std::string& str);
    static std::string categoryToString(ActivityCategory cat);
    static ActivitySeverity severityFromString(const std::string& str);
    static std::string severityToString(ActivitySeverity sev);

    // =========================================================================
    // Builder methods for common activity types
    // =========================================================================

    /**
     * @brief Create a login activity
     */
    static ActivityLog createLoginActivity(
        ActorType actorType, int actorId, const std::string& actorName,
        const std::string& actorEmail, bool success,
        const std::string& ipAddress = "", const std::string& failureReason = "");

    /**
     * @brief Create a logout activity
     */
    static ActivityLog createLogoutActivity(
        ActorType actorType, int actorId, const std::string& actorName,
        const std::string& actorEmail);

    /**
     * @brief Create a form submission activity
     */
    static ActivityLog createFormSubmittedActivity(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& formId, const std::string& formName);

    /**
     * @brief Create a form approval/rejection activity
     */
    static ActivityLog createFormReviewActivity(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int studentId, const std::string& studentName,
        const std::string& formId, const std::string& formName, bool approved,
        const std::string& reason = "");

    /**
     * @brief Create a student registration activity
     */
    static ActivityLog createStudentRegisteredActivity(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& ipAddress = "");

    /**
     * @brief Create an access change activity (revoke/restore)
     */
    static ActivityLog createAccessChangeActivity(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int studentId, const std::string& studentName, bool revoked);

    /**
     * @brief Create a user created activity
     */
    static ActivityLog createUserCreatedActivity(
        int adminId, const std::string& adminName, const std::string& adminEmail,
        int newUserId, const std::string& newUserName, const std::string& newUserEmail,
        const std::string& role);

    /**
     * @brief Create a curriculum selected activity
     */
    static ActivityLog createCurriculumSelectedActivity(
        int studentId, const std::string& studentName, const std::string& studentEmail,
        const std::string& curriculumId, const std::string& curriculumName);

private:
    int id_;
    ActorType actorType_;
    int actorId_;
    std::string actorName_;
    std::string actorEmail_;
    std::string actionType_;
    ActivityCategory actionCategory_;
    std::string description_;
    std::string entityType_;
    std::string entityId_;
    std::string entityName_;
    nlohmann::json details_;
    ActivitySeverity severity_;
    std::string ipAddress_;
    std::string userAgent_;
    std::string sessionId_;
    std::string createdAt_;
};

} // namespace Models
} // namespace StudentIntake

#endif // ACTIVITY_LOG_H
