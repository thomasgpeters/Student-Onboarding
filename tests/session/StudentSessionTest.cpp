#include <gtest/gtest.h>
#include "session/StudentSession.h"
#include "models/Student.h"
#include "models/FormData.h"
#include "models/Curriculum.h"
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

    EXPECT_EQ(session.getStudent().getId(), "");
    EXPECT_EQ(session.getCurrentFormId(), "");
    EXPECT_FALSE(session.isLoggedIn());
}

// =============================================================================
// Student Management Tests
// =============================================================================

TEST_F(StudentSessionTest, SetStudent_UpdatesStudentData) {
    Session::StudentSession session;

    session.setStudent(student_);

    EXPECT_EQ(session.getStudent().getId(), student_.getId());
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

TEST_F(StudentSessionTest, SetLoggedIn_UpdatesAuthState) {
    Session::StudentSession session;

    EXPECT_FALSE(session.isLoggedIn());

    session.setLoggedIn(true);
    EXPECT_TRUE(session.isLoggedIn());

    session.setLoggedIn(false);
    EXPECT_FALSE(session.isLoggedIn());
}

TEST_F(StudentSessionTest, SetAuthToken_StoresToken) {
    Session::StudentSession session;

    session.setAuthToken("test-token-123");
    EXPECT_EQ(session.getAuthToken(), "test-token-123");
}

// =============================================================================
// Form Navigation Tests
// =============================================================================

TEST_F(StudentSessionTest, SetCurrentFormId_UpdatesFormId) {
    Session::StudentSession session;

    session.setCurrentFormId("personal_info");
    EXPECT_EQ(session.getCurrentFormId(), "personal_info");

    session.setCurrentFormId("emergency_contact");
    EXPECT_EQ(session.getCurrentFormId(), "emergency_contact");
}

// =============================================================================
// Curriculum Tests
// =============================================================================

TEST_F(StudentSessionTest, SetCurrentCurriculum_UpdatesCurriculum) {
    Session::StudentSession session;

    Models::Curriculum curriculum;
    curriculum.setId("10");
    curriculum.setName("Test Program");

    session.setCurrentCurriculum(curriculum);

    EXPECT_EQ(session.getCurrentCurriculum().getId(), "10");
    EXPECT_EQ(session.getCurrentCurriculum().getName(), "Test Program");
}

TEST_F(StudentSessionTest, HasCurriculumSelected_ReturnsTrueWhenSet) {
    Session::StudentSession session;

    EXPECT_FALSE(session.hasCurriculumSelected());

    Models::Curriculum curriculum;
    curriculum.setId("10");
    session.setCurrentCurriculum(curriculum);

    EXPECT_TRUE(session.hasCurriculumSelected());
}

// =============================================================================
// Endorsement Tests
// =============================================================================

TEST_F(StudentSessionTest, AddSelectedEndorsement_AddsToList) {
    Session::StudentSession session;

    Models::Curriculum endorsement;
    endorsement.setId("100");
    endorsement.setName("Endorsement 1");

    session.addSelectedEndorsement(endorsement);

    EXPECT_EQ(session.getSelectedEndorsements().size(), 1);
    EXPECT_EQ(session.getSelectedEndorsements()[0].getId(), "100");
}

TEST_F(StudentSessionTest, ClearSelectedEndorsements_RemovesAll) {
    Session::StudentSession session;

    Models::Curriculum e1, e2;
    e1.setId("100");
    e2.setId("200");

    session.addSelectedEndorsement(e1);
    session.addSelectedEndorsement(e2);
    session.clearSelectedEndorsements();

    EXPECT_TRUE(session.getSelectedEndorsements().empty());
}

// =============================================================================
// Form Data Cache Tests
// =============================================================================

TEST_F(StudentSessionTest, SetFormData_StoresData) {
    Session::StudentSession session;

    Models::FormData formData;
    formData.setFormId("personal_info");
    formData.setField("first_name", "John");

    session.setFormData("personal_info", formData);

    EXPECT_TRUE(session.hasFormData("personal_info"));
}

TEST_F(StudentSessionTest, GetFormData_RetrievesStoredData) {
    Session::StudentSession session;

    Models::FormData formData;
    formData.setFormId("personal_info");
    formData.setField("first_name", "John");
    formData.setField("last_name", "Doe");

    session.setFormData("personal_info", formData);
    auto retrieved = session.getFormData("personal_info");

    EXPECT_EQ(retrieved.getFormId(), "personal_info");
}

TEST_F(StudentSessionTest, HasFormData_ReturnsFalseForMissingData) {
    Session::StudentSession session;

    EXPECT_FALSE(session.hasFormData("nonexistent_form"));
}

TEST_F(StudentSessionTest, ClearFormData_RemovesSpecificForm) {
    Session::StudentSession session;

    Models::FormData fd1, fd2;
    fd1.setFormId("personal_info");
    fd2.setFormId("emergency_contact");

    session.setFormData("personal_info", fd1);
    session.setFormData("emergency_contact", fd2);

    session.clearFormData("personal_info");

    EXPECT_FALSE(session.hasFormData("personal_info"));
    EXPECT_TRUE(session.hasFormData("emergency_contact"));
}

TEST_F(StudentSessionTest, ClearAllFormData_RemovesAllForms) {
    Session::StudentSession session;

    Models::FormData fd1, fd2, fd3;
    fd1.setFormId("personal_info");
    fd2.setFormId("emergency_contact");
    fd3.setFormId("academic_history");

    session.setFormData("personal_info", fd1);
    session.setFormData("emergency_contact", fd2);
    session.setFormData("academic_history", fd3);

    session.clearAllFormData();

    EXPECT_FALSE(session.hasFormData("personal_info"));
    EXPECT_FALSE(session.hasFormData("emergency_contact"));
    EXPECT_FALSE(session.hasFormData("academic_history"));
}

// =============================================================================
// Required Forms Tests
// =============================================================================

TEST_F(StudentSessionTest, SetRequiredFormIds_StoresFormIds) {
    Session::StudentSession session;

    std::vector<std::string> formIds = {"personal_info", "emergency_contact", "consent"};
    session.setRequiredFormIds(formIds);

    EXPECT_EQ(session.getRequiredFormIds().size(), 3);
}

TEST_F(StudentSessionTest, AddRequiredFormId_AddsToList) {
    Session::StudentSession session;

    session.addRequiredFormId("personal_info");
    session.addRequiredFormId("emergency_contact");

    EXPECT_EQ(session.getRequiredFormIds().size(), 2);
}

TEST_F(StudentSessionTest, IsFormRequired_ReturnsCorrectValue) {
    Session::StudentSession session;

    session.addRequiredFormId("personal_info");
    session.addRequiredFormId("consent");

    EXPECT_TRUE(session.isFormRequired("personal_info"));
    EXPECT_TRUE(session.isFormRequired("consent"));
    EXPECT_FALSE(session.isFormRequired("optional_form"));
}

// =============================================================================
// Session Reset Tests
// =============================================================================

TEST_F(StudentSessionTest, Reset_ClearsAllState) {
    Session::StudentSession session;

    // Set up some state
    session.setStudent(student_);
    session.setLoggedIn(true);
    session.setCurrentFormId("personal_info");

    Models::Curriculum curriculum;
    curriculum.setId("10");
    session.setCurrentCurriculum(curriculum);

    Models::FormData formData;
    formData.setFormId("personal_info");
    session.setFormData("personal_info", formData);

    // Reset
    session.reset();

    // Verify state is cleared
    EXPECT_EQ(session.getStudent().getId(), "");
    EXPECT_FALSE(session.isLoggedIn());
    EXPECT_EQ(session.getCurrentFormId(), "");
    EXPECT_FALSE(session.hasFormData("personal_info"));
}

TEST_F(StudentSessionTest, Logout_ClearsAuthState) {
    Session::StudentSession session;

    session.setLoggedIn(true);
    session.setAuthToken("test-token");

    session.logout();

    EXPECT_FALSE(session.isLoggedIn());
}

// =============================================================================
// Progress Tracking Tests
// =============================================================================

TEST_F(StudentSessionTest, GetProgressPercentage_CalculatesCorrectly) {
    Session::StudentSession session;

    // Set required forms
    std::vector<std::string> required = {"form1", "form2", "form3", "form4"};
    session.setRequiredFormIds(required);

    // Complete half of them by adding form data
    Models::FormData fd1, fd2;
    fd1.setFormId("form1");
    fd2.setFormId("form2");
    session.setFormData("form1", fd1);
    session.setFormData("form2", fd2);

    // 2 out of 4 = 50%
    EXPECT_EQ(session.getCompletedFormsCount(), 2);
    EXPECT_EQ(session.getTotalRequiredFormsCount(), 4);
}

TEST_F(StudentSessionTest, IsIntakeComplete_ReturnsTrueWhenAllFormsComplete) {
    Session::StudentSession session;

    std::vector<std::string> required = {"form1", "form2"};
    session.setRequiredFormIds(required);

    // Initially not complete
    EXPECT_FALSE(session.isIntakeComplete());

    // Complete all forms
    Models::FormData fd1, fd2;
    fd1.setFormId("form1");
    fd2.setFormId("form2");
    session.setFormData("form1", fd1);
    session.setFormData("form2", fd2);

    EXPECT_TRUE(session.isIntakeComplete());
}

// =============================================================================
// Session ID Tests
// =============================================================================

TEST_F(StudentSessionTest, SetSessionId_StoresId) {
    Session::StudentSession session;

    session.setSessionId("session-abc-123");
    EXPECT_EQ(session.getSessionId(), "session-abc-123");
}
