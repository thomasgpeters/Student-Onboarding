#include "Student.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Models {

Student::Student()
    : id_("")
    , email_("")
    , firstName_("")
    , middleName_("")
    , lastName_("")
    , preferredName_("")
    , curriculumId_("")
    , studentType_("undergraduate")
    , gender_("")
    , preferredPronouns_("")
    , phoneNumber_("")
    , alternatePhone_("")
    , addressLine1_("")
    , addressLine2_("")
    , city_("")
    , state_("")
    , zipCode_("")
    , ssn_("")
    , citizenshipStatus_("")
    , isInternational_(false)
    , isTransferStudent_(false)
    , isVeteran_(false)
    , requiresFinancialAid_(false) {
    enrollmentDate_ = std::chrono::system_clock::now();
    dateOfBirth_ = std::chrono::system_clock::time_point{};
}

Student::Student(const std::string& id, const std::string& email)
    : id_(id)
    , email_(email)
    , firstName_("")
    , middleName_("")
    , lastName_("")
    , preferredName_("")
    , curriculumId_("")
    , studentType_("undergraduate")
    , gender_("")
    , preferredPronouns_("")
    , phoneNumber_("")
    , alternatePhone_("")
    , addressLine1_("")
    , addressLine2_("")
    , city_("")
    , state_("")
    , zipCode_("")
    , ssn_("")
    , citizenshipStatus_("")
    , isInternational_(false)
    , isTransferStudent_(false)
    , isVeteran_(false)
    , requiresFinancialAid_(false) {
    enrollmentDate_ = std::chrono::system_clock::now();
    dateOfBirth_ = std::chrono::system_clock::time_point{};
}

std::string Student::getFullName() const {
    if (firstName_.empty() && lastName_.empty()) {
        return email_;
    }
    return firstName_ + " " + lastName_;
}

void Student::markFormCompleted(const std::string& formId) {
    if (!hasCompletedForm(formId)) {
        completedForms_.push_back(formId);
    }
}

bool Student::hasCompletedForm(const std::string& formId) const {
    return std::find(completedForms_.begin(), completedForms_.end(), formId)
           != completedForms_.end();
}

void Student::resetCompletedForms() {
    completedForms_.clear();
}

int Student::getAge() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now - dateOfBirth_;
    auto days = std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24;
    return static_cast<int>(days / 365);
}

nlohmann::json Student::toJson() const {
    nlohmann::json j;

    // Use snake_case for ApiLogicServer compatibility
    if (!id_.empty()) {
        // Try to send as integer if possible
        try {
            j["id"] = std::stoi(id_);
        } catch (...) {
            j["id"] = id_;
        }
    }

    j["email"] = email_;
    j["first_name"] = firstName_;
    j["middle_name"] = middleName_;
    j["last_name"] = lastName_;
    j["preferred_name"] = preferredName_;
    j["gender"] = gender_;
    j["preferred_pronouns"] = preferredPronouns_;
    j["phone_number"] = phoneNumber_;
    j["alternate_phone"] = alternatePhone_;
    j["address_line1"] = addressLine1_;
    j["address_line2"] = addressLine2_;
    j["city"] = city_;
    j["state"] = state_;
    j["zip_code"] = zipCode_;
    j["ssn"] = ssn_;
    j["citizenship_status"] = citizenshipStatus_;

    if (!curriculumId_.empty()) {
        try {
            j["curriculum_id"] = std::stoi(curriculumId_);
        } catch (...) {
            j["curriculum_id"] = curriculumId_;
        }
    }

    j["student_type"] = studentType_;
    j["is_international"] = isInternational_;
    j["is_transfer_student"] = isTransferStudent_;
    j["is_veteran"] = isVeteran_;
    j["requires_financial_aid"] = requiresFinancialAid_;

    // Convert time points to ISO 8601 date strings (date only for DB)
    auto enrollmentTime = std::chrono::system_clock::to_time_t(enrollmentDate_);
    std::ostringstream enrollmentStream;
    enrollmentStream << std::put_time(std::gmtime(&enrollmentTime), "%Y-%m-%d");
    j["enrollment_date"] = enrollmentStream.str();

    auto dobTime = std::chrono::system_clock::to_time_t(dateOfBirth_);
    std::ostringstream dobStream;
    dobStream << std::put_time(std::gmtime(&dobTime), "%Y-%m-%d");
    j["date_of_birth"] = dobStream.str();

    return j;
}

Student Student::fromJson(const nlohmann::json& json) {
    Student student;

    // Handle 'id' - could be int or string (at top level in JSON:API)
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            student.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            student.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // For JSON:API format, attributes are nested under "attributes"
    // Otherwise, attributes are at the top level
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // Handle email - ApiLogicServer uses snake_case
    if (attrs.contains("email")) {
        if (attrs["email"].is_string()) student.email_ = attrs["email"].get<std::string>();
    }

    // Handle firstName/first_name
    if (attrs.contains("firstName") && attrs["firstName"].is_string()) {
        student.firstName_ = attrs["firstName"].get<std::string>();
    } else if (attrs.contains("first_name") && attrs["first_name"].is_string()) {
        student.firstName_ = attrs["first_name"].get<std::string>();
    }

    // Handle lastName/last_name
    if (attrs.contains("lastName") && attrs["lastName"].is_string()) {
        student.lastName_ = attrs["lastName"].get<std::string>();
    } else if (attrs.contains("last_name") && attrs["last_name"].is_string()) {
        student.lastName_ = attrs["last_name"].get<std::string>();
    }

    // Handle middleName/middle_name
    if (attrs.contains("middleName") && attrs["middleName"].is_string()) {
        student.middleName_ = attrs["middleName"].get<std::string>();
    } else if (attrs.contains("middle_name") && attrs["middle_name"].is_string()) {
        student.middleName_ = attrs["middle_name"].get<std::string>();
    }

    // Handle preferredName/preferred_name
    if (attrs.contains("preferredName") && attrs["preferredName"].is_string()) {
        student.preferredName_ = attrs["preferredName"].get<std::string>();
    } else if (attrs.contains("preferred_name") && attrs["preferred_name"].is_string()) {
        student.preferredName_ = attrs["preferred_name"].get<std::string>();
    }

    // Handle gender
    if (attrs.contains("gender") && attrs["gender"].is_string()) {
        student.gender_ = attrs["gender"].get<std::string>();
    }

    // Handle preferredPronouns/preferred_pronouns
    if (attrs.contains("preferredPronouns") && attrs["preferredPronouns"].is_string()) {
        student.preferredPronouns_ = attrs["preferredPronouns"].get<std::string>();
    } else if (attrs.contains("preferred_pronouns") && attrs["preferred_pronouns"].is_string()) {
        student.preferredPronouns_ = attrs["preferred_pronouns"].get<std::string>();
    }

    // Handle phoneNumber/phone_number
    if (attrs.contains("phoneNumber") && attrs["phoneNumber"].is_string()) {
        student.phoneNumber_ = attrs["phoneNumber"].get<std::string>();
    } else if (attrs.contains("phone_number") && attrs["phone_number"].is_string()) {
        student.phoneNumber_ = attrs["phone_number"].get<std::string>();
    }

    // Handle alternatePhone/alternate_phone
    if (attrs.contains("alternatePhone") && attrs["alternatePhone"].is_string()) {
        student.alternatePhone_ = attrs["alternatePhone"].get<std::string>();
    } else if (attrs.contains("alternate_phone") && attrs["alternate_phone"].is_string()) {
        student.alternatePhone_ = attrs["alternate_phone"].get<std::string>();
    }

    // Handle addressLine1/address_line1
    if (attrs.contains("addressLine1") && attrs["addressLine1"].is_string()) {
        student.addressLine1_ = attrs["addressLine1"].get<std::string>();
    } else if (attrs.contains("address_line1") && attrs["address_line1"].is_string()) {
        student.addressLine1_ = attrs["address_line1"].get<std::string>();
    }

    // Handle addressLine2/address_line2
    if (attrs.contains("addressLine2") && attrs["addressLine2"].is_string()) {
        student.addressLine2_ = attrs["addressLine2"].get<std::string>();
    } else if (attrs.contains("address_line2") && attrs["address_line2"].is_string()) {
        student.addressLine2_ = attrs["address_line2"].get<std::string>();
    }

    // Handle city
    if (attrs.contains("city") && attrs["city"].is_string()) {
        student.city_ = attrs["city"].get<std::string>();
    }

    // Handle state
    if (attrs.contains("state") && attrs["state"].is_string()) {
        student.state_ = attrs["state"].get<std::string>();
    }

    // Handle zipCode/zip_code
    if (attrs.contains("zipCode") && attrs["zipCode"].is_string()) {
        student.zipCode_ = attrs["zipCode"].get<std::string>();
    } else if (attrs.contains("zip_code") && attrs["zip_code"].is_string()) {
        student.zipCode_ = attrs["zip_code"].get<std::string>();
    }

    // Handle ssn
    if (attrs.contains("ssn") && attrs["ssn"].is_string()) {
        student.ssn_ = attrs["ssn"].get<std::string>();
    }

    // Handle citizenshipStatus/citizenship_status
    if (attrs.contains("citizenshipStatus") && attrs["citizenshipStatus"].is_string()) {
        student.citizenshipStatus_ = attrs["citizenshipStatus"].get<std::string>();
    } else if (attrs.contains("citizenship_status") && attrs["citizenship_status"].is_string()) {
        student.citizenshipStatus_ = attrs["citizenship_status"].get<std::string>();
    }

    // Handle curriculumId/curriculum_id - could be int or string
    if (attrs.contains("curriculumId")) {
        if (attrs["curriculumId"].is_string()) {
            student.curriculumId_ = attrs["curriculumId"].get<std::string>();
        } else if (attrs["curriculumId"].is_number()) {
            student.curriculumId_ = std::to_string(attrs["curriculumId"].get<int>());
        }
    } else if (attrs.contains("curriculum_id")) {
        if (attrs["curriculum_id"].is_string()) {
            student.curriculumId_ = attrs["curriculum_id"].get<std::string>();
        } else if (attrs["curriculum_id"].is_number()) {
            student.curriculumId_ = std::to_string(attrs["curriculum_id"].get<int>());
        }
    }

    // Handle studentType/student_type
    if (attrs.contains("studentType") && attrs["studentType"].is_string()) {
        student.studentType_ = attrs["studentType"].get<std::string>();
    } else if (attrs.contains("student_type") && attrs["student_type"].is_string()) {
        student.studentType_ = attrs["student_type"].get<std::string>();
    }

    // Handle boolean fields with snake_case alternatives
    if (attrs.contains("isInternational") && attrs["isInternational"].is_boolean()) {
        student.isInternational_ = attrs["isInternational"].get<bool>();
    } else if (attrs.contains("is_international") && attrs["is_international"].is_boolean()) {
        student.isInternational_ = attrs["is_international"].get<bool>();
    }

    if (attrs.contains("isTransferStudent") && attrs["isTransferStudent"].is_boolean()) {
        student.isTransferStudent_ = attrs["isTransferStudent"].get<bool>();
    } else if (attrs.contains("is_transfer_student") && attrs["is_transfer_student"].is_boolean()) {
        student.isTransferStudent_ = attrs["is_transfer_student"].get<bool>();
    }

    if (attrs.contains("isVeteran") && attrs["isVeteran"].is_boolean()) {
        student.isVeteran_ = attrs["isVeteran"].get<bool>();
    } else if (attrs.contains("is_veteran") && attrs["is_veteran"].is_boolean()) {
        student.isVeteran_ = attrs["is_veteran"].get<bool>();
    }

    if (attrs.contains("requiresFinancialAid") && attrs["requiresFinancialAid"].is_boolean()) {
        student.requiresFinancialAid_ = attrs["requiresFinancialAid"].get<bool>();
    } else if (attrs.contains("requires_financial_aid") && attrs["requires_financial_aid"].is_boolean()) {
        student.requiresFinancialAid_ = attrs["requires_financial_aid"].get<bool>();
    }

    if (attrs.contains("completedForms") && attrs["completedForms"].is_array()) {
        student.completedForms_ = attrs["completedForms"].get<std::vector<std::string>>();
    }

    // Parse date strings
    if (attrs.contains("enrollmentDate") && attrs["enrollmentDate"].is_string()) {
        std::string dateStr = attrs["enrollmentDate"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        student.enrollmentDate_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else if (attrs.contains("enrollment_date") && attrs["enrollment_date"].is_string()) {
        std::string dateStr = attrs["enrollment_date"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        student.enrollmentDate_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    if (attrs.contains("dateOfBirth") && attrs["dateOfBirth"].is_string()) {
        std::string dateStr = attrs["dateOfBirth"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        student.dateOfBirth_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    } else if (attrs.contains("date_of_birth") && attrs["date_of_birth"].is_string()) {
        std::string dateStr = attrs["date_of_birth"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        student.dateOfBirth_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    return student;
}

} // namespace Models
} // namespace StudentIntake
