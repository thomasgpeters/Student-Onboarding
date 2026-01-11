#include "Curriculum.h"
#include <algorithm>

namespace StudentIntake {
namespace Models {

Curriculum::Curriculum()
    : id_("")
    , name_("")
    , description_("")
    , department_("")
    , degreeType_("bachelor")
    , creditHours_(0)
    , durationSemesters_(0)
    , isActive_(true)
    , isOnline_(false) {
}

Curriculum::Curriculum(const std::string& id, const std::string& name)
    : id_(id)
    , name_(name)
    , description_("")
    , department_("")
    , degreeType_("bachelor")
    , creditHours_(0)
    , durationSemesters_(0)
    , isActive_(true)
    , isOnline_(false) {
}

void Curriculum::addRequiredForm(const std::string& formId) {
    if (!requiresForm(formId)) {
        requiredForms_.push_back(formId);
    }
}

void Curriculum::removeRequiredForm(const std::string& formId) {
    auto it = std::find(requiredForms_.begin(), requiredForms_.end(), formId);
    if (it != requiredForms_.end()) {
        requiredForms_.erase(it);
    }
}

bool Curriculum::requiresForm(const std::string& formId) const {
    return std::find(requiredForms_.begin(), requiredForms_.end(), formId)
           != requiredForms_.end();
}

nlohmann::json Curriculum::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["name"] = name_;
    j["description"] = description_;
    j["department"] = department_;
    j["degreeType"] = degreeType_;
    j["creditHours"] = creditHours_;
    j["durationSemesters"] = durationSemesters_;
    j["requiredForms"] = requiredForms_;
    j["prerequisites"] = prerequisites_;
    j["isActive"] = isActive_;
    j["isOnline"] = isOnline_;
    return j;
}

Curriculum Curriculum::fromJson(const nlohmann::json& json) {
    Curriculum curriculum;

    if (json.contains("id")) curriculum.id_ = json["id"].get<std::string>();
    if (json.contains("name")) curriculum.name_ = json["name"].get<std::string>();
    if (json.contains("description")) curriculum.description_ = json["description"].get<std::string>();
    if (json.contains("department")) curriculum.department_ = json["department"].get<std::string>();
    if (json.contains("degreeType")) curriculum.degreeType_ = json["degreeType"].get<std::string>();
    if (json.contains("creditHours")) curriculum.creditHours_ = json["creditHours"].get<int>();
    if (json.contains("durationSemesters")) curriculum.durationSemesters_ = json["durationSemesters"].get<int>();
    if (json.contains("requiredForms")) {
        curriculum.requiredForms_ = json["requiredForms"].get<std::vector<std::string>>();
    }
    if (json.contains("prerequisites")) {
        curriculum.prerequisites_ = json["prerequisites"].get<std::vector<std::string>>();
    }
    if (json.contains("isActive")) curriculum.isActive_ = json["isActive"].get<bool>();
    if (json.contains("isOnline")) curriculum.isOnline_ = json["isOnline"].get<bool>();

    return curriculum;
}

// Department implementation
nlohmann::json Department::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["code"] = code;
    j["dean"] = dean;
    j["contactEmail"] = contactEmail;
    j["curriculumIds"] = curriculumIds;
    return j;
}

Department Department::fromJson(const nlohmann::json& json) {
    Department dept;
    if (json.contains("id")) dept.id = json["id"].get<std::string>();
    if (json.contains("name")) dept.name = json["name"].get<std::string>();
    if (json.contains("code")) dept.code = json["code"].get<std::string>();
    if (json.contains("dean")) dept.dean = json["dean"].get<std::string>();
    if (json.contains("contactEmail")) dept.contactEmail = json["contactEmail"].get<std::string>();
    if (json.contains("curriculumIds")) {
        dept.curriculumIds = json["curriculumIds"].get<std::vector<std::string>>();
    }
    return dept;
}

} // namespace Models
} // namespace StudentIntake
