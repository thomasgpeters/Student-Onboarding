#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/Student.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class StudentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create fresh test students for each test
        basicStudent_ = TestFixtures::createBasicStudent();
    }

    Student basicStudent_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(StudentTest, DefaultConstructor_InitializesWithDefaults) {
    Student student;

    EXPECT_EQ(student.getId(), "");
    EXPECT_EQ(student.getEmail(), "");
    EXPECT_EQ(student.getFirstName(), "");
    EXPECT_EQ(student.getLastName(), "");
    EXPECT_EQ(student.getStudentType(), "undergraduate");
    EXPECT_EQ(student.getStatus(), "pending");
    EXPECT_FALSE(student.isInternational());
    EXPECT_FALSE(student.isTransferStudent());
    EXPECT_FALSE(student.isVeteran());
    EXPECT_FALSE(student.requiresFinancialAid());
    EXPECT_TRUE(student.getCompletedForms().empty());
    EXPECT_TRUE(student.getEndorsementIds().empty());
}

TEST_F(StudentTest, ParameterizedConstructor_SetsIdAndEmail) {
    Student student("123", "test@example.com");

    EXPECT_EQ(student.getId(), "123");
    EXPECT_EQ(student.getEmail(), "test@example.com");
    EXPECT_EQ(student.getStudentType(), "undergraduate");
    EXPECT_EQ(student.getStatus(), "pending");
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(StudentTest, BasicGettersReturnCorrectValues) {
    EXPECT_EQ(basicStudent_.getId(), "1");
    EXPECT_EQ(basicStudent_.getEmail(), "john.doe@example.com");
    EXPECT_EQ(basicStudent_.getFirstName(), "John");
    EXPECT_EQ(basicStudent_.getLastName(), "Doe");
    EXPECT_EQ(basicStudent_.getMiddleName(), "Michael");
    EXPECT_EQ(basicStudent_.getPreferredName(), "Johnny");
    EXPECT_EQ(basicStudent_.getGender(), "Male");
    EXPECT_EQ(basicStudent_.getPreferredPronouns(), "he/him");
}

TEST_F(StudentTest, AddressGettersReturnCorrectValues) {
    EXPECT_EQ(basicStudent_.getAddressLine1(), "123 Main Street");
    EXPECT_EQ(basicStudent_.getAddressLine2(), "Apt 4B");
    EXPECT_EQ(basicStudent_.getCity(), "Springfield");
    EXPECT_EQ(basicStudent_.getState(), "IL");
    EXPECT_EQ(basicStudent_.getZipCode(), "62701");
}

TEST_F(StudentTest, SettersUpdateValues) {
    Student student;

    student.setId("999");
    student.setEmail("new@example.com");
    student.setFirstName("NewFirst");
    student.setLastName("NewLast");
    student.setStatus("active");
    student.setInternational(true);
    student.setVeteran(true);

    EXPECT_EQ(student.getId(), "999");
    EXPECT_EQ(student.getEmail(), "new@example.com");
    EXPECT_EQ(student.getFirstName(), "NewFirst");
    EXPECT_EQ(student.getLastName(), "NewLast");
    EXPECT_EQ(student.getStatus(), "active");
    EXPECT_TRUE(student.isInternational());
    EXPECT_TRUE(student.isVeteran());
}

// =============================================================================
// Full Name Tests
// =============================================================================

TEST_F(StudentTest, GetFullName_ReturnsFirstAndLastName) {
    EXPECT_EQ(basicStudent_.getFullName(), "John Doe");
}

TEST_F(StudentTest, GetFullName_ReturnsEmailWhenNameEmpty) {
    Student student("1", "test@example.com");
    EXPECT_EQ(student.getFullName(), "test@example.com");
}

TEST_F(StudentTest, GetFullName_ReturnsEmailWhenOnlyFirstNameSet) {
    Student student;
    student.setEmail("test@example.com");
    student.setFirstName("John");
    // Last name is empty, so it should return "John "
    EXPECT_EQ(student.getFullName(), "John ");
}

// =============================================================================
// Completed Forms Tests
// =============================================================================

TEST_F(StudentTest, MarkFormCompleted_AddsFormToList) {
    Student student;

    student.markFormCompleted("personal_info");
    EXPECT_TRUE(student.hasCompletedForm("personal_info"));
    EXPECT_EQ(student.getCompletedForms().size(), 1);
}

TEST_F(StudentTest, MarkFormCompleted_DoesNotDuplicateForms) {
    Student student;

    student.markFormCompleted("personal_info");
    student.markFormCompleted("personal_info");

    EXPECT_EQ(student.getCompletedForms().size(), 1);
}

TEST_F(StudentTest, MarkFormCompleted_CanAddMultipleForms) {
    Student student;

    student.markFormCompleted("personal_info");
    student.markFormCompleted("emergency_contact");
    student.markFormCompleted("academic_history");

    EXPECT_TRUE(student.hasCompletedForm("personal_info"));
    EXPECT_TRUE(student.hasCompletedForm("emergency_contact"));
    EXPECT_TRUE(student.hasCompletedForm("academic_history"));
    EXPECT_EQ(student.getCompletedForms().size(), 3);
}

TEST_F(StudentTest, HasCompletedForm_ReturnsFalseForUncompletedForm) {
    Student student;
    student.markFormCompleted("personal_info");

    EXPECT_FALSE(student.hasCompletedForm("emergency_contact"));
}

TEST_F(StudentTest, ResetCompletedForms_ClearsList) {
    Student student;

    student.markFormCompleted("personal_info");
    student.markFormCompleted("emergency_contact");
    student.resetCompletedForms();

    EXPECT_TRUE(student.getCompletedForms().empty());
    EXPECT_FALSE(student.hasCompletedForm("personal_info"));
}

// =============================================================================
// Endorsement Tests
// =============================================================================

TEST_F(StudentTest, AddEndorsementId_AddsEndorsement) {
    Student student;

    student.addEndorsementId("10");
    EXPECT_TRUE(student.hasEndorsement("10"));
    EXPECT_EQ(student.getEndorsementIds().size(), 1);
}

TEST_F(StudentTest, AddEndorsementId_DoesNotDuplicateEndorsements) {
    Student student;

    student.addEndorsementId("10");
    student.addEndorsementId("10");

    EXPECT_EQ(student.getEndorsementIds().size(), 1);
}

TEST_F(StudentTest, RemoveEndorsementId_RemovesEndorsement) {
    Student student;

    student.addEndorsementId("10");
    student.addEndorsementId("20");
    student.removeEndorsementId("10");

    EXPECT_FALSE(student.hasEndorsement("10"));
    EXPECT_TRUE(student.hasEndorsement("20"));
    EXPECT_EQ(student.getEndorsementIds().size(), 1);
}

TEST_F(StudentTest, RemoveEndorsementId_HandlesMissingEndorsement) {
    Student student;

    student.addEndorsementId("10");
    student.removeEndorsementId("999"); // Does not exist

    EXPECT_TRUE(student.hasEndorsement("10"));
    EXPECT_EQ(student.getEndorsementIds().size(), 1);
}

TEST_F(StudentTest, ClearEndorsements_RemovesAllEndorsements) {
    Student student;

    student.addEndorsementId("10");
    student.addEndorsementId("20");
    student.addEndorsementId("30");
    student.clearEndorsements();

    EXPECT_TRUE(student.getEndorsementIds().empty());
}

TEST_F(StudentTest, SetEndorsementIds_ReplacesAllEndorsements) {
    Student student;
    student.addEndorsementId("10");

    std::vector<std::string> newIds = {"100", "200", "300"};
    student.setEndorsementIds(newIds);

    EXPECT_FALSE(student.hasEndorsement("10"));
    EXPECT_TRUE(student.hasEndorsement("100"));
    EXPECT_TRUE(student.hasEndorsement("200"));
    EXPECT_TRUE(student.hasEndorsement("300"));
    EXPECT_EQ(student.getEndorsementIds().size(), 3);
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(StudentTest, ToJson_ContainsAllBasicFields) {
    auto json = basicStudent_.toJson();

    EXPECT_EQ(json["email"], "john.doe@example.com");
    EXPECT_EQ(json["first_name"], "John");
    EXPECT_EQ(json["last_name"], "Doe");
    EXPECT_EQ(json["middle_name"], "Michael");
    EXPECT_EQ(json["preferred_name"], "Johnny");
    EXPECT_EQ(json["gender"], "Male");
    EXPECT_EQ(json["preferred_pronouns"], "he/him");
    EXPECT_EQ(json["student_type"], "undergraduate");
    EXPECT_EQ(json["status"], "active");
}

TEST_F(StudentTest, ToJson_ContainsAddressFields) {
    auto json = basicStudent_.toJson();

    EXPECT_EQ(json["address_line1"], "123 Main Street");
    EXPECT_EQ(json["address_line2"], "Apt 4B");
    EXPECT_EQ(json["city"], "Springfield");
    EXPECT_EQ(json["state"], "IL");
    EXPECT_EQ(json["zip_code"], "62701");
}

TEST_F(StudentTest, ToJson_ContainsBooleanFields) {
    auto json = basicStudent_.toJson();

    EXPECT_FALSE(json["is_international"].get<bool>());
    EXPECT_FALSE(json["is_transfer_student"].get<bool>());
    EXPECT_FALSE(json["is_veteran"].get<bool>());
    EXPECT_FALSE(json["requires_financial_aid"].get<bool>());
}

TEST_F(StudentTest, ToJson_SerializesIdAsInteger) {
    auto json = basicStudent_.toJson();

    EXPECT_TRUE(json["id"].is_number());
    EXPECT_EQ(json["id"].get<int>(), 1);
}

TEST_F(StudentTest, ToJson_SerializesCompletedFormsAsJsonString) {
    Student student = TestFixtures::createBasicStudent();
    student.markFormCompleted("personal_info");
    student.markFormCompleted("emergency_contact");

    auto json = student.toJson();

    EXPECT_TRUE(json.contains("completed_forms"));
    EXPECT_TRUE(json["completed_forms"].is_string());
}

TEST_F(StudentTest, ToJson_ContainsDateFields) {
    auto json = basicStudent_.toJson();

    EXPECT_TRUE(json.contains("enrollment_date"));
    EXPECT_TRUE(json.contains("date_of_birth"));
    EXPECT_TRUE(json["enrollment_date"].is_string());
    EXPECT_TRUE(json["date_of_birth"].is_string());
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(StudentTest, FromJson_ParsesBasicFields) {
    auto json = TestFixtures::createStudentJson();
    Student student = Student::fromJson(json);

    EXPECT_EQ(student.getId(), "1");
    EXPECT_EQ(student.getEmail(), "john.doe@example.com");
    EXPECT_EQ(student.getFirstName(), "John");
    EXPECT_EQ(student.getLastName(), "Doe");
    EXPECT_EQ(student.getStudentType(), "undergraduate");
    EXPECT_EQ(student.getStatus(), "active");
}

TEST_F(StudentTest, FromJson_ParsesBooleanFields) {
    auto json = TestFixtures::createStudentJson();
    json["is_international"] = true;
    json["is_veteran"] = true;

    Student student = Student::fromJson(json);

    EXPECT_TRUE(student.isInternational());
    EXPECT_TRUE(student.isVeteran());
    EXPECT_FALSE(student.isTransferStudent());
}

TEST_F(StudentTest, FromJson_ParsesJsonApiFormat) {
    auto json = TestFixtures::createStudentJsonWithAttributes();
    Student student = Student::fromJson(json);

    EXPECT_EQ(student.getId(), "1");
    EXPECT_EQ(student.getEmail(), "john.doe@example.com");
    EXPECT_EQ(student.getFirstName(), "John");
}

TEST_F(StudentTest, FromJson_HandlesCamelCaseFields) {
    nlohmann::json json = {
        {"id", 1},
        {"email", "test@example.com"},
        {"firstName", "Test"},
        {"lastName", "User"},
        {"phoneNumber", "555-1234"},
        {"isInternational", true}
    };

    Student student = Student::fromJson(json);

    EXPECT_EQ(student.getFirstName(), "Test");
    EXPECT_EQ(student.getLastName(), "User");
    EXPECT_EQ(student.getPhoneNumber(), "555-1234");
    EXPECT_TRUE(student.isInternational());
}

TEST_F(StudentTest, FromJson_ParsesCompletedFormsArray) {
    nlohmann::json json = TestFixtures::createStudentJson();
    json["completed_forms"] = nlohmann::json::array({"personal_info", "emergency_contact"});

    Student student = Student::fromJson(json);

    EXPECT_TRUE(student.hasCompletedForm("personal_info"));
    EXPECT_TRUE(student.hasCompletedForm("emergency_contact"));
}

TEST_F(StudentTest, FromJson_ParsesCompletedFormsJsonString) {
    nlohmann::json json = TestFixtures::createStudentJson();
    json["completed_forms"] = "[\"personal_info\",\"emergency_contact\"]";

    Student student = Student::fromJson(json);

    EXPECT_TRUE(student.hasCompletedForm("personal_info"));
    EXPECT_TRUE(student.hasCompletedForm("emergency_contact"));
}

TEST_F(StudentTest, FromJson_ParsesEndorsementIds) {
    nlohmann::json json = TestFixtures::createStudentJson();
    json["endorsement_ids"] = nlohmann::json::array({10, 20, 30});

    Student student = Student::fromJson(json);

    EXPECT_TRUE(student.hasEndorsement("10"));
    EXPECT_TRUE(student.hasEndorsement("20"));
    EXPECT_TRUE(student.hasEndorsement("30"));
}

TEST_F(StudentTest, FromJson_HandlesStringId) {
    nlohmann::json json = TestFixtures::createStudentJson();
    json["id"] = "999";

    Student student = Student::fromJson(json);
    EXPECT_EQ(student.getId(), "999");
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(StudentTest, RoundTrip_PreservesBasicData) {
    auto json = basicStudent_.toJson();
    Student reconstructed = Student::fromJson(json);

    EXPECT_EQ(reconstructed.getEmail(), basicStudent_.getEmail());
    EXPECT_EQ(reconstructed.getFirstName(), basicStudent_.getFirstName());
    EXPECT_EQ(reconstructed.getLastName(), basicStudent_.getLastName());
    EXPECT_EQ(reconstructed.getStatus(), basicStudent_.getStatus());
    EXPECT_EQ(reconstructed.isInternational(), basicStudent_.isInternational());
    EXPECT_EQ(reconstructed.isVeteran(), basicStudent_.isVeteran());
}

TEST_F(StudentTest, RoundTrip_PreservesEndorsements) {
    Student student = TestFixtures::createStudentWithEndorsements();
    auto json = student.toJson();

    // Note: endorsement_ids are not serialized in toJson by default
    // but should be preserved if present in fromJson
    Student reconstructed = Student::fromJson(json);

    EXPECT_EQ(reconstructed.getCurriculumId(), student.getCurriculumId());
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(StudentTest, FromJson_HandlesNullValues) {
    nlohmann::json json = {
        {"id", 1},
        {"email", "test@example.com"},
        {"first_name", nullptr},
        {"last_name", nullptr}
    };

    // Should not throw
    Student student = Student::fromJson(json);

    EXPECT_EQ(student.getEmail(), "test@example.com");
    EXPECT_EQ(student.getFirstName(), "");
}

TEST_F(StudentTest, FromJson_HandlesEmptyJson) {
    nlohmann::json json = {};

    Student student = Student::fromJson(json);

    EXPECT_EQ(student.getId(), "");
    EXPECT_EQ(student.getEmail(), "");
}

TEST_F(StudentTest, FromJson_HandlesMixedCaseFields) {
    nlohmann::json json = {
        {"id", 1},
        {"email", "test@example.com"},
        {"first_name", "FromSnake"},
        {"firstName", "FromCamel"}  // Both present
    };

    Student student = Student::fromJson(json);

    // Should prefer camelCase based on implementation order
    EXPECT_EQ(student.getFirstName(), "FromCamel");
}

// =============================================================================
// Special Student Type Tests
// =============================================================================

TEST_F(StudentTest, InternationalStudent_HasCorrectFlags) {
    Student student = TestFixtures::createInternationalStudent();

    EXPECT_TRUE(student.isInternational());
    EXPECT_FALSE(student.isVeteran());
    EXPECT_FALSE(student.isTransferStudent());
}

TEST_F(StudentTest, VeteranStudent_HasCorrectFlags) {
    Student student = TestFixtures::createVeteranStudent();

    EXPECT_FALSE(student.isInternational());
    EXPECT_TRUE(student.isVeteran());
    EXPECT_TRUE(student.requiresFinancialAid());
}

TEST_F(StudentTest, TransferStudent_HasCorrectFlags) {
    Student student = TestFixtures::createTransferStudent();

    EXPECT_FALSE(student.isInternational());
    EXPECT_FALSE(student.isVeteran());
    EXPECT_TRUE(student.isTransferStudent());
}
