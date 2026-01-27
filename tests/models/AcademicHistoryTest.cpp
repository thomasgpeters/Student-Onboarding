#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/AcademicHistory.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class AcademicHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        highSchoolRecord_ = TestFixtures::createHighSchoolRecord("1");
        undergraduateRecord_ = TestFixtures::createUndergraduateRecord("1");
        vocationalRecord_ = TestFixtures::createVocationalRecord("1");
    }

    AcademicHistory highSchoolRecord_;
    AcademicHistory undergraduateRecord_;
    AcademicHistory vocationalRecord_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(AcademicHistoryTest, DefaultConstructor_InitializesWithDefaults) {
    AcademicHistory record;

    EXPECT_EQ(record.getStudentId(), "");
    EXPECT_EQ(record.getInstitutionName(), "");
    EXPECT_EQ(record.getInstitutionType(), "");
    EXPECT_EQ(record.getDegreeEarned(), "");
    EXPECT_EQ(record.getMajor(), "");
    EXPECT_EQ(record.getMinor(), "");
    EXPECT_DOUBLE_EQ(record.getGpa(), 0.0);
    EXPECT_FALSE(record.isCurrentlyAttending());
    EXPECT_FALSE(record.isTranscriptReceived());
}

TEST_F(AcademicHistoryTest, ParameterizedConstructor_SetsStudentId) {
    AcademicHistory record("123");

    EXPECT_EQ(record.getStudentId(), "123");
    EXPECT_EQ(record.getInstitutionName(), "");
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(AcademicHistoryTest, CompoundKeyFields_GettersReturnCorrectValues) {
    EXPECT_EQ(highSchoolRecord_.getStudentId(), "1");
    EXPECT_EQ(highSchoolRecord_.getInstitutionName(), "Springfield High School");
    EXPECT_EQ(highSchoolRecord_.getInstitutionType(), "high_school");
}

TEST_F(AcademicHistoryTest, InstitutionFields_GettersReturnCorrectValues) {
    EXPECT_EQ(highSchoolRecord_.getInstitutionCity(), "Springfield");
    EXPECT_EQ(highSchoolRecord_.getInstitutionState(), "IL");
    EXPECT_EQ(highSchoolRecord_.getInstitutionCountry(), "United States");
}

TEST_F(AcademicHistoryTest, AcademicFields_GettersReturnCorrectValues) {
    EXPECT_EQ(undergraduateRecord_.getDegreeEarned(), "Bachelor of Science");
    EXPECT_EQ(undergraduateRecord_.getMajor(), "Computer Science");
    EXPECT_DOUBLE_EQ(undergraduateRecord_.getGpa(), 3.5);
    EXPECT_DOUBLE_EQ(undergraduateRecord_.getGpaScale(), 4.0);
}

TEST_F(AcademicHistoryTest, DateFields_GettersReturnCorrectValues) {
    EXPECT_EQ(highSchoolRecord_.getStartDate(), "2016-08-01");
    EXPECT_EQ(highSchoolRecord_.getEndDate(), "2020-05-15");
}

TEST_F(AcademicHistoryTest, SettersUpdateValues) {
    AcademicHistory record;

    record.setStudentId("999");
    record.setInstitutionName("Test University");
    record.setInstitutionType("undergraduate");
    record.setMajor("Mathematics");
    record.setMinor("Physics");
    record.setGpa(3.75);
    record.setCurrentlyAttending(true);

    EXPECT_EQ(record.getStudentId(), "999");
    EXPECT_EQ(record.getInstitutionName(), "Test University");
    EXPECT_EQ(record.getInstitutionType(), "undergraduate");
    EXPECT_EQ(record.getMajor(), "Mathematics");
    EXPECT_EQ(record.getMinor(), "Physics");
    EXPECT_DOUBLE_EQ(record.getGpa(), 3.75);
    EXPECT_TRUE(record.isCurrentlyAttending());
}

// =============================================================================
// isEmpty Tests
// =============================================================================

TEST_F(AcademicHistoryTest, IsEmpty_ReturnsTrueForEmptyRecord) {
    AcademicHistory record;
    EXPECT_TRUE(record.isEmpty());
}

TEST_F(AcademicHistoryTest, IsEmpty_ReturnsFalseWhenInstitutionNameSet) {
    AcademicHistory record;
    record.setInstitutionName("Test University");
    EXPECT_FALSE(record.isEmpty());
}

TEST_F(AcademicHistoryTest, IsEmpty_ReturnsFalseForCompleteRecord) {
    EXPECT_FALSE(highSchoolRecord_.isEmpty());
}

// =============================================================================
// hasValidKey Tests
// =============================================================================

TEST_F(AcademicHistoryTest, HasValidKey_ReturnsFalseForEmptyRecord) {
    AcademicHistory record;
    EXPECT_FALSE(record.hasValidKey());
}

TEST_F(AcademicHistoryTest, HasValidKey_ReturnsFalseWhenMissingFields) {
    AcademicHistory record("1");
    record.setInstitutionName("Test University");
    // Missing institution type
    EXPECT_FALSE(record.hasValidKey());
}

TEST_F(AcademicHistoryTest, HasValidKey_ReturnsTrueWhenAllKeyFieldsSet) {
    AcademicHistory record("1");
    record.setInstitutionName("Test University");
    record.setInstitutionType("undergraduate");
    EXPECT_TRUE(record.hasValidKey());
}

TEST_F(AcademicHistoryTest, HasValidKey_ReturnsTrueForCompleteRecord) {
    EXPECT_TRUE(highSchoolRecord_.hasValidKey());
    EXPECT_TRUE(undergraduateRecord_.hasValidKey());
    EXPECT_TRUE(vocationalRecord_.hasValidKey());
}

// =============================================================================
// getCompoundKey Tests
// =============================================================================

TEST_F(AcademicHistoryTest, GetCompoundKey_ReturnsFormattedKey) {
    EXPECT_EQ(highSchoolRecord_.getCompoundKey(), "1|Springfield High School|high_school");
}

TEST_F(AcademicHistoryTest, GetCompoundKey_UniquenessForDifferentRecords) {
    EXPECT_NE(highSchoolRecord_.getCompoundKey(), undergraduateRecord_.getCompoundKey());
    EXPECT_NE(undergraduateRecord_.getCompoundKey(), vocationalRecord_.getCompoundKey());
}

// =============================================================================
// Institution Type Tests
// =============================================================================

TEST_F(AcademicHistoryTest, InstitutionTypes_ValidTypes) {
    std::vector<std::string> validTypes = {
        "high_school", "undergraduate", "graduate", "vocational", "vocational_certificate"
    };

    for (const auto& type : validTypes) {
        AcademicHistory record("1");
        record.setInstitutionName("Test Institution");
        record.setInstitutionType(type);

        EXPECT_TRUE(record.hasValidKey()) << "Failed for type: " << type;
        EXPECT_EQ(record.getInstitutionType(), type);
    }
}

// =============================================================================
// GPA Tests
// =============================================================================

TEST_F(AcademicHistoryTest, Gpa_CanBeSetToVariousValues) {
    AcademicHistory record;

    record.setGpa(4.0);
    EXPECT_DOUBLE_EQ(record.getGpa(), 4.0);

    record.setGpa(3.5);
    EXPECT_DOUBLE_EQ(record.getGpa(), 3.5);

    record.setGpa(0.0);
    EXPECT_DOUBLE_EQ(record.getGpa(), 0.0);
}

TEST_F(AcademicHistoryTest, GpaScale_DefaultsAndCustom) {
    AcademicHistory record;

    // Default GPA scale should be 0 (unset)
    EXPECT_DOUBLE_EQ(record.getGpaScale(), 0.0);

    record.setGpaScale(4.0);
    EXPECT_DOUBLE_EQ(record.getGpaScale(), 4.0);

    // Some international scales use 5.0
    record.setGpaScale(5.0);
    EXPECT_DOUBLE_EQ(record.getGpaScale(), 5.0);
}

// =============================================================================
// Currently Attending Tests
// =============================================================================

TEST_F(AcademicHistoryTest, CurrentlyAttending_CanBeToggled) {
    AcademicHistory record;

    EXPECT_FALSE(record.isCurrentlyAttending());

    record.setCurrentlyAttending(true);
    EXPECT_TRUE(record.isCurrentlyAttending());

    record.setCurrentlyAttending(false);
    EXPECT_FALSE(record.isCurrentlyAttending());
}

TEST_F(AcademicHistoryTest, CurrentlyAttending_GraduateRecord) {
    auto gradRecord = TestFixtures::createGraduateRecord("1");
    EXPECT_TRUE(gradRecord.isCurrentlyAttending());
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(AcademicHistoryTest, ToJson_ContainsCompoundKeyFields) {
    auto json = highSchoolRecord_.toJson();

    EXPECT_TRUE(json.contains("student_id") || json.contains("studentId"));
    EXPECT_TRUE(json.contains("institution_name") || json.contains("institutionName"));
    EXPECT_TRUE(json.contains("institution_type") || json.contains("institutionType"));
}

TEST_F(AcademicHistoryTest, ToJson_ContainsInstitutionFields) {
    auto json = highSchoolRecord_.toJson();

    // Check for snake_case or camelCase variants
    bool hasCity = json.contains("institution_city") || json.contains("institutionCity");
    bool hasState = json.contains("institution_state") || json.contains("institutionState");
    bool hasCountry = json.contains("institution_country") || json.contains("institutionCountry");

    EXPECT_TRUE(hasCity);
    EXPECT_TRUE(hasState);
    EXPECT_TRUE(hasCountry);
}

TEST_F(AcademicHistoryTest, ToJson_ContainsAcademicFields) {
    auto json = undergraduateRecord_.toJson();

    bool hasDegree = json.contains("degree_earned") || json.contains("degreeEarned");
    bool hasMajor = json.contains("major");
    bool hasGpa = json.contains("gpa");

    EXPECT_TRUE(hasDegree);
    EXPECT_TRUE(hasMajor);
    EXPECT_TRUE(hasGpa);
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(AcademicHistoryTest, FromJson_ParsesCompoundKeyFields) {
    nlohmann::json json = {
        {"student_id", 1},
        {"institution_name", "Test University"},
        {"institution_type", "undergraduate"}
    };

    AcademicHistory record = AcademicHistory::fromJson(json);

    EXPECT_EQ(record.getStudentId(), "1");
    EXPECT_EQ(record.getInstitutionName(), "Test University");
    EXPECT_EQ(record.getInstitutionType(), "undergraduate");
}

TEST_F(AcademicHistoryTest, FromJson_ParsesAcademicFields) {
    nlohmann::json json = {
        {"student_id", 1},
        {"institution_name", "Test University"},
        {"institution_type", "graduate"},
        {"degree_earned", "Master of Science"},
        {"major", "Computer Science"},
        {"gpa", 3.9},
        {"gpa_scale", 4.0}
    };

    AcademicHistory record = AcademicHistory::fromJson(json);

    EXPECT_EQ(record.getDegreeEarned(), "Master of Science");
    EXPECT_EQ(record.getMajor(), "Computer Science");
    EXPECT_DOUBLE_EQ(record.getGpa(), 3.9);
    EXPECT_DOUBLE_EQ(record.getGpaScale(), 4.0);
}

TEST_F(AcademicHistoryTest, FromJson_ParsesBooleanFields) {
    nlohmann::json json = {
        {"student_id", 1},
        {"institution_name", "Test University"},
        {"institution_type", "undergraduate"},
        {"currently_attending", true},
        {"transcript_received", true}
    };

    AcademicHistory record = AcademicHistory::fromJson(json);

    EXPECT_TRUE(record.isCurrentlyAttending());
    EXPECT_TRUE(record.isTranscriptReceived());
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(AcademicHistoryTest, RoundTrip_PreservesData) {
    auto json = undergraduateRecord_.toJson();
    AcademicHistory reconstructed = AcademicHistory::fromJson(json);

    EXPECT_EQ(reconstructed.getStudentId(), undergraduateRecord_.getStudentId());
    EXPECT_EQ(reconstructed.getInstitutionName(), undergraduateRecord_.getInstitutionName());
    EXPECT_EQ(reconstructed.getInstitutionType(), undergraduateRecord_.getInstitutionType());
    EXPECT_EQ(reconstructed.getMajor(), undergraduateRecord_.getMajor());
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(AcademicHistoryTest, FromJson_HandlesEmptyJson) {
    nlohmann::json json = {};

    AcademicHistory record = AcademicHistory::fromJson(json);

    EXPECT_EQ(record.getStudentId(), "");
    EXPECT_FALSE(record.hasValidKey());
}

TEST_F(AcademicHistoryTest, FromJson_HandlesNullValues) {
    nlohmann::json json = {
        {"student_id", 1},
        {"institution_name", nullptr},
        {"degree_earned", nullptr}
    };

    AcademicHistory record = AcademicHistory::fromJson(json);

    EXPECT_EQ(record.getStudentId(), "1");
    EXPECT_EQ(record.getInstitutionName(), "");
}
