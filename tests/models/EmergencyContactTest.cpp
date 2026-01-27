#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "models/EmergencyContact.h"
#include "utils/TestUtils.h"

using namespace StudentIntake::Models;
using namespace TestUtils;

// =============================================================================
// Test Fixture
// =============================================================================

class EmergencyContactTest : public ::testing::Test {
protected:
    void SetUp() override {
        primaryContact_ = TestFixtures::createPrimaryContact("1");
        secondaryContact_ = TestFixtures::createSecondaryContact("1");
    }

    EmergencyContact primaryContact_;
    EmergencyContact secondaryContact_;
};

// =============================================================================
// Constructor Tests
// =============================================================================

TEST_F(EmergencyContactTest, DefaultConstructor_InitializesWithDefaults) {
    EmergencyContact contact;

    EXPECT_EQ(contact.getStudentId(), "");
    EXPECT_EQ(contact.getRelationship(), "");
    EXPECT_EQ(contact.getPhone(), "");
    EXPECT_EQ(contact.getFirstName(), "");
    EXPECT_EQ(contact.getLastName(), "");
    EXPECT_EQ(contact.getEmail(), "");
    EXPECT_EQ(contact.getCountry(), "United States");
    EXPECT_FALSE(contact.isPrimary());
    EXPECT_EQ(contact.getPriority(), 1);
}

TEST_F(EmergencyContactTest, ParameterizedConstructor_SetsStudentId) {
    EmergencyContact contact("123");

    EXPECT_EQ(contact.getStudentId(), "123");
    EXPECT_EQ(contact.getRelationship(), "");
    EXPECT_EQ(contact.getPhone(), "");
    EXPECT_EQ(contact.getCountry(), "United States");
}

// =============================================================================
// Getter/Setter Tests
// =============================================================================

TEST_F(EmergencyContactTest, CompoundKeyFields_GettersReturnCorrectValues) {
    EXPECT_EQ(primaryContact_.getStudentId(), "1");
    EXPECT_EQ(primaryContact_.getRelationship(), "Spouse");
    EXPECT_EQ(primaryContact_.getPhone(), "555-111-2222");
}

TEST_F(EmergencyContactTest, NameFields_GettersReturnCorrectValues) {
    EXPECT_EQ(primaryContact_.getFirstName(), "Jane");
    EXPECT_EQ(primaryContact_.getLastName(), "Doe");
    EXPECT_EQ(primaryContact_.getEmail(), "jane.doe@example.com");
}

TEST_F(EmergencyContactTest, AddressFields_GettersReturnCorrectValues) {
    EXPECT_EQ(primaryContact_.getStreet1(), "123 Main Street");
    EXPECT_EQ(primaryContact_.getStreet2(), "Apt 4B");
    EXPECT_EQ(primaryContact_.getCity(), "Springfield");
    EXPECT_EQ(primaryContact_.getState(), "IL");
    EXPECT_EQ(primaryContact_.getPostalCode(), "62701");
    EXPECT_EQ(primaryContact_.getCountry(), "United States");
}

TEST_F(EmergencyContactTest, SettersUpdateValues) {
    EmergencyContact contact;

    contact.setStudentId("999");
    contact.setRelationship("Parent");
    contact.setPhone("555-999-8888");
    contact.setFirstName("Test");
    contact.setLastName("Contact");
    contact.setPrimary(true);
    contact.setPriority(5);

    EXPECT_EQ(contact.getStudentId(), "999");
    EXPECT_EQ(contact.getRelationship(), "Parent");
    EXPECT_EQ(contact.getPhone(), "555-999-8888");
    EXPECT_EQ(contact.getFirstName(), "Test");
    EXPECT_EQ(contact.getLastName(), "Contact");
    EXPECT_TRUE(contact.isPrimary());
    EXPECT_EQ(contact.getPriority(), 5);
}

// =============================================================================
// isEmpty Tests
// =============================================================================

TEST_F(EmergencyContactTest, IsEmpty_ReturnsTrueForEmptyContact) {
    EmergencyContact contact;
    EXPECT_TRUE(contact.isEmpty());
}

TEST_F(EmergencyContactTest, IsEmpty_ReturnsFalseWhenFirstNameSet) {
    EmergencyContact contact;
    contact.setFirstName("Jane");
    EXPECT_FALSE(contact.isEmpty());
}

TEST_F(EmergencyContactTest, IsEmpty_ReturnsFalseWhenLastNameSet) {
    EmergencyContact contact;
    contact.setLastName("Doe");
    EXPECT_FALSE(contact.isEmpty());
}

TEST_F(EmergencyContactTest, IsEmpty_ReturnsFalseWhenPhoneSet) {
    EmergencyContact contact;
    contact.setPhone("555-123-4567");
    EXPECT_FALSE(contact.isEmpty());
}

TEST_F(EmergencyContactTest, IsEmpty_ReturnsFalseForCompleteContact) {
    EXPECT_FALSE(primaryContact_.isEmpty());
}

// =============================================================================
// hasValidKey Tests
// =============================================================================

TEST_F(EmergencyContactTest, HasValidKey_ReturnsFalseForEmptyContact) {
    EmergencyContact contact;
    EXPECT_FALSE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, HasValidKey_ReturnsFalseWhenOnlyStudentIdSet) {
    EmergencyContact contact("1");
    EXPECT_FALSE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, HasValidKey_ReturnsFalseWhenMissingRelationship) {
    EmergencyContact contact("1");
    contact.setPhone("555-123-4567");
    EXPECT_FALSE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, HasValidKey_ReturnsFalseWhenMissingPhone) {
    EmergencyContact contact("1");
    contact.setRelationship("Parent");
    EXPECT_FALSE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, HasValidKey_ReturnsTrueWhenAllKeyFieldsSet) {
    EmergencyContact contact("1");
    contact.setRelationship("Parent");
    contact.setPhone("555-123-4567");
    EXPECT_TRUE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, HasValidKey_ReturnsTrueForCompleteContact) {
    EXPECT_TRUE(primaryContact_.hasValidKey());
}

// =============================================================================
// getCompoundKey Tests
// =============================================================================

TEST_F(EmergencyContactTest, GetCompoundKey_ReturnsFormattedKey) {
    EXPECT_EQ(primaryContact_.getCompoundKey(), "1|Spouse|555-111-2222");
}

TEST_F(EmergencyContactTest, GetCompoundKey_HandlesEmptyFields) {
    EmergencyContact contact;
    EXPECT_EQ(contact.getCompoundKey(), "||");
}

TEST_F(EmergencyContactTest, GetCompoundKey_UniquenessForDifferentContacts) {
    EXPECT_NE(primaryContact_.getCompoundKey(), secondaryContact_.getCompoundKey());
}

// =============================================================================
// JSON Serialization Tests
// =============================================================================

TEST_F(EmergencyContactTest, ToJson_ContainsCompoundKeyFields) {
    auto json = primaryContact_.toJson();

    EXPECT_TRUE(json.contains("student_id"));
    EXPECT_TRUE(json.contains("contact_relationship"));
    EXPECT_TRUE(json.contains("phone"));
}

TEST_F(EmergencyContactTest, ToJson_ContainsNameFields) {
    auto json = primaryContact_.toJson();

    EXPECT_EQ(json["first_name"], "Jane");
    EXPECT_EQ(json["last_name"], "Doe");
    EXPECT_EQ(json["email"], "jane.doe@example.com");
}

TEST_F(EmergencyContactTest, ToJson_ContainsAddressFields) {
    auto json = primaryContact_.toJson();

    EXPECT_EQ(json["street1"], "123 Main Street");
    EXPECT_EQ(json["street2"], "Apt 4B");
    EXPECT_EQ(json["city"], "Springfield");
    EXPECT_EQ(json["state"], "IL");
    EXPECT_EQ(json["postal_code"], "62701");
    EXPECT_EQ(json["country"], "United States");
}

TEST_F(EmergencyContactTest, ToJson_ContainsPriorityFields) {
    auto json = primaryContact_.toJson();

    EXPECT_TRUE(json["is_primary"].get<bool>());
    EXPECT_EQ(json["priority"].get<int>(), 1);
}

TEST_F(EmergencyContactTest, ToJson_SerializesStudentIdAsInteger) {
    auto json = primaryContact_.toJson();

    EXPECT_TRUE(json["student_id"].is_number());
    EXPECT_EQ(json["student_id"].get<int>(), 1);
}

TEST_F(EmergencyContactTest, ToJson_UsesCorrectRelationshipFieldName) {
    auto json = primaryContact_.toJson();

    // Should use "contact_relationship" for database compatibility
    EXPECT_TRUE(json.contains("contact_relationship"));
    EXPECT_EQ(json["contact_relationship"], "Spouse");
}

// =============================================================================
// JSON Deserialization Tests
// =============================================================================

TEST_F(EmergencyContactTest, FromJson_ParsesCompoundKeyFields) {
    auto json = TestFixtures::createEmergencyContactJson();
    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getStudentId(), "1");
    EXPECT_EQ(contact.getRelationship(), "Spouse");
    EXPECT_EQ(contact.getPhone(), "555-111-2222");
}

TEST_F(EmergencyContactTest, FromJson_ParsesNameFields) {
    auto json = TestFixtures::createEmergencyContactJson();
    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getFirstName(), "Jane");
    EXPECT_EQ(contact.getLastName(), "Doe");
    EXPECT_EQ(contact.getEmail(), "jane.doe@example.com");
}

TEST_F(EmergencyContactTest, FromJson_ParsesAddressFields) {
    auto json = TestFixtures::createEmergencyContactJson();
    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getStreet1(), "123 Main Street");
    EXPECT_EQ(contact.getStreet2(), "Apt 4B");
    EXPECT_EQ(contact.getCity(), "Springfield");
    EXPECT_EQ(contact.getState(), "IL");
    EXPECT_EQ(contact.getPostalCode(), "62701");
    EXPECT_EQ(contact.getCountry(), "United States");
}

TEST_F(EmergencyContactTest, FromJson_ParsesPriorityFields) {
    auto json = TestFixtures::createEmergencyContactJson();
    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_TRUE(contact.isPrimary());
    EXPECT_EQ(contact.getPriority(), 1);
}

TEST_F(EmergencyContactTest, FromJson_ParsesJsonApiFormat) {
    nlohmann::json json = {
        {"type", "emergency_contact"},
        {"attributes", TestFixtures::createEmergencyContactJson()}
    };

    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getFirstName(), "Jane");
    EXPECT_EQ(contact.getRelationship(), "Spouse");
}

TEST_F(EmergencyContactTest, FromJson_HandlesIntegerStudentId) {
    nlohmann::json json = TestFixtures::createEmergencyContactJson();
    json["student_id"] = 999;

    EmergencyContact contact = EmergencyContact::fromJson(json);
    EXPECT_EQ(contact.getStudentId(), "999");
}

TEST_F(EmergencyContactTest, FromJson_HandlesStringStudentId) {
    nlohmann::json json = TestFixtures::createEmergencyContactJson();
    json["student_id"] = "999";

    EmergencyContact contact = EmergencyContact::fromJson(json);
    EXPECT_EQ(contact.getStudentId(), "999");
}

// =============================================================================
// Round-trip Serialization Tests
// =============================================================================

TEST_F(EmergencyContactTest, RoundTrip_PreservesAllData) {
    auto json = primaryContact_.toJson();
    EmergencyContact reconstructed = EmergencyContact::fromJson(json);

    EXPECT_EQ(reconstructed.getStudentId(), primaryContact_.getStudentId());
    EXPECT_EQ(reconstructed.getRelationship(), primaryContact_.getRelationship());
    EXPECT_EQ(reconstructed.getPhone(), primaryContact_.getPhone());
    EXPECT_EQ(reconstructed.getFirstName(), primaryContact_.getFirstName());
    EXPECT_EQ(reconstructed.getLastName(), primaryContact_.getLastName());
    EXPECT_EQ(reconstructed.getEmail(), primaryContact_.getEmail());
    EXPECT_EQ(reconstructed.getCity(), primaryContact_.getCity());
    EXPECT_EQ(reconstructed.getState(), primaryContact_.getState());
    EXPECT_EQ(reconstructed.isPrimary(), primaryContact_.isPrimary());
    EXPECT_EQ(reconstructed.getPriority(), primaryContact_.getPriority());
}

TEST_F(EmergencyContactTest, RoundTrip_PreservesCompoundKey) {
    auto json = primaryContact_.toJson();
    EmergencyContact reconstructed = EmergencyContact::fromJson(json);

    EXPECT_EQ(reconstructed.getCompoundKey(), primaryContact_.getCompoundKey());
}

// =============================================================================
// Edge Case Tests
// =============================================================================

TEST_F(EmergencyContactTest, FromJson_HandlesNullValues) {
    nlohmann::json json = {
        {"student_id", 1},
        {"contact_relationship", "Parent"},
        {"phone", "555-1234"},
        {"first_name", nullptr},
        {"last_name", nullptr}
    };

    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getFirstName(), "");
    EXPECT_EQ(contact.getLastName(), "");
}

TEST_F(EmergencyContactTest, FromJson_HandlesEmptyJson) {
    nlohmann::json json = {};

    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getStudentId(), "");
    EXPECT_EQ(contact.getRelationship(), "");
    EXPECT_FALSE(contact.hasValidKey());
}

TEST_F(EmergencyContactTest, FromJson_HandlesPartialData) {
    nlohmann::json json = {
        {"student_id", 1},
        {"first_name", "John"}
    };

    EmergencyContact contact = EmergencyContact::fromJson(json);

    EXPECT_EQ(contact.getStudentId(), "1");
    EXPECT_EQ(contact.getFirstName(), "John");
    EXPECT_EQ(contact.getRelationship(), "");
}

// =============================================================================
// Relationship Type Tests
// =============================================================================

TEST_F(EmergencyContactTest, DifferentRelationshipTypes) {
    std::vector<std::string> relationships = {
        "Spouse", "Parent", "Grandparent", "Sibling", "Friend", "Other"
    };

    for (const auto& rel : relationships) {
        EmergencyContact contact("1");
        contact.setRelationship(rel);
        contact.setPhone("555-1234");

        EXPECT_TRUE(contact.hasValidKey());
        EXPECT_EQ(contact.getRelationship(), rel);
    }
}

// =============================================================================
// Priority Tests
// =============================================================================

TEST_F(EmergencyContactTest, Priority_CanBeSetToAnyValue) {
    EmergencyContact contact;

    contact.setPriority(1);
    EXPECT_EQ(contact.getPriority(), 1);

    contact.setPriority(5);
    EXPECT_EQ(contact.getPriority(), 5);

    contact.setPriority(100);
    EXPECT_EQ(contact.getPriority(), 100);
}

TEST_F(EmergencyContactTest, IsPrimary_IndependentOfPriority) {
    EmergencyContact contact;

    contact.setPriority(1);
    contact.setPrimary(false);

    EXPECT_EQ(contact.getPriority(), 1);
    EXPECT_FALSE(contact.isPrimary());
}
