#include "InstructorService.h"
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <set>

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

// Helper to get today's date
static std::string getTodayDate() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%d");
    return oss.str();
}

InstructorService::InstructorService()
    : apiClient_(std::make_shared<ApiClient>()) {
}

InstructorService::InstructorService(std::shared_ptr<ApiClient> apiClient)
    : apiClient_(apiClient) {
}

InstructorService::~InstructorService() = default;

// =============================================================================
// Helper Methods
// =============================================================================

InstructorResult InstructorService::parseResponse(const ApiResponse& response) {
    InstructorResult result;
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

nlohmann::json InstructorService::buildJsonApiPayload(const std::string& type,
                                                        const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["attributes"] = attributes;
    return payload;
}

nlohmann::json InstructorService::buildJsonApiPayload(const std::string& type,
                                                        const std::string& id,
                                                        const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["id"] = id;
    payload["data"]["attributes"] = attributes;
    return payload;
}

StudentProgressSummary InstructorService::parseStudentProgressSummary(const nlohmann::json& json) {
    StudentProgressSummary summary;

    if (json.contains("student_id") && json["student_id"].is_number()) {
        summary.studentId = json["student_id"].get<int>();
    }
    if (json.contains("student_name") && json["student_name"].is_string()) {
        summary.studentName = json["student_name"].get<std::string>();
    }
    if (json.contains("email") && json["email"].is_string()) {
        summary.email = json["email"].get<std::string>();
    }
    if (json.contains("enrollment_id") && json["enrollment_id"].is_number()) {
        summary.enrollmentId = json["enrollment_id"].get<int>();
    }
    if (json.contains("course_name") && json["course_name"].is_string()) {
        summary.courseName = json["course_name"].get<std::string>();
    }
    if (json.contains("overall_progress") && json["overall_progress"].is_number()) {
        summary.overallProgress = json["overall_progress"].get<double>();
    }
    if (json.contains("hours_completed") && json["hours_completed"].is_number()) {
        summary.hoursCompleted = json["hours_completed"].get<double>();
    }
    if (json.contains("modules_completed") && json["modules_completed"].is_number()) {
        summary.modulesCompleted = json["modules_completed"].get<int>();
    }
    if (json.contains("total_modules") && json["total_modules"].is_number()) {
        summary.totalModules = json["total_modules"].get<int>();
    }
    if (json.contains("last_activity") && json["last_activity"].is_string()) {
        summary.lastActivity = json["last_activity"].get<std::string>();
    }
    if (json.contains("status") && json["status"].is_string()) {
        summary.status = json["status"].get<std::string>();
    }

    return summary;
}

CalendarEvent InstructorService::parseCalendarEvent(const nlohmann::json& json) {
    CalendarEvent event;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            event.id = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            event.id = std::to_string(json["id"].get<int>());
        }
    }
    if (json.contains("title") && json["title"].is_string()) {
        event.title = json["title"].get<std::string>();
    }
    if (json.contains("date") && json["date"].is_string()) {
        event.date = json["date"].get<std::string>();
    }
    if (json.contains("start_time") && json["start_time"].is_string()) {
        event.startTime = json["start_time"].get<std::string>();
    }
    if (json.contains("end_time") && json["end_time"].is_string()) {
        event.endTime = json["end_time"].get<std::string>();
    }
    if (json.contains("type") && json["type"].is_string()) {
        event.type = json["type"].get<std::string>();
    }
    if (json.contains("student_id") && json["student_id"].is_number()) {
        event.studentId = json["student_id"].get<int>();
    }
    if (json.contains("student_name") && json["student_name"].is_string()) {
        event.studentName = json["student_name"].get<std::string>();
    }
    if (json.contains("location") && json["location"].is_string()) {
        event.location = json["location"].get<std::string>();
    }
    if (json.contains("status") && json["status"].is_string()) {
        event.status = json["status"].get<std::string>();
    }

    return event;
}

// =============================================================================
// Instructor Profile API Endpoints
// =============================================================================

Models::Instructor InstructorService::getInstructorByUserId(int userId) {
    std::string endpoint = "/Instructor?filter[user_id]=" + std::to_string(userId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data") && !response.data["data"].empty()) {
        return Models::Instructor::fromJson(response.data["data"][0]);
    }

    return Models::Instructor();
}

Models::Instructor InstructorService::getInstructor(const std::string& instructorId) {
    auto response = apiClient_->get("/Instructor/" + instructorId);

    if (response.success && response.data.contains("data")) {
        return Models::Instructor::fromJson(response.data["data"]);
    }

    return Models::Instructor();
}

std::vector<Models::Instructor> InstructorService::getAllInstructors() {
    std::vector<Models::Instructor> instructors;
    auto response = apiClient_->get("/Instructor");

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            instructors.push_back(Models::Instructor::fromJson(item));
        }
    }

    return instructors;
}

std::vector<Models::Instructor> InstructorService::getInstructorsByType(Models::InstructorType type) {
    std::vector<Models::Instructor> instructors;
    std::string typeStr = Models::Instructor::instructorTypeToString(type);
    std::string endpoint = "/Instructor?filter[instructor_type]=" + typeStr;
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            instructors.push_back(Models::Instructor::fromJson(item));
        }
    }

    return instructors;
}

InstructorResult InstructorService::updateInstructor(const Models::Instructor& instructor) {
    auto attrs = instructor.toJson();
    auto payload = buildJsonApiPayload("Instructor", instructor.getId(), attrs);
    auto response = apiClient_->patch("/Instructor/" + instructor.getId(), payload);

    return parseResponse(response);
}

std::vector<Models::InstructorQualification> InstructorService::getInstructorQualifications(int instructorId) {
    std::vector<Models::InstructorQualification> qualifications;
    std::string endpoint = "/InstructorQualification?filter[instructor_id]=" + std::to_string(instructorId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            qualifications.push_back(Models::InstructorQualification::fromJson(item));
        }
    }

    return qualifications;
}

InstructorResult InstructorService::addQualification(const Models::InstructorQualification& qualification) {
    auto attrs = qualification.toJson();
    auto payload = buildJsonApiPayload("InstructorQualification", attrs);
    auto response = apiClient_->post("/InstructorQualification", payload);

    return parseResponse(response);
}

InstructorResult InstructorService::updateQualification(const Models::InstructorQualification& qualification) {
    auto attrs = qualification.toJson();
    auto payload = buildJsonApiPayload("InstructorQualification", qualification.getId(), attrs);
    auto response = apiClient_->patch("/InstructorQualification/" + qualification.getId(), payload);

    return parseResponse(response);
}

// =============================================================================
// Instructor Assignment API Endpoints
// =============================================================================

std::vector<Models::InstructorAssignment> InstructorService::getInstructorAssignments(int instructorId) {
    std::vector<Models::InstructorAssignment> assignments;
    std::string endpoint = "/InstructorAssignment?filter[instructor_id]=" + std::to_string(instructorId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            assignments.push_back(Models::InstructorAssignment::fromJson(item));
        }
    }

    return assignments;
}

std::vector<Models::InstructorAssignment> InstructorService::getActiveAssignments(int instructorId) {
    std::vector<Models::InstructorAssignment> assignments;
    std::string endpoint = "/InstructorAssignment?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[is_active]=true";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            assignments.push_back(Models::InstructorAssignment::fromJson(item));
        }
    }

    return assignments;
}

InstructorResult InstructorService::assignStudent(int instructorId, int studentId, int enrollmentId,
                                                    const std::string& role) {
    nlohmann::json attrs;
    attrs["instructor_id"] = instructorId;
    attrs["student_id"] = studentId;
    attrs["enrollment_id"] = enrollmentId;
    attrs["assignment_role"] = role;
    attrs["is_active"] = true;
    attrs["assigned_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("InstructorAssignment", attrs);
    auto response = apiClient_->post("/InstructorAssignment", payload);

    return parseResponse(response);
}

InstructorResult InstructorService::removeAssignment(const std::string& assignmentId) {
    auto response = apiClient_->del("/InstructorAssignment/" + assignmentId);
    return parseResponse(response);
}

InstructorResult InstructorService::updateAssignment(const Models::InstructorAssignment& assignment) {
    auto attrs = assignment.toJson();
    auto payload = buildJsonApiPayload("InstructorAssignment", assignment.getId(), attrs);
    auto response = apiClient_->patch("/InstructorAssignment/" + assignment.getId(), payload);

    return parseResponse(response);
}

// =============================================================================
// Student Progress Observation API Endpoints
// =============================================================================

std::vector<StudentProgressSummary> InstructorService::getAssignedStudentProgress(int instructorId) {
    std::vector<StudentProgressSummary> summaries;

    // Get active assignments first
    auto assignments = getActiveAssignments(instructorId);

    for (const auto& assignment : assignments) {
        StudentProgressSummary summary;
        summary.studentId = assignment.getStudentId();
        summary.enrollmentId = assignment.getEnrollmentId();

        // Get enrollment details
        std::string enrollmentEndpoint = "/StudentCourseEnrollment/" + std::to_string(assignment.getEnrollmentId());
        auto enrollmentResponse = apiClient_->get(enrollmentEndpoint);

        if (enrollmentResponse.success && enrollmentResponse.data.contains("data")) {
            auto& data = enrollmentResponse.data["data"];
            if (data.contains("attributes")) {
                auto& attrs = data["attributes"];
                if (attrs.contains("progress_percentage")) {
                    summary.overallProgress = attrs["progress_percentage"].get<double>();
                }
                if (attrs.contains("total_time_spent")) {
                    summary.hoursCompleted = attrs["total_time_spent"].get<double>() / 3600.0;
                }
            }
        }

        // Get student details
        std::string studentEndpoint = "/Student/" + std::to_string(assignment.getStudentId());
        auto studentResponse = apiClient_->get(studentEndpoint);

        if (studentResponse.success && studentResponse.data.contains("data")) {
            auto& data = studentResponse.data["data"];
            if (data.contains("attributes")) {
                auto& attrs = data["attributes"];
                if (attrs.contains("first_name") && attrs.contains("last_name")) {
                    summary.studentName = attrs["first_name"].get<std::string>() + " " +
                                         attrs["last_name"].get<std::string>();
                }
                if (attrs.contains("email")) {
                    summary.email = attrs["email"].get<std::string>();
                }
            }
        }

        summaries.push_back(summary);
    }

    return summaries;
}

StudentProgressSummary InstructorService::getStudentProgressDetail(int instructorId, int studentId) {
    StudentProgressSummary summary;
    summary.studentId = studentId;

    // Get student details
    std::string studentEndpoint = "/Student/" + std::to_string(studentId);
    auto studentResponse = apiClient_->get(studentEndpoint);

    if (studentResponse.success && studentResponse.data.contains("data")) {
        auto& data = studentResponse.data["data"];
        if (data.contains("attributes")) {
            auto& attrs = data["attributes"];
            if (attrs.contains("first_name") && attrs.contains("last_name")) {
                summary.studentName = attrs["first_name"].get<std::string>() + " " +
                                     attrs["last_name"].get<std::string>();
            }
            if (attrs.contains("email")) {
                summary.email = attrs["email"].get<std::string>();
            }
        }
    }

    // Get enrollments
    std::string enrollmentEndpoint = "/StudentCourseEnrollment?filter[student_id]=" + std::to_string(studentId);
    auto enrollmentResponse = apiClient_->get(enrollmentEndpoint);

    if (enrollmentResponse.success && enrollmentResponse.data.contains("data")
        && !enrollmentResponse.data["data"].empty()) {
        auto& enrollment = enrollmentResponse.data["data"][0];
        if (enrollment.contains("id")) {
            if (enrollment["id"].is_number()) {
                summary.enrollmentId = enrollment["id"].get<int>();
            }
        }
        if (enrollment.contains("attributes")) {
            auto& attrs = enrollment["attributes"];
            if (attrs.contains("progress_percentage")) {
                summary.overallProgress = attrs["progress_percentage"].get<double>();
            }
            if (attrs.contains("total_time_spent")) {
                summary.hoursCompleted = attrs["total_time_spent"].get<double>() / 3600.0;
            }
        }
    }

    // Get recent feedback
    summary.recentFeedback = getStudentFeedbackByInstructor(studentId, instructorId);

    return summary;
}

std::vector<Models::StudentModuleProgress> InstructorService::getStudentCourseProgress(int studentId, int courseId) {
    std::vector<Models::StudentModuleProgress> progressList;

    // First get enrollment
    std::string enrollmentEndpoint = "/StudentCourseEnrollment?filter[student_id]=" + std::to_string(studentId)
                                   + "&filter[course_id]=" + std::to_string(courseId);
    auto enrollmentResponse = apiClient_->get(enrollmentEndpoint);

    if (enrollmentResponse.success && enrollmentResponse.data.contains("data")
        && !enrollmentResponse.data["data"].empty()) {
        std::string enrollmentId;
        auto& enrollment = enrollmentResponse.data["data"][0];
        if (enrollment.contains("id")) {
            if (enrollment["id"].is_string()) {
                enrollmentId = enrollment["id"].get<std::string>();
            } else if (enrollment["id"].is_number()) {
                enrollmentId = std::to_string(enrollment["id"].get<int>());
            }
        }

        // Get module progress
        std::string progressEndpoint = "/StudentModuleProgress?filter[enrollment_id]=" + enrollmentId;
        auto progressResponse = apiClient_->get(progressEndpoint);

        if (progressResponse.success && progressResponse.data.contains("data")) {
            for (const auto& item : progressResponse.data["data"]) {
                progressList.push_back(Models::StudentModuleProgress::fromJson(item));
            }
        }
    }

    return progressList;
}

std::vector<Models::StudentSkillProgress> InstructorService::getStudentSkillProgress(int studentId, int enrollmentId) {
    std::vector<Models::StudentSkillProgress> progressList;
    std::string endpoint = "/StudentSkillProgress?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[enrollment_id]=" + std::to_string(enrollmentId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            progressList.push_back(Models::StudentSkillProgress::fromJson(item));
        }
    }

    return progressList;
}

std::vector<StudentProgressSummary> InstructorService::getStudentsNeedingAttention(int instructorId) {
    std::vector<StudentProgressSummary> needsAttention;

    auto allProgress = getAssignedStudentProgress(instructorId);

    for (const auto& summary : allProgress) {
        // Consider students needing attention if progress is low or inactive
        if (summary.overallProgress < 25.0 || summary.status == "inactive") {
            needsAttention.push_back(summary);
        }
    }

    return needsAttention;
}

// =============================================================================
// Session Scheduling API Endpoints
// =============================================================================

std::vector<Models::ScheduledSession> InstructorService::getInstructorSessions(int instructorId) {
    std::vector<Models::ScheduledSession> sessions;
    std::string endpoint = "/ScheduledSession?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&sort=scheduled_date,start_time";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            sessions.push_back(Models::ScheduledSession::fromJson(item));
        }
    }

    return sessions;
}

std::vector<Models::ScheduledSession> InstructorService::getSessionsByDateRange(int instructorId,
                                                                                   const std::string& startDate,
                                                                                   const std::string& endDate) {
    std::vector<Models::ScheduledSession> sessions;
    std::string endpoint = "/ScheduledSession?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[scheduled_date][gte]=" + startDate
                         + "&filter[scheduled_date][lte]=" + endDate
                         + "&sort=scheduled_date,start_time";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            sessions.push_back(Models::ScheduledSession::fromJson(item));
        }
    }

    return sessions;
}

std::vector<Models::ScheduledSession> InstructorService::getTodaySessions(int instructorId) {
    std::string today = getTodayDate();
    return getSessionsByDateRange(instructorId, today, today);
}

std::vector<Models::ScheduledSession> InstructorService::getUpcomingSessions(int instructorId, int limit) {
    std::vector<Models::ScheduledSession> sessions;
    std::string today = getTodayDate();
    std::string endpoint = "/ScheduledSession?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[scheduled_date][gte]=" + today
                         + "&filter[status][in]=scheduled,confirmed"
                         + "&sort=scheduled_date,start_time"
                         + "&page[limit]=" + std::to_string(limit);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            sessions.push_back(Models::ScheduledSession::fromJson(item));
        }
    }

    return sessions;
}

std::vector<Models::ScheduledSession> InstructorService::getStudentSessions(int studentId) {
    std::vector<Models::ScheduledSession> sessions;
    std::string endpoint = "/ScheduledSession?filter[student_id]=" + std::to_string(studentId)
                         + "&sort=-scheduled_date,-start_time";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            sessions.push_back(Models::ScheduledSession::fromJson(item));
        }
    }

    return sessions;
}

Models::ScheduledSession InstructorService::getSession(const std::string& sessionId) {
    auto response = apiClient_->get("/ScheduledSession/" + sessionId);

    if (response.success && response.data.contains("data")) {
        return Models::ScheduledSession::fromJson(response.data["data"]);
    }

    return Models::ScheduledSession();
}

InstructorResult InstructorService::scheduleSession(const Models::ScheduledSession& session) {
    auto attrs = session.toJson();
    attrs["created_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("ScheduledSession", attrs);
    auto response = apiClient_->post("/ScheduledSession", payload);

    return parseResponse(response);
}

InstructorResult InstructorService::updateSession(const Models::ScheduledSession& session) {
    auto attrs = session.toJson();
    auto payload = buildJsonApiPayload("ScheduledSession", session.getId(), attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + session.getId(), payload);

    return parseResponse(response);
}

InstructorResult InstructorService::cancelSession(const std::string& sessionId, const std::string& reason) {
    nlohmann::json attrs;
    attrs["status"] = "cancelled";
    attrs["cancellation_reason"] = reason;
    attrs["cancelled_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("ScheduledSession", sessionId, attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + sessionId, payload);

    return parseResponse(response);
}

InstructorResult InstructorService::rescheduleSession(const std::string& sessionId,
                                                        const std::string& newDate,
                                                        const std::string& newStartTime,
                                                        const std::string& newEndTime) {
    // Get current session to increment reschedule count
    auto currentSession = getSession(sessionId);

    nlohmann::json attrs;
    attrs["scheduled_date"] = newDate;
    attrs["start_time"] = newStartTime;
    attrs["end_time"] = newEndTime;
    attrs["status"] = "rescheduled";
    attrs["reschedule_count"] = currentSession.getRescheduleCount() + 1;

    auto payload = buildJsonApiPayload("ScheduledSession", sessionId, attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + sessionId, payload);

    return parseResponse(response);
}

InstructorResult InstructorService::startSession(const std::string& sessionId) {
    nlohmann::json attrs;
    attrs["status"] = "in_progress";
    attrs["actual_start_time"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("ScheduledSession", sessionId, attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + sessionId, payload);

    return parseResponse(response);
}

InstructorResult InstructorService::completeSession(const std::string& sessionId, const std::string& notes) {
    nlohmann::json attrs;
    attrs["status"] = "completed";
    attrs["actual_end_time"] = getCurrentTimestamp();
    attrs["completed_at"] = getCurrentTimestamp();
    if (!notes.empty()) {
        attrs["notes"] = notes;
    }

    auto payload = buildJsonApiPayload("ScheduledSession", sessionId, attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + sessionId, payload);

    return parseResponse(response);
}

InstructorResult InstructorService::recordNoShow(const std::string& sessionId, const std::string& notes) {
    nlohmann::json attrs;
    attrs["status"] = "no_show";
    if (!notes.empty()) {
        attrs["notes"] = notes;
    }

    auto payload = buildJsonApiPayload("ScheduledSession", sessionId, attrs);
    auto response = apiClient_->patch("/ScheduledSession/" + sessionId, payload);

    return parseResponse(response);
}

std::vector<CalendarEvent> InstructorService::getCalendarEvents(int instructorId,
                                                                  const std::string& startDate,
                                                                  const std::string& endDate) {
    std::vector<CalendarEvent> events;

    auto sessions = getSessionsByDateRange(instructorId, startDate, endDate);

    for (const auto& session : sessions) {
        CalendarEvent event;
        event.id = session.getId();
        event.title = session.getDisplayName();
        event.date = session.getScheduledDate();
        event.startTime = session.getStartTime();
        event.endTime = session.getEndTime();
        event.type = session.getSessionTypeString();
        event.studentId = session.getStudentId();
        event.location = session.getLocationName();
        event.status = session.getStatusString();
        events.push_back(event);
    }

    return events;
}

// =============================================================================
// Instructor Availability API Endpoints
// =============================================================================

std::vector<Models::InstructorAvailability> InstructorService::getInstructorAvailability(int instructorId) {
    std::vector<Models::InstructorAvailability> availability;
    std::string endpoint = "/InstructorAvailability?filter[instructor_id]=" + std::to_string(instructorId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            availability.push_back(Models::InstructorAvailability::fromJson(item));
        }
    }

    return availability;
}

InstructorResult InstructorService::setAvailability(const Models::InstructorAvailability& availability) {
    auto attrs = availability.toJson();

    if (availability.getId().empty()) {
        auto payload = buildJsonApiPayload("InstructorAvailability", attrs);
        auto response = apiClient_->post("/InstructorAvailability", payload);
        return parseResponse(response);
    } else {
        auto payload = buildJsonApiPayload("InstructorAvailability", availability.getId(), attrs);
        auto response = apiClient_->patch("/InstructorAvailability/" + availability.getId(), payload);
        return parseResponse(response);
    }
}

InstructorResult InstructorService::removeAvailability(const std::string& availabilityId) {
    auto response = apiClient_->del("/InstructorAvailability/" + availabilityId);
    return parseResponse(response);
}

std::vector<std::string> InstructorService::getAvailableSlots(int instructorId, const std::string& date,
                                                                int durationMinutes) {
    std::vector<std::string> slots;

    // Get availability for the day of week
    auto availability = getInstructorAvailability(instructorId);

    // Get existing sessions for the date
    auto sessions = getSessionsByDateRange(instructorId, date, date);

    // This would require more complex logic to compute available time slots
    // For now, return empty and let the UI handle availability checking

    return slots;
}

// =============================================================================
// Student Feedback API Endpoints
// =============================================================================

std::vector<Models::StudentFeedback> InstructorService::getInstructorFeedback(int instructorId) {
    std::vector<Models::StudentFeedback> feedbackList;
    std::string endpoint = "/StudentFeedback?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&sort=-created_at";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            feedbackList.push_back(Models::StudentFeedback::fromJson(item));
        }
    }

    return feedbackList;
}

std::vector<Models::StudentFeedback> InstructorService::getStudentFeedback(int studentId) {
    std::vector<Models::StudentFeedback> feedbackList;
    std::string endpoint = "/StudentFeedback?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[visible_to_student]=true"
                         + "&sort=-created_at";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            feedbackList.push_back(Models::StudentFeedback::fromJson(item));
        }
    }

    return feedbackList;
}

std::vector<Models::StudentFeedback> InstructorService::getStudentFeedbackByInstructor(int studentId, int instructorId) {
    std::vector<Models::StudentFeedback> feedbackList;
    std::string endpoint = "/StudentFeedback?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[instructor_id]=" + std::to_string(instructorId)
                         + "&sort=-created_at";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            feedbackList.push_back(Models::StudentFeedback::fromJson(item));
        }
    }

    return feedbackList;
}

std::vector<Models::StudentFeedback> InstructorService::getSessionFeedback(int sessionId) {
    std::vector<Models::StudentFeedback> feedbackList;
    std::string endpoint = "/StudentFeedback?filter[session_id]=" + std::to_string(sessionId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            feedbackList.push_back(Models::StudentFeedback::fromJson(item));
        }
    }

    return feedbackList;
}

std::vector<Models::StudentFeedback> InstructorService::getPendingFollowUps(int instructorId) {
    std::vector<Models::StudentFeedback> feedbackList;
    std::string endpoint = "/StudentFeedback?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[requires_follow_up]=true"
                         + "&filter[follow_up_completed]=false"
                         + "&sort=follow_up_date";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            feedbackList.push_back(Models::StudentFeedback::fromJson(item));
        }
    }

    return feedbackList;
}

Models::StudentFeedback InstructorService::getFeedback(const std::string& feedbackId) {
    auto response = apiClient_->get("/StudentFeedback/" + feedbackId);

    if (response.success && response.data.contains("data")) {
        return Models::StudentFeedback::fromJson(response.data["data"]);
    }

    return Models::StudentFeedback();
}

InstructorResult InstructorService::createFeedback(const Models::StudentFeedback& feedback) {
    auto attrs = feedback.toJson();
    attrs["created_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentFeedback", attrs);
    auto response = apiClient_->post("/StudentFeedback", payload);

    return parseResponse(response);
}

InstructorResult InstructorService::updateFeedback(const Models::StudentFeedback& feedback) {
    auto attrs = feedback.toJson();
    auto payload = buildJsonApiPayload("StudentFeedback", feedback.getId(), attrs);
    auto response = apiClient_->patch("/StudentFeedback/" + feedback.getId(), payload);

    return parseResponse(response);
}

InstructorResult InstructorService::deleteFeedback(const std::string& feedbackId) {
    auto response = apiClient_->del("/StudentFeedback/" + feedbackId);
    return parseResponse(response);
}

InstructorResult InstructorService::completeFollowUp(const std::string& feedbackId) {
    nlohmann::json attrs;
    attrs["follow_up_completed"] = true;
    attrs["follow_up_completed_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentFeedback", feedbackId, attrs);
    auto response = apiClient_->patch("/StudentFeedback/" + feedbackId, payload);

    return parseResponse(response);
}

InstructorResult InstructorService::acknowledgeFeedback(const std::string& feedbackId, int studentId) {
    nlohmann::json attrs;
    attrs["acknowledged_by_student"] = true;
    attrs["acknowledged_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentFeedback", feedbackId, attrs);
    auto response = apiClient_->patch("/StudentFeedback/" + feedbackId, payload);

    return parseResponse(response);
}

// =============================================================================
// Skill Validation API Endpoints
// =============================================================================

std::vector<Models::SkillCategory> InstructorService::getSkillCategories() {
    std::vector<Models::SkillCategory> categories;
    std::string endpoint = "/SkillCategory?sort=display_order";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            categories.push_back(Models::SkillCategory::fromJson(item));
        }
    }

    return categories;
}

std::vector<Models::SkillItem> InstructorService::getSkillItems(int categoryId) {
    std::vector<Models::SkillItem> items;
    std::string endpoint = "/SkillItem?filter[category_id]=" + std::to_string(categoryId)
                         + "&sort=display_order";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            items.push_back(Models::SkillItem::fromJson(item));
        }
    }

    return items;
}

std::vector<Models::SkillItem> InstructorService::getAllSkillItems() {
    std::vector<Models::SkillItem> items;
    std::string endpoint = "/SkillItem?sort=category_id,display_order";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            items.push_back(Models::SkillItem::fromJson(item));
        }
    }

    return items;
}

std::vector<Models::StudentSkillProgress> InstructorService::getSkillProgress(int studentId, int enrollmentId) {
    std::vector<Models::StudentSkillProgress> progressList;
    std::string endpoint = "/StudentSkillProgress?filter[student_id]=" + std::to_string(studentId)
                         + "&filter[enrollment_id]=" + std::to_string(enrollmentId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            progressList.push_back(Models::StudentSkillProgress::fromJson(item));
        }
    }

    return progressList;
}

std::vector<Models::SkillValidation> InstructorService::getInstructorValidations(int instructorId) {
    std::vector<Models::SkillValidation> validations;
    std::string endpoint = "/SkillValidation?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&sort=-validated_at";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            validations.push_back(Models::SkillValidation::fromJson(item));
        }
    }

    return validations;
}

std::vector<Models::SkillValidation> InstructorService::getStudentValidations(int studentId) {
    std::vector<Models::SkillValidation> validations;
    std::string endpoint = "/SkillValidation?filter[student_id]=" + std::to_string(studentId)
                         + "&sort=-validated_at";
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            validations.push_back(Models::SkillValidation::fromJson(item));
        }
    }

    return validations;
}

std::vector<Models::SkillValidation> InstructorService::getSessionValidations(int sessionId) {
    std::vector<Models::SkillValidation> validations;
    std::string endpoint = "/SkillValidation?filter[session_id]=" + std::to_string(sessionId);
    auto response = apiClient_->get(endpoint);

    if (response.success && response.data.contains("data")) {
        for (const auto& item : response.data["data"]) {
            validations.push_back(Models::SkillValidation::fromJson(item));
        }
    }

    return validations;
}

Models::SkillValidation InstructorService::getValidation(const std::string& validationId) {
    auto response = apiClient_->get("/SkillValidation/" + validationId);

    if (response.success && response.data.contains("data")) {
        return Models::SkillValidation::fromJson(response.data["data"]);
    }

    return Models::SkillValidation();
}

InstructorResult InstructorService::createValidation(const Models::SkillValidation& validation) {
    auto attrs = validation.toJson();
    if (attrs["validated_at"].get<std::string>().empty()) {
        attrs["validated_at"] = getCurrentTimestamp();
    }

    auto payload = buildJsonApiPayload("SkillValidation", attrs);
    auto response = apiClient_->post("/SkillValidation", payload);

    return parseResponse(response);
}

InstructorResult InstructorService::updateValidation(const Models::SkillValidation& validation) {
    auto attrs = validation.toJson();
    auto payload = buildJsonApiPayload("SkillValidation", validation.getId(), attrs);
    auto response = apiClient_->patch("/SkillValidation/" + validation.getId(), payload);

    return parseResponse(response);
}

InstructorResult InstructorService::deleteValidation(const std::string& validationId) {
    auto response = apiClient_->del("/SkillValidation/" + validationId);
    return parseResponse(response);
}

bool InstructorService::hasPassedAllSkills(int studentId, int enrollmentId) {
    auto allSkills = getAllSkillItems();
    auto progress = getSkillProgress(studentId, enrollmentId);

    int validatedCount = 0;
    for (const auto& p : progress) {
        if (p.isValidated()) {
            validatedCount++;
        }
    }

    // Check if all required skills are validated
    int requiredCount = 0;
    for (const auto& skill : allSkills) {
        if (skill.requiresDemonstration()) {
            requiredCount++;
        }
    }

    return validatedCount >= requiredCount;
}

std::vector<Models::SkillItem> InstructorService::getSkillsNeedingValidation(int studentId, int enrollmentId) {
    std::vector<Models::SkillItem> needsValidation;

    auto allSkills = getAllSkillItems();
    auto progress = getSkillProgress(studentId, enrollmentId);

    // Create a set of validated skill IDs
    std::set<int> validatedSkillIds;
    for (const auto& p : progress) {
        if (p.isValidated()) {
            validatedSkillIds.insert(p.getSkillItemId());
        }
    }

    // Find skills not yet validated
    for (const auto& skill : allSkills) {
        if (skill.requiresDemonstration()) {
            int skillId = std::stoi(skill.getId());
            if (validatedSkillIds.find(skillId) == validatedSkillIds.end()) {
                needsValidation.push_back(skill);
            }
        }
    }

    return needsValidation;
}

// =============================================================================
// CDL Testing API Endpoints
// =============================================================================

InstructorResult InstructorService::recordCdlTestResult(int instructorId, int studentId,
                                                          const std::string& testType,
                                                          bool passed, double score,
                                                          const std::string& notes) {
    nlohmann::json attrs;
    attrs["instructor_id"] = instructorId;
    attrs["student_id"] = studentId;
    attrs["test_type"] = testType;
    attrs["passed"] = passed;
    attrs["score"] = score;
    attrs["notes"] = notes;
    attrs["test_date"] = getTodayDate();
    attrs["tested_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("CdlTestResult", attrs);
    auto response = apiClient_->post("/CdlTestResult", payload);

    return parseResponse(response);
}

nlohmann::json InstructorService::getCdlTestResults(int studentId) {
    std::string endpoint = "/CdlTestResult?filter[student_id]=" + std::to_string(studentId)
                         + "&sort=-test_date";
    auto response = apiClient_->get(endpoint);

    if (response.success) {
        return response.data;
    }

    return nlohmann::json::object();
}

bool InstructorService::isStudentReadyForCdlTest(int studentId, int enrollmentId) {
    // Check if student has passed all required skills
    if (!hasPassedAllSkills(studentId, enrollmentId)) {
        return false;
    }

    // Check if student has completed required hours
    auto skillProgress = getSkillProgress(studentId, enrollmentId);
    double totalHours = 0.0;
    for (const auto& p : skillProgress) {
        totalHours += p.getPracticeHours();
    }

    // Minimum 40 hours typically required for CDL training
    return totalHours >= 40.0;
}

InstructorResult InstructorService::issueCdlRecommendation(int instructorId, int studentId,
                                                             const std::string& endorsements,
                                                             const std::string& notes) {
    nlohmann::json attrs;
    attrs["instructor_id"] = instructorId;
    attrs["student_id"] = studentId;
    attrs["endorsements"] = endorsements;
    attrs["notes"] = notes;
    attrs["issued_at"] = getCurrentTimestamp();
    attrs["recommendation_type"] = "cdl_ready";

    auto payload = buildJsonApiPayload("CdlRecommendation", attrs);
    auto response = apiClient_->post("/CdlRecommendation", payload);

    return parseResponse(response);
}

// =============================================================================
// Dashboard API Endpoints
// =============================================================================

InstructorDashboardStats InstructorService::getDashboardStats(int instructorId) {
    InstructorDashboardStats stats;
    stats.activeStudents = 0;
    stats.sessionsToday = 0;
    stats.sessionsThisWeek = 0;
    stats.pendingValidations = 0;
    stats.pendingFeedback = 0;
    stats.averageStudentProgress = 0.0;
    stats.studentsCompletedToday = 0;
    stats.upcomingTests = 0;

    // Get active students count
    auto assignments = getActiveAssignments(instructorId);
    stats.activeStudents = static_cast<int>(assignments.size());

    // Get today's sessions
    auto todaySessions = getTodaySessions(instructorId);
    stats.sessionsToday = static_cast<int>(todaySessions.size());

    // Get pending follow-ups as pending feedback
    auto pendingFollowUps = getPendingFollowUps(instructorId);
    stats.pendingFeedback = static_cast<int>(pendingFollowUps.size());

    // Calculate average progress
    auto progressList = getAssignedStudentProgress(instructorId);
    if (!progressList.empty()) {
        double totalProgress = 0.0;
        for (const auto& p : progressList) {
            totalProgress += p.overallProgress;
        }
        stats.averageStudentProgress = totalProgress / static_cast<double>(progressList.size());
    }

    // Get upcoming sessions for the week
    std::string today = getTodayDate();
    // Simple calculation for week end - add 7 days
    // In a real implementation, you'd use proper date arithmetic
    auto upcomingSessions = getUpcomingSessions(instructorId, 50);
    stats.sessionsThisWeek = static_cast<int>(upcomingSessions.size());

    // Count upcoming tests
    for (const auto& session : upcomingSessions) {
        if (session.getSessionType() == Models::SessionType::SkillsTest ||
            session.getSessionType() == Models::SessionType::RoadTest) {
            stats.upcomingTests++;
        }
    }

    return stats;
}

nlohmann::json InstructorService::getRecentActivity(int instructorId, int limit) {
    nlohmann::json activity = nlohmann::json::array();

    // Get recent sessions
    auto sessions = getUpcomingSessions(instructorId, limit / 2);
    for (const auto& session : sessions) {
        nlohmann::json item;
        item["type"] = "session";
        item["title"] = session.getDisplayName();
        item["date"] = session.getScheduledDate();
        item["status"] = session.getStatusString();
        activity.push_back(item);
    }

    // Get recent feedback
    auto feedback = getInstructorFeedback(instructorId);
    int feedbackCount = 0;
    for (const auto& fb : feedback) {
        if (feedbackCount >= limit / 2) break;
        nlohmann::json item;
        item["type"] = "feedback";
        item["title"] = fb.getSubject();
        item["date"] = fb.getCreatedAt();
        item["feedback_type"] = fb.getFeedbackTypeString();
        activity.push_back(item);
        feedbackCount++;
    }

    return activity;
}

nlohmann::json InstructorService::getNotifications(int instructorId) {
    std::string endpoint = "/InstructorNotification?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[read]=false"
                         + "&sort=-created_at";
    auto response = apiClient_->get(endpoint);

    if (response.success) {
        return response.data;
    }

    return nlohmann::json::array();
}

InstructorResult InstructorService::markNotificationRead(const std::string& notificationId) {
    nlohmann::json attrs;
    attrs["read"] = true;
    attrs["read_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("InstructorNotification", notificationId, attrs);
    auto response = apiClient_->patch("/InstructorNotification/" + notificationId, payload);

    return parseResponse(response);
}

// =============================================================================
// Async Operations
// =============================================================================

void InstructorService::scheduleSessionAsync(const Models::ScheduledSession& session, InstructorCallback callback) {
    auto attrs = session.toJson();
    attrs["created_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("ScheduledSession", attrs);

    apiClient_->postAsync("/ScheduledSession", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void InstructorService::createFeedbackAsync(const Models::StudentFeedback& feedback, InstructorCallback callback) {
    auto attrs = feedback.toJson();
    attrs["created_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("StudentFeedback", attrs);

    apiClient_->postAsync("/StudentFeedback", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void InstructorService::createValidationAsync(const Models::SkillValidation& validation, InstructorCallback callback) {
    auto attrs = validation.toJson();
    if (attrs["validated_at"].get<std::string>().empty()) {
        attrs["validated_at"] = getCurrentTimestamp();
    }

    auto payload = buildJsonApiPayload("SkillValidation", attrs);

    apiClient_->postAsync("/SkillValidation", payload,
        [this, callback](const ApiResponse& response) {
            callback(parseResponse(response));
        });
}

void InstructorService::getAssignedStudentProgressAsync(int instructorId,
                                                          std::function<void(const std::vector<StudentProgressSummary>&)> callback) {
    // For async, we'll fetch assignments first then get progress
    std::string endpoint = "/InstructorAssignment?filter[instructor_id]=" + std::to_string(instructorId)
                         + "&filter[is_active]=true";

    apiClient_->getAsync(endpoint,
        [this, instructorId, callback](const ApiResponse& response) {
            std::vector<StudentProgressSummary> summaries;

            if (response.success && response.data.contains("data")) {
                for (const auto& item : response.data["data"]) {
                    auto assignment = Models::InstructorAssignment::fromJson(item);
                    StudentProgressSummary summary;
                    summary.studentId = assignment.getStudentId();
                    summary.enrollmentId = assignment.getEnrollmentId();
                    summaries.push_back(summary);
                }
            }

            callback(summaries);
        });
}

void InstructorService::getDashboardStatsAsync(int instructorId,
                                                 std::function<void(const InstructorDashboardStats&)> callback) {
    // For simplicity, call sync version in async wrapper
    // In production, this would be properly async
    auto stats = getDashboardStats(instructorId);
    callback(stats);
}

} // namespace Api
} // namespace StudentIntake
