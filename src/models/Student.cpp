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

    // Handle 'id' - could be int or string
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            student.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            student.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Handle email - ApiLogicServer uses snake_case
    if (json.contains("email")) {
        if (json["email"].is_string()) student.email_ = json["email"].get<std::string>();
    }

    // Handle firstName/first_name
    if (json.contains("firstName") && json["firstName"].is_string()) {
        student.firstName_ = json["firstName"].get<std::string>();
    } else if (json.contains("first_name") && json["first_name"].is_string()) {
        student.firstName_ = json["first_name"].get<std::string>();
    }

    // Handle lastName/last_name
    if (json.contains("lastName") && json["lastName"].is_string()) {
        student.lastName_ = json["lastName"].get<std::string>();
    } else if (json.contains("last_name") && json["last_name"].is_string()) {
        student.lastName_ = json["last_name"].get<std::string>();
    }

    // Handle curriculumId/curriculum_id - could be int or string
    if (json.contains("curriculumId")) {
        if (json["curriculumId"].is_string()) {
            student.curriculumId_ = json["curriculumId"].get<std::string>();
        } else if (json["curriculumId"].is_number()) {
            student.curriculumId_ = std::to_string(json["curriculumId"].get<int>());
        }
    } else if (json.contains("curriculum_id")) {
        if (json["curriculum_id"].is_string()) {
            student.curriculumId_ = json["curriculum_id"].get<std::string>();
        } else if (json["curriculum_id"].is_number()) {
            student.curriculumId_ = std::to_string(json["curriculum_id"].get<int>());
        }
    }

    // Handle studentType/student_type
    if (json.contains("studentType") && json["studentType"].is_string()) {
        student.studentType_ = json["studentType"].get<std::string>();
    } else if (json.contains("student_type") && json["student_type"].is_string()) {
        student.studentType_ = json["student_type"].get<std::string>();
    }

    // Handle boolean fields with snake_case alternatives
    if (json.contains("isInternational") && json["isInternational"].is_boolean()) {
        student.isInternational_ = json["isInternational"].get<bool>();
    } else if (json.contains("is_international") && json["is_international"].is_boolean()) {
        student.isInternational_ = json["is_international"].get<bool>();
    }

    if (json.contains("isTransferStudent") && json["isTransferStudent"].is_boolean()) {
        student.isTransferStudent_ = json["isTransferStudent"].get<bool>();
    } else if (json.contains("is_transfer_student") && json["is_transfer_student"].is_boolean()) {
        student.isTransferStudent_ = json["is_transfer_student"].get<bool>();
    }

    if (json.contains("isVeteran") && json["isVeteran"].is_boolean()) {
        student.isVeteran_ = json["isVeteran"].get<bool>();
    } else if (json.contains("is_veteran") && json["is_veteran"].is_boolean()) {
        student.isVeteran_ = json["is_veteran"].get<bool>();
    }

    if (json.contains("requiresFinancialAid") && json["requiresFinancialAid"].is_boolean()) {
        student.requiresFinancialAid_ = json["requiresFinancialAid"].get<bool>();
    } else if (json.contains("requires_financial_aid") && json["requires_financial_aid"].is_boolean()) {
        student.requiresFinancialAid_ = json["requires_financial_aid"].get<bool>();
    }

    if (json.contains("completedForms") && json["completedForms"].is_array()) {
        student.completedForms_ = json["completedForms"].get<std::vector<std::string>>();
    }

    // Parse date strings
    if (json.contains("enrollmentDate")) {
        std::string dateStr = json["enrollmentDate"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        student.enrollmentDate_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    if (json.contains("dateOfBirth")) {
        std::string dateStr = json["dateOfBirth"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        student.dateOfBirth_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    return student;
}

} // namespace Models
} // namespace StudentIntake
