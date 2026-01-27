#include "utils/TestUtils.h"
#include <iomanip>
#include <sstream>

namespace TestUtils {

// =============================================================================
// Student Fixtures
// =============================================================================

StudentIntake::Models::Student TestFixtures::createBasicStudent() {
    StudentIntake::Models::Student student("1", "john.doe@example.com");
    student.setFirstName("John");
    student.setLastName("Doe");
    student.setMiddleName("Michael");
    student.setPreferredName("Johnny");
    student.setGender("Male");
    student.setPreferredPronouns("he/him");
    student.setPhoneNumber("555-123-4567");
    student.setAlternatePhone("555-987-6543");
    student.setAddressLine1("123 Main Street");
    student.setAddressLine2("Apt 4B");
    student.setCity("Springfield");
    student.setState("IL");
    student.setZipCode("62701");
    student.setStudentType("undergraduate");
    student.setStatus("active");
    student.setCurriculumId("1");
    student.setCitizenshipStatus("US Citizen");
    student.setAppUserId(100);
    return student;
}

StudentIntake::Models::Student TestFixtures::createInternationalStudent() {
    StudentIntake::Models::Student student = createBasicStudent();
    student.setId("2");
    student.setEmail("maria.garcia@example.com");
    student.setFirstName("Maria");
    student.setLastName("Garcia");
    student.setInternational(true);
    student.setCitizenshipStatus("F-1 Visa");
    return student;
}

StudentIntake::Models::Student TestFixtures::createVeteranStudent() {
    StudentIntake::Models::Student student = createBasicStudent();
    student.setId("3");
    student.setEmail("james.miller@example.com");
    student.setFirstName("James");
    student.setLastName("Miller");
    student.setVeteran(true);
    student.setRequiresFinancialAid(true);
    return student;
}

StudentIntake::Models::Student TestFixtures::createTransferStudent() {
    StudentIntake::Models::Student student = createBasicStudent();
    student.setId("4");
    student.setEmail("sarah.johnson@example.com");
    student.setFirstName("Sarah");
    student.setLastName("Johnson");
    student.setTransferStudent(true);
    return student;
}

StudentIntake::Models::Student TestFixtures::createStudentWithEndorsements() {
    StudentIntake::Models::Student student = createBasicStudent();
    student.setId("5");
    student.setStudentType("certificate");
    student.setCurriculumId("10");
    student.addEndorsementId("20");
    student.addEndorsementId("21");
    student.addEndorsementId("22");
    return student;
}

// =============================================================================
// EmergencyContact Fixtures
// =============================================================================

StudentIntake::Models::EmergencyContact TestFixtures::createPrimaryContact(const std::string& studentId) {
    StudentIntake::Models::EmergencyContact contact(studentId);
    contact.setFirstName("Jane");
    contact.setLastName("Doe");
    contact.setRelationship("Spouse");
    contact.setPhone("555-111-2222");
    contact.setAlternatePhone("555-333-4444");
    contact.setEmail("jane.doe@example.com");
    contact.setStreet1("123 Main Street");
    contact.setStreet2("Apt 4B");
    contact.setCity("Springfield");
    contact.setState("IL");
    contact.setPostalCode("62701");
    contact.setCountry("United States");
    contact.setPrimary(true);
    contact.setPriority(1);
    return contact;
}

StudentIntake::Models::EmergencyContact TestFixtures::createSecondaryContact(const std::string& studentId) {
    StudentIntake::Models::EmergencyContact contact(studentId);
    contact.setFirstName("Robert");
    contact.setLastName("Doe");
    contact.setRelationship("Parent");
    contact.setPhone("555-555-6666");
    contact.setEmail("robert.doe@example.com");
    contact.setStreet1("456 Oak Avenue");
    contact.setCity("Chicago");
    contact.setState("IL");
    contact.setPostalCode("60601");
    contact.setCountry("United States");
    contact.setPrimary(false);
    contact.setPriority(2);
    return contact;
}

StudentIntake::Models::EmergencyContact TestFixtures::createEmptyContact() {
    return StudentIntake::Models::EmergencyContact();
}

// =============================================================================
// User Fixtures
// =============================================================================

StudentIntake::Models::User TestFixtures::createStudentUser() {
    StudentIntake::Models::User user;
    user.setId(1);
    user.setEmail("student@example.com");
    user.setPasswordHash("hashed_password_123");
    user.setFirstName("Test");
    user.setLastName("Student");
    user.setPhoneNumber("555-100-1000");
    user.setActive(true);
    user.setLoginEnabled(true);
    user.setEmailVerified(true);
    user.addRole(StudentIntake::Models::UserRole::Student);
    return user;
}

StudentIntake::Models::User TestFixtures::createInstructorUser() {
    StudentIntake::Models::User user;
    user.setId(2);
    user.setEmail("instructor@example.com");
    user.setPasswordHash("hashed_password_456");
    user.setFirstName("Test");
    user.setLastName("Instructor");
    user.setPhoneNumber("555-200-2000");
    user.setActive(true);
    user.setLoginEnabled(true);
    user.setEmailVerified(true);
    user.addRole(StudentIntake::Models::UserRole::Instructor);
    return user;
}

StudentIntake::Models::User TestFixtures::createAdminUser() {
    StudentIntake::Models::User user;
    user.setId(3);
    user.setEmail("admin@example.com");
    user.setPasswordHash("hashed_password_789");
    user.setFirstName("Test");
    user.setLastName("Admin");
    user.setPhoneNumber("555-300-3000");
    user.setActive(true);
    user.setLoginEnabled(true);
    user.setEmailVerified(true);
    user.addRole(StudentIntake::Models::UserRole::Admin);
    return user;
}

StudentIntake::Models::User TestFixtures::createMultiRoleUser() {
    StudentIntake::Models::User user;
    user.setId(4);
    user.setEmail("multirole@example.com");
    user.setPasswordHash("hashed_password_multi");
    user.setFirstName("Multi");
    user.setLastName("Role");
    user.setPhoneNumber("555-400-4000");
    user.setActive(true);
    user.setLoginEnabled(true);
    user.setEmailVerified(true);
    user.addRole(StudentIntake::Models::UserRole::Student);
    user.addRole(StudentIntake::Models::UserRole::Instructor);
    return user;
}

// =============================================================================
// AcademicHistory Fixtures
// =============================================================================

StudentIntake::Models::AcademicHistory TestFixtures::createHighSchoolRecord(const std::string& studentId) {
    StudentIntake::Models::AcademicHistory record(studentId);
    record.setInstitutionName("Springfield High School");
    record.setInstitutionType("high_school");
    record.setInstitutionCity("Springfield");
    record.setInstitutionState("IL");
    record.setInstitutionCountry("United States");
    record.setStartDate("2016-08-01");
    record.setEndDate("2020-05-15");
    record.setDegreeEarned("High School Diploma");
    record.setGpa(3.8);
    record.setGpaScale(4.0);
    return record;
}

StudentIntake::Models::AcademicHistory TestFixtures::createUndergraduateRecord(const std::string& studentId) {
    StudentIntake::Models::AcademicHistory record(studentId);
    record.setInstitutionName("University of Illinois");
    record.setInstitutionType("undergraduate");
    record.setInstitutionCity("Urbana");
    record.setInstitutionState("IL");
    record.setInstitutionCountry("United States");
    record.setStartDate("2020-08-15");
    record.setEndDate("2024-05-15");
    record.setDegreeEarned("Bachelor of Science");
    record.setMajor("Computer Science");
    record.setGpa(3.5);
    record.setGpaScale(4.0);
    return record;
}

StudentIntake::Models::AcademicHistory TestFixtures::createVocationalRecord(const std::string& studentId) {
    StudentIntake::Models::AcademicHistory record(studentId);
    record.setInstitutionName("Springfield Vocational School");
    record.setInstitutionType("vocational");
    record.setInstitutionCity("Springfield");
    record.setInstitutionState("IL");
    record.setInstitutionCountry("United States");
    record.setStartDate("2022-01-15");
    record.setEndDate("2022-06-15");
    record.setDegreeEarned("CDL Class A Certificate");
    return record;
}

StudentIntake::Models::AcademicHistory TestFixtures::createGraduateRecord(const std::string& studentId) {
    StudentIntake::Models::AcademicHistory record(studentId);
    record.setInstitutionName("MIT");
    record.setInstitutionType("graduate");
    record.setInstitutionCity("Cambridge");
    record.setInstitutionState("MA");
    record.setInstitutionCountry("United States");
    record.setStartDate("2024-08-15");
    record.setDegreeEarned("Master of Science");
    record.setMajor("Computer Science");
    record.setGpa(3.9);
    record.setGpaScale(4.0);
    record.setCurrentlyAttending(true);
    return record;
}

// =============================================================================
// Curriculum Fixtures
// =============================================================================

StudentIntake::Models::Curriculum TestFixtures::createAccreditedProgram() {
    StudentIntake::Models::Curriculum curriculum;
    curriculum.setId("1");
    curriculum.setName("Bachelor of Science in Computer Science");
    curriculum.setCode("BSCS");
    curriculum.setDescription("A comprehensive computer science program");
    curriculum.setDegreeType("bachelor");
    curriculum.setCreditHours(120);
    curriculum.setDurationSemesters(8);
    curriculum.setDurationInterval("semester");
    curriculum.setActive(true);
    return curriculum;
}

StudentIntake::Models::Curriculum TestFixtures::createVocationalProgram() {
    StudentIntake::Models::Curriculum curriculum;
    curriculum.setId("10");
    curriculum.setName("CDL Class A Training");
    curriculum.setCode("CDL-A");
    curriculum.setDescription("Commercial Driver's License Class A training program");
    curriculum.setDegreeType("certificate");
    curriculum.setCreditHours(0);
    curriculum.setDurationSemesters(4);
    curriculum.setDurationInterval("week");
    curriculum.setActive(true);
    curriculum.setCdlClass("A");
    return curriculum;
}

StudentIntake::Models::Curriculum TestFixtures::createEndorsementProgram() {
    StudentIntake::Models::Curriculum curriculum;
    curriculum.setId("20");
    curriculum.setName("Hazmat Endorsement");
    curriculum.setCode("HAZMAT");
    curriculum.setDescription("Hazardous materials endorsement training");
    curriculum.setDegreeType("certificate");
    curriculum.setCreditHours(0);
    curriculum.setDurationSemesters(1);
    curriculum.setDurationInterval("week");
    curriculum.setActive(true);
    curriculum.setEndorsement(true);
    return curriculum;
}

// =============================================================================
// ActivityLog Fixtures
// =============================================================================

StudentIntake::Models::ActivityLog TestFixtures::createLoginActivity() {
    StudentIntake::Models::ActivityLog log;
    log.setActorType(StudentIntake::Models::ActorType::Student);
    log.setActorId(1);
    log.setActionCategory(StudentIntake::Models::ActivityCategory::Authentication);
    log.setActionType("login");
    log.setDescription("User logged in successfully");
    log.setSeverity(StudentIntake::Models::ActivitySeverity::Info);
    return log;
}

StudentIntake::Models::ActivityLog TestFixtures::createFormSubmissionActivity() {
    StudentIntake::Models::ActivityLog log;
    log.setActorType(StudentIntake::Models::ActorType::Student);
    log.setActorId(1);
    log.setActionCategory(StudentIntake::Models::ActivityCategory::Forms);
    log.setActionType("submit_form");
    log.setDescription("Personal information form submitted");
    log.setSeverity(StudentIntake::Models::ActivitySeverity::Info);
    log.setEntityType("form");
    log.setEntityId("personal_info");
    return log;
}

StudentIntake::Models::ActivityLog TestFixtures::createErrorActivity() {
    StudentIntake::Models::ActivityLog log;
    log.setActorType(StudentIntake::Models::ActorType::System);
    log.setActorId(0);
    log.setActionCategory(StudentIntake::Models::ActivityCategory::System);
    log.setActionType("error");
    log.setDescription("Database connection failed");
    log.setSeverity(StudentIntake::Models::ActivitySeverity::Error);
    return log;
}

// =============================================================================
// JSON Fixtures
// =============================================================================

nlohmann::json TestFixtures::createStudentJson() {
    return {
        {"id", 1},
        {"email", "john.doe@example.com"},
        {"first_name", "John"},
        {"last_name", "Doe"},
        {"middle_name", "Michael"},
        {"preferred_name", "Johnny"},
        {"gender", "Male"},
        {"preferred_pronouns", "he/him"},
        {"phone_number", "555-123-4567"},
        {"alternate_phone", "555-987-6543"},
        {"address_line1", "123 Main Street"},
        {"address_line2", "Apt 4B"},
        {"city", "Springfield"},
        {"state", "IL"},
        {"zip_code", "62701"},
        {"student_type", "undergraduate"},
        {"status", "active"},
        {"curriculum_id", 1},
        {"is_international", false},
        {"is_transfer_student", false},
        {"is_veteran", false},
        {"requires_financial_aid", false},
        {"date_of_birth", "2000-01-15"},
        {"enrollment_date", "2024-08-20"}
    };
}

nlohmann::json TestFixtures::createStudentJsonWithAttributes() {
    return {
        {"id", "1"},
        {"type", "student"},
        {"attributes", createStudentJson()}
    };
}

nlohmann::json TestFixtures::createEmergencyContactJson() {
    return {
        {"student_id", 1},
        {"first_name", "Jane"},
        {"last_name", "Doe"},
        {"contact_relationship", "Spouse"},
        {"phone", "555-111-2222"},
        {"alternate_phone", "555-333-4444"},
        {"email", "jane.doe@example.com"},
        {"street1", "123 Main Street"},
        {"street2", "Apt 4B"},
        {"city", "Springfield"},
        {"state", "IL"},
        {"postal_code", "62701"},
        {"country", "United States"},
        {"is_primary", true},
        {"priority", 1}
    };
}

nlohmann::json TestFixtures::createUserJson() {
    return {
        {"id", 1},
        {"email", "student@example.com"},
        {"first_name", "Test"},
        {"last_name", "Student"},
        {"phone_number", "555-100-1000"},
        {"is_active", true},
        {"login_enabled", true},
        {"email_verified", true},
        {"roles", nlohmann::json::array({"student"})}
    };
}

// =============================================================================
// JsonTestHelper
// =============================================================================

bool JsonTestHelper::hasAllKeys(const nlohmann::json& json, const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        if (!json.contains(key)) {
            return false;
        }
    }
    return true;
}

bool JsonTestHelper::jsonEquals(const nlohmann::json& a, const nlohmann::json& b) {
    // Simple equality check - nlohmann::json handles this well
    return a == b;
}

std::string JsonTestHelper::prettyPrint(const nlohmann::json& json) {
    return json.dump(2);
}

// =============================================================================
// DateTimeHelper
// =============================================================================

std::chrono::system_clock::time_point DateTimeHelper::parseDate(const std::string& dateStr) {
    std::tm tm = {};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

std::string DateTimeHelper::formatDate(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream ss;
    ss << std::put_time(std::gmtime(&time), "%Y-%m-%d");
    return ss.str();
}

int DateTimeHelper::calculateAge(const std::chrono::system_clock::time_point& dob) {
    auto now = std::chrono::system_clock::now();
    auto duration = now - dob;
    auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;
    return static_cast<int>(days / 365);
}

} // namespace TestUtils
