#include "StudentAddress.h"

namespace StudentIntake {
namespace Models {

StudentAddress::StudentAddress()
    : id_("")
    , studentId_("")
    , addressType_("permanent")
    , street1_("")
    , street2_("")
    , city_("")
    , state_("")
    , postalCode_("")
    , country_("United States")
    , isPrimary_(false) {
}

StudentAddress::StudentAddress(const std::string& studentId, const std::string& addressType)
    : id_("")
    , studentId_(studentId)
    , addressType_(addressType)
    , street1_("")
    , street2_("")
    , city_("")
    , state_("")
    , postalCode_("")
    , country_("United States")
    , isPrimary_(addressType == "permanent") {
}

bool StudentAddress::isEmpty() const {
    return street1_.empty() && city_.empty() && state_.empty() && postalCode_.empty();
}

nlohmann::json StudentAddress::toJson() const {
    nlohmann::json j;

    if (!id_.empty()) {
        try {
            j["id"] = std::stoi(id_);
        } catch (...) {
            j["id"] = id_;
        }
    }

    // student_id as integer
    if (!studentId_.empty()) {
        try {
            j["student_id"] = std::stoi(studentId_);
        } catch (...) {
            j["student_id"] = studentId_;
        }
    }

    j["address_type"] = addressType_;
    j["street1"] = street1_;
    j["street2"] = street2_;
    j["city"] = city_;
    j["state"] = state_;
    j["postal_code"] = postalCode_;
    j["country"] = country_;
    j["is_primary"] = isPrimary_;

    return j;
}

StudentAddress StudentAddress::fromJson(const nlohmann::json& json) {
    StudentAddress address;

    // Handle 'id'
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            address.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            address.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // For JSON:API format, attributes may be nested
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // Handle student_id
    if (attrs.contains("student_id")) {
        if (attrs["student_id"].is_string()) {
            address.studentId_ = attrs["student_id"].get<std::string>();
        } else if (attrs["student_id"].is_number()) {
            address.studentId_ = std::to_string(attrs["student_id"].get<int>());
        }
    } else if (attrs.contains("studentId")) {
        if (attrs["studentId"].is_string()) {
            address.studentId_ = attrs["studentId"].get<std::string>();
        } else if (attrs["studentId"].is_number()) {
            address.studentId_ = std::to_string(attrs["studentId"].get<int>());
        }
    }

    // Handle address_type
    if (attrs.contains("address_type") && attrs["address_type"].is_string()) {
        address.addressType_ = attrs["address_type"].get<std::string>();
    } else if (attrs.contains("addressType") && attrs["addressType"].is_string()) {
        address.addressType_ = attrs["addressType"].get<std::string>();
    }

    // Handle street1
    if (attrs.contains("street1") && attrs["street1"].is_string()) {
        address.street1_ = attrs["street1"].get<std::string>();
    }

    // Handle street2
    if (attrs.contains("street2") && attrs["street2"].is_string()) {
        address.street2_ = attrs["street2"].get<std::string>();
    }

    // Handle city
    if (attrs.contains("city") && attrs["city"].is_string()) {
        address.city_ = attrs["city"].get<std::string>();
    }

    // Handle state
    if (attrs.contains("state") && attrs["state"].is_string()) {
        address.state_ = attrs["state"].get<std::string>();
    }

    // Handle postal_code
    if (attrs.contains("postal_code") && attrs["postal_code"].is_string()) {
        address.postalCode_ = attrs["postal_code"].get<std::string>();
    } else if (attrs.contains("postalCode") && attrs["postalCode"].is_string()) {
        address.postalCode_ = attrs["postalCode"].get<std::string>();
    }

    // Handle country
    if (attrs.contains("country") && attrs["country"].is_string()) {
        address.country_ = attrs["country"].get<std::string>();
    }

    // Handle is_primary
    if (attrs.contains("is_primary") && attrs["is_primary"].is_boolean()) {
        address.isPrimary_ = attrs["is_primary"].get<bool>();
    } else if (attrs.contains("isPrimary") && attrs["isPrimary"].is_boolean()) {
        address.isPrimary_ = attrs["isPrimary"].get<bool>();
    }

    return address;
}

} // namespace Models
} // namespace StudentIntake
