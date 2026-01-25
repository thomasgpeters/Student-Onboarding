#ifndef TRAINING_SESSION_H
#define TRAINING_SESSION_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Session status enumeration
 */
enum class SessionStatus {
    Scheduled,
    Confirmed,
    InProgress,
    Completed,
    Cancelled,
    NoShow,
    Rescheduled
};

/**
 * @brief Session type enumeration
 */
enum class SessionType {
    Orientation,
    Classroom,
    PreTripInspection,
    BasicControl,
    RangePractice,
    RoadPractice,
    SkillsTest,
    RoadTest,
    FinalExam,
    Remedial,
    Evaluation,
    Other
};

/**
 * @brief Represents a scheduled training session
 */
class ScheduledSession {
public:
    ScheduledSession();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    SessionType getSessionType() const { return sessionType_; }
    std::string getSessionTypeString() const;
    std::string getTitle() const { return title_; }
    std::string getDescription() const { return description_; }
    std::string getScheduledDate() const { return scheduledDate_; }
    std::string getStartTime() const { return startTime_; }
    std::string getEndTime() const { return endTime_; }
    int getDurationMinutes() const { return durationMinutes_; }
    std::string getLocationType() const { return locationType_; }
    std::string getLocationName() const { return locationName_; }
    std::string getLocationAddress() const { return locationAddress_; }
    std::string getVehicleInfo() const { return vehicleInfo_; }
    SessionStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    std::string getActualStartTime() const { return actualStartTime_; }
    std::string getActualEndTime() const { return actualEndTime_; }
    std::string getCompletedAt() const { return completedAt_; }
    std::string getCancellationReason() const { return cancellationReason_; }
    int getRescheduleCount() const { return rescheduleCount_; }
    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setSessionType(SessionType type) { sessionType_ = type; }
    void setSessionTypeFromString(const std::string& type);
    void setTitle(const std::string& title) { title_ = title; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setScheduledDate(const std::string& date) { scheduledDate_ = date; }
    void setStartTime(const std::string& time) { startTime_ = time; }
    void setEndTime(const std::string& time) { endTime_ = time; }
    void setDurationMinutes(int minutes) { durationMinutes_ = minutes; }
    void setLocationType(const std::string& type) { locationType_ = type; }
    void setLocationName(const std::string& name) { locationName_ = name; }
    void setLocationAddress(const std::string& addr) { locationAddress_ = addr; }
    void setVehicleInfo(const std::string& info) { vehicleInfo_ = info; }
    void setStatus(SessionStatus status) { status_ = status; }
    void setStatusFromString(const std::string& status);
    void setActualStartTime(const std::string& time) { actualStartTime_ = time; }
    void setActualEndTime(const std::string& time) { actualEndTime_ = time; }
    void setCompletedAt(const std::string& time) { completedAt_ = time; }
    void setCancellationReason(const std::string& reason) { cancellationReason_ = reason; }
    void setRescheduleCount(int count) { rescheduleCount_ = count; }
    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static ScheduledSession fromJson(const nlohmann::json& json);

    // Helper methods
    static SessionType stringToSessionType(const std::string& type);
    static std::string sessionTypeToString(SessionType type);
    static SessionStatus stringToSessionStatus(const std::string& status);
    static std::string sessionStatusToString(SessionStatus status);

    // Utility
    bool isUpcoming() const;
    bool canCancel() const;
    bool canReschedule() const;
    std::string getDisplayName() const;

private:
    std::string id_;
    int instructorId_;
    int studentId_;
    int enrollmentId_;
    SessionType sessionType_;
    std::string title_;
    std::string description_;
    std::string scheduledDate_;
    std::string startTime_;
    std::string endTime_;
    int durationMinutes_;
    std::string locationType_;
    std::string locationName_;
    std::string locationAddress_;
    std::string vehicleInfo_;
    SessionStatus status_;
    std::string actualStartTime_;
    std::string actualEndTime_;
    std::string completedAt_;
    std::string cancellationReason_;
    int rescheduleCount_;
    std::string notes_;
};

/**
 * @brief Feedback type enumeration
 */
enum class FeedbackType {
    ProgressUpdate,
    SessionReview,
    SkillAssessment,
    Behavior,
    SafetyConcern,
    Commendation,
    General,
    Other
};

/**
 * @brief Represents instructor feedback on a student
 */
class StudentFeedback {
public:
    StudentFeedback();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getSessionId() const { return sessionId_; }
    FeedbackType getFeedbackType() const { return feedbackType_; }
    std::string getFeedbackTypeString() const;
    std::string getFeedbackCategory() const { return feedbackCategory_; }
    std::string getSubject() const { return subject_; }
    std::string getFeedbackText() const { return feedbackText_; }
    int getPerformanceRating() const { return performanceRating_; }
    int getRatingScale() const { return ratingScale_; }
    std::string getStrengths() const { return strengths_; }
    std::string getAreasForImprovement() const { return areasForImprovement_; }
    std::string getRecommendedActions() const { return recommendedActions_; }
    bool isPrivate() const { return isPrivate_; }
    bool isVisibleToStudent() const { return visibleToStudent_; }
    bool isAcknowledgedByStudent() const { return acknowledgedByStudent_; }
    std::string getAcknowledgedAt() const { return acknowledgedAt_; }
    bool requiresFollowUp() const { return requiresFollowUp_; }
    std::string getFollowUpDate() const { return followUpDate_; }
    bool isFollowUpCompleted() const { return followUpCompleted_; }
    std::string getCreatedAt() const { return createdAt_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setSessionId(int id) { sessionId_ = id; }
    void setFeedbackType(FeedbackType type) { feedbackType_ = type; }
    void setFeedbackTypeFromString(const std::string& type);
    void setFeedbackCategory(const std::string& cat) { feedbackCategory_ = cat; }
    void setSubject(const std::string& subject) { subject_ = subject; }
    void setFeedbackText(const std::string& text) { feedbackText_ = text; }
    void setPerformanceRating(int rating) { performanceRating_ = rating; }
    void setRatingScale(int scale) { ratingScale_ = scale; }
    void setStrengths(const std::string& strengths) { strengths_ = strengths; }
    void setAreasForImprovement(const std::string& areas) { areasForImprovement_ = areas; }
    void setRecommendedActions(const std::string& actions) { recommendedActions_ = actions; }
    void setPrivate(bool priv) { isPrivate_ = priv; }
    void setVisibleToStudent(bool visible) { visibleToStudent_ = visible; }
    void setAcknowledgedByStudent(bool ack) { acknowledgedByStudent_ = ack; }
    void setAcknowledgedAt(const std::string& time) { acknowledgedAt_ = time; }
    void setRequiresFollowUp(bool requires) { requiresFollowUp_ = requires; }
    void setFollowUpDate(const std::string& date) { followUpDate_ = date; }
    void setFollowUpCompleted(bool completed) { followUpCompleted_ = completed; }
    void setCreatedAt(const std::string& time) { createdAt_ = time; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentFeedback fromJson(const nlohmann::json& json);

    // Helper methods
    static FeedbackType stringToFeedbackType(const std::string& type);
    static std::string feedbackTypeToString(FeedbackType type);

    // Utility
    double getRatingPercentage() const;

private:
    std::string id_;
    int instructorId_;
    int studentId_;
    int enrollmentId_;
    int sessionId_;
    FeedbackType feedbackType_;
    std::string feedbackCategory_;
    std::string subject_;
    std::string feedbackText_;
    int performanceRating_;
    int ratingScale_;
    std::string strengths_;
    std::string areasForImprovement_;
    std::string recommendedActions_;
    bool isPrivate_;
    bool visibleToStudent_;
    bool acknowledgedByStudent_;
    std::string acknowledgedAt_;
    bool requiresFollowUp_;
    std::string followUpDate_;
    bool followUpCompleted_;
    std::string createdAt_;
};

/**
 * @brief Validation result enumeration
 */
enum class ValidationResult {
    Pass,
    Fail,
    NeedsPractice,
    Deferred,
    Incomplete
};

/**
 * @brief Represents a skill validation record
 */
class SkillValidation {
public:
    SkillValidation();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getSkillItemId() const { return skillItemId_; }
    int getSessionId() const { return sessionId_; }
    std::string getValidationType() const { return validationType_; }
    ValidationResult getResult() const { return result_; }
    std::string getResultString() const;
    int getAttemptNumber() const { return attemptNumber_; }
    double getScore() const { return score_; }
    double getMaxScore() const { return maxScore_; }
    double getPointsEarned() const { return pointsEarned_; }
    std::string getErrorsNoted() const { return errorsNoted_; }
    bool hasCriticalError() const { return criticalError_; }
    std::string getValidatedAt() const { return validatedAt_; }
    bool isOfficialTest() const { return isOfficialTest_; }
    std::string getWitnessName() const { return witnessName_; }
    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setSkillItemId(int id) { skillItemId_ = id; }
    void setSessionId(int id) { sessionId_ = id; }
    void setValidationType(const std::string& type) { validationType_ = type; }
    void setResult(ValidationResult result) { result_ = result; }
    void setResultFromString(const std::string& result);
    void setAttemptNumber(int num) { attemptNumber_ = num; }
    void setScore(double score) { score_ = score; }
    void setMaxScore(double max) { maxScore_ = max; }
    void setPointsEarned(double points) { pointsEarned_ = points; }
    void setErrorsNoted(const std::string& errors) { errorsNoted_ = errors; }
    void setCriticalError(bool critical) { criticalError_ = critical; }
    void setValidatedAt(const std::string& time) { validatedAt_ = time; }
    void setOfficialTest(bool official) { isOfficialTest_ = official; }
    void setWitnessName(const std::string& name) { witnessName_ = name; }
    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static SkillValidation fromJson(const nlohmann::json& json);

    // Helper methods
    static ValidationResult stringToValidationResult(const std::string& result);
    static std::string validationResultToString(ValidationResult result);

    // Utility
    double getScorePercentage() const;
    bool isPassing() const { return result_ == ValidationResult::Pass; }

private:
    std::string id_;
    int instructorId_;
    int studentId_;
    int enrollmentId_;
    int skillItemId_;
    int sessionId_;
    std::string validationType_;
    ValidationResult result_;
    int attemptNumber_;
    double score_;
    double maxScore_;
    double pointsEarned_;
    std::string errorsNoted_;
    bool criticalError_;
    std::string validatedAt_;
    bool isOfficialTest_;
    std::string witnessName_;
    std::string notes_;
};

/**
 * @brief Represents a skill category
 */
class SkillCategory {
public:
    SkillCategory();

    std::string getId() const { return id_; }
    std::string getCode() const { return code_; }
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    std::string getCategoryType() const { return categoryType_; }
    double getMinimumPracticeHours() const { return minimumPracticeHours_; }
    bool requiresValidation() const { return requiresValidation_; }
    int getDisplayOrder() const { return displayOrder_; }

    void setId(const std::string& id) { id_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setCategoryType(const std::string& type) { categoryType_ = type; }
    void setMinimumPracticeHours(double hours) { minimumPracticeHours_ = hours; }
    void setRequiresValidation(bool requires) { requiresValidation_ = requires; }
    void setDisplayOrder(int order) { displayOrder_ = order; }

    nlohmann::json toJson() const;
    static SkillCategory fromJson(const nlohmann::json& json);

private:
    std::string id_;
    std::string code_;
    std::string name_;
    std::string description_;
    std::string categoryType_;
    double minimumPracticeHours_;
    bool requiresValidation_;
    int displayOrder_;
};

/**
 * @brief Represents a specific skill item
 */
class SkillItem {
public:
    SkillItem();

    std::string getId() const { return id_; }
    int getCategoryId() const { return categoryId_; }
    std::string getCode() const { return code_; }
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    std::string getPassingCriteria() const { return passingCriteria_; }
    std::string getCriticalErrors() const { return criticalErrors_; }
    int getPointValue() const { return pointValue_; }
    bool requiresDemonstration() const { return requiresDemonstration_; }
    int getMinimumSuccessfulAttempts() const { return minimumSuccessfulAttempts_; }
    int getDisplayOrder() const { return displayOrder_; }

    void setId(const std::string& id) { id_ = id; }
    void setCategoryId(int id) { categoryId_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setPassingCriteria(const std::string& criteria) { passingCriteria_ = criteria; }
    void setCriticalErrors(const std::string& errors) { criticalErrors_ = errors; }
    void setPointValue(int value) { pointValue_ = value; }
    void setRequiresDemonstration(bool requires) { requiresDemonstration_ = requires; }
    void setMinimumSuccessfulAttempts(int attempts) { minimumSuccessfulAttempts_ = attempts; }
    void setDisplayOrder(int order) { displayOrder_ = order; }

    nlohmann::json toJson() const;
    static SkillItem fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int categoryId_;
    std::string code_;
    std::string name_;
    std::string description_;
    std::string passingCriteria_;
    std::string criticalErrors_;
    int pointValue_;
    bool requiresDemonstration_;
    int minimumSuccessfulAttempts_;
    int displayOrder_;
};

/**
 * @brief Represents student progress on a skill
 */
class StudentSkillProgress {
public:
    StudentSkillProgress();

    std::string getId() const { return id_; }
    int getStudentId() const { return studentId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    int getSkillItemId() const { return skillItemId_; }
    std::string getStatus() const { return status_; }
    int getPracticeCount() const { return practiceCount_; }
    int getSuccessfulCount() const { return successfulCount_; }
    int getFailedCount() const { return failedCount_; }
    double getBestScore() const { return bestScore_; }
    bool isValidated() const { return isValidated_; }
    std::string getValidatedAt() const { return validatedAt_; }
    double getPracticeHours() const { return practiceHours_; }

    void setId(const std::string& id) { id_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setSkillItemId(int id) { skillItemId_ = id; }
    void setStatus(const std::string& status) { status_ = status; }
    void setPracticeCount(int count) { practiceCount_ = count; }
    void setSuccessfulCount(int count) { successfulCount_ = count; }
    void setFailedCount(int count) { failedCount_ = count; }
    void setBestScore(double score) { bestScore_ = score; }
    void setValidated(bool validated) { isValidated_ = validated; }
    void setValidatedAt(const std::string& time) { validatedAt_ = time; }
    void setPracticeHours(double hours) { practiceHours_ = hours; }

    nlohmann::json toJson() const;
    static StudentSkillProgress fromJson(const nlohmann::json& json);

    double getSuccessRate() const;

private:
    std::string id_;
    int studentId_;
    int enrollmentId_;
    int skillItemId_;
    std::string status_;
    int practiceCount_;
    int successfulCount_;
    int failedCount_;
    double bestScore_;
    bool isValidated_;
    std::string validatedAt_;
    double practiceHours_;
};

} // namespace Models
} // namespace StudentIntake

#endif // TRAINING_SESSION_H
