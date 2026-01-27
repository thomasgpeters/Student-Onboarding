#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/Curriculum.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class CurriculumTest : public ::testing::Test {
protected:
    void SetUp() override {
        accreditedProgram_ = TestFixtures::createAccreditedProgram();
        vocationalProgram_ = TestFixtures::createVocationalProgram();
        endorsementProgram_ = TestFixtures::createEndorsementProgram();
    }

    Curriculum accreditedProgram_;
    Curriculum vocationalProgram_;
    Curriculum endorsementProgram_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(CurriculumTest, DefaultConstructor_InitializesWithDefaults) {
    Curriculum curriculum;

    EXPECT_EQ(curriculum.getId(), "");
    EXPECT_EQ(curriculum.getName(), "");
    EXPECT_EQ(curriculum.getCode(), "");
    EXPECT_EQ(curriculum.getDescription(), "");
    EXPECT_FALSE(curriculum.isActive());
    EXPECT_FALSE(curriculum.isEndorsement());
    EXPECT_TRUE(curriculum.getRequiredForms().empty());
}

TEST_F(CurriculumTest, ParameterizedConstructor_SetsIdAndName) {
    Curriculum curriculum("123", "Test Program");

    EXPECT_EQ(curriculum.getId(), "123");
    EXPECT_EQ(curriculum.getName(), "Test Program");
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(CurriculumTest, BasicGettersReturnCorrectValues) {
    EXPECT_EQ(accreditedProgram_.getId(), "1");
    EXPECT_EQ(accreditedProgram_.getName(), "Bachelor of Science in Computer Science");
    EXPECT_EQ(accreditedProgram_.getCode(), "BSCS");
    EXPECT_EQ(accreditedProgram_.getDescription(), "A comprehensive computer science program");
}

TEST_F(CurriculumTest, AcademicFieldsGettersReturnCorrectValues) {
    EXPECT_EQ(accreditedProgram_.getDegreeType(), "bachelor");
    EXPECT_EQ(accreditedProgram_.getCreditHours(), 120);
    EXPECT_EQ(accreditedProgram_.getDurationSemesters(), 8);
    EXPECT_EQ(accreditedProgram_.getDurationInterval(), "semester");
}

TEST_F(CurriculumTest, VocationalProgram_HasCdlClass) {
    EXPECT_EQ(vocationalProgram_.getCdlClass(), "A");
    EXPECT_EQ(vocationalProgram_.getDegreeType(), "certificate");
}

TEST_F(CurriculumTest, EndorsementProgram_IsEndorsement) {
    EXPECT_TRUE(endorsementProgram_.isEndorsement());
    EXPECT_FALSE(accreditedProgram_.isEndorsement());
    EXPECT_FALSE(vocationalProgram_.isEndorsement());
}

TEST_F(CurriculumTest, SettersUpdateValues) {
    Curriculum curriculum;

    curriculum.setId("999");
    curriculum.setName("New Program");
    curriculum.setCode("NEWP");
    curriculum.setDescription("A new test program");
    curriculum.setDegreeType("master");
    curriculum.setCreditHours(36);
    curriculum.setActive(true);
    curriculum.setOnline(true);

    EXPECT_EQ(curriculum.getId(), "999");
    EXPECT_EQ(curriculum.getName(), "New Program");
    EXPECT_EQ(curriculum.getCode(), "NEWP");
    EXPECT_EQ(curriculum.getDegreeType(), "master");
    EXPECT_EQ(curriculum.getCreditHours(), 36);
    EXPECT_TRUE(curriculum.isActive());
    EXPECT_TRUE(curriculum.isOnline());
}

// =============================================================================
// isActive Tests
// =============================================================================

TEST_F(CurriculumTest, IsActive_ReturnsTrueForActiveProgram) {
    EXPECT_TRUE(accreditedProgram_.isActive());
    EXPECT_TRUE(vocationalProgram_.isActive());
}

TEST_F(CurriculumTest, IsActive_CanBeToggled) {
    Curriculum curriculum;

    EXPECT_FALSE(curriculum.isActive());

    curriculum.setActive(true);
    EXPECT_TRUE(curriculum.isActive());

    curriculum.setActive(false);
    EXPECT_FALSE(curriculum.isActive());
}

// =============================================================================
// Required Forms Tests
// =============================================================================

TEST_F(CurriculumTest, AddRequiredForm_AddsForm) {
    Curriculum curriculum;

    curriculum.addRequiredForm("personal_info");
    EXPECT_TRUE(curriculum.requiresForm("personal_info"));
    EXPECT_EQ(curriculum.getRequiredForms().size(), 1);
}

TEST_F(CurriculumTest, AddRequiredForm_DoesNotDuplicateForms) {
    Curriculum curriculum;

    curriculum.addRequiredForm("personal_info");
    curriculum.addRequiredForm("personal_info");

    EXPECT_EQ(curriculum.getRequiredForms().size(), 1);
}

TEST_F(CurriculumTest, RemoveRequiredForm_RemovesForm) {
    Curriculum curriculum;

    curriculum.addRequiredForm("personal_info");
    curriculum.addRequiredForm("emergency_contact");
    curriculum.removeRequiredForm("personal_info");

    EXPECT_FALSE(curriculum.requiresForm("personal_info"));
    EXPECT_TRUE(curriculum.requiresForm("emergency_contact"));
    EXPECT_EQ(curriculum.getRequiredForms().size(), 1);
}

TEST_F(CurriculumTest, RequiresForm_ReturnsFalseForUnrequiredForm) {
    Curriculum curriculum;
    curriculum.addRequiredForm("personal_info");

    EXPECT_FALSE(curriculum.requiresForm("emergency_contact"));
}

TEST_F(CurriculumTest, SetRequiredForms_ReplacesAllForms) {
    Curriculum curriculum;
    curriculum.addRequiredForm("personal_info");

    std::vector<std::string> newForms = {"academic_history", "financial_aid", "consent"};
    curriculum.setRequiredForms(newForms);

    EXPECT_FALSE(curriculum.requiresForm("personal_info"));
    EXPECT_TRUE(curriculum.requiresForm("academic_history"));
    EXPECT_TRUE(curriculum.requiresForm("financial_aid"));
    EXPECT_TRUE(curriculum.requiresForm("consent"));
    EXPECT_EQ(curriculum.getRequiredForms().size(), 3);
}

// =============================================================================
// Form ID Mapping Tests
// =============================================================================

TEST_F(CurriculumTest, FormIdToTypeId_MapsCorrectly) {
    EXPECT_EQ(Curriculum::formIdToTypeId("personal_info"), 1);
    EXPECT_EQ(Curriculum::formIdToTypeId("emergency_contact"), 2);
    EXPECT_EQ(Curriculum::formIdToTypeId("academic_history"), 3);
}

TEST_F(CurriculumTest, TypeIdToFormId_MapsCorrectly) {
    EXPECT_EQ(Curriculum::typeIdToFormId(1), "personal_info");
    EXPECT_EQ(Curriculum::typeIdToFormId(2), "emergency_contact");
    EXPECT_EQ(Curriculum::typeIdToFormId(3), "academic_history");
}

TEST_F(CurriculumTest, FormIdMapping_RoundTrip) {
    std::vector<std::string> formIds = {
        "personal_info", "emergency_contact", "academic_history",
        "medical_info", "financial_aid", "document_upload", "consent"
    };

    for (const auto& formId : formIds) {
        int typeId = Curriculum::formIdToTypeId(formId);
        if (typeId != -1) {  // Only test valid mappings
            std::string reconstructed = Curriculum::typeIdToFormId(typeId);
            EXPECT_EQ(reconstructed, formId) << "Failed round-trip for: " << formId;
        }
    }
}

// =============================================================================
// Duration Formatting Tests
// =============================================================================

TEST_F(CurriculumTest, GetFormattedDuration_Semesters) {
    Curriculum curriculum;
    curriculum.setDurationSemesters(4);
    curriculum.setDurationInterval("semester");

    std::string formatted = curriculum.getFormattedDuration();
    EXPECT_TRUE(formatted.find("4") != std::string::npos);
    EXPECT_TRUE(formatted.find("emester") != std::string::npos);
}

TEST_F(CurriculumTest, GetFormattedDuration_Weeks) {
    Curriculum curriculum;
    curriculum.setDurationSemesters(4);
    curriculum.setDurationInterval("week");

    std::string formatted = curriculum.getFormattedDuration();
    EXPECT_TRUE(formatted.find("4") != std::string::npos);
    EXPECT_TRUE(formatted.find("eek") != std::string::npos);
}

// =============================================================================
// Department Tests
// =============================================================================

TEST_F(CurriculumTest, DepartmentFields_CanBeSetAndRetrieved) {
    Curriculum curriculum;

    curriculum.setDepartment("Computer Science");
    curriculum.setDepartmentId(5);

    EXPECT_EQ(curriculum.getDepartment(), "Computer Science");
    EXPECT_EQ(curriculum.getDepartmentId(), 5);
}

TEST_F(CurriculumTest, DepartmentIdToName_MapsCorrectly) {
    // Test that the static method works for known department IDs
    std::string name = Curriculum::departmentIdToName(1);
    EXPECT_FALSE(name.empty());
}

// =============================================================================
// CDL Class Tests
// =============================================================================

TEST_F(CurriculumTest, CdlClass_CanBeSetToClassA) {
    Curriculum curriculum;
    curriculum.setCdlClass("A");
    EXPECT_EQ(curriculum.getCdlClass(), "A");
}

TEST_F(CurriculumTest, CdlClass_CanBeSetToClassB) {
    Curriculum curriculum;
    curriculum.setCdlClass("B");
    EXPECT_EQ(curriculum.getCdlClass(), "B");
}

TEST_F(CurriculumTest, CdlClass_CanBeEmpty) {
    Curriculum curriculum;
    EXPECT_EQ(curriculum.getCdlClass(), "");
}

// =============================================================================
// Degree Type Tests
// =============================================================================

TEST_F(CurriculumTest, DegreeTypes_ValidTypes) {
    std::vector<std::string> validTypes = {
        "bachelor", "master", "doctoral", "certificate", "associate"
    };

    for (const auto& type : validTypes) {
        Curriculum curriculum;
        curriculum.setDegreeType(type);
        EXPECT_EQ(curriculum.getDegreeType(), type);
    }
}

// =============================================================================
// Prerequisites Tests
// =============================================================================

TEST_F(CurriculumTest, Prerequisites_CanBeSetAndRetrieved) {
    Curriculum curriculum;

    std::vector<std::string> prereqs = {"MATH101", "CS100", "PHYS101"};
    curriculum.setPrerequisites(prereqs);

    EXPECT_EQ(curriculum.getPrerequisites().size(), 3);
    EXPECT_EQ(curriculum.getPrerequisites()[0], "MATH101");
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(CurriculumTest, ToJson_ContainsBasicFields) {
    auto json = accreditedProgram_.toJson();

    EXPECT_TRUE(json.contains("id"));
    EXPECT_TRUE(json.contains("name"));
    EXPECT_TRUE(json.contains("code"));
}

TEST_F(CurriculumTest, ToJson_ContainsAcademicFields) {
    auto json = accreditedProgram_.toJson();

    bool hasDegreeType = json.contains("degree_type") || json.contains("degreeType");
    bool hasCreditHours = json.contains("credit_hours") || json.contains("creditHours");

    EXPECT_TRUE(hasDegreeType);
    EXPECT_TRUE(hasCreditHours);
}

TEST_F(CurriculumTest, ToJson_ContainsBooleanFields) {
    auto json = accreditedProgram_.toJson();

    bool hasActive = json.contains("is_active") || json.contains("isActive") || json.contains("active");

    EXPECT_TRUE(hasActive);
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(CurriculumTest, FromJson_ParsesBasicFields) {
    nlohmann::json json = {
        {"id", "1"},
        {"name", "Test Program"},
        {"code", "TEST"},
        {"description", "A test program"}
    };

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_EQ(curriculum.getId(), "1");
    EXPECT_EQ(curriculum.getName(), "Test Program");
    EXPECT_EQ(curriculum.getCode(), "TEST");
    EXPECT_EQ(curriculum.getDescription(), "A test program");
}

TEST_F(CurriculumTest, FromJson_ParsesAcademicFields) {
    nlohmann::json json = {
        {"id", "1"},
        {"name", "Test Program"},
        {"degree_type", "master"},
        {"credit_hours", 36},
        {"duration_semesters", 4}
    };

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_EQ(curriculum.getDegreeType(), "master");
    EXPECT_EQ(curriculum.getCreditHours(), 36);
    EXPECT_EQ(curriculum.getDurationSemesters(), 4);
}

TEST_F(CurriculumTest, FromJson_ParsesBooleanFields) {
    nlohmann::json json = {
        {"id", "1"},
        {"name", "Test Program"},
        {"is_active", true},
        {"is_online", true},
        {"is_endorsement", true}
    };

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_TRUE(curriculum.isActive());
    EXPECT_TRUE(curriculum.isOnline());
    EXPECT_TRUE(curriculum.isEndorsement());
}

TEST_F(CurriculumTest, FromJson_ParsesRequiredForms) {
    nlohmann::json json = {
        {"id", "1"},
        {"name", "Test Program"},
        {"required_forms", nlohmann::json::array({"personal_info", "consent"})}
    };

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_TRUE(curriculum.requiresForm("personal_info"));
    EXPECT_TRUE(curriculum.requiresForm("consent"));
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(CurriculumTest, RoundTrip_PreservesData) {
    auto json = accreditedProgram_.toJson();
    Curriculum reconstructed = Curriculum::fromJson(json);

    EXPECT_EQ(reconstructed.getId(), accreditedProgram_.getId());
    EXPECT_EQ(reconstructed.getName(), accreditedProgram_.getName());
    EXPECT_EQ(reconstructed.getCode(), accreditedProgram_.getCode());
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(CurriculumTest, FromJson_HandlesEmptyJson) {
    nlohmann::json json = {};

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_EQ(curriculum.getId(), "");
    EXPECT_EQ(curriculum.getName(), "");
}

TEST_F(CurriculumTest, FromJson_HandlesNullValues) {
    nlohmann::json json = {
        {"id", "1"},
        {"name", nullptr},
        {"description", nullptr}
    };

    Curriculum curriculum = Curriculum::fromJson(json);

    EXPECT_EQ(curriculum.getId(), "1");
    EXPECT_EQ(curriculum.getName(), "");
}

TEST_F(CurriculumTest, FromJson_HandlesIntegerId) {
    nlohmann::json json = {
        {"id", 123},
        {"name", "Test Program"}
    };

    Curriculum curriculum = Curriculum::fromJson(json);
    EXPECT_EQ(curriculum.getId(), "123");
}
