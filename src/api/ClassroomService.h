#ifndef CLASSROOM_SERVICE_H
#define CLASSROOM_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "ApiClient.h"
#include "models/Course.h"
#include "models/StudentProgress.h"
#include "models/Assessment.h"

namespace StudentIntake {
namespace Api {

/**
 * @brief Result of a classroom API operation
 */
struct ClassroomResult {
    bool success;
    std::string id;
    std::string message;
    std::vector<std::string> errors;
    nlohmann::json responseData;
};

/**
 * @brief Callback for async classroom operations
 */
using ClassroomCallback = std::function<void(const ClassroomResult&)>;

/**
 * @brief Service for classroom-related API operations
 *
 * Handles all API interactions for the online classroom feature including:
 * - Course management
 * - Module and content access
 * - Student enrollment and progress tracking
 * - Time logging for regulatory compliance
 * - Assessments (quizzes and exams)
 * - Assessment reports
 */
class ClassroomService {
public:
    ClassroomService();
    explicit ClassroomService(std::shared_ptr<ApiClient> apiClient);
    ~ClassroomService();

    // API client management
    void setApiClient(std::shared_ptr<ApiClient> client) { apiClient_ = client; }
    std::shared_ptr<ApiClient> getApiClient() const { return apiClient_; }

    // =========================================================================
    // Course API Endpoints
    // =========================================================================

    /**
     * @brief Get all available courses
     */
    std::vector<Models::Course> getCourses();

    /**
     * @brief Get courses for a specific curriculum
     */
    std::vector<Models::Course> getCoursesForCurriculum(int curriculumId);

    /**
     * @brief Get a specific course by ID
     */
    Models::Course getCourse(const std::string& courseId);

    /**
     * @brief Check if a curriculum has an associated online course
     */
    bool curriculumHasOnlineCourse(int curriculumId);

    // =========================================================================
    // Course Module API Endpoints
    // =========================================================================

    /**
     * @brief Get all modules for a course
     */
    std::vector<Models::CourseModule> getCourseModules(const std::string& courseId);

    /**
     * @brief Get a specific module by ID
     */
    Models::CourseModule getModule(const std::string& moduleId);

    /**
     * @brief Get the next module for a student (based on progress)
     */
    Models::CourseModule getNextModule(const std::string& enrollmentId);

    // =========================================================================
    // Module Content API Endpoints
    // =========================================================================

    /**
     * @brief Get all content for a module
     */
    std::vector<Models::ModuleContent> getModuleContents(const std::string& moduleId);

    /**
     * @brief Get a specific content item by ID
     */
    Models::ModuleContent getContent(const std::string& contentId);

    // =========================================================================
    // Student Enrollment API Endpoints
    // =========================================================================

    /**
     * @brief Get all enrollments for a student
     */
    std::vector<Models::StudentCourseEnrollment> getStudentEnrollments(int studentId);

    /**
     * @brief Get a specific enrollment
     */
    Models::StudentCourseEnrollment getEnrollment(const std::string& enrollmentId);

    /**
     * @brief Get student's enrollment in a specific course
     */
    Models::StudentCourseEnrollment getStudentCourseEnrollment(int studentId, int courseId);

    /**
     * @brief Enroll a student in a course
     */
    ClassroomResult enrollStudent(int studentId, int courseId);

    /**
     * @brief Update enrollment status
     */
    ClassroomResult updateEnrollment(const Models::StudentCourseEnrollment& enrollment);

    /**
     * @brief Start a student's course (marks as started)
     */
    ClassroomResult startCourse(const std::string& enrollmentId);

    /**
     * @brief Check if student is enrolled in a course
     */
    bool isStudentEnrolled(int studentId, int courseId);

    // =========================================================================
    // Student Progress API Endpoints
    // =========================================================================

    /**
     * @brief Get all module progress for an enrollment
     */
    std::vector<Models::StudentModuleProgress> getModuleProgressList(const std::string& enrollmentId);

    /**
     * @brief Get progress for a specific module
     */
    Models::StudentModuleProgress getModuleProgress(int studentId, int moduleId);

    /**
     * @brief Create or update module progress
     */
    ClassroomResult saveModuleProgress(const Models::StudentModuleProgress& progress);

    /**
     * @brief Mark a module as started
     */
    ClassroomResult startModule(int studentId, int moduleId, int enrollmentId);

    /**
     * @brief Mark a module as completed
     */
    ClassroomResult completeModule(int studentId, int moduleId);

    /**
     * @brief Get content progress for a student
     */
    Models::StudentContentProgress getContentProgress(int studentId, int contentId);

    /**
     * @brief Update content progress
     */
    ClassroomResult saveContentProgress(const Models::StudentContentProgress& progress);

    /**
     * @brief Mark content as completed
     */
    ClassroomResult completeContent(int studentId, int contentId, int moduleProgressId);

    // =========================================================================
    // Time Logging API Endpoints
    // =========================================================================

    /**
     * @brief Start a new time logging session
     */
    ClassroomResult startTimeSession(const Models::StudentTimeLog& log);

    /**
     * @brief End a time logging session
     */
    ClassroomResult endTimeSession(const std::string& logId, int durationSeconds);

    /**
     * @brief Log time for a specific activity (convenience method)
     */
    ClassroomResult logTime(int studentId, int enrollmentId, int courseId,
                            int moduleId, int contentId,
                            Models::ActivityType activity, int durationSeconds);

    /**
     * @brief Get time logs for a student enrollment
     */
    std::vector<Models::StudentTimeLog> getTimeLogs(const std::string& enrollmentId);

    /**
     * @brief Get total time spent by student in a course
     */
    int getTotalTimeSpent(int studentId, int courseId);

    /**
     * @brief Get time spent by student in a specific module
     */
    int getModuleTimeSpent(int studentId, int moduleId);

    // =========================================================================
    // Assessment API Endpoints
    // =========================================================================

    /**
     * @brief Get all assessments for a course
     */
    std::vector<Models::Assessment> getCourseAssessments(int courseId);

    /**
     * @brief Get assessment for a specific module
     */
    Models::Assessment getModuleAssessment(int moduleId);

    /**
     * @brief Get the final exam for a course
     */
    Models::Assessment getFinalExam(int courseId);

    /**
     * @brief Get a specific assessment by ID
     */
    Models::Assessment getAssessment(const std::string& assessmentId);

    /**
     * @brief Get questions for an assessment
     */
    std::vector<Models::AssessmentQuestion> getAssessmentQuestions(const std::string& assessmentId);

    // =========================================================================
    // Assessment Attempt API Endpoints
    // =========================================================================

    /**
     * @brief Start a new assessment attempt
     */
    ClassroomResult startAssessmentAttempt(int studentId, int assessmentId, int enrollmentId);

    /**
     * @brief Get current in-progress attempt for student
     */
    Models::StudentAssessmentAttempt getCurrentAttempt(int studentId, int assessmentId);

    /**
     * @brief Get all attempts for a student on an assessment
     */
    std::vector<Models::StudentAssessmentAttempt> getStudentAttempts(int studentId, int assessmentId);

    /**
     * @brief Submit an answer for a question
     */
    ClassroomResult submitAnswer(int attemptId, int questionId,
                                  const std::string& answer,
                                  const std::vector<std::string>& answers = {});

    /**
     * @brief Submit the assessment (finish attempt)
     */
    ClassroomResult submitAssessment(const std::string& attemptId);

    /**
     * @brief Get attempt details including answers
     */
    Models::StudentAssessmentAttempt getAttemptDetails(const std::string& attemptId);

    /**
     * @brief Get answers for an attempt
     */
    std::vector<Models::StudentAssessmentAnswer> getAttemptAnswers(const std::string& attemptId);

    /**
     * @brief Check if student can attempt assessment (has attempts remaining)
     */
    bool canAttemptAssessment(int studentId, int assessmentId);

    /**
     * @brief Get remaining attempts for student
     */
    int getRemainingAttempts(int studentId, int assessmentId);

    // =========================================================================
    // Assessment Report API Endpoints
    // =========================================================================

    /**
     * @brief Generate an assessment report for a completed enrollment
     */
    ClassroomResult generateReport(int studentId, int enrollmentId, int courseId);

    /**
     * @brief Get an existing report
     */
    Models::AssessmentReport getReport(const std::string& reportId);

    /**
     * @brief Get report by enrollment
     */
    Models::AssessmentReport getReportByEnrollment(const std::string& enrollmentId);

    /**
     * @brief Get all reports for a student
     */
    std::vector<Models::AssessmentReport> getStudentReports(int studentId);

    /**
     * @brief Issue a certificate for a completed course
     */
    ClassroomResult issueCertificate(const std::string& reportId);

    // =========================================================================
    // Async Operations
    // =========================================================================

    void enrollStudentAsync(int studentId, int courseId, ClassroomCallback callback);
    void submitAnswerAsync(int attemptId, int questionId,
                           const std::string& answer, ClassroomCallback callback);
    void submitAssessmentAsync(const std::string& attemptId, ClassroomCallback callback);
    void logTimeAsync(int studentId, int enrollmentId, int courseId,
                      int moduleId, int contentId,
                      Models::ActivityType activity, int durationSeconds,
                      ClassroomCallback callback);

private:
    std::shared_ptr<ApiClient> apiClient_;

    // Helper methods
    ClassroomResult parseResponse(const ApiResponse& response);
    nlohmann::json buildJsonApiPayload(const std::string& type, const nlohmann::json& attributes);
    nlohmann::json buildJsonApiPayload(const std::string& type, const std::string& id,
                                        const nlohmann::json& attributes);
};

} // namespace Api
} // namespace StudentIntake

#endif // CLASSROOM_SERVICE_H
