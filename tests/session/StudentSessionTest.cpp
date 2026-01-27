#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "session/StudentSession.h"
#include "utils/TestUtils.h"

using namespace StudentIntake;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class StudentSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        student_ = TestFixtures::createBasicStudent();
    }

    Models::Student student_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(StudentSessionTest, DefaultConstructor_InitializesWithEmptyState) {
    Session::StudentSession session;

    EXPECT_EQ(session.getStudentId(), "");
    EXPECT_EQ(session.getCurrentFormIndex(), 0);
    EXPECT_FALSE(session.isAuthenticated());
}

// =============================================================================
// Student Management Tests
// =============================================================================

TEST_F(StudentSessionTest, SetStudent_UpdatesStudentData) {
    Session::StudentSession session;

    session.setStudent(student_);

    EXPECT_EQ(session.getStudentId(), student_.getId());
    EXPECT_EQ(session.getStudent().getEmail(), student_.getEmail());
}

TEST_F(StudentSessionTest, GetStudent_ReturnsCurrentStudent) {
    Session::StudentSession session;
    session.setStudent(student_);

    auto& retrievedStudent = session.getStudent();

    EXPECT_EQ(retrievedStudent.getFirstName(), "John");
    EXPECT_EQ(retrievedStudent.getLastName(), "Doe");
}

// =============================================================================
// Authentication Tests
// =============================================================================

TEST_F(StudentSessionTest, SetAuthenticated_UpdatesAuthState) {
    Session::StudentSession session;

    EXPECT_FALSE(session.isAuthenticated());

    session.setAuthenticated(true);
    EXPECT_TRUE(session.isAuthenticated());

    session.setAuthenticated(false);
    EXPECT_FALSE(session.isAuthenticated());
}

// =============================================================================
// Form Navigation Tests
// =============================================================================

TEST_F(StudentSessionTest, SetCurrentFormIndex_UpdatesIndex) {
    Session::StudentSession session;

    session.setCurrentFormIndex(0);
    EXPECT_EQ(session.getCurrentFormIndex(), 0);

    session.setCurrentFormIndex(3);
    EXPECT_EQ(session.getCurrentFormIndex(), 3);

    session.setCurrentFormIndex(6);
    EXPECT_EQ(session.getCurrentFormIndex(), 6);
}

TEST_F(StudentSessionTest, NavigateToNextForm_IncrementsIndex) {
    Session::StudentSession session;

    session.setCurrentFormIndex(0);
    session.navigateToNextForm();

    EXPECT_EQ(session.getCurrentFormIndex(), 1);
}

TEST_F(StudentSessionTest, NavigateToPreviousForm_DecrementsIndex) {
    Session::StudentSession session;

    session.setCurrentFormIndex(3);
    session.navigateToPreviousForm();

    EXPECT_EQ(session.getCurrentFormIndex(), 2);
}

TEST_F(StudentSessionTest, NavigateToPreviousForm_DoesNotGoBelowZero) {
    Session::StudentSession session;

    session.setCurrentFormIndex(0);
    session.navigateToPreviousForm();

    EXPECT_EQ(session.getCurrentFormIndex(), 0);
}

// =============================================================================
// Curriculum Tests
// =============================================================================

TEST_F(StudentSessionTest, SetCurriculumId_UpdatesCurriculum) {
    Session::StudentSession session;

    session.setCurriculumId("10");
    EXPECT_EQ(session.getCurriculumId(), "10");
}

TEST_F(StudentSessionTest, Curriculum_CanBeChanged) {
    Session::StudentSession session;

    session.setCurriculumId("10");
    EXPECT_EQ(session.getCurriculumId(), "10");

    session.setCurriculumId("20");
    EXPECT_EQ(session.getCurriculumId(), "20");
}

// =============================================================================
// Endorsement Tests
// =============================================================================

TEST_F(StudentSessionTest, AddEndorsement_AddsToList) {
    Session::StudentSession session;

    session.addEndorsementId("100");
    EXPECT_TRUE(session.hasEndorsement("100"));
}

TEST_F(StudentSessionTest, AddEndorsement_DoesNotDuplicate) {
    Session::StudentSession session;

    session.addEndorsementId("100");
    session.addEndorsementId("100");

    EXPECT_EQ(session.getEndorsementIds().size(), 1);
}

TEST_F(StudentSessionTest, RemoveEndorsement_RemovesFromList) {
    Session::StudentSession session;

    session.addEndorsementId("100");
    session.addEndorsementId("200");
    session.removeEndorsementId("100");

    EXPECT_FALSE(session.hasEndorsement("100"));
    EXPECT_TRUE(session.hasEndorsement("200"));
}

TEST_F(StudentSessionTest, ClearEndorsements_RemovesAll) {
    Session::StudentSession session;

    session.addEndorsementId("100");
    session.addEndorsementId("200");
    session.addEndorsementId("300");
    session.clearEndorsements();

    EXPECT_TRUE(session.getEndorsementIds().empty());
}

// =============================================================================
// Form Data Cache Tests
// =============================================================================

TEST_F(StudentSessionTest, SetFormData_StoresData) {
    Session::StudentSession session;

    nlohmann::json formData = {
        {"first_name", "John"},
        {"last_name", "Doe"}
    };

    session.setFormData("personal_info", formData);

    EXPECT_TRUE(session.hasFormData("personal_info"));
}

TEST_F(StudentSessionTest, GetFormData_RetrievesStoredData) {
    Session::StudentSession session;

    nlohmann::json formData = {
        {"first_name", "John"},
        {"last_name", "Doe"}
    };

    session.setFormData("personal_info", formData);
    auto retrieved = session.getFormData("personal_info");

    EXPECT_EQ(retrieved["first_name"], "John");
    EXPECT_EQ(retrieved["last_name"], "Doe");
}

TEST_F(StudentSessionTest, HasFormData_ReturnsFalseForMissingData) {
    Session::StudentSession session;

    EXPECT_FALSE(session.hasFormData("nonexistent_form"));
}

TEST_F(StudentSessionTest, ClearFormData_RemovesSpecificForm) {
    Session::StudentSession session;

    session.setFormData("personal_info", {{"key", "value1"}});
    session.setFormData("emergency_contact", {{"key", "value2"}});

    session.clearFormData("personal_info");

    EXPECT_FALSE(session.hasFormData("personal_info"));
    EXPECT_TRUE(session.hasFormData("emergency_contact"));
}

TEST_F(StudentSessionTest, ClearAllFormData_RemovesAllForms) {
    Session::StudentSession session;

    session.setFormData("personal_info", {{"key", "value1"}});
    session.setFormData("emergency_contact", {{"key", "value2"}});
    session.setFormData("academic_history", {{"key", "value3"}});

    session.clearAllFormData();

    EXPECT_FALSE(session.hasFormData("personal_info"));
    EXPECT_FALSE(session.hasFormData("emergency_contact"));
    EXPECT_FALSE(session.hasFormData("academic_history"));
}

// =============================================================================
// Session Reset Tests
// =============================================================================

TEST_F(StudentSessionTest, Reset_ClearsAllState) {
    Session::StudentSession session;

    // Set up some state
    session.setStudent(student_);
    session.setAuthenticated(true);
    session.setCurrentFormIndex(5);
    session.setCurriculumId("10");
    session.addEndorsementId("100");
    session.setFormData("personal_info", {{"key", "value"}});

    // Reset
    session.reset();

    // Verify all state is cleared
    EXPECT_EQ(session.getStudentId(), "");
    EXPECT_FALSE(session.isAuthenticated());
    EXPECT_EQ(session.getCurrentFormIndex(), 0);
    EXPECT_EQ(session.getCurriculumId(), "");
    EXPECT_TRUE(session.getEndorsementIds().empty());
    EXPECT_FALSE(session.hasFormData("personal_info"));
}

// =============================================================================
// Form Completion Tracking Tests
// =============================================================================

TEST_F(StudentSessionTest, MarkFormCompleted_TracksCompletion) {
    Session::StudentSession session;
    session.setStudent(student_);

    session.markFormCompleted("personal_info");

    EXPECT_TRUE(session.isFormCompleted("personal_info"));
}

TEST_F(StudentSessionTest, IsFormCompleted_ReturnsFalseForIncomplete) {
    Session::StudentSession session;
    session.setStudent(student_);

    EXPECT_FALSE(session.isFormCompleted("personal_info"));
}

TEST_F(StudentSessionTest, GetCompletedForms_ReturnsAllCompleted) {
    Session::StudentSession session;
    session.setStudent(student_);

    session.markFormCompleted("personal_info");
    session.markFormCompleted("emergency_contact");
    session.markFormCompleted("consent");

    auto completed = session.getCompletedForms();

    EXPECT_EQ(completed.size(), 3);
}

// =============================================================================
// Progress Calculation Tests
// =============================================================================

TEST_F(StudentSessionTest, GetProgress_ReturnsZeroWhenNoFormsCompleted) {
    Session::StudentSession session;
    session.setStudent(student_);
    session.setTotalForms(7);

    EXPECT_EQ(session.getProgress(), 0);
}

TEST_F(StudentSessionTest, GetProgress_CalculatesCorrectPercentage) {
    Session::StudentSession session;
    session.setStudent(student_);
    session.setTotalForms(4);

    session.markFormCompleted("personal_info");
    session.markFormCompleted("emergency_contact");

    // 2 out of 4 = 50%
    EXPECT_EQ(session.getProgress(), 50);
}

TEST_F(StudentSessionTest, GetProgress_Returns100WhenAllCompleted) {
    Session::StudentSession session;
    session.setStudent(student_);
    session.setTotalForms(3);

    session.markFormCompleted("form1");
    session.markFormCompleted("form2");
    session.markFormCompleted("form3");

    EXPECT_EQ(session.getProgress(), 100);
}

// =============================================================================
// Validation State Tests
// =============================================================================

TEST_F(StudentSessionTest, SetValidationErrors_StoresErrors) {
    Session::StudentSession session;

    std::vector<std::string> errors = {
        "First name is required",
        "Email is invalid"
    };

    session.setValidationErrors("personal_info", errors);

    EXPECT_TRUE(session.hasValidationErrors("personal_info"));
}

TEST_F(StudentSessionTest, GetValidationErrors_RetrievesErrors) {
    Session::StudentSession session;

    std::vector<std::string> errors = {"Error 1", "Error 2"};
    session.setValidationErrors("personal_info", errors);

    auto retrieved = session.getValidationErrors("personal_info");

    EXPECT_EQ(retrieved.size(), 2);
    EXPECT_EQ(retrieved[0], "Error 1");
}

TEST_F(StudentSessionTest, ClearValidationErrors_RemovesErrors) {
    Session::StudentSession session;

    session.setValidationErrors("personal_info", {"Error"});
    session.clearValidationErrors("personal_info");

    EXPECT_FALSE(session.hasValidationErrors("personal_info"));
}

// =============================================================================
// Mode Tests
// =============================================================================

TEST_F(StudentSessionTest, SetMode_UpdatesMode) {
    Session::StudentSession session;

    session.setMode("vocational");
    EXPECT_EQ(session.getMode(), "vocational");

    session.setMode("accredited");
    EXPECT_EQ(session.getMode(), "accredited");
}

TEST_F(StudentSessionTest, IsVocationalMode_ReturnsCorrectValue) {
    Session::StudentSession session;

    session.setMode("vocational");
    EXPECT_TRUE(session.isVocationalMode());

    session.setMode("accredited");
    EXPECT_FALSE(session.isVocationalMode());
}
