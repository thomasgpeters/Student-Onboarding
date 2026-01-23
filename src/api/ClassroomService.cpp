#include "ClassroomService.h"
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Api {

// Helper to get current timestamp in ISO format
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

ClassroomService::ClassroomService()
    : apiClient_(std::make_shared<ApiClient>()) {
}

ClassroomService::ClassroomService(std::shared_ptr<ApiClient> apiClient)
    : apiClient_(apiClient) {
}

ClassroomService::~ClassroomService() = default;

// =============================================================================
// Helper Methods
// =============================================================================

ClassroomResult ClassroomService::parseResponse(const ApiResponse& response) {
    ClassroomResult result;
    result.success = response.success;
    result.message = response.errorMessage;
    result.responseData = response.data;

    if (response.success && response.data.contains("data")) {
        auto& data = response.data["data"];
        if (data.contains("id")) {
            if (data["id"].is_string()) {
                result.id = data["id"].get<std::string>();
            } else if (data["id"].is_number()) {
                result.id = std::to_string(data["id"].get<int>());
            }
        }
    }

    if (!response.success) {
        result.errors.push_back(response.errorMessage);
    }

    return result;
}

nlohmann::json ClassroomService::buildJsonApiPayload(const std::string& type,
                                                       const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["attributes"] = attributes;
    return payload;
}

nlohmann::json ClassroomService::buildJsonApiPayload(const std::string& type,
                                                       const std::string& id,
                                                       const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["id"] = id;
    payload["data"]["attributes"] = attributes;
    return payload;
}

// =============================================================================
// Course API Endpoints
// =============================================================================

std::vector<Models::Course> ClassroomService::getCourses() {
    std::vector<Models::Course> courses;
    auto response = apiClient_->get("/Course");

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            courses.push_back(Models::Course::fromJson(item));
        }
    }

    return courses;
}

std::vector<Models::Course> ClassroomService::getCoursesForCurriculum(int curriculumId) {
    std::vector<Models::Course> courses;
    std::string endpoint = "/Course?filter[curriculum_id]=" + std::to_string(curriculumId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            courses.push_back(Models::Course::fromJson(item));
        }
    }

    return courses;
}

Models::Course ClassroomService::getCourse(const std::string& courseId) {
    auto response = apiClient_->get("/Course/" + courseId);

    if (response.success && response.data.contains("data")) {
        return Models::Course::fromJson(response.data["data"]);
    }

    return Models::Course();
}

bool ClassroomService::curriculumHasOnlineCourse(int curriculumId) {
    auto courses = getCoursesForCurriculum(curriculumId);
    return !courses.empty();
}

// =============================================================================
// Course Module API Endpoints
// =============================================================================

std::vector<Models::CourseModule> ClassroomService::getCourseModules(const std::string& courseId) {
    std::vector<Models::CourseModule> modules;
    std::string endpoint = "/CourseModule?filter[course_id]=" + courseId + "&sort=module_number";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            modules.push_back(Models::CourseModule::fromJson(item));
        }
    }

    return modules;
}

Models::CourseModule ClassroomService::getModule(const std::string& moduleId) {
    auto response = apiClient_->get("/CourseModule/" + moduleId);

    if (response.success && response.data.contains("data")) {
        return Models::CourseModule::fromJson(response.data["data"]);
    }

    return Models::CourseModule();
}

Models::CourseModule ClassroomService::getNextModule(const std::string& enrollmentId) {
    // Get enrollment to find current module
    auto enrollment = getEnrollment(enrollmentId);
    int currentModuleId = enrollment.getCurrentModuleId();

    // Get all modules for the course
    auto modules = getCourseModules(std::to_string(enrollment.getCourseId()));

    // Find next module
    for (size_t i = 0; i < modules.size(); ++i) {
        if (std::stoi(modules[i].getId()) == currentModuleId && i + 1 < modules.size()) {
            return modules[i + 1];
        }
    }

    // If no current module, return first module
    if (currentModuleId == 0 && !modules.empty()) {
        return modules[0];
    }

    return Models::CourseModule();
}

// =============================================================================
// Module Content API Endpoints
// =============================================================================

std::vector<Models::ModuleContent> ClassroomService::getModuleContents(const std::string& moduleId) {
    std::vector<Models::ModuleContent> contents;
    std::string endpoint = "/ModuleContent?filter[module_id]=" + moduleId + "&sort=content_order";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            contents.push_back(Models::ModuleContent::fromJson(item));
        }
    }

    return contents;
}

Models::ModuleContent ClassroomService::getContent(const std::string& contentId) {
    auto response = apiClient_->get("/ModuleContent/" + contentId);

    if (response.success && response.data.contains("data")) {
        return Models::ModuleContent::fromJson(response.data["data"]);
    }

    return Models::ModuleContent();
}

// =============================================================================
// Student Enrollment API Endpoints
// =============================================================================

std::vector<Models::StudentCourseEnrollment> ClassroomService::getStudentEnrollments(int studentId) {
    std::vector<Models::StudentCourseEnrollment> enrollments;
    std::string endpoint = "/StudentCourseEnrollment?filter[student_id]=" + std::to_string(studentId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            enrollments.push_back(Models::StudentCourseEnrollment::fromJson(item));
        }
    }

    return enrollments;
}

Models::StudentCourseEnrollment ClassroomService::getEnrollment(const std::string& enrollmentId) {
    auto response = apiClient_->get("/StudentCourseEnrollment/" + enrollmentId);

    if (response.success && response.data.contains("data")) {
        return Models::StudentCourseEnrollment::fromJson(response.data["data"]);
    }

    return Models::StudentCourseEnrollment();
}

Models::StudentCourseEnrollment ClassroomService::getStudentCourseEnrollment(int studentId, int courseId) {
    std::string endpoint = "/StudentCourseEnrollment?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[course_id]=" + std::to_string(courseId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::StudentCourseEnrollment::fromJson(response.data["data"][0]);
    }

    return Models::StudentCourseEnrollment();
}

ClassroomResult ClassroomService::enrollStudent(int studentId, int courseId) {
    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["course_id"] = courseId;
    attrs["enrollment_status"] = "active";
    attrs["completion_status"] = "not_started";
    attrs["progress_percentage"] = 0;
    attrs["total_time_spent"] = 0;
    attrs["enrolled_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentCourseEnrollment", attrs);
    auto response = apiClient_->post("/StudentCourseEnrollment", payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::updateEnrollment(const Models::StudentCourseEnrollment& enrollment) {
    auto attrs = enrollment.toJson();
    auto payload = buildJsonApiPayload("StudentCourseEnrollment", enrollment.getId(), attrs);
    auto response = apiClient_->patch("/StudentCourseEnrollment/" + enrollment.getId(), payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::startCourse(const std::string& enrollmentId) {
    nlohmann::json attrs;
    attrs["completion_status"] = "in_progress";
    attrs["started_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentCourseEnrollment", enrollmentId, attrs);
    auto response = apiClient_->patch("/StudentCourseEnrollment/" + enrollmentId, payload);

    return parseResponse(response);
}

bool ClassroomService::isStudentEnrolled(int studentId, int courseId) {
    auto enrollment = getStudentCourseEnrollment(studentId, courseId);
    return !enrollment.getId().empty() && enrollment.isActive();
}

// =============================================================================
// Student Progress API Endpoints
// =============================================================================

std::vector<Models::StudentModuleProgress> ClassroomService::getModuleProgressList(const std::string& enrollmentId) {
    std::vector<Models::StudentModuleProgress> progressList;
    std::string endpoint = "/StudentModuleProgress?filter[enrollment_id]=" + enrollmentId;
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            progressList.push_back(Models::StudentModuleProgress::fromJson(item));
        }
    }

    return progressList;
}

Models::StudentModuleProgress ClassroomService::getModuleProgress(int studentId, int moduleId) {
    std::string endpoint = "/StudentModuleProgress?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[module_id]=" + std::to_string(moduleId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::StudentModuleProgress::fromJson(response.data["data"][0]);
    }

    return Models::StudentModuleProgress();
}

ClassroomResult ClassroomService::saveModuleProgress(const Models::StudentModuleProgress& progress) {
    auto attrs = progress.toJson();

    if (progress.getId().empty()) {
        // Create new
        auto payload = buildJsonApiPayload("StudentModuleProgress", attrs);
        auto response = apiClient_->post("/StudentModuleProgress", payload);
        return parseResponse(response);
    } else {
        // Update existing
        auto payload = buildJsonApiPayload("StudentModuleProgress", progress.getId(), attrs);
        auto response = apiClient_->patch("/StudentModuleProgress/" + progress.getId(), payload);
        return parseResponse(response);
    }
}

ClassroomResult ClassroomService::startModule(int studentId, int moduleId, int enrollmentId) {
    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["module_id"] = moduleId;
    attrs["enrollment_id"] = enrollmentId;
    attrs["status"] = "in_progress";
    attrs["progress_percentage"] = 0;
    attrs["time_spent"] = 0;
    attrs["completed_content_ids"] = nlohmann::json::array();
    attrs["attempts"] = 0;
    attrs["started_at"] = getCurrentTimestamp();
    attrs["last_accessed_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentModuleProgress", attrs);
    auto response = apiClient_->post("/StudentModuleProgress", payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::completeModule(int studentId, int moduleId) {
    auto progress = getModuleProgress(studentId, moduleId);
    if (progress.getId().empty()) {
        ClassroomResult result;
        result.success = false;
        result.message = "Module progress not found";
        return result;
    }

    nlohmann::json attrs;
    attrs["status"] = "completed";
    attrs["progress_percentage"] = 100;
    attrs["completed_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentModuleProgress", progress.getId(), attrs);
    auto response = apiClient_->patch("/StudentModuleProgress/" + progress.getId(), payload);

    return parseResponse(response);
}

Models::StudentContentProgress ClassroomService::getContentProgress(int studentId, int contentId) {
    std::string endpoint = "/StudentContentProgress?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[content_id]=" + std::to_string(contentId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::StudentContentProgress::fromJson(response.data["data"][0]);
    }

    return Models::StudentContentProgress();
}

ClassroomResult ClassroomService::saveContentProgress(const Models::StudentContentProgress& progress) {
    auto attrs = progress.toJson();

    if (progress.getId().empty()) {
        auto payload = buildJsonApiPayload("StudentContentProgress", attrs);
        auto response = apiClient_->post("/StudentContentProgress", payload);
        return parseResponse(response);
    } else {
        auto payload = buildJsonApiPayload("StudentContentProgress", progress.getId(), attrs);
        auto response = apiClient_->patch("/StudentContentProgress/" + progress.getId(), payload);
        return parseResponse(response);
    }
}

ClassroomResult ClassroomService::completeContent(int studentId, int contentId, int moduleProgressId) {
    auto progress = getContentProgress(studentId, contentId);

    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["content_id"] = contentId;
    attrs["module_progress_id"] = moduleProgressId;
    attrs["status"] = "completed";
    attrs["completed"] = true;
    attrs["completed_at"] = getCurrentTimestamp();

    if (progress.getId().empty()) {
        // Create new completed progress
        auto payload = buildJsonApiPayload("StudentContentProgress", attrs);
        auto response = apiClient_->post("/StudentContentProgress", payload);
        return parseResponse(response);
    } else {
        // Update existing
        auto payload = buildJsonApiPayload("StudentContentProgress", progress.getId(), attrs);
        auto response = apiClient_->patch("/StudentContentProgress/" + progress.getId(), payload);
        return parseResponse(response);
    }
}

// =============================================================================
// Time Logging API Endpoints
// =============================================================================

ClassroomResult ClassroomService::startTimeSession(const Models::StudentTimeLog& log) {
    auto attrs = log.toJson();
    auto payload = buildJsonApiPayload("StudentTimeLog", attrs);
    auto response = apiClient_->post("/StudentTimeLog", payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::endTimeSession(const std::string& logId, int durationSeconds) {
    nlohmann::json attrs;
    attrs["session_end"] = getCurrentTimestamp();
    attrs["duration_seconds"] = durationSeconds;
    attrs["is_validated"] = true;
    attrs["validation_method"] = "automatic";

    auto payload = buildJsonApiPayload("StudentTimeLog", logId, attrs);
    auto response = apiClient_->patch("/StudentTimeLog/" + logId, payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::logTime(int studentId, int enrollmentId, int courseId,
                                           int moduleId, int contentId,
                                           Models::ActivityType activity, int durationSeconds) {
    Models::StudentTimeLog log;
    log.setStudentId(studentId);
    log.setEnrollmentId(enrollmentId);
    log.setCourseId(courseId);
    log.setModuleId(moduleId);
    log.setContentId(contentId);
    log.setActivityType(activity);
    log.setSessionStart(getCurrentTimestamp());
    log.setSessionEnd(getCurrentTimestamp());
    log.setDurationSeconds(durationSeconds);
    log.setValidated(true);
    log.setValidationMethod("automatic");

    auto attrs = log.toJson();
    auto payload = buildJsonApiPayload("StudentTimeLog", attrs);
    auto response = apiClient_->post("/StudentTimeLog", payload);

    return parseResponse(response);
}

std::vector<Models::StudentTimeLog> ClassroomService::getTimeLogs(const std::string& enrollmentId) {
    std::vector<Models::StudentTimeLog> logs;
    std::string endpoint = "/StudentTimeLog?filter[enrollment_id]=" + enrollmentId + "&sort=-session_start";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            logs.push_back(Models::StudentTimeLog::fromJson(item));
        }
    }

    return logs;
}

int ClassroomService::getTotalTimeSpent(int studentId, int courseId) {
    std::string endpoint = "/StudentTimeLog?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[course_id]=" + std::to_string(courseId);
    auto response = apiClient_->get(endpoint);

    int totalSeconds = 0;
    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            auto log = Models::StudentTimeLog::fromJson(item);
            totalSeconds += log.getDurationSeconds();
        }
    }

    return totalSeconds;
}

int ClassroomService::getModuleTimeSpent(int studentId, int moduleId) {
    std::string endpoint = "/StudentTimeLog?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[module_id]=" + std::to_string(moduleId);
    auto response = apiClient_->get(endpoint);

    int totalSeconds = 0;
    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            auto log = Models::StudentTimeLog::fromJson(item);
            totalSeconds += log.getDurationSeconds();
        }
    }

    return totalSeconds;
}

// =============================================================================
// Assessment API Endpoints
// =============================================================================

std::vector<Models::Assessment> ClassroomService::getCourseAssessments(int courseId) {
    std::vector<Models::Assessment> assessments;
    std::string endpoint = "/Assessment?filter[course_id]=" + std::to_string(courseId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            assessments.push_back(Models::Assessment::fromJson(item));
        }
    }

    return assessments;
}

Models::Assessment ClassroomService::getModuleAssessment(int moduleId) {
    std::string endpoint = "/Assessment?filter[module_id]=" + std::to_string(moduleId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::Assessment::fromJson(response.data["data"][0]);
    }

    return Models::Assessment();
}

Models::Assessment ClassroomService::getFinalExam(int courseId) {
    std::string endpoint = "/Assessment?filter[course_id]=" + std::to_string(courseId)
                         + "&filter[assessment_type]=final_exam";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::Assessment::fromJson(response.data["data"][0]);
    }

    return Models::Assessment();
}

Models::Assessment ClassroomService::getAssessment(const std::string& assessmentId) {
    auto response = apiClient_->get("/Assessment/" + assessmentId);

    if (response.success && response.data.contains("data")) {
        return Models::Assessment::fromJson(response.data["data"]);
    }

    return Models::Assessment();
}

std::vector<Models::AssessmentQuestion> ClassroomService::getAssessmentQuestions(const std::string& assessmentId) {
    std::vector<Models::AssessmentQuestion> questions;
    std::string endpoint = "/AssessmentQuestion?filter[assessment_id]=" + assessmentId + "&sort=question_order";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            questions.push_back(Models::AssessmentQuestion::fromJson(item));
        }
    }

    return questions;
}

// =============================================================================
// Assessment Attempt API Endpoints
// =============================================================================

ClassroomResult ClassroomService::startAssessmentAttempt(int studentId, int assessmentId, int enrollmentId) {
    // Get attempt count
    auto attempts = getStudentAttempts(studentId, assessmentId);
    int attemptNumber = static_cast<int>(attempts.size()) + 1;

    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["assessment_id"] = assessmentId;
    attrs["enrollment_id"] = enrollmentId;
    attrs["attempt_number"] = attemptNumber;
    attrs["status"] = "in_progress";
    attrs["started_at"] = getCurrentTimestamp();
    attrs["time_spent_seconds"] = 0;
    attrs["total_questions"] = 0;
    attrs["correct_answers"] = 0;
    attrs["score"] = 0;
    attrs["passed"] = false;

    auto payload = buildJsonApiPayload("StudentAssessmentAttempt", attrs);
    auto response = apiClient_->post("/StudentAssessmentAttempt", payload);

    return parseResponse(response);
}

Models::StudentAssessmentAttempt ClassroomService::getCurrentAttempt(int studentId, int assessmentId) {
    std::string endpoint = "/StudentAssessmentAttempt?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[assessment_id]=" + std::to_string(assessmentId)
                         + "&filter[status]=in_progress";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::StudentAssessmentAttempt::fromJson(response.data["data"][0]);
    }

    return Models::StudentAssessmentAttempt();
}

std::vector<Models::StudentAssessmentAttempt> ClassroomService::getStudentAttempts(int studentId, int assessmentId) {
    std::vector<Models::StudentAssessmentAttempt> attempts;
    std::string endpoint = "/StudentAssessmentAttempt?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[assessment_id]=" + std::to_string(assessmentId)
                         + "&sort=attempt_number";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            attempts.push_back(Models::StudentAssessmentAttempt::fromJson(item));
        }
    }

    return attempts;
}

ClassroomResult ClassroomService::submitAnswer(int attemptId, int questionId,
                                                const std::string& answer,
                                                const std::vector<std::string>& answers) {
    nlohmann::json attrs;
    attrs["attempt_id"] = attemptId;
    attrs["question_id"] = questionId;
    attrs["answer_given"] = answer;
    attrs["answers_given"] = answers;
    attrs["answered_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentAssessmentAnswer", attrs);
    auto response = apiClient_->post("/StudentAssessmentAnswer", payload);

    return parseResponse(response);
}

ClassroomResult ClassroomService::submitAssessment(const std::string& attemptId) {
    // Get the attempt
    auto attempt = getAttemptDetails(attemptId);
    if (attempt.getId().empty()) {
        ClassroomResult result;
        result.success = false;
        result.message = "Attempt not found";
        return result;
    }

    // Get answers and calculate score
    auto answers = getAttemptAnswers(attemptId);

    // Get assessment to check passing score
    auto assessment = getAssessment(std::to_string(attempt.getAssessmentId()));

    int totalQuestions = static_cast<int>(answers.size());
    int correctAnswers = 0;
    for (const auto& ans : answers) {
        if (ans.isCorrect()) {
            correctAnswers++;
        }
    }

    double score = totalQuestions > 0 ? (static_cast<double>(correctAnswers) / totalQuestions) * 100.0 : 0.0;
    bool passed = score >= assessment.getPassingScore();

    nlohmann::json attrs;
    attrs["status"] = "graded";
    attrs["submitted_at"] = getCurrentTimestamp();
    attrs["total_questions"] = totalQuestions;
    attrs["correct_answers"] = correctAnswers;
    attrs["score"] = score;
    attrs["passed"] = passed;

    auto payload = buildJsonApiPayload("StudentAssessmentAttempt", attemptId, attrs);
    auto response = apiClient_->patch("/StudentAssessmentAttempt/" + attemptId, payload);

    return parseResponse(response);
}

Models::StudentAssessmentAttempt ClassroomService::getAttemptDetails(const std::string& attemptId) {
    auto response = apiClient_->get("/StudentAssessmentAttempt/" + attemptId);

    if (response.success && response.data.contains("data")) {
        return Models::StudentAssessmentAttempt::fromJson(response.data["data"]);
    }

    return Models::StudentAssessmentAttempt();
}

std::vector<Models::StudentAssessmentAnswer> ClassroomService::getAttemptAnswers(const std::string& attemptId) {
    std::vector<Models::StudentAssessmentAnswer> answers;
    std::string endpoint = "/StudentAssessmentAnswer?filter[attempt_id]=" + attemptId;
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            answers.push_back(Models::StudentAssessmentAnswer::fromJson(item));
        }
    }

    return answers;
}

bool ClassroomService::canAttemptAssessment(int studentId, int assessmentId) {
    return getRemainingAttempts(studentId, assessmentId) > 0;
}

int ClassroomService::getRemainingAttempts(int studentId, int assessmentId) {
    auto assessment = getAssessment(std::to_string(assessmentId));
    auto attempts = getStudentAttempts(studentId, assessmentId);

    int maxAttempts = assessment.getMaxAttempts();
    int usedAttempts = static_cast<int>(attempts.size());

    return std::max(0, maxAttempts - usedAttempts);
}

// =============================================================================
// Assessment Report API Endpoints
// =============================================================================

ClassroomResult ClassroomService::generateReport(int studentId, int enrollmentId, int courseId) {
    // This would typically be a server-side operation that compiles all the data
    // For now, we create a report record that the backend can populate

    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["enrollment_id"] = enrollmentId;
    attrs["course_id"] = courseId;
    attrs["report_type"] = "completion";
    attrs["generated_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("AssessmentReport", attrs);
    auto response = apiClient_->post("/AssessmentReport", payload);

    return parseResponse(response);
}

Models::AssessmentReport ClassroomService::getReport(const std::string& reportId) {
    auto response = apiClient_->get("/AssessmentReport/" + reportId);

    if (response.success && response.data.contains("data")) {
        return Models::AssessmentReport::fromJson(response.data["data"]);
    }

    return Models::AssessmentReport();
}

Models::AssessmentReport ClassroomService::getReportByEnrollment(const std::string& enrollmentId) {
    std::string endpoint = "/AssessmentReport?filter[enrollment_id]=" + enrollmentId;
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::AssessmentReport::fromJson(response.data["data"][0]);
    }

    return Models::AssessmentReport();
}

std::vector<Models::AssessmentReport> ClassroomService::getStudentReports(int studentId) {
    std::vector<Models::AssessmentReport> reports;
    std::string endpoint = "/AssessmentReport?filter[student_id]=" + std::to_string(studentId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            reports.push_back(Models::AssessmentReport::fromJson(item));
        }
    }

    return reports;
}

ClassroomResult ClassroomService::issueCertificate(const std::string& reportId) {
    // Generate a certificate number
    std::string certNumber = "CERT-" + reportId + "-" + getCurrentTimestamp().substr(0, 10);

    nlohmann::json attrs;
    attrs["certificate_issued"] = true;
    attrs["certificate_number"] = certNumber;
    attrs["certificate_issued_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("AssessmentReport", reportId, attrs);
    auto response = apiClient_->patch("/AssessmentReport/" + reportId, payload);

    return parseResponse(response);
}

// =============================================================================
// Async Operations
// =============================================================================

void ClassroomService::enrollStudentAsync(int studentId, int courseId, ClassroomCallback callback) {
    nlohmann::json attrs;
    attrs["student_id"] = studentId;
    attrs["course_id"] = courseId;
    attrs["enrollment_status"] = "active";
    attrs["completion_status"] = "not_started";
    attrs["progress_percentage"] = 0;
    attrs["total_time_spent"] = 0;
    attrs["enrolled_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentCourseEnrollment", attrs);

    apiClient_->postAsync("/StudentCourseEnrollment", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void ClassroomService::submitAnswerAsync(int attemptId, int questionId,
                                          const std::string& answer, ClassroomCallback callback) {
    nlohmann::json attrs;
    attrs["attempt_id"] = attemptId;
    attrs["question_id"] = questionId;
    attrs["answer_given"] = answer;
    attrs["answered_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentAssessmentAnswer", attrs);

    apiClient_->postAsync("/StudentAssessmentAnswer", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void ClassroomService::submitAssessmentAsync(const std::string& attemptId, ClassroomCallback callback) {
    // For async, we'll just mark as submitted and let backend grade
    nlohmann::json attrs;
    attrs["status"] = "submitted";
    attrs["submitted_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentAssessmentAttempt", attemptId, attrs);

    apiClient_->patchAsync("/StudentAssessmentAttempt/" + attemptId, payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void ClassroomService::logTimeAsync(int studentId, int enrollmentId, int courseId,
                                     int moduleId, int contentId,
                                     Models::ActivityType activity, int durationSeconds,
                                     ClassroomCallback callback) {
    Models::StudentTimeLog log;
    log.setStudentId(studentId);
    log.setEnrollmentId(enrollmentId);
    log.setCourseId(courseId);
    log.setModuleId(moduleId);
    log.setContentId(contentId);
    log.setActivityType(activity);
    log.setSessionStart(getCurrentTimestamp());
    log.setSessionEnd(getCurrentTimestamp());
    log.setDurationSeconds(durationSeconds);
    log.setValidated(true);
    log.setValidationMethod("automatic");

    auto attrs = log.toJson();
    auto payload = buildJsonApiPayload("StudentTimeLog", attrs);

    apiClient_->postAsync("/StudentTimeLog", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

} // namespace Api
} // namespace StudentIntake
