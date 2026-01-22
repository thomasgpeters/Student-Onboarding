#include "EmergencyContact.h"

namespace StudentIntake {
namespace Models {

EmergencyContact::EmergencyContact()
    : studentId_("")
    , relationship_("")
    , phone_("")
    , firstName_("")
    , lastName_("")
    , alternatePhone_("")
    , email_("")
    , street1_("")
    , street2_("")
    , city_("")
    , state_("")
    , postalCode_("")
    , country_("United States")
    , isPrimary_(false)
    , priority_(1) {
}

EmergencyContact::EmergencyContact(const std::string& studentId)
    : studentId_(studentId)
    , relationship_("")
    , phone_("")
    , firstName_("")
    , lastName_("")
    , alternatePhone_("")
    , email_("")
    , street1_("")
    , street2_("")
    , city_("")
    , state_("")
    , postalCode_("")
    , country_("United States")
    , isPrimary_(false)
    , priority_(1) {
}

bool EmergencyContact::isEmpty() const {
    return firstName_.empty() && lastName_.empty() && phone_.empty();
}

bool EmergencyContact::hasValidKey() const {
    return !studentId_.empty() && !relationship_.empty() && !phone_.empty();
}

std::string EmergencyContact::getCompoundKey() const {
    return studentId_ + "|" + relationship_ + "|" + phone_;
}

nlohmann::json EmergencyContact::toJson() const {
    nlohmann::json j;

    // student_id as integer
    if (!studentId_.empty()) {
        try {
            j["student_id"] = std::stoi(studentId_);
        } catch (...) {
            j["student_id"] = studentId_;
        }
    }

    j["first_name"] = firstName_;
    j["last_name"] = lastName_;
    j["contact_relationship"] = relationship_;
    j["phone"] = phone_;
    j["alternate_phone"] = alternatePhone_;
    j["email"] = email_;
    j["street1"] = street1_;
    j["street2"] = street2_;
    j["city"] = city_;
    j["state"] = state_;
    j["postal_code"] = postalCode_;
    j["country"] = country_;
    j["is_primary"] = isPrimary_;
    j["priority"] = priority_;

    return j;
}

EmergencyContact EmergencyContact::fromJson(const nlohmann::json& json) {
    EmergencyContact contact;

    // For JSON:API format, attributes may be nested
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // Handle student_id (part of compound key)
    if (attrs.contains("student_id") && !attrs["student_id"].is_null()) {
        if (attrs["student_id"].is_string()) {
            contact.studentId_ = attrs["student_id"].get<std::string>();
        } else if (attrs["student_id"].is_number()) {
            contact.studentId_ = std::to_string(attrs["student_id"].get<int>());
        }
    }

    // Handle contact_relationship (part of compound key)
    if (attrs.contains("contact_relationship") && !attrs["contact_relationship"].is_null() && attrs["contact_relationship"].is_string()) {
        contact.relationship_ = attrs["contact_relationship"].get<std::string>();
    }

    // Handle phone (part of compound key)
    if (attrs.contains("phone") && !attrs["phone"].is_null() && attrs["phone"].is_string()) {
        contact.phone_ = attrs["phone"].get<std::string>();
    }

    // Handle first_name
    if (attrs.contains("first_name") && !attrs["first_name"].is_null() && attrs["first_name"].is_string()) {
        contact.firstName_ = attrs["first_name"].get<std::string>();
    }

    // Handle last_name
    if (attrs.contains("last_name") && !attrs["last_name"].is_null() && attrs["last_name"].is_string()) {
        contact.lastName_ = attrs["last_name"].get<std::string>();
    }

    // Handle alternate_phone
    if (attrs.contains("alternate_phone") && !attrs["alternate_phone"].is_null() && attrs["alternate_phone"].is_string()) {
        contact.alternatePhone_ = attrs["alternate_phone"].get<std::string>();
    }

    // Handle email
    if (attrs.contains("email") && !attrs["email"].is_null() && attrs["email"].is_string()) {
        contact.email_ = attrs["email"].get<std::string>();
    }

    // Handle street1
    if (attrs.contains("street1") && !attrs["street1"].is_null() && attrs["street1"].is_string()) {
        contact.street1_ = attrs["street1"].get<std::string>();
    }

    // Handle street2
    if (attrs.contains("street2") && !attrs["street2"].is_null() && attrs["street2"].is_string()) {
        contact.street2_ = attrs["street2"].get<std::string>();
    }

    // Handle city
    if (attrs.contains("city") && !attrs["city"].is_null() && attrs["city"].is_string()) {
        contact.city_ = attrs["city"].get<std::string>();
    }

    // Handle state
    if (attrs.contains("state") && !attrs["state"].is_null() && attrs["state"].is_string()) {
        contact.state_ = attrs["state"].get<std::string>();
    }

    // Handle postal_code
    if (attrs.contains("postal_code") && !attrs["postal_code"].is_null() && attrs["postal_code"].is_string()) {
        contact.postalCode_ = attrs["postal_code"].get<std::string>();
    }

    // Handle country
    if (attrs.contains("country") && !attrs["country"].is_null() && attrs["country"].is_string()) {
        contact.country_ = attrs["country"].get<std::string>();
    }

    // Handle is_primary
    if (attrs.contains("is_primary") && !attrs["is_primary"].is_null() && attrs["is_primary"].is_boolean()) {
        contact.isPrimary_ = attrs["is_primary"].get<bool>();
    }

    // Handle priority
    if (attrs.contains("priority") && !attrs["priority"].is_null() && attrs["priority"].is_number()) {
        contact.priority_ = attrs["priority"].get<int>();
    }

    return contact;
}

} // namespace Models
} // namespace StudentIntake
