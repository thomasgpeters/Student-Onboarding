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
    j["id"] = id_;
    j["email"] = email_;
    j["firstName"] = firstName_;
    j["lastName"] = lastName_;
    j["curriculumId"] = curriculumId_;
    j["studentType"] = studentType_;
    j["isInternational"] = isInternational_;
    j["isTransferStudent"] = isTransferStudent_;
    j["isVeteran"] = isVeteran_;
    j["requiresFinancialAid"] = requiresFinancialAid_;
    j["completedForms"] = completedForms_;

    // Convert time points to ISO 8601 strings
    auto enrollmentTime = std::chrono::system_clock::to_time_t(enrollmentDate_);
    std::ostringstream enrollmentStream;
    enrollmentStream << std::put_time(std::gmtime(&enrollmentTime), "%Y-%m-%dT%H:%M:%SZ");
    j["enrollmentDate"] = enrollmentStream.str();

    auto dobTime = std::chrono::system_clock::to_time_t(dateOfBirth_);
    std::ostringstream dobStream;
    dobStream << std::put_time(std::gmtime(&dobTime), "%Y-%m-%dT%H:%M:%SZ");
    j["dateOfBirth"] = dobStream.str();

    return j;
}

Student Student::fromJson(const nlohmann::json& json) {
    Student student;

    if (json.contains("id")) student.id_ = json["id"].get<std::string>();
    if (json.contains("email")) student.email_ = json["email"].get<std::string>();
    if (json.contains("firstName")) student.firstName_ = json["firstName"].get<std::string>();
    if (json.contains("lastName")) student.lastName_ = json["lastName"].get<std::string>();
    if (json.contains("curriculumId")) student.curriculumId_ = json["curriculumId"].get<std::string>();
    if (json.contains("studentType")) student.studentType_ = json["studentType"].get<std::string>();
    if (json.contains("isInternational")) student.isInternational_ = json["isInternational"].get<bool>();
    if (json.contains("isTransferStudent")) student.isTransferStudent_ = json["isTransferStudent"].get<bool>();
    if (json.contains("isVeteran")) student.isVeteran_ = json["isVeteran"].get<bool>();
    if (json.contains("requiresFinancialAid")) student.requiresFinancialAid_ = json["requiresFinancialAid"].get<bool>();

    if (json.contains("completedForms")) {
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
