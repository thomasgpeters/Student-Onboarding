#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/ActivityLog.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class ActivityLogTest : public ::testing::Test {
protected:
    void SetUp() override {
        loginActivity_ = TestFixtures::createLoginActivity();
        formActivity_ = TestFixtures::createFormSubmissionActivity();
        errorActivity_ = TestFixtures::createErrorActivity();
    }

    ActivityLog loginActivity_;
    ActivityLog formActivity_;
    ActivityLog errorActivity_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(ActivityLogTest, DefaultConstructor_InitializesWithDefaults) {
    ActivityLog log;

    EXPECT_EQ(log.getId(), 0);
    EXPECT_EQ(log.getActorId(), 0);
    EXPECT_EQ(log.getActorName(), "");
    EXPECT_EQ(log.getDescription(), "");
    EXPECT_EQ(log.getIpAddress(), "");
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(ActivityLogTest, ActorFields_GettersReturnCorrectValues) {
    EXPECT_EQ(loginActivity_.getActorType(), ActorType::Student);
    EXPECT_EQ(loginActivity_.getActorId(), 1);
}

TEST_F(ActivityLogTest, ActionFields_GettersReturnCorrectValues) {
    EXPECT_EQ(loginActivity_.getActionType(), "login");
    EXPECT_EQ(loginActivity_.getActionCategory(), ActivityCategory::Authentication);
}

TEST_F(ActivityLogTest, DescriptionAndSeverity_GettersReturnCorrectValues) {
    EXPECT_EQ(loginActivity_.getDescription(), "User logged in successfully");
    EXPECT_EQ(loginActivity_.getSeverity(), ActivitySeverity::Info);
}

TEST_F(ActivityLogTest, SettersUpdateValues) {
    ActivityLog log;

    log.setId(100);
    log.setActorType(ActorType::Admin);
    log.setActorId(5);
    log.setActorName("Admin User");
    log.setActorEmail("admin@example.com");
    log.setActionType("create_user");
    log.setActionCategory(ActivityCategory::Admin);
    log.setDescription("Created a new user");
    log.setSeverity(ActivitySeverity::Success);
    log.setIpAddress("192.168.1.100");

    EXPECT_EQ(log.getId(), 100);
    EXPECT_EQ(log.getActorType(), ActorType::Admin);
    EXPECT_EQ(log.getActorId(), 5);
    EXPECT_EQ(log.getActorName(), "Admin User");
    EXPECT_EQ(log.getActorEmail(), "admin@example.com");
    EXPECT_EQ(log.getActionType(), "create_user");
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Admin);
    EXPECT_EQ(log.getDescription(), "Created a new user");
    EXPECT_EQ(log.getSeverity(), ActivitySeverity::Success);
    EXPECT_EQ(log.getIpAddress(), "192.168.1.100");
}

// =============================================================================
// Entity Fields Tests
// =============================================================================

TEST_F(ActivityLogTest, EntityFields_CanBeSetAndRetrieved) {
    EXPECT_EQ(formActivity_.getEntityType(), "form");
    EXPECT_EQ(formActivity_.getEntityId(), "personal_info");
}

TEST_F(ActivityLogTest, EntityName_CanBeSetAndRetrieved) {
    ActivityLog log;
    log.setEntityName("Personal Information Form");
    EXPECT_EQ(log.getEntityName(), "Personal Information Form");
}

// =============================================================================
// Details JSON Tests
// =============================================================================

TEST_F(ActivityLogTest, Details_CanStoreJsonObject) {
    ActivityLog log;

    nlohmann::json details = {
        {"old_value", "pending"},
        {"new_value", "approved"},
        {"reason", "All requirements met"}
    };
    log.setDetails(details);

    auto retrieved = log.getDetails();
    EXPECT_EQ(retrieved["old_value"], "pending");
    EXPECT_EQ(retrieved["new_value"], "approved");
    EXPECT_EQ(retrieved["reason"], "All requirements met");
}

// =============================================================================
// Session Fields Tests
// =============================================================================

TEST_F(ActivityLogTest, SessionFields_CanBeSetAndRetrieved) {
    ActivityLog log;

    log.setSessionId("session_abc123");
    log.setUserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64)");

    EXPECT_EQ(log.getSessionId(), "session_abc123");
    EXPECT_EQ(log.getUserAgent(), "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
}

// =============================================================================
// Actor Type Conversion Tests
// =============================================================================

TEST_F(ActivityLogTest, ActorTypeFromString_ConvertsValidStrings) {
    EXPECT_EQ(ActivityLog::actorTypeFromString("student"), ActorType::Student);
    EXPECT_EQ(ActivityLog::actorTypeFromString("instructor"), ActorType::Instructor);
    EXPECT_EQ(ActivityLog::actorTypeFromString("admin"), ActorType::Admin);
    EXPECT_EQ(ActivityLog::actorTypeFromString("system"), ActorType::System);
}

TEST_F(ActivityLogTest, ActorTypeToString_ConvertsTypesToStrings) {
    EXPECT_EQ(ActivityLog::actorTypeToString(ActorType::Student), "student");
    EXPECT_EQ(ActivityLog::actorTypeToString(ActorType::Instructor), "instructor");
    EXPECT_EQ(ActivityLog::actorTypeToString(ActorType::Admin), "admin");
    EXPECT_EQ(ActivityLog::actorTypeToString(ActorType::System), "system");
}

TEST_F(ActivityLogTest, ActorTypeConversion_RoundTrip) {
    std::vector<ActorType> types = {
        ActorType::Student, ActorType::Instructor, ActorType::Admin, ActorType::System
    };

    for (auto type : types) {
        std::string str = ActivityLog::actorTypeToString(type);
        ActorType reconstructed = ActivityLog::actorTypeFromString(str);
        EXPECT_EQ(reconstructed, type);
    }
}

// =============================================================================
// Category Conversion Tests
// =============================================================================

TEST_F(ActivityLogTest, CategoryFromString_ConvertsValidStrings) {
    EXPECT_EQ(ActivityLog::categoryFromString("authentication"), ActivityCategory::Authentication);
    EXPECT_EQ(ActivityLog::categoryFromString("forms"), ActivityCategory::Forms);
    EXPECT_EQ(ActivityLog::categoryFromString("profile"), ActivityCategory::Profile);
    EXPECT_EQ(ActivityLog::categoryFromString("admin"), ActivityCategory::Admin);
    EXPECT_EQ(ActivityLog::categoryFromString("system"), ActivityCategory::System);
}

TEST_F(ActivityLogTest, CategoryToString_ConvertsCategoriesToStrings) {
    EXPECT_EQ(ActivityLog::categoryToString(ActivityCategory::Authentication), "authentication");
    EXPECT_EQ(ActivityLog::categoryToString(ActivityCategory::Forms), "forms");
    EXPECT_EQ(ActivityLog::categoryToString(ActivityCategory::Profile), "profile");
    EXPECT_EQ(ActivityLog::categoryToString(ActivityCategory::Admin), "admin");
    EXPECT_EQ(ActivityLog::categoryToString(ActivityCategory::System), "system");
}

// =============================================================================
// Severity Conversion Tests
// =============================================================================

TEST_F(ActivityLogTest, SeverityFromString_ConvertsValidStrings) {
    EXPECT_EQ(ActivityLog::severityFromString("info"), ActivitySeverity::Info);
    EXPECT_EQ(ActivityLog::severityFromString("success"), ActivitySeverity::Success);
    EXPECT_EQ(ActivityLog::severityFromString("warning"), ActivitySeverity::Warning);
    EXPECT_EQ(ActivityLog::severityFromString("error"), ActivitySeverity::Error);
}

TEST_F(ActivityLogTest, SeverityToString_ConvertsSeveritiesToStrings) {
    EXPECT_EQ(ActivityLog::severityToString(ActivitySeverity::Info), "info");
    EXPECT_EQ(ActivityLog::severityToString(ActivitySeverity::Success), "success");
    EXPECT_EQ(ActivityLog::severityToString(ActivitySeverity::Warning), "warning");
    EXPECT_EQ(ActivityLog::severityToString(ActivitySeverity::Error), "error");
}

// =============================================================================
// Display Helper Tests
// =============================================================================

TEST_F(ActivityLogTest, GetActorTypeString_ReturnsDisplayString) {
    loginActivity_.setActorType(ActorType::Student);
    EXPECT_EQ(loginActivity_.getActorTypeString(), "Student");

    loginActivity_.setActorType(ActorType::Admin);
    EXPECT_EQ(loginActivity_.getActorTypeString(), "Admin");
}

TEST_F(ActivityLogTest, GetCategoryString_ReturnsDisplayString) {
    loginActivity_.setActionCategory(ActivityCategory::Authentication);
    EXPECT_EQ(loginActivity_.getCategoryString(), "Authentication");

    formActivity_.setActionCategory(ActivityCategory::Forms);
    EXPECT_EQ(formActivity_.getCategoryString(), "Forms");
}

TEST_F(ActivityLogTest, GetSeverityString_ReturnsDisplayString) {
    loginActivity_.setSeverity(ActivitySeverity::Info);
    EXPECT_EQ(loginActivity_.getSeverityString(), "info");

    errorActivity_.setSeverity(ActivitySeverity::Error);
    EXPECT_EQ(errorActivity_.getSeverityString(), "error");
}

// =============================================================================
// Builder Method Tests
// =============================================================================

TEST_F(ActivityLogTest, CreateLoginActivity_Success) {
    auto log = ActivityLog::createLoginActivity(
        ActorType::Student, 1, "John Doe", "john@example.com", true, "192.168.1.100");

    EXPECT_EQ(log.getActorType(), ActorType::Student);
    EXPECT_EQ(log.getActorId(), 1);
    EXPECT_EQ(log.getActorName(), "John Doe");
    EXPECT_EQ(log.getActorEmail(), "john@example.com");
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Authentication);
    EXPECT_EQ(log.getIpAddress(), "192.168.1.100");
    EXPECT_EQ(log.getSeverity(), ActivitySeverity::Success);
}

TEST_F(ActivityLogTest, CreateLoginActivity_Failure) {
    auto log = ActivityLog::createLoginActivity(
        ActorType::Student, 1, "John Doe", "john@example.com", false, "192.168.1.100", "Invalid password");

    EXPECT_EQ(log.getSeverity(), ActivitySeverity::Warning);
    EXPECT_TRUE(log.getDescription().find("fail") != std::string::npos ||
                log.getDescription().find("Invalid") != std::string::npos);
}

TEST_F(ActivityLogTest, CreateLogoutActivity) {
    auto log = ActivityLog::createLogoutActivity(
        ActorType::Instructor, 2, "Jane Smith", "jane@example.com");

    EXPECT_EQ(log.getActorType(), ActorType::Instructor);
    EXPECT_EQ(log.getActorId(), 2);
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Authentication);
}

TEST_F(ActivityLogTest, CreateFormSubmittedActivity) {
    auto log = ActivityLog::createFormSubmittedActivity(
        1, "John Doe", "john@example.com", "personal_info", "Personal Information");

    EXPECT_EQ(log.getActorType(), ActorType::Student);
    EXPECT_EQ(log.getActorId(), 1);
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Forms);
    EXPECT_EQ(log.getEntityId(), "personal_info");
}

TEST_F(ActivityLogTest, CreateStudentRegisteredActivity) {
    auto log = ActivityLog::createStudentRegisteredActivity(
        1, "John Doe", "john@example.com", "192.168.1.100");

    EXPECT_EQ(log.getActorType(), ActorType::Student);
    EXPECT_EQ(log.getActorId(), 1);
    EXPECT_EQ(log.getSeverity(), ActivitySeverity::Success);
}

TEST_F(ActivityLogTest, CreateAccessChangeActivity_Revoked) {
    auto log = ActivityLog::createAccessChangeActivity(
        3, "Admin User", "admin@example.com", 1, "John Doe", true);

    EXPECT_EQ(log.getActorType(), ActorType::Admin);
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Admin);
    EXPECT_TRUE(log.getDescription().find("revok") != std::string::npos ||
                log.getDescription().find("access") != std::string::npos);
}

TEST_F(ActivityLogTest, CreateAccessChangeActivity_Restored) {
    auto log = ActivityLog::createAccessChangeActivity(
        3, "Admin User", "admin@example.com", 1, "John Doe", false);

    EXPECT_EQ(log.getActorType(), ActorType::Admin);
    EXPECT_TRUE(log.getDescription().find("restor") != std::string::npos ||
                log.getDescription().find("access") != std::string::npos);
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(ActivityLogTest, ToJson_ContainsActorFields) {
    auto json = loginActivity_.toJson();

    bool hasActorType = json.contains("actor_type") || json.contains("actorType");
    bool hasActorId = json.contains("actor_id") || json.contains("actorId");

    EXPECT_TRUE(hasActorType);
    EXPECT_TRUE(hasActorId);
}

TEST_F(ActivityLogTest, ToJson_ContainsActionFields) {
    auto json = loginActivity_.toJson();

    bool hasActionType = json.contains("action_type") || json.contains("actionType");
    bool hasDescription = json.contains("description");

    EXPECT_TRUE(hasActionType);
    EXPECT_TRUE(hasDescription);
}

TEST_F(ActivityLogTest, ToJson_ContainsSeverity) {
    auto json = loginActivity_.toJson();

    bool hasSeverity = json.contains("severity");
    EXPECT_TRUE(hasSeverity);
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(ActivityLogTest, FromJson_ParsesActorFields) {
    nlohmann::json json = {
        {"id", 1},
        {"actor_type", "student"},
        {"actor_id", 100},
        {"actor_name", "John Doe"},
        {"actor_email", "john@example.com"}
    };

    ActivityLog log = ActivityLog::fromJson(json);

    EXPECT_EQ(log.getId(), 1);
    EXPECT_EQ(log.getActorType(), ActorType::Student);
    EXPECT_EQ(log.getActorId(), 100);
    EXPECT_EQ(log.getActorName(), "John Doe");
    EXPECT_EQ(log.getActorEmail(), "john@example.com");
}

TEST_F(ActivityLogTest, FromJson_ParsesActionFields) {
    nlohmann::json json = {
        {"action_type", "login"},
        {"action_category", "authentication"},
        {"description", "User logged in"},
        {"severity", "success"}
    };

    ActivityLog log = ActivityLog::fromJson(json);

    EXPECT_EQ(log.getActionType(), "login");
    EXPECT_EQ(log.getActionCategory(), ActivityCategory::Authentication);
    EXPECT_EQ(log.getDescription(), "User logged in");
    EXPECT_EQ(log.getSeverity(), ActivitySeverity::Success);
}

TEST_F(ActivityLogTest, FromJson_ParsesEntityFields) {
    nlohmann::json json = {
        {"entity_type", "form"},
        {"entity_id", "personal_info"},
        {"entity_name", "Personal Information Form"}
    };

    ActivityLog log = ActivityLog::fromJson(json);

    EXPECT_EQ(log.getEntityType(), "form");
    EXPECT_EQ(log.getEntityId(), "personal_info");
    EXPECT_EQ(log.getEntityName(), "Personal Information Form");
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(ActivityLogTest, RoundTrip_PreservesData) {
    auto json = loginActivity_.toJson();
    ActivityLog reconstructed = ActivityLog::fromJson(json);

    EXPECT_EQ(reconstructed.getActorType(), loginActivity_.getActorType());
    EXPECT_EQ(reconstructed.getActorId(), loginActivity_.getActorId());
    EXPECT_EQ(reconstructed.getActionType(), loginActivity_.getActionType());
    EXPECT_EQ(reconstructed.getDescription(), loginActivity_.getDescription());
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(ActivityLogTest, FromJson_HandlesEmptyJson) {
    nlohmann::json json = {};

    ActivityLog log = ActivityLog::fromJson(json);

    EXPECT_EQ(log.getId(), 0);
    EXPECT_EQ(log.getActorId(), 0);
}

TEST_F(ActivityLogTest, FromJson_HandlesNullValues) {
    nlohmann::json json = {
        {"id", 1},
        {"actor_name", nullptr},
        {"description", nullptr}
    };

    ActivityLog log = ActivityLog::fromJson(json);

    EXPECT_EQ(log.getId(), 1);
    EXPECT_EQ(log.getActorName(), "");
}

// =============================================================================
// Activity Categories Specific Tests
// =============================================================================

TEST_F(ActivityLogTest, FormActivity_HasCorrectCategory) {
    EXPECT_EQ(formActivity_.getActionCategory(), ActivityCategory::Forms);
}

TEST_F(ActivityLogTest, ErrorActivity_HasCorrectSeverity) {
    EXPECT_EQ(errorActivity_.getSeverity(), ActivitySeverity::Error);
    EXPECT_EQ(errorActivity_.getActorType(), ActorType::System);
}
