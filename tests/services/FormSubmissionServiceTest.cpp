#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "api/ApiClient.h"
#include "api/FormSubmissionService.h"
#include "models/Student.h"
#include "models/EmergencyContact.h"
#include "utils/TestUtils.h"

using namespace StudentIntake;
using namespace StudentIntake::Models;
using namespace TestUtils;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;

// =============================================================================
// Mock API Client
// =============================================================================

class MockApiClient : public Api::ApiClient {
public:
    MockApiClient() : Api::ApiClient("http://test-api.example.com") {}

    MOCK_METHOD(Api::ApiResponse, get, (const std::string& endpoint), (override));
    MOCK_METHOD(Api::ApiResponse, post, (const std::string& endpoint, const nlohmann::json& data), (override));
    MOCK_METHOD(Api::ApiResponse, put, (const std::string& endpoint, const nlohmann::json& data), (override));
    MOCK_METHOD(Api::ApiResponse, patch, (const std::string& endpoint, const nlohmann::json& data), (override));
    MOCK_METHOD(Api::ApiResponse, del, (const std::string& endpoint), (override));
};

// =============================================================================
// Test Fixture
// =============================================================================

class FormSubmissionServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        student_ = TestFixtures::createBasicStudent();
        primaryContact_ = TestFixtures::createPrimaryContact("1");
        secondaryContact_ = TestFixtures::createSecondaryContact("1");
    }

    Student student_;
    EmergencyContact primaryContact_;
    EmergencyContact secondaryContact_;
};

// =============================================================================
// Student Model JSON Generation Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, StudentToJson_GeneratesValidPayload) {
    auto json = student_.toJson();

    EXPECT_EQ(json["email"], "john.doe@example.com");
    EXPECT_EQ(json["first_name"], "John");
    EXPECT_EQ(json["last_name"], "Doe");
    EXPECT_EQ(json["student_type"], "undergraduate");
}

TEST_F(FormSubmissionServiceTest, StudentToJson_ContainsRequiredFields) {
    auto json = student_.toJson();

    std::vector<std::string> requiredFields = {
        "email", "first_name", "last_name", "phone_number",
        "address_line1", "city", "state", "zip_code"
    };

    for (const auto& field : requiredFields) {
        EXPECT_TRUE(json.contains(field)) << "Missing field: " << field;
    }
}

TEST_F(FormSubmissionServiceTest, StudentToJson_BooleanFieldsAreProperType) {
    auto json = student_.toJson();

    EXPECT_TRUE(json["is_international"].is_boolean());
    EXPECT_TRUE(json["is_veteran"].is_boolean());
    EXPECT_TRUE(json["is_transfer_student"].is_boolean());
    EXPECT_TRUE(json["requires_financial_aid"].is_boolean());
}

// =============================================================================
// Emergency Contact JSON Generation Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, EmergencyContactToJson_GeneratesValidPayload) {
    auto json = primaryContact_.toJson();

    EXPECT_EQ(json["first_name"], "Jane");
    EXPECT_EQ(json["last_name"], "Doe");
    EXPECT_EQ(json["contact_relationship"], "Spouse");
    EXPECT_EQ(json["phone"], "555-111-2222");
}

TEST_F(FormSubmissionServiceTest, EmergencyContactToJson_ContainsCompoundKey) {
    auto json = primaryContact_.toJson();

    EXPECT_TRUE(json.contains("student_id"));
    EXPECT_TRUE(json.contains("contact_relationship"));
    EXPECT_TRUE(json.contains("phone"));
}

TEST_F(FormSubmissionServiceTest, EmergencyContactToJson_ContainsAddressFields) {
    auto json = primaryContact_.toJson();

    EXPECT_EQ(json["street1"], "123 Main Street");
    EXPECT_EQ(json["city"], "Springfield");
    EXPECT_EQ(json["state"], "IL");
    EXPECT_EQ(json["postal_code"], "62701");
}

// =============================================================================
// JSON:API Format Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, JsonApiFormat_StudentPayload) {
    // Test that we can construct proper JSON:API format
    nlohmann::json payload = {
        {"data", {
            {"type", "student"},
            {"attributes", student_.toJson()}
        }}
    };

    EXPECT_TRUE(payload.contains("data"));
    EXPECT_EQ(payload["data"]["type"], "student");
    EXPECT_TRUE(payload["data"]["attributes"].is_object());
}

TEST_F(FormSubmissionServiceTest, JsonApiFormat_EmergencyContactPayload) {
    nlohmann::json payload = {
        {"data", {
            {"type", "emergency_contact"},
            {"attributes", primaryContact_.toJson()}
        }}
    };

    EXPECT_TRUE(payload.contains("data"));
    EXPECT_EQ(payload["data"]["type"], "emergency_contact");
}

// =============================================================================
// Compound Key Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, EmergencyContact_CompoundKeyGeneration) {
    std::string key = primaryContact_.getCompoundKey();

    // Format: studentId|relationship|phone
    EXPECT_EQ(key, "1|Spouse|555-111-2222");
}

TEST_F(FormSubmissionServiceTest, EmergencyContact_DifferentContactsHaveDifferentKeys) {
    EXPECT_NE(primaryContact_.getCompoundKey(), secondaryContact_.getCompoundKey());
}

TEST_F(FormSubmissionServiceTest, EmergencyContact_HasValidKey) {
    EXPECT_TRUE(primaryContact_.hasValidKey());
    EXPECT_TRUE(secondaryContact_.hasValidKey());

    EmergencyContact incomplete("1");
    EXPECT_FALSE(incomplete.hasValidKey());
}

// =============================================================================
// Form Data Validation Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_EmptyFieldsAreEmptyStrings) {
    Student emptyStudent;

    auto json = emptyStudent.toJson();

    EXPECT_EQ(json["first_name"], "");
    EXPECT_EQ(json["last_name"], "");
    EXPECT_EQ(json["phone_number"], "");
}

TEST_F(FormSubmissionServiceTest, EmergencyContact_EmptyIsDetected) {
    EmergencyContact empty;

    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(primaryContact_.isEmpty());
}

// =============================================================================
// Multiple Emergency Contacts Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, MultipleContacts_CanBeSerializedSeparately) {
    auto json1 = primaryContact_.toJson();
    auto json2 = secondaryContact_.toJson();

    EXPECT_EQ(json1["contact_relationship"], "Spouse");
    EXPECT_EQ(json2["contact_relationship"], "Parent");
}

TEST_F(FormSubmissionServiceTest, MultipleContacts_HaveDifferentPriorities) {
    EXPECT_EQ(primaryContact_.getPriority(), 1);
    EXPECT_EQ(secondaryContact_.getPriority(), 2);
}

TEST_F(FormSubmissionServiceTest, MultipleContacts_PrimaryFlagIsCorrect) {
    EXPECT_TRUE(primaryContact_.isPrimary());
    EXPECT_FALSE(secondaryContact_.isPrimary());
}

// =============================================================================
// Date Formatting Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_EnrollmentDateIsFormatted) {
    auto json = student_.toJson();

    std::string enrollmentDate = json["enrollment_date"];

    // Should be in YYYY-MM-DD format
    EXPECT_EQ(enrollmentDate.length(), 10);
    EXPECT_EQ(enrollmentDate[4], '-');
    EXPECT_EQ(enrollmentDate[7], '-');
}

TEST_F(FormSubmissionServiceTest, Student_DateOfBirthIsFormatted) {
    auto json = student_.toJson();

    std::string dob = json["date_of_birth"];

    // Should be in YYYY-MM-DD format
    EXPECT_EQ(dob.length(), 10);
}

// =============================================================================
// Curriculum and Endorsement Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_CurriculumIdIsSerialized) {
    Student student = TestFixtures::createStudentWithEndorsements();
    auto json = student.toJson();

    // curriculum_id should be present
    EXPECT_TRUE(json.contains("curriculum_id"));
}

TEST_F(FormSubmissionServiceTest, Student_EndorsementsAreTracked) {
    Student student = TestFixtures::createStudentWithEndorsements();

    EXPECT_TRUE(student.hasEndorsement("20"));
    EXPECT_TRUE(student.hasEndorsement("21"));
    EXPECT_TRUE(student.hasEndorsement("22"));
    EXPECT_EQ(student.getEndorsementIds().size(), 3);
}

// =============================================================================
// Form Completion Tracking Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_FormCompletionIsTracked) {
    Student student;

    student.markFormCompleted("personal_info");
    student.markFormCompleted("emergency_contact");

    EXPECT_TRUE(student.hasCompletedForm("personal_info"));
    EXPECT_TRUE(student.hasCompletedForm("emergency_contact"));
    EXPECT_FALSE(student.hasCompletedForm("academic_history"));
}

TEST_F(FormSubmissionServiceTest, Student_CompletedFormsCanBeReset) {
    Student student;

    student.markFormCompleted("personal_info");
    student.markFormCompleted("emergency_contact");
    student.resetCompletedForms();

    EXPECT_FALSE(student.hasCompletedForm("personal_info"));
    EXPECT_FALSE(student.hasCompletedForm("emergency_contact"));
}

// =============================================================================
// Error Response Handling Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, ApiResponse_SuccessCodeDetection) {
    Api::ApiResponse successResponse;
    successResponse.statusCode = 200;
    successResponse.success = true;

    EXPECT_TRUE(successResponse.success);
    EXPECT_EQ(successResponse.statusCode, 200);
}

TEST_F(FormSubmissionServiceTest, ApiResponse_ErrorCodeDetection) {
    Api::ApiResponse errorResponse;
    errorResponse.statusCode = 400;
    errorResponse.success = false;
    errorResponse.errorMessage = "Validation failed";

    EXPECT_FALSE(errorResponse.success);
    EXPECT_EQ(errorResponse.statusCode, 400);
    EXPECT_EQ(errorResponse.errorMessage, "Validation failed");
}

TEST_F(FormSubmissionServiceTest, ApiResponse_ServerErrorDetection) {
    Api::ApiResponse serverError;
    serverError.statusCode = 500;
    serverError.success = false;
    serverError.errorMessage = "Internal server error";

    EXPECT_FALSE(serverError.success);
    EXPECT_GE(serverError.statusCode, 500);
}

// =============================================================================
// Payload Size Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, StudentPayload_IsReasonableSize) {
    auto json = student_.toJson();
    std::string jsonStr = json.dump();

    // Payload should be reasonable size (under 10KB for a single student)
    EXPECT_LT(jsonStr.size(), 10240);
}

TEST_F(FormSubmissionServiceTest, EmergencyContactPayload_IsReasonableSize) {
    auto json = primaryContact_.toJson();
    std::string jsonStr = json.dump();

    // Payload should be reasonable size (under 5KB for a single contact)
    EXPECT_LT(jsonStr.size(), 5120);
}

// =============================================================================
// Special Characters Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_HandlesSpecialCharactersInNames) {
    Student student;
    student.setFirstName("José");
    student.setLastName("O'Brien-García");

    auto json = student.toJson();

    EXPECT_EQ(json["first_name"], "José");
    EXPECT_EQ(json["last_name"], "O'Brien-García");
}

TEST_F(FormSubmissionServiceTest, EmergencyContact_HandlesSpecialCharactersInAddress) {
    EmergencyContact contact("1");
    contact.setStreet1("123 Main St. #456");
    contact.setCity("São Paulo");

    auto json = contact.toJson();

    EXPECT_EQ(json["street1"], "123 Main St. #456");
    EXPECT_EQ(json["city"], "São Paulo");
}

// =============================================================================
// Null/Empty Value Handling Tests
// =============================================================================

TEST_F(FormSubmissionServiceTest, Student_OptionalFieldsCanBeEmpty) {
    Student student("1", "test@example.com");

    auto json = student.toJson();

    // Middle name is optional
    EXPECT_EQ(json["middle_name"], "");
    // Preferred name is optional
    EXPECT_EQ(json["preferred_name"], "");
}

TEST_F(FormSubmissionServiceTest, EmergencyContact_OptionalFieldsCanBeEmpty) {
    EmergencyContact contact("1");
    contact.setRelationship("Parent");
    contact.setPhone("555-1234");
    contact.setFirstName("John");
    contact.setLastName("Doe");
    // street2 is optional

    auto json = contact.toJson();

    EXPECT_EQ(json["street2"], "");
    EXPECT_EQ(json["alternate_phone"], "");
}
