#ifndef CURRICULUM_H
#define CURRICULUM_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Represents a curriculum/program of study
 */
class Curriculum {
public:
    Curriculum();
    Curriculum(const std::string& id, const std::string& name);

    // Getters
    std::string getId() const { return id_; }
    std::string getCode() const { return code_; }
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    std::string getDepartment() const { return department_; }
    std::string getDegreeType() const { return degreeType_; }
    int getCreditHours() const { return creditHours_; }
    int getDurationSemesters() const { return durationSemesters_; }
    std::vector<std::string> getRequiredForms() const { return requiredForms_; }
    std::vector<std::string> getPrerequisites() const { return prerequisites_; }
    bool isActive() const { return isActive_; }
    bool isOnline() const { return isOnline_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setDepartment(const std::string& department) { department_ = department; }
    void setDegreeType(const std::string& degreeType) { degreeType_ = degreeType; }
    void setCreditHours(int creditHours) { creditHours_ = creditHours; }
    void setDurationSemesters(int duration) { durationSemesters_ = duration; }
    void setRequiredForms(const std::vector<std::string>& forms) { requiredForms_ = forms; }
    void setPrerequisites(const std::vector<std::string>& prereqs) { prerequisites_ = prereqs; }
    void setActive(bool active) { isActive_ = active; }
    void setOnline(bool online) { isOnline_ = online; }

    // Form management
    void addRequiredForm(const std::string& formId);
    void removeRequiredForm(const std::string& formId);
    bool requiresForm(const std::string& formId) const;

    // Serialization
    nlohmann::json toJson() const;
    static Curriculum fromJson(const nlohmann::json& json);

private:
    std::string id_;
    std::string code_;  // Unique program code (required)
    std::string name_;
    std::string description_;
    std::string department_;
    std::string degreeType_;  // "bachelor", "master", "doctoral", "certificate", "associate"
    int creditHours_;
    int durationSemesters_;
    std::vector<std::string> requiredForms_;
    std::vector<std::string> prerequisites_;
    bool isActive_;
    bool isOnline_;
};

/**
 * @brief Department information
 */
struct Department {
    std::string id;
    std::string name;
    std::string code;
    std::string dean;
    std::string contactEmail;
    std::vector<std::string> curriculumIds;

    nlohmann::json toJson() const;
    static Department fromJson(const nlohmann::json& json);
};

} // namespace Models
} // namespace StudentIntake

#endif // CURRICULUM_H
