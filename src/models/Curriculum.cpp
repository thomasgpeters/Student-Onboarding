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
    j["degree_type"] = degreeType_;
    j["credit_hours"] = creditHours_;
    j["duration_semesters"] = durationSemesters_;
    j["required_forms"] = requiredForms_;
    j["prerequisites"] = prerequisites_;
    j["is_active"] = isActive_;
    j["is_online"] = isOnline_;
    return j;
}

Curriculum Curriculum::fromJson(const nlohmann::json& json) {
    Curriculum curriculum;

    // Handle JSON:API format where attributes are nested, or flat JSON
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // Handle 'id' at top level (JSON:API) or in attributes
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            curriculum.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            curriculum.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Handle both camelCase and snake_case field names from API
    // name
    if (attrs.contains("name")) curriculum.name_ = attrs["name"].get<std::string>();

    // description
    if (attrs.contains("description") && !attrs["description"].is_null())
        curriculum.description_ = attrs["description"].get<std::string>();

    // department / department_id
    if (attrs.contains("department")) curriculum.department_ = attrs["department"].get<std::string>();
    else if (attrs.contains("department_id") && !attrs["department_id"].is_null()) {
        if (attrs["department_id"].is_string()) {
            curriculum.department_ = attrs["department_id"].get<std::string>();
        } else if (attrs["department_id"].is_number()) {
            curriculum.department_ = std::to_string(attrs["department_id"].get<int>());
        }
    }

    // degreeType / degree_type
    if (attrs.contains("degreeType")) curriculum.degreeType_ = attrs["degreeType"].get<std::string>();
    else if (attrs.contains("degree_type") && !attrs["degree_type"].is_null())
        curriculum.degreeType_ = attrs["degree_type"].get<std::string>();

    // creditHours / credit_hours
    if (attrs.contains("creditHours")) curriculum.creditHours_ = attrs["creditHours"].get<int>();
    else if (attrs.contains("credit_hours") && !attrs["credit_hours"].is_null())
        curriculum.creditHours_ = attrs["credit_hours"].get<int>();

    // durationSemesters / duration_semesters
    if (attrs.contains("durationSemesters")) curriculum.durationSemesters_ = attrs["durationSemesters"].get<int>();
    else if (attrs.contains("duration_semesters") && !attrs["duration_semesters"].is_null())
        curriculum.durationSemesters_ = attrs["duration_semesters"].get<int>();

    // requiredForms / required_forms
    if (attrs.contains("requiredForms")) {
        curriculum.requiredForms_ = attrs["requiredForms"].get<std::vector<std::string>>();
    } else if (attrs.contains("required_forms") && !attrs["required_forms"].is_null()) {
        curriculum.requiredForms_ = attrs["required_forms"].get<std::vector<std::string>>();
    }

    // prerequisites
    if (attrs.contains("prerequisites") && !attrs["prerequisites"].is_null()) {
        curriculum.prerequisites_ = attrs["prerequisites"].get<std::vector<std::string>>();
    }

    // isActive / is_active
    if (attrs.contains("isActive")) curriculum.isActive_ = attrs["isActive"].get<bool>();
    else if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        curriculum.isActive_ = attrs["is_active"].get<bool>();

    // isOnline / is_online
    if (attrs.contains("isOnline")) curriculum.isOnline_ = attrs["isOnline"].get<bool>();
    else if (attrs.contains("is_online") && !attrs["is_online"].is_null())
        curriculum.isOnline_ = attrs["is_online"].get<bool>();

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
