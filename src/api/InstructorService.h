#ifndef INSTRUCTOR_SERVICE_H
#define INSTRUCTOR_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "ApiClient.h"
#include "models/Instructor.h"
#include "models/TrainingSession.h"
#include "models/StudentProgress.h"

namespace StudentIntake {
namespace Api {

/**
 * @brief Result of an instructor API operation
 */
struct InstructorResult {
    bool success;
    std::string id;
    std::string message;
    std::vector<std::string> errors;
    nlohmann::json responseData;
};

/**
 * @brief Callback for async instructor operations
 */
using InstructorCallback = std::function<void(const InstructorResult&)>;

/**
 * @brief Summary of a student's progress for instructor view
 */
struct StudentProgressSummary {
    int studentId;
    std::string studentName;
    std::string email;
    int enrollmentId;
    std::string courseName;
    double overallProgress;
    double hoursCompleted;
    int modulesCompleted;
    int totalModules;
    std::string lastActivity;
    std::string status;
    std::vector<Models::StudentFeedback> recentFeedback;
};

/**
 * @brief Calendar event for instructor schedule
 */
struct CalendarEvent {
    std::string id;
    std::string title;
    std::string date;
    std::string startTime;
    std::string endTime;
    std::string type;
    int studentId;
    std::string studentName;
    std::string location;
    std::string status;
};

/**
 * @brief Dashboard statistics for instructor
 */
struct InstructorDashboardStats {
    int activeStudents;
    int sessionsToday;
    int sessionsThisWeek;
    int pendingValidations;
    int pendingFeedback;
    double averageStudentProgress;
    int studentsCompletedToday;
    int upcomingTests;
};

/**
 * @brief Service for instructor-related API operations
 *
 * Handles all API interactions for the instructor feature including:
 * - Instructor authentication and profile management
 * - Student progress observation
 * - Session scheduling (skills practice, pre-trip, road tests)
 * - Feedback management
 * - Skill validation and CDL testing
 */
class InstructorService {
public:
    InstructorService();
    explicit InstructorService(std::shared_ptr<ApiClient> apiClient);
    ~InstructorService();

    // API client management
    void setApiClient(std::shared_ptr<ApiClient> client) { apiClient_ = client; }
    std::shared_ptr<ApiClient> getApiClient() const { return apiClient_; }

    // =========================================================================
    // Instructor Profile API Endpoints
    // =========================================================================

    /**
     * @brief Get instructor profile by user ID
     */
    Models::Instructor getInstructorByUserId(int userId);

    /**
     * @brief Get instructor profile by ID
     */
    Models::Instructor getInstructor(const std::string& instructorId);

    /**
     * @brief Get all instructors
     */
    std::vector<Models::Instructor> getAllInstructors();

    /**
     * @brief Get instructors by type (instructor, examiner, or both)
     */
    std::vector<Models::Instructor> getInstructorsByType(Models::InstructorType type);

    /**
     * @brief Update instructor profile
     */
    InstructorResult updateInstructor(const Models::Instructor& instructor);

    /**
     * @brief Get instructor qualifications
     */
    std::vector<Models::InstructorQualification> getInstructorQualifications(int instructorId);

    /**
     * @brief Add a qualification to an instructor
     */
    InstructorResult addQualification(const Models::InstructorQualification& qualification);

    /**
     * @brief Update a qualification
     */
    InstructorResult updateQualification(const Models::InstructorQualification& qualification);

    // =========================================================================
    // Instructor Assignment API Endpoints
    // =========================================================================

    /**
     * @brief Get students assigned to an instructor
     */
    std::vector<Models::InstructorAssignment> getInstructorAssignments(int instructorId);

    /**
     * @brief Get current active assignments for an instructor
     */
    std::vector<Models::InstructorAssignment> getActiveAssignments(int instructorId);

    /**
     * @brief Assign a student to an instructor
     */
    InstructorResult assignStudent(int instructorId, int studentId, int enrollmentId,
                                    const std::string& role = "primary_instructor");

    /**
     * @brief Remove a student assignment
     */
    InstructorResult removeAssignment(const std::string& assignmentId);

    /**
     * @brief Update an assignment
     */
    InstructorResult updateAssignment(const Models::InstructorAssignment& assignment);

    // =========================================================================
    // Student Progress Observation API Endpoints
    // =========================================================================

    /**
     * @brief Get progress summary for all assigned students
     */
    std::vector<StudentProgressSummary> getAssignedStudentProgress(int instructorId);

    /**
     * @brief Get detailed progress for a specific student
     */
    StudentProgressSummary getStudentProgressDetail(int instructorId, int studentId);

    /**
     * @brief Get student's course progress (modules, content, time)
     */
    std::vector<Models::StudentModuleProgress> getStudentCourseProgress(int studentId, int courseId);

    /**
     * @brief Get student's skill progress
     */
    std::vector<Models::StudentSkillProgress> getStudentSkillProgress(int studentId, int enrollmentId);

    /**
     * @brief Get students needing attention (low progress, missed sessions, etc.)
     */
    std::vector<StudentProgressSummary> getStudentsNeedingAttention(int instructorId);

    // =========================================================================
    // Session Scheduling API Endpoints
    // =========================================================================

    /**
     * @brief Get all sessions for an instructor
     */
    std::vector<Models::ScheduledSession> getInstructorSessions(int instructorId);

    /**
     * @brief Get sessions for a specific date range
     */
    std::vector<Models::ScheduledSession> getSessionsByDateRange(int instructorId,
                                                                   const std::string& startDate,
                                                                   const std::string& endDate);

    /**
     * @brief Get sessions for today
     */
    std::vector<Models::ScheduledSession> getTodaySessions(int instructorId);

    /**
     * @brief Get upcoming sessions
     */
    std::vector<Models::ScheduledSession> getUpcomingSessions(int instructorId, int limit = 10);

    /**
     * @brief Get sessions for a specific student
     */
    std::vector<Models::ScheduledSession> getStudentSessions(int studentId);

    /**
     * @brief Get a specific session
     */
    Models::ScheduledSession getSession(const std::string& sessionId);

    /**
     * @brief Schedule a new session
     */
    InstructorResult scheduleSession(const Models::ScheduledSession& session);

    /**
     * @brief Update a session
     */
    InstructorResult updateSession(const Models::ScheduledSession& session);

    /**
     * @brief Cancel a session
     */
    InstructorResult cancelSession(const std::string& sessionId, const std::string& reason);

    /**
     * @brief Reschedule a session
     */
    InstructorResult rescheduleSession(const std::string& sessionId,
                                         const std::string& newDate,
                                         const std::string& newStartTime,
                                         const std::string& newEndTime);

    /**
     * @brief Start a session (mark as in progress)
     */
    InstructorResult startSession(const std::string& sessionId);

    /**
     * @brief Complete a session
     */
    InstructorResult completeSession(const std::string& sessionId, const std::string& notes = "");

    /**
     * @brief Record a no-show for a session
     */
    InstructorResult recordNoShow(const std::string& sessionId, const std::string& notes = "");

    /**
     * @brief Get calendar events for instructor
     */
    std::vector<CalendarEvent> getCalendarEvents(int instructorId,
                                                   const std::string& startDate,
                                                   const std::string& endDate);

    // =========================================================================
    // Instructor Availability API Endpoints
    // =========================================================================

    /**
     * @brief Get instructor availability
     */
    std::vector<Models::InstructorAvailability> getInstructorAvailability(int instructorId);

    /**
     * @brief Set instructor availability
     */
    InstructorResult setAvailability(const Models::InstructorAvailability& availability);

    /**
     * @brief Remove an availability slot
     */
    InstructorResult removeAvailability(const std::string& availabilityId);

    /**
     * @brief Get available time slots for scheduling
     */
    std::vector<std::string> getAvailableSlots(int instructorId, const std::string& date,
                                                 int durationMinutes);

    // =========================================================================
    // Student Feedback API Endpoints
    // =========================================================================

    /**
     * @brief Get all feedback provided by an instructor
     */
    std::vector<Models::StudentFeedback> getInstructorFeedback(int instructorId);

    /**
     * @brief Get feedback for a specific student
     */
    std::vector<Models::StudentFeedback> getStudentFeedback(int studentId);

    /**
     * @brief Get feedback for a student from a specific instructor
     */
    std::vector<Models::StudentFeedback> getStudentFeedbackByInstructor(int studentId, int instructorId);

    /**
     * @brief Get feedback for a session
     */
    std::vector<Models::StudentFeedback> getSessionFeedback(int sessionId);

    /**
     * @brief Get pending follow-up feedback
     */
    std::vector<Models::StudentFeedback> getPendingFollowUps(int instructorId);

    /**
     * @brief Get a specific feedback record
     */
    Models::StudentFeedback getFeedback(const std::string& feedbackId);

    /**
     * @brief Create new feedback
     */
    InstructorResult createFeedback(const Models::StudentFeedback& feedback);

    /**
     * @brief Update feedback
     */
    InstructorResult updateFeedback(const Models::StudentFeedback& feedback);

    /**
     * @brief Delete feedback
     */
    InstructorResult deleteFeedback(const std::string& feedbackId);

    /**
     * @brief Mark follow-up as completed
     */
    InstructorResult completeFollowUp(const std::string& feedbackId);

    /**
     * @brief Student acknowledges feedback
     */
    InstructorResult acknowledgeFeedback(const std::string& feedbackId, int studentId);

    // =========================================================================
    // Skill Validation API Endpoints
    // =========================================================================

    /**
     * @brief Get skill categories
     */
    std::vector<Models::SkillCategory> getSkillCategories();

    /**
     * @brief Get skill items for a category
     */
    std::vector<Models::SkillItem> getSkillItems(int categoryId);

    /**
     * @brief Get all skill items
     */
    std::vector<Models::SkillItem> getAllSkillItems();

    /**
     * @brief Get student's skill progress
     */
    std::vector<Models::StudentSkillProgress> getSkillProgress(int studentId, int enrollmentId);

    /**
     * @brief Get validations performed by an instructor
     */
    std::vector<Models::SkillValidation> getInstructorValidations(int instructorId);

    /**
     * @brief Get validations for a student
     */
    std::vector<Models::SkillValidation> getStudentValidations(int studentId);

    /**
     * @brief Get validations for a session
     */
    std::vector<Models::SkillValidation> getSessionValidations(int sessionId);

    /**
     * @brief Get a specific validation
     */
    Models::SkillValidation getValidation(const std::string& validationId);

    /**
     * @brief Create a new skill validation
     */
    InstructorResult createValidation(const Models::SkillValidation& validation);

    /**
     * @brief Update a validation
     */
    InstructorResult updateValidation(const Models::SkillValidation& validation);

    /**
     * @brief Delete a validation
     */
    InstructorResult deleteValidation(const std::string& validationId);

    /**
     * @brief Check if student has passed all required skills
     */
    bool hasPassedAllSkills(int studentId, int enrollmentId);

    /**
     * @brief Get skills needing validation for a student
     */
    std::vector<Models::SkillItem> getSkillsNeedingValidation(int studentId, int enrollmentId);

    // =========================================================================
    // CDL Testing API Endpoints
    // =========================================================================

    /**
     * @brief Record a CDL test result
     */
    InstructorResult recordCdlTestResult(int instructorId, int studentId,
                                          const std::string& testType,
                                          bool passed, double score,
                                          const std::string& notes = "");

    /**
     * @brief Get CDL test results for a student
     */
    nlohmann::json getCdlTestResults(int studentId);

    /**
     * @brief Check if student is ready for CDL test
     */
    bool isStudentReadyForCdlTest(int studentId, int enrollmentId);

    /**
     * @brief Issue CDL recommendation
     */
    InstructorResult issueCdlRecommendation(int instructorId, int studentId,
                                             const std::string& endorsements,
                                             const std::string& notes = "");

    // =========================================================================
    // Dashboard API Endpoints
    // =========================================================================

    /**
     * @brief Get dashboard statistics for an instructor
     */
    InstructorDashboardStats getDashboardStats(int instructorId);

    /**
     * @brief Get recent activity feed
     */
    nlohmann::json getRecentActivity(int instructorId, int limit = 20);

    /**
     * @brief Get notifications for instructor
     */
    nlohmann::json getNotifications(int instructorId);

    /**
     * @brief Mark notification as read
     */
    InstructorResult markNotificationRead(const std::string& notificationId);

    // =========================================================================
    // Async Operations
    // =========================================================================

    void scheduleSessionAsync(const Models::ScheduledSession& session, InstructorCallback callback);
    void createFeedbackAsync(const Models::StudentFeedback& feedback, InstructorCallback callback);
    void createValidationAsync(const Models::SkillValidation& validation, InstructorCallback callback);
    void getAssignedStudentProgressAsync(int instructorId,
                                          std::function<void(const std::vector<StudentProgressSummary>&)> callback);
    void getDashboardStatsAsync(int instructorId,
                                 std::function<void(const InstructorDashboardStats&)> callback);

private:
    std::shared_ptr<ApiClient> apiClient_;

    // Helper methods
    InstructorResult parseResponse(const ApiResponse& response);
    nlohmann::json buildJsonApiPayload(const std::string& type, const nlohmann::json& attributes);
    nlohmann::json buildJsonApiPayload(const std::string& type, const std::string& id,
                                        const nlohmann::json& attributes);
    StudentProgressSummary parseStudentProgressSummary(const nlohmann::json& json);
    CalendarEvent parseCalendarEvent(const nlohmann::json& json);
};

} // namespace Api
} // namespace StudentIntake

#endif // INSTRUCTOR_SERVICE_H
