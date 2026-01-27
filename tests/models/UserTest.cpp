#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/User.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// User Test Fixture
// =============================================================================

class UserTest : public ::testing::Test {
protected:
    void SetUp() override {
        studentUser_ = TestFixtures::createStudentUser();
        instructorUser_ = TestFixtures::createInstructorUser();
        adminUser_ = TestFixtures::createAdminUser();
        multiRoleUser_ = TestFixtures::createMultiRoleUser();
    }

    User studentUser_;
    User instructorUser_;
    User adminUser_;
    User multiRoleUser_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(UserTest, DefaultConstructor_InitializesWithDefaults) {
    User user;

    EXPECT_EQ(user.getId(), 0);
    EXPECT_EQ(user.getEmail(), "");
    EXPECT_EQ(user.getFirstName(), "");
    EXPECT_EQ(user.getLastName(), "");
    EXPECT_FALSE(user.isActive());
    EXPECT_FALSE(user.isLoginEnabled());
    EXPECT_FALSE(user.isEmailVerified());
    EXPECT_EQ(user.getFailedLoginAttempts(), 0);
    EXPECT_TRUE(user.getRoles().empty());
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(UserTest, BasicGettersReturnCorrectValues) {
    EXPECT_EQ(studentUser_.getId(), 1);
    EXPECT_EQ(studentUser_.getEmail(), "student@example.com");
    EXPECT_EQ(studentUser_.getFirstName(), "Test");
    EXPECT_EQ(studentUser_.getLastName(), "Student");
    EXPECT_EQ(studentUser_.getPhoneNumber(), "555-100-1000");
}

TEST_F(UserTest, StatusGettersReturnCorrectValues) {
    EXPECT_TRUE(studentUser_.isActive());
    EXPECT_TRUE(studentUser_.isLoginEnabled());
    EXPECT_TRUE(studentUser_.isEmailVerified());
}

TEST_F(UserTest, SettersUpdateValues) {
    User user;

    user.setId(999);
    user.setEmail("new@example.com");
    user.setFirstName("New");
    user.setLastName("User");
    user.setActive(true);
    user.setLoginEnabled(true);
    user.setEmailVerified(true);
    user.setFailedLoginAttempts(3);

    EXPECT_EQ(user.getId(), 999);
    EXPECT_EQ(user.getEmail(), "new@example.com");
    EXPECT_EQ(user.getFirstName(), "New");
    EXPECT_EQ(user.getLastName(), "User");
    EXPECT_TRUE(user.isActive());
    EXPECT_TRUE(user.isLoginEnabled());
    EXPECT_TRUE(user.isEmailVerified());
    EXPECT_EQ(user.getFailedLoginAttempts(), 3);
}

// =============================================================================
// Role Management Tests
// =============================================================================

TEST_F(UserTest, HasRole_ReturnsTrueForAssignedRole) {
    EXPECT_TRUE(studentUser_.hasRole(UserRole::Student));
    EXPECT_TRUE(instructorUser_.hasRole(UserRole::Instructor));
    EXPECT_TRUE(adminUser_.hasRole(UserRole::Admin));
}

TEST_F(UserTest, HasRole_ReturnsFalseForUnassignedRole) {
    EXPECT_FALSE(studentUser_.hasRole(UserRole::Admin));
    EXPECT_FALSE(studentUser_.hasRole(UserRole::Instructor));
    EXPECT_FALSE(instructorUser_.hasRole(UserRole::Student));
}

TEST_F(UserTest, IsStudent_ReturnsTrueForStudentRole) {
    EXPECT_TRUE(studentUser_.isStudent());
    EXPECT_FALSE(studentUser_.isInstructor());
    EXPECT_FALSE(studentUser_.isAdmin());
}

TEST_F(UserTest, IsInstructor_ReturnsTrueForInstructorRole) {
    EXPECT_FALSE(instructorUser_.isStudent());
    EXPECT_TRUE(instructorUser_.isInstructor());
    EXPECT_FALSE(instructorUser_.isAdmin());
}

TEST_F(UserTest, IsAdmin_ReturnsTrueForAdminRole) {
    EXPECT_FALSE(adminUser_.isStudent());
    EXPECT_FALSE(adminUser_.isInstructor());
    EXPECT_TRUE(adminUser_.isAdmin());
}

TEST_F(UserTest, HasMultipleRoles_ReturnsTrueForMultiRoleUser) {
    EXPECT_TRUE(multiRoleUser_.hasMultipleRoles());
    EXPECT_FALSE(studentUser_.hasMultipleRoles());
}

TEST_F(UserTest, MultiRoleUser_HasAllAssignedRoles) {
    EXPECT_TRUE(multiRoleUser_.isStudent());
    EXPECT_TRUE(multiRoleUser_.isInstructor());
    EXPECT_FALSE(multiRoleUser_.isAdmin());
}

TEST_F(UserTest, AddRole_AddsNewRole) {
    User user;

    user.addRole(UserRole::Student);
    EXPECT_TRUE(user.hasRole(UserRole::Student));
    EXPECT_EQ(user.getRoles().size(), 1);
}

TEST_F(UserTest, AddRole_DoesNotDuplicateRoles) {
    User user;

    user.addRole(UserRole::Student);
    user.addRole(UserRole::Student);

    EXPECT_EQ(user.getRoles().size(), 1);
}

TEST_F(UserTest, RemoveRole_RemovesSpecificRole) {
    User user = multiRoleUser_;

    user.removeRole(UserRole::Student);

    EXPECT_FALSE(user.hasRole(UserRole::Student));
    EXPECT_TRUE(user.hasRole(UserRole::Instructor));
}

TEST_F(UserTest, ClearRoles_RemovesAllRoles) {
    User user = multiRoleUser_;

    user.clearRoles();

    EXPECT_TRUE(user.getRoles().empty());
    EXPECT_FALSE(user.hasRole(UserRole::Student));
    EXPECT_FALSE(user.hasRole(UserRole::Instructor));
}

TEST_F(UserTest, SetRoles_ReplacesAllRoles) {
    User user = studentUser_;

    std::vector<UserRole> newRoles = {UserRole::Admin, UserRole::Instructor};
    user.setRoles(newRoles);

    EXPECT_FALSE(user.hasRole(UserRole::Student));
    EXPECT_TRUE(user.hasRole(UserRole::Admin));
    EXPECT_TRUE(user.hasRole(UserRole::Instructor));
}

// =============================================================================
// Role Conversion Tests
// =============================================================================

TEST_F(UserTest, StringToRole_ConvertsValidStrings) {
    EXPECT_EQ(User::stringToRole("student"), UserRole::Student);
    EXPECT_EQ(User::stringToRole("instructor"), UserRole::Instructor);
    EXPECT_EQ(User::stringToRole("admin"), UserRole::Admin);
}

TEST_F(UserTest, StringToRole_HandlesUpperCase) {
    EXPECT_EQ(User::stringToRole("STUDENT"), UserRole::Student);
    EXPECT_EQ(User::stringToRole("Student"), UserRole::Student);
}

TEST_F(UserTest, RoleToString_ConvertsRolesToStrings) {
    EXPECT_EQ(User::roleToString(UserRole::Student), "student");
    EXPECT_EQ(User::roleToString(UserRole::Instructor), "instructor");
    EXPECT_EQ(User::roleToString(UserRole::Admin), "admin");
}

TEST_F(UserTest, RoleToDisplayString_ReturnsDisplayNames) {
    EXPECT_EQ(User::roleToDisplayString(UserRole::Student), "Student");
    EXPECT_EQ(User::roleToDisplayString(UserRole::Instructor), "Instructor");
    EXPECT_EQ(User::roleToDisplayString(UserRole::Admin), "Administrator");
}

// =============================================================================
// Full Name Tests
// =============================================================================

TEST_F(UserTest, GetFullName_ReturnsCombinedName) {
    EXPECT_EQ(studentUser_.getFullName(), "Test Student");
}

TEST_F(UserTest, GetFullName_HandlesEmptyNames) {
    User user;
    EXPECT_EQ(user.getFullName(), " ");
}

// =============================================================================
// Display Name Tests
// =============================================================================

TEST_F(UserTest, GetDisplayName_ReturnsPreferredNameIfSet) {
    User user;
    user.setPreferredName("Johnny");
    user.setFirstName("John");
    user.setLastName("Doe");

    EXPECT_EQ(user.getDisplayName(), "Johnny");
}

TEST_F(UserTest, GetDisplayName_ReturnsFullNameIfNoPreferred) {
    EXPECT_EQ(studentUser_.getDisplayName(), "Test Student");
}

// =============================================================================
// Login Status Tests
// =============================================================================

TEST_F(UserTest, CanLogin_ReturnsTrueWhenEnabled) {
    EXPECT_TRUE(studentUser_.canLogin());
}

TEST_F(UserTest, CanLogin_ReturnsFalseWhenInactive) {
    User user = studentUser_;
    user.setActive(false);

    EXPECT_FALSE(user.canLogin());
}

TEST_F(UserTest, CanLogin_ReturnsFalseWhenLoginDisabled) {
    User user = studentUser_;
    user.setLoginEnabled(false);

    EXPECT_FALSE(user.canLogin());
}

TEST_F(UserTest, IsLocked_ReturnsTrueWhenLockedUntilSet) {
    User user = studentUser_;
    user.setLockedUntil("2099-12-31T23:59:59Z");

    EXPECT_TRUE(user.isLocked());
}

TEST_F(UserTest, IsLocked_ReturnsFalseWhenNotLocked) {
    EXPECT_FALSE(studentUser_.isLocked());
}

// =============================================================================
// Home Route Tests
// =============================================================================

TEST_F(UserTest, GetHomeRoute_ReturnsStudentRoute) {
    EXPECT_EQ(studentUser_.getHomeRoute(), "/student");
}

TEST_F(UserTest, GetHomeRoute_ReturnsInstructorRoute) {
    EXPECT_EQ(instructorUser_.getHomeRoute(), "/instructor");
}

TEST_F(UserTest, GetHomeRoute_ReturnsAdminRoute) {
    EXPECT_EQ(adminUser_.getHomeRoute(), "/admin");
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(UserTest, ToJson_ContainsBasicFields) {
    auto json = studentUser_.toJson();

    EXPECT_EQ(json["id"], 1);
    EXPECT_EQ(json["email"], "student@example.com");
    EXPECT_EQ(json["first_name"], "Test");
    EXPECT_EQ(json["last_name"], "Student");
}

TEST_F(UserTest, ToJson_ContainsStatusFields) {
    auto json = studentUser_.toJson();

    EXPECT_TRUE(json["is_active"].get<bool>());
    EXPECT_TRUE(json["login_enabled"].get<bool>());
    EXPECT_TRUE(json["email_verified"].get<bool>());
}

TEST_F(UserTest, ToJson_ContainsRoles) {
    auto json = multiRoleUser_.toJson();

    EXPECT_TRUE(json.contains("roles"));
    EXPECT_TRUE(json["roles"].is_array());
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(UserTest, FromJson_ParsesBasicFields) {
    auto json = TestFixtures::createUserJson();
    User user = User::fromJson(json);

    EXPECT_EQ(user.getId(), 1);
    EXPECT_EQ(user.getEmail(), "student@example.com");
    EXPECT_EQ(user.getFirstName(), "Test");
    EXPECT_EQ(user.getLastName(), "Student");
}

TEST_F(UserTest, FromJson_ParsesStatusFields) {
    auto json = TestFixtures::createUserJson();
    User user = User::fromJson(json);

    EXPECT_TRUE(user.isActive());
    EXPECT_TRUE(user.isLoginEnabled());
    EXPECT_TRUE(user.isEmailVerified());
}

TEST_F(UserTest, FromJson_ParsesRolesArray) {
    nlohmann::json json = TestFixtures::createUserJson();
    json["roles"] = nlohmann::json::array({"student", "instructor"});

    User user = User::fromJson(json);

    EXPECT_TRUE(user.isStudent());
    EXPECT_TRUE(user.isInstructor());
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(UserTest, RoundTrip_PreservesBasicData) {
    auto json = studentUser_.toJson();
    User reconstructed = User::fromJson(json);

    EXPECT_EQ(reconstructed.getId(), studentUser_.getId());
    EXPECT_EQ(reconstructed.getEmail(), studentUser_.getEmail());
    EXPECT_EQ(reconstructed.getFirstName(), studentUser_.getFirstName());
    EXPECT_EQ(reconstructed.getLastName(), studentUser_.getLastName());
}

// =============================================================================
// AdminProfile Tests
// =============================================================================

class AdminProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        profile_.setId(1);
        profile_.setUserId(100);
        profile_.setEmployeeId("EMP001");
        profile_.setDepartmentId(5);
        profile_.setAdminLevel(AdminLevel::Manager);
        profile_.setCanManageStudents(true);
        profile_.setCanManageCurriculum(true);
        profile_.setCanManageInstructors(false);
        profile_.setCanManageAdmins(false);
        profile_.setCanViewReports(true);
        profile_.setCanRevokeAccess(true);
        profile_.setActive(true);
    }

    AdminProfile profile_;
};

TEST_F(AdminProfileTest, GettersReturnCorrectValues) {
    EXPECT_EQ(profile_.getId(), 1);
    EXPECT_EQ(profile_.getUserId(), 100);
    EXPECT_EQ(profile_.getEmployeeId(), "EMP001");
    EXPECT_EQ(profile_.getDepartmentId(), 5);
    EXPECT_EQ(profile_.getAdminLevel(), AdminLevel::Manager);
}

TEST_F(AdminProfileTest, PermissionGettersReturnCorrectValues) {
    EXPECT_TRUE(profile_.canManageStudents());
    EXPECT_TRUE(profile_.canManageCurriculum());
    EXPECT_FALSE(profile_.canManageInstructors());
    EXPECT_FALSE(profile_.canManageAdmins());
    EXPECT_TRUE(profile_.canViewReports());
    EXPECT_TRUE(profile_.canRevokeAccess());
}

TEST_F(AdminProfileTest, AdminLevelConversion) {
    EXPECT_EQ(AdminProfile::stringToAdminLevel("staff"), AdminLevel::Staff);
    EXPECT_EQ(AdminProfile::stringToAdminLevel("manager"), AdminLevel::Manager);
    EXPECT_EQ(AdminProfile::stringToAdminLevel("super_admin"), AdminLevel::SuperAdmin);

    EXPECT_EQ(AdminProfile::adminLevelToString(AdminLevel::Staff), "staff");
    EXPECT_EQ(AdminProfile::adminLevelToString(AdminLevel::Manager), "manager");
    EXPECT_EQ(AdminProfile::adminLevelToString(AdminLevel::SuperAdmin), "super_admin");
}

TEST_F(AdminProfileTest, GetAdminLevelString_ReturnsDisplayName) {
    profile_.setAdminLevel(AdminLevel::Staff);
    EXPECT_EQ(profile_.getAdminLevelString(), "staff");

    profile_.setAdminLevel(AdminLevel::SuperAdmin);
    EXPECT_EQ(profile_.getAdminLevelString(), "super_admin");
}

// =============================================================================
// StudentProfile Tests
// =============================================================================

class StudentProfileTest : public ::testing::Test {
protected:
    void SetUp() override {
        profile_.setId(1);
        profile_.setUserId(100);
        profile_.setDateOfBirth("2000-01-15");
        profile_.setGender("Male");
        profile_.setPreferredPronouns("he/him");
        profile_.setCurriculumId(5);
        profile_.setStudentType("undergraduate");
        profile_.setInternational(false);
        profile_.setVeteran(true);
        profile_.setIntakeStatus("active");
    }

    StudentProfile profile_;
};

TEST_F(StudentProfileTest, GettersReturnCorrectValues) {
    EXPECT_EQ(profile_.getId(), 1);
    EXPECT_EQ(profile_.getUserId(), 100);
    EXPECT_EQ(profile_.getDateOfBirth(), "2000-01-15");
    EXPECT_EQ(profile_.getGender(), "Male");
    EXPECT_EQ(profile_.getPreferredPronouns(), "he/him");
    EXPECT_EQ(profile_.getCurriculumId(), 5);
    EXPECT_EQ(profile_.getStudentType(), "undergraduate");
}

TEST_F(StudentProfileTest, FlagsReturnCorrectValues) {
    EXPECT_FALSE(profile_.isInternational());
    EXPECT_TRUE(profile_.isVeteran());
    EXPECT_FALSE(profile_.isTransferStudent());
}

// =============================================================================
// UserSession Tests
// =============================================================================

class UserSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        session_.setId(1);
        session_.setUserId(100);
        session_.setSessionToken("token123");
        session_.setRefreshToken("refresh123");
        session_.setActiveRole(UserRole::Student);
        session_.setActive(true);
    }

    UserSession session_;
};

TEST_F(UserSessionTest, GettersReturnCorrectValues) {
    EXPECT_EQ(session_.getId(), 1);
    EXPECT_EQ(session_.getUserId(), 100);
    EXPECT_EQ(session_.getSessionToken(), "token123");
    EXPECT_EQ(session_.getRefreshToken(), "refresh123");
    EXPECT_EQ(session_.getActiveRole(), UserRole::Student);
    EXPECT_TRUE(session_.isActive());
}

TEST_F(UserSessionTest, IsExpired_ReturnsFalseWhenNoExpirySet) {
    UserSession session;
    EXPECT_FALSE(session.isExpired());
}

TEST_F(UserSessionTest, SettersUpdateValues) {
    UserSession session;

    session.setIpAddress("192.168.1.1");
    session.setUserAgent("TestBrowser/1.0");

    EXPECT_EQ(session.getIpAddress(), "192.168.1.1");
    EXPECT_EQ(session.getUserAgent(), "TestBrowser/1.0");
}
