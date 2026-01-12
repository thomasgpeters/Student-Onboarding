#include "CurriculumManager.h"
#include <fstream>
#include <algorithm>

namespace StudentIntake {
namespace Curriculum {

CurriculumManager::CurriculumManager()
    : isLoaded_(false) {
}

CurriculumManager::CurriculumManager(std::shared_ptr<Api::FormSubmissionService> apiService)
    : apiService_(apiService)
    , isLoaded_(false) {
}

CurriculumManager::~CurriculumManager() {
}

bool CurriculumManager::loadCurriculums() {
    if (apiService_) {
        curriculums_ = apiService_->getCurriculums();
        if (!curriculums_.empty()) {
            // Also load departments (from API or defaults)
            // For now, load default departments since API may not have department endpoint
            initializeDefaultDepartments();
            isLoaded_ = true;
            return true;
        }
    }

    // Fall back to defaults if API call fails
    initializeDefaultCurriculums();
    initializeDefaultDepartments();
    isLoaded_ = true;
    return true;
}

bool CurriculumManager::loadFromConfig(const std::string& configPath) {
    try {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json config;
        file >> config;

        curriculums_.clear();
        departments_.clear();

        if (config.contains("curriculums")) {
            for (const auto& item : config["curriculums"]) {
                curriculums_.push_back(Models::Curriculum::fromJson(item));
            }
        }

        if (config.contains("departments")) {
            for (const auto& item : config["departments"]) {
                departments_.push_back(Models::Department::fromJson(item));
            }
        }

        isLoaded_ = true;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void CurriculumManager::initializeDefaultCurriculums() {
    curriculums_.clear();

    // Computer Science Department
    Models::Curriculum cs_bs("cs_bs", "Bachelor of Science in Computer Science");
    cs_bs.setDescription("Comprehensive program covering software development, algorithms, and systems.");
    cs_bs.setDepartment("computer_science");
    cs_bs.setDegreeType("bachelor");
    cs_bs.setCreditHours(120);
    cs_bs.setDurationSemesters(8);
    cs_bs.setRequiredForms({"personal_info", "emergency_contact", "academic_history"});
    curriculums_.push_back(cs_bs);

    Models::Curriculum cs_ms("cs_ms", "Master of Science in Computer Science");
    cs_ms.setDescription("Advanced study in computer science with research opportunities.");
    cs_ms.setDepartment("computer_science");
    cs_ms.setDegreeType("master");
    cs_ms.setCreditHours(36);
    cs_ms.setDurationSemesters(4);
    cs_ms.setRequiredForms({"personal_info", "emergency_contact", "academic_history", "documents"});
    curriculums_.push_back(cs_ms);

    // Business Department
    Models::Curriculum bus_bba("bus_bba", "Bachelor of Business Administration");
    bus_bba.setDescription("Broad business education with specialization options.");
    bus_bba.setDepartment("business");
    bus_bba.setDegreeType("bachelor");
    bus_bba.setCreditHours(120);
    bus_bba.setDurationSemesters(8);
    bus_bba.setRequiredForms({"personal_info", "emergency_contact", "academic_history"});
    curriculums_.push_back(bus_bba);

    Models::Curriculum bus_mba("bus_mba", "Master of Business Administration");
    bus_mba.setDescription("Professional graduate degree in business management.");
    bus_mba.setDepartment("business");
    bus_mba.setDegreeType("master");
    bus_mba.setCreditHours(48);
    bus_mba.setDurationSemesters(4);
    bus_mba.setRequiredForms({"personal_info", "emergency_contact", "academic_history", "documents"});
    curriculums_.push_back(bus_mba);

    // Engineering Department
    Models::Curriculum eng_bsee("eng_bsee", "Bachelor of Science in Electrical Engineering");
    eng_bsee.setDescription("Study of electrical systems, electronics, and communications.");
    eng_bsee.setDepartment("engineering");
    eng_bsee.setDegreeType("bachelor");
    eng_bsee.setCreditHours(128);
    eng_bsee.setDurationSemesters(8);
    eng_bsee.setRequiredForms({"personal_info", "emergency_contact", "academic_history", "medical_info"});
    curriculums_.push_back(eng_bsee);

    Models::Curriculum eng_bsme("eng_bsme", "Bachelor of Science in Mechanical Engineering");
    eng_bsme.setDescription("Design and analysis of mechanical systems.");
    eng_bsme.setDepartment("engineering");
    eng_bsme.setDegreeType("bachelor");
    eng_bsme.setCreditHours(128);
    eng_bsme.setDurationSemesters(8);
    eng_bsme.setRequiredForms({"personal_info", "emergency_contact", "academic_history", "medical_info"});
    curriculums_.push_back(eng_bsme);

    // Nursing Department
    Models::Curriculum nur_bsn("nur_bsn", "Bachelor of Science in Nursing");
    nur_bsn.setDescription("Professional nursing education for healthcare careers.");
    nur_bsn.setDepartment("nursing");
    nur_bsn.setDegreeType("bachelor");
    nur_bsn.setCreditHours(120);
    nur_bsn.setDurationSemesters(8);
    nur_bsn.setRequiredForms({"personal_info", "emergency_contact", "academic_history", "medical_info", "documents"});
    curriculums_.push_back(nur_bsn);

    // Arts and Sciences
    Models::Curriculum art_ba("art_ba", "Bachelor of Arts in Psychology");
    art_ba.setDescription("Study of human behavior and mental processes.");
    art_ba.setDepartment("arts_sciences");
    art_ba.setDegreeType("bachelor");
    art_ba.setCreditHours(120);
    art_ba.setDurationSemesters(8);
    art_ba.setRequiredForms({"personal_info", "emergency_contact", "academic_history"});
    curriculums_.push_back(art_ba);

    // Certificate Programs
    Models::Curriculum cert_da("cert_da", "Certificate in Data Analytics");
    cert_da.setDescription("Professional certificate in data analysis and visualization.");
    cert_da.setDepartment("computer_science");
    cert_da.setDegreeType("certificate");
    cert_da.setCreditHours(18);
    cert_da.setDurationSemesters(2);
    cert_da.setRequiredForms({"personal_info", "emergency_contact"});
    curriculums_.push_back(cert_da);

    Models::Curriculum cert_pm("cert_pm", "Certificate in Project Management");
    cert_pm.setDescription("Professional certificate in project management methodologies.");
    cert_pm.setDepartment("business");
    cert_pm.setDegreeType("certificate");
    cert_pm.setCreditHours(15);
    cert_pm.setDurationSemesters(2);
    cert_pm.setRequiredForms({"personal_info", "emergency_contact"});
    curriculums_.push_back(cert_pm);
}

void CurriculumManager::initializeDefaultDepartments() {
    departments_.clear();

    Models::Department cs;
    cs.id = "computer_science";
    cs.name = "Computer Science";
    cs.code = "CS";
    cs.dean = "Dr. Jane Smith";
    cs.contactEmail = "cs@university.edu";
    cs.curriculumIds = {"cs_bs", "cs_ms", "cert_da"};
    departments_.push_back(cs);

    Models::Department bus;
    bus.id = "business";
    bus.name = "Business Administration";
    bus.code = "BUS";
    bus.dean = "Dr. John Brown";
    bus.contactEmail = "business@university.edu";
    bus.curriculumIds = {"bus_bba", "bus_mba", "cert_pm"};
    departments_.push_back(bus);

    Models::Department eng;
    eng.id = "engineering";
    eng.name = "Engineering";
    eng.code = "ENG";
    eng.dean = "Dr. Robert Johnson";
    eng.contactEmail = "engineering@university.edu";
    eng.curriculumIds = {"eng_bsee", "eng_bsme"};
    departments_.push_back(eng);

    Models::Department nur;
    nur.id = "nursing";
    nur.name = "Nursing";
    nur.code = "NUR";
    nur.dean = "Dr. Sarah Williams";
    nur.contactEmail = "nursing@university.edu";
    nur.curriculumIds = {"nur_bsn"};
    departments_.push_back(nur);

    Models::Department arts;
    arts.id = "arts_sciences";
    arts.name = "Arts and Sciences";
    arts.code = "A&S";
    arts.dean = "Dr. Michael Davis";
    arts.contactEmail = "artsci@university.edu";
    arts.curriculumIds = {"art_ba"};
    departments_.push_back(arts);
}

std::vector<Models::Curriculum> CurriculumManager::getAllCurriculums() const {
    return curriculums_;
}

std::vector<Models::Curriculum> CurriculumManager::getActiveCurriculums() const {
    std::vector<Models::Curriculum> active;
    for (const auto& curriculum : curriculums_) {
        if (curriculum.isActive()) {
            active.push_back(curriculum);
        }
    }
    return active;
}

std::vector<Models::Curriculum> CurriculumManager::getCurriculumsByDepartment(
    const std::string& department) const {
    std::vector<Models::Curriculum> result;
    for (const auto& curriculum : curriculums_) {
        if (curriculum.getDepartment() == department) {
            result.push_back(curriculum);
        }
    }
    return result;
}

std::vector<Models::Curriculum> CurriculumManager::getCurriculumsByDegreeType(
    const std::string& degreeType) const {
    std::vector<Models::Curriculum> result;
    for (const auto& curriculum : curriculums_) {
        if (curriculum.getDegreeType() == degreeType) {
            result.push_back(curriculum);
        }
    }
    return result;
}

Models::Curriculum CurriculumManager::getCurriculum(const std::string& curriculumId) const {
    for (const auto& curriculum : curriculums_) {
        if (curriculum.getId() == curriculumId) {
            return curriculum;
        }
    }
    return Models::Curriculum();
}

std::vector<Models::Department> CurriculumManager::getAllDepartments() const {
    return departments_;
}

Models::Department CurriculumManager::getDepartment(const std::string& departmentId) const {
    for (const auto& dept : departments_) {
        if (dept.id == departmentId) {
            return dept;
        }
    }
    return Models::Department();
}

std::vector<std::string> CurriculumManager::getDepartmentNames() const {
    std::vector<std::string> names;
    for (const auto& dept : departments_) {
        names.push_back(dept.name);
    }
    return names;
}

std::vector<std::string> CurriculumManager::getDegreeTypes() const {
    return {"bachelor", "master", "doctoral", "certificate", "associate"};
}

std::vector<Models::Curriculum> CurriculumManager::searchCurriculums(
    const std::string& query) const {
    std::vector<Models::Curriculum> results;
    std::string lowerQuery = query;
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

    for (const auto& curriculum : curriculums_) {
        std::string name = curriculum.getName();
        std::string desc = curriculum.getDescription();
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

        if (name.find(lowerQuery) != std::string::npos ||
            desc.find(lowerQuery) != std::string::npos) {
            results.push_back(curriculum);
        }
    }

    return results;
}

void CurriculumManager::clearCache() {
    curriculums_.clear();
    departments_.clear();
    isLoaded_ = false;
}

void CurriculumManager::refresh() {
    clearCache();
    loadCurriculums();
}

} // namespace Curriculum
} // namespace StudentIntake
