#include "Curriculum.h"
#include <algorithm>
#include <map>

namespace StudentIntake {
namespace Models {

Curriculum::Curriculum()
    : id_("")
    , code_("")
    , name_("")
    , description_("")
    , department_("")
    , departmentId_(0)
    , degreeType_("certificate")
    , creditHours_(0)
    , durationSemesters_(0)
    , durationInterval_("semester")
    , isActive_(true)
    , isOnline_(false) {
}

Curriculum::Curriculum(const std::string& id, const std::string& name)
    : id_(id)
    , code_("")
    , name_(name)
    , description_("")
    , department_("")
    , departmentId_(0)
    , degreeType_("certificate")
    , creditHours_(0)
    , durationSemesters_(0)
    , durationInterval_("semester")
    , isActive_(true)
    , isOnline_(false) {
}

std::string Curriculum::getFormattedDuration() const {
    if (durationSemesters_ <= 0) {
        return "-";
    }

    std::string interval = durationInterval_;
    if (interval.empty()) {
        interval = "semester";
    }

    // Capitalize first letter
    std::string displayInterval = interval;
    if (!displayInterval.empty()) {
        displayInterval[0] = std::toupper(displayInterval[0]);
    }

    // Pluralize if duration > 1
    if (durationSemesters_ > 1) {
        displayInterval += "s";
    }

    return std::to_string(durationSemesters_) + " " + displayInterval;
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

// Form type ID mapping: string form ID <-> integer form_type_id
int Curriculum::formIdToTypeId(const std::string& formId) {
    static const std::map<std::string, int> mapping = {
        {"personal_info", 1},
        {"emergency_contact", 2},
        {"medical_info", 3},
        {"academic_history", 4},
        {"financial_aid", 5},
        {"document_upload", 6},
        {"consent", 7}
    };
    auto it = mapping.find(formId);
    return (it != mapping.end()) ? it->second : 0;
}

std::string Curriculum::typeIdToFormId(int typeId) {
    static const std::map<int, std::string> mapping = {
        {1, "personal_info"},
        {2, "emergency_contact"},
        {3, "medical_info"},
        {4, "academic_history"},
        {5, "financial_aid"},
        {6, "document_upload"},
        {7, "consent"}
    };
    auto it = mapping.find(typeId);
    return (it != mapping.end()) ? it->second : "";
}

std::string Curriculum::departmentIdToName(int departmentId) {
    // Departments are now loaded dynamically from the API
    // The CurriculumListWidget and CurriculumEditorWidget load departments
    // and map department_id to names themselves
    // This function returns empty to indicate no static mapping is available
    (void)departmentId; // Suppress unused parameter warning
    return "";
}

nlohmann::json Curriculum::toJson() const {
    nlohmann::json j;
    // Note: id is NOT included here - in JSON:API format, id goes at data level, not in attributes
    j["code"] = code_;
    j["name"] = name_;
    j["description"] = description_;
    j["department_id"] = departmentId_;
    j["degree_type"] = degreeType_;
    j["credit_hours"] = creditHours_;
    j["duration_semesters"] = durationSemesters_;
    j["duration_interval"] = durationInterval_;
    j["is_active"] = isActive_;
    j["is_online"] = isOnline_;

    // Convert string form IDs to integer form_type_ids for the API
    nlohmann::json formTypeIds = nlohmann::json::array();
    for (const auto& formId : requiredForms_) {
        int typeId = formIdToTypeId(formId);
        if (typeId > 0) {
            formTypeIds.push_back(typeId);
        }
    }
    j["required_form_ids"] = formTypeIds;

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
    // code
    if (attrs.contains("code") && !attrs["code"].is_null())
        curriculum.code_ = attrs["code"].get<std::string>();

    // name
    if (attrs.contains("name")) curriculum.name_ = attrs["name"].get<std::string>();

    // description
    if (attrs.contains("description") && !attrs["description"].is_null())
        curriculum.description_ = attrs["description"].get<std::string>();

    // department (display name)
    if (attrs.contains("department") && !attrs["department"].is_null())
        curriculum.department_ = attrs["department"].get<std::string>();

    // department_id (integer ID for API)
    if (attrs.contains("department_id") && !attrs["department_id"].is_null()) {
        if (attrs["department_id"].is_number()) {
            curriculum.departmentId_ = attrs["department_id"].get<int>();
        } else if (attrs["department_id"].is_string()) {
            curriculum.departmentId_ = std::stoi(attrs["department_id"].get<std::string>());
        }
    }

    // If department name is empty but we have a department_id, look up the name
    if (curriculum.department_.empty() && curriculum.departmentId_ > 0) {
        curriculum.department_ = departmentIdToName(curriculum.departmentId_);
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

    // durationInterval / duration_interval
    if (attrs.contains("durationInterval") && !attrs["durationInterval"].is_null())
        curriculum.durationInterval_ = attrs["durationInterval"].get<std::string>();
    else if (attrs.contains("duration_interval") && !attrs["duration_interval"].is_null())
        curriculum.durationInterval_ = attrs["duration_interval"].get<std::string>();
    else
        curriculum.durationInterval_ = "semester";  // Default

    // requiredForms / required_forms / required_form_ids
    // Handle multiple formats: string arrays, integer arrays, or from junction table
    if (attrs.contains("requiredForms")) {
        curriculum.requiredForms_ = attrs["requiredForms"].get<std::vector<std::string>>();
    } else if (attrs.contains("required_forms") && !attrs["required_forms"].is_null()) {
        // Could be strings or integers
        if (attrs["required_forms"].is_array() && !attrs["required_forms"].empty()) {
            if (attrs["required_forms"][0].is_string()) {
                curriculum.requiredForms_ = attrs["required_forms"].get<std::vector<std::string>>();
            } else if (attrs["required_forms"][0].is_number()) {
                // Convert integer IDs to string form IDs
                for (const auto& id : attrs["required_forms"]) {
                    std::string formId = typeIdToFormId(id.get<int>());
                    if (!formId.empty()) {
                        curriculum.requiredForms_.push_back(formId);
                    }
                }
            }
        }
    } else if (attrs.contains("required_form_ids") && !attrs["required_form_ids"].is_null()) {
        // Array of integer form type IDs
        for (const auto& id : attrs["required_form_ids"]) {
            std::string formId = typeIdToFormId(id.get<int>());
            if (!formId.empty()) {
                curriculum.requiredForms_.push_back(formId);
            }
        }
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
