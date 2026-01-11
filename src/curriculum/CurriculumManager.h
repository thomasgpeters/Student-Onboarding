#ifndef CURRICULUM_MANAGER_H
#define CURRICULUM_MANAGER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "models/Curriculum.h"
#include "api/FormSubmissionService.h"

namespace StudentIntake {
namespace Curriculum {

/**
 * @brief Manages curriculum data and operations
 */
class CurriculumManager {
public:
    CurriculumManager();
    explicit CurriculumManager(std::shared_ptr<Api::FormSubmissionService> apiService);
    ~CurriculumManager();

    // Service dependency
    void setApiService(std::shared_ptr<Api::FormSubmissionService> service) { apiService_ = service; }

    // Load curriculums
    bool loadCurriculums();
    bool loadFromConfig(const std::string& configPath);

    // Curriculum access
    std::vector<Models::Curriculum> getAllCurriculums() const;
    std::vector<Models::Curriculum> getActiveCurriculums() const;
    std::vector<Models::Curriculum> getCurriculumsByDepartment(const std::string& department) const;
    std::vector<Models::Curriculum> getCurriculumsByDegreeType(const std::string& degreeType) const;
    Models::Curriculum getCurriculum(const std::string& curriculumId) const;

    // Department access
    std::vector<Models::Department> getAllDepartments() const;
    Models::Department getDepartment(const std::string& departmentId) const;
    std::vector<std::string> getDepartmentNames() const;

    // Degree types
    std::vector<std::string> getDegreeTypes() const;

    // Search and filter
    std::vector<Models::Curriculum> searchCurriculums(const std::string& query) const;

    // Check if data is loaded
    bool isLoaded() const { return isLoaded_; }

    // Cache management
    void clearCache();
    void refresh();

private:
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::vector<Models::Curriculum> curriculums_;
    std::vector<Models::Department> departments_;
    bool isLoaded_;

    void initializeDefaultCurriculums();
    void initializeDefaultDepartments();
};

} // namespace Curriculum
} // namespace StudentIntake

#endif // CURRICULUM_MANAGER_H
