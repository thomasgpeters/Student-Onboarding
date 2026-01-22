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
    int getDepartmentId() const { return departmentId_; }
    std::string getDegreeType() const { return degreeType_; }
    int getCreditHours() const { return creditHours_; }
    int getDurationSemesters() const { return durationSemesters_; }
    std::string getDurationInterval() const { return durationInterval_; }
    std::string getFormattedDuration() const;  // Returns "2 Semesters", "1 Month", etc.
    std::vector<std::string> getRequiredForms() const { return requiredForms_; }
    std::vector<std::string> getPrerequisites() const { return prerequisites_; }
    bool isActive() const { return isActive_; }
    bool isOnline() const { return isOnline_; }
    bool isEndorsement() const { return isEndorsement_; }
    std::string getCdlClass() const { return cdlClass_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& description) { description_ = description; }
    void setDepartment(const std::string& department) { department_ = department; }
    void setDepartmentId(int departmentId) { departmentId_ = departmentId; }
    void setDegreeType(const std::string& degreeType) { degreeType_ = degreeType; }
    void setCreditHours(int creditHours) { creditHours_ = creditHours; }
    void setDurationSemesters(int duration) { durationSemesters_ = duration; }
    void setDurationInterval(const std::string& interval) { durationInterval_ = interval; }
    void setRequiredForms(const std::vector<std::string>& forms) { requiredForms_ = forms; }
    void setPrerequisites(const std::vector<std::string>& prereqs) { prerequisites_ = prereqs; }
    void setActive(bool active) { isActive_ = active; }
    void setOnline(bool online) { isOnline_ = online; }
    void setEndorsement(bool endorsement) { isEndorsement_ = endorsement; }
    void setCdlClass(const std::string& cdlClass) { cdlClass_ = cdlClass; }

    // Form management
    void addRequiredForm(const std::string& formId);
    void removeRequiredForm(const std::string& formId);
    bool requiresForm(const std::string& formId) const;

    // Form type ID mapping (string ID <-> integer ID)
    static int formIdToTypeId(const std::string& formId);
    static std::string typeIdToFormId(int typeId);

    // Department ID mapping (integer ID <-> name)
    static std::string departmentIdToName(int departmentId);

    // Serialization
    nlohmann::json toJson() const;
    static Curriculum fromJson(const nlohmann::json& json);

private:
    std::string id_;
    std::string code_;  // Unique program code (required)
    std::string name_;
    std::string description_;
    std::string department_;      // Department name (for display)
    int departmentId_;            // Department ID (for API)
    std::string degreeType_;  // "bachelor", "master", "doctoral", "certificate", "associate"
    int creditHours_;
    int durationSemesters_;
    std::string durationInterval_;  // "semester", "month", "week", "day"
    std::vector<std::string> requiredForms_;
    std::vector<std::string> prerequisites_;
    bool isActive_;
    bool isOnline_;
    bool isEndorsement_;       // True for add-on endorsements, false for base programs
    std::string cdlClass_;     // CDL class: "A", "B", or empty for non-CDL
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
