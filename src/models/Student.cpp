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
    , lastName_("")
    , curriculumId_("")
    , studentType_("undergraduate")
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
    , lastName_("")
    , curriculumId_("")
    , studentType_("undergraduate")
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
    j["last_name"] = lastName_;

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
