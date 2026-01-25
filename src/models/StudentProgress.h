#ifndef STUDENT_PROGRESS_H
#define STUDENT_PROGRESS_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Enrollment status enumeration
 */
enum class EnrollmentStatus {
    Active,
    Paused,
    Withdrawn,
    Completed,
    Expired
};

/**
 * @brief Completion status enumeration
 */
enum class CompletionStatus {
    NotStarted,
    InProgress,
    Completed,
    Failed
};

/**
 * @brief Progress status enumeration for modules
 */
enum class ProgressStatus {
    NotStarted,
    InProgress,
    Completed,
    Failed,
    Locked
};

/**
 * @brief Represents a student's enrollment in a course
 */
class StudentCourseEnrollment {
public:
    StudentCourseEnrollment();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getCourseId() const { return courseId_; }
    std::string getEnrolledAt() const { return enrolledAt_; }
    EnrollmentStatus getEnrollmentStatus() const { return enrollmentStatus_; }
    std::string getEnrollmentStatusString() const;
    int getCurrentModuleId() const { return currentModuleId_; }
    double getProgressPercentage() const { return progressPercentage_; }
    int getTotalTimeSpent() const { return totalTimeSpent_; }
    std::string getStartedAt() const { return startedAt_; }
    std::string getCompletedAt() const { return completedAt_; }
    CompletionStatus getCompletionStatus() const { return completionStatus_; }
    std::string getCompletionStatusString() const;
    double getFinalScore() const { return finalScore_; }
    bool hasPassed() const { return passed_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setEnrolledAt(const std::string& timestamp) { enrolledAt_ = timestamp; }
    void setEnrollmentStatus(EnrollmentStatus status) { enrollmentStatus_ = status; }
    void setEnrollmentStatusFromString(const std::string& status);
    void setCurrentModuleId(int id) { currentModuleId_ = id; }
    void setProgressPercentage(double percentage) { progressPercentage_ = percentage; }
    void setTotalTimeSpent(int seconds) { totalTimeSpent_ = seconds; }
    void setStartedAt(const std::string& timestamp) { startedAt_ = timestamp; }
    void setCompletedAt(const std::string& timestamp) { completedAt_ = timestamp; }
    void setCompletionStatus(CompletionStatus status) { completionStatus_ = status; }
    void setCompletionStatusFromString(const std::string& status);
    void setFinalScore(double score) { finalScore_ = score; }
    void setPassed(bool passed) { passed_ = passed; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentCourseEnrollment fromJson(const nlohmann::json& json);

    // Helper methods
    static EnrollmentStatus stringToEnrollmentStatus(const std::string& status);
    static std::string enrollmentStatusToString(EnrollmentStatus status);
    static CompletionStatus stringToCompletionStatus(const std::string& status);
    static std::string completionStatusToString(CompletionStatus status);

    // Utility
    std::string getFormattedTimeSpent() const;  // Returns "HH:MM:SS" format
    bool isActive() const { return enrollmentStatus_ == EnrollmentStatus::Active; }
    bool isCompleted() const { return completionStatus_ == CompletionStatus::Completed; }

private:
    std::string id_;
    int studentId_;
    int courseId_;
    std::string enrolledAt_;
    EnrollmentStatus enrollmentStatus_;
    int currentModuleId_;
    double progressPercentage_;
    int totalTimeSpent_;  // Seconds
    std::string startedAt_;
    std::string completedAt_;
    CompletionStatus completionStatus_;
    double finalScore_;
    bool passed_;
};

/**
 * @brief Represents a student's progress through a module
 */
class StudentModuleProgress {
public:
    StudentModuleProgress();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getModuleId() const { return moduleId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    ProgressStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    double getProgressPercentage() const { return progressPercentage_; }
    int getTimeSpent() const { return timeSpent_; }
    std::vector<int> getCompletedContentIds() const { return completedContentIds_; }
    int getAttempts() const { return attempts_; }
    double getBestScore() const { return bestScore_; }
    double getLastScore() const { return lastScore_; }
    std::string getStartedAt() const { return startedAt_; }
    std::string getCompletedAt() const { return completedAt_; }
    std::string getLastAccessedAt() const { return lastAccessedAt_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setModuleId(int id) { moduleId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setStatus(ProgressStatus status) { status_ = status; }
    void setStatusFromString(const std::string& status);
    void setProgressPercentage(double percentage) { progressPercentage_ = percentage; }
    void setTimeSpent(int seconds) { timeSpent_ = seconds; }
    void setCompletedContentIds(const std::vector<int>& ids) { completedContentIds_ = ids; }
    void addCompletedContentId(int id);
    void setAttempts(int count) { attempts_ = count; }
    void setBestScore(double score) { bestScore_ = score; }
    void setLastScore(double score) { lastScore_ = score; }
    void setStartedAt(const std::string& timestamp) { startedAt_ = timestamp; }
    void setCompletedAt(const std::string& timestamp) { completedAt_ = timestamp; }
    void setLastAccessedAt(const std::string& timestamp) { lastAccessedAt_ = timestamp; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentModuleProgress fromJson(const nlohmann::json& json);

    // Helper methods
    static ProgressStatus stringToProgressStatus(const std::string& status);
    static std::string progressStatusToString(ProgressStatus status);

    // Utility
    bool isContentCompleted(int contentId) const;
    bool isCompleted() const { return status_ == ProgressStatus::Completed; }
    bool isLocked() const { return status_ == ProgressStatus::Locked; }

private:
    std::string id_;
    int studentId_;
    int moduleId_;
    int enrollmentId_;
    ProgressStatus status_;
    double progressPercentage_;
    int timeSpent_;  // Seconds
    std::vector<int> completedContentIds_;
    int attempts_;
    double bestScore_;
    double lastScore_;
    std::string startedAt_;
    std::string completedAt_;
    std::string lastAccessedAt_;
};

/**
 * @brief Represents a student's progress through a content item
 */
class StudentContentProgress {
public:
    StudentContentProgress();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getContentId() const { return contentId_; }
    int getModuleProgressId() const { return moduleProgressId_; }
    std::string getStatus() const { return status_; }
    int getTimeSpent() const { return timeSpent_; }
    int getLastPosition() const { return lastPosition_; }
    int getViewCount() const { return viewCount_; }
    bool isCompleted() const { return completed_; }
    std::string getCompletedAt() const { return completedAt_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setContentId(int id) { contentId_ = id; }
    void setModuleProgressId(int id) { moduleProgressId_ = id; }
    void setStatus(const std::string& status) { status_ = status; }
    void setTimeSpent(int seconds) { timeSpent_ = seconds; }
    void setLastPosition(int seconds) { lastPosition_ = seconds; }
    void setViewCount(int count) { viewCount_ = count; }
    void setCompleted(bool completed) { completed_ = completed; }
    void setCompletedAt(const std::string& timestamp) { completedAt_ = timestamp; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentContentProgress fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int studentId_;
    int contentId_;
    int moduleProgressId_;
    std::string status_;  // 'not_started', 'in_progress', 'completed'
    int timeSpent_;       // Seconds
    int lastPosition_;    // For video: last playback position in seconds
    int viewCount_;
    bool completed_;
    std::string completedAt_;
};

/**
 * @brief Activity type enumeration for time logging
 */
enum class ActivityType {
    Reading,
    Video,
    Quiz,
    Review,
    Navigation,
    Idle
};

/**
 * @brief Represents a time log entry for a student's session
 *
 * Used for regulatory compliance and tracking student engagement.
 */
class StudentTimeLog {
public:
    StudentTimeLog();

    // Getters
    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getCourseId() const { return courseId_; }
    int getModuleId() const { return moduleId_; }
    int getContentId() const { return contentId_; }
    std::string getSessionStart() const { return sessionStart_; }
    std::string getSessionEnd() const { return sessionEnd_; }
    int getDurationSeconds() const { return durationSeconds_; }
    ActivityType getActivityType() const { return activityType_; }
    std::string getActivityTypeString() const;
    bool isValidated() const { return isValidated_; }
    std::string getValidationMethod() const { return validationMethod_; }
    std::string getIpAddress() const { return ipAddress_; }
    std::string getUserAgent() const { return userAgent_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setModuleId(int id) { moduleId_ = id; }
    void setContentId(int id) { contentId_ = id; }
    void setSessionStart(const std::string& timestamp) { sessionStart_ = timestamp; }
    void setSessionEnd(const std::string& timestamp) { sessionEnd_ = timestamp; }
    void setDurationSeconds(int seconds) { durationSeconds_ = seconds; }
    void setActivityType(ActivityType type) { activityType_ = type; }
    void setActivityTypeFromString(const std::string& type);
    void setValidated(bool validated) { isValidated_ = validated; }
    void setValidationMethod(const std::string& method) { validationMethod_ = method; }
    void setIpAddress(const std::string& ip) { ipAddress_ = ip; }
    void setUserAgent(const std::string& agent) { userAgent_ = agent; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentTimeLog fromJson(const nlohmann::json& json);

    // Helper methods
    static ActivityType stringToActivityType(const std::string& type);
    static std::string activityTypeToString(ActivityType type);

    // Utility
    std::string getFormattedDuration() const;  // Returns "HH:MM:SS" format

private:
    std::string id_;
    int studentId_;
    int enrollmentId_;
    int courseId_;
    int moduleId_;
    int contentId_;
    std::string sessionStart_;
    std::string sessionEnd_;
    int durationSeconds_;
    ActivityType activityType_;
    bool isValidated_;
    std::string validationMethod_;
    std::string ipAddress_;
    std::string userAgent_;
};

} // namespace Models
} // namespace StudentIntake

#endif // STUDENT_PROGRESS_H
