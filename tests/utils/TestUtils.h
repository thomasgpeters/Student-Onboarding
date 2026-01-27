#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <nlohmann/json.hpp>
#include "models/Student.h"
#include "models/EmergencyContact.h"
#include "models/User.h"
#include "models/AcademicHistory.h"
#include "models/Curriculum.h"
#include "models/ActivityLog.h"

namespace TestUtils {

/**
 * @brief Factory class for creating test fixtures
 */
class TestFixtures {
public:
    // Student fixtures
    static StudentIntake::Models::Student createBasicStudent();
    static StudentIntake::Models::Student createInternationalStudent();
    static StudentIntake::Models::Student createVeteranStudent();
    static StudentIntake::Models::Student createTransferStudent();
    static StudentIntake::Models::Student createStudentWithEndorsements();

    // EmergencyContact fixtures
    static StudentIntake::Models::EmergencyContact createPrimaryContact(const std::string& studentId);
    static StudentIntake::Models::EmergencyContact createSecondaryContact(const std::string& studentId);
    static StudentIntake::Models::EmergencyContact createEmptyContact();

    // User fixtures
    static StudentIntake::Models::User createStudentUser();
    static StudentIntake::Models::User createInstructorUser();
    static StudentIntake::Models::User createAdminUser();
    static StudentIntake::Models::User createMultiRoleUser();

    // AcademicHistory fixtures
    static StudentIntake::Models::AcademicHistory createHighSchoolRecord(const std::string& studentId);
    static StudentIntake::Models::AcademicHistory createUndergraduateRecord(const std::string& studentId);
    static StudentIntake::Models::AcademicHistory createVocationalRecord(const std::string& studentId);

    // Curriculum fixtures
    static StudentIntake::Models::Curriculum createAccreditedProgram();
    static StudentIntake::Models::Curriculum createVocationalProgram();
    static StudentIntake::Models::Curriculum createEndorsementProgram();

    // AcademicHistory additional helper for testing
    static StudentIntake::Models::AcademicHistory createGraduateRecord(const std::string& studentId);

    // ActivityLog fixtures
    static StudentIntake::Models::ActivityLog createLoginActivity();
    static StudentIntake::Models::ActivityLog createFormSubmissionActivity();
    static StudentIntake::Models::ActivityLog createErrorActivity();

    // JSON fixtures
    static nlohmann::json createStudentJson();
    static nlohmann::json createStudentJsonWithAttributes();
    static nlohmann::json createEmergencyContactJson();
    static nlohmann::json createUserJson();
};

/**
 * @brief Helper functions for testing JSON serialization
 */
class JsonTestHelper {
public:
    // Check if JSON contains all expected keys
    static bool hasAllKeys(const nlohmann::json& json, const std::vector<std::string>& keys);

    // Compare two JSON objects, ignoring order
    static bool jsonEquals(const nlohmann::json& a, const nlohmann::json& b);

    // Pretty print JSON for debugging
    static std::string prettyPrint(const nlohmann::json& json);
};

/**
 * @brief Helper for date/time testing
 */
class DateTimeHelper {
public:
    // Create a time_point from a date string (YYYY-MM-DD)
    static std::chrono::system_clock::time_point parseDate(const std::string& dateStr);

    // Format a time_point as a date string (YYYY-MM-DD)
    static std::string formatDate(const std::chrono::system_clock::time_point& tp);

    // Calculate age from date of birth
    static int calculateAge(const std::chrono::system_clock::time_point& dob);
};

} // namespace TestUtils

#endif // TEST_UTILS_H
