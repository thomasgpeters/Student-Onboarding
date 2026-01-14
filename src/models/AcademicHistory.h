#ifndef ACADEMIC_HISTORY_H
#define ACADEMIC_HISTORY_H

#include <string>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Model class representing an academic history record
 *
 * Each student can have multiple academic history records (high school, colleges, etc.)
 */
class AcademicHistory {
public:
    AcademicHistory();
    AcademicHistory(const std::string& studentId);

    // Getters
    std::string getId() const { return id_; }
    std::string getStudentId() const { return studentId_; }
    std::string getInstitutionName() const { return institutionName_; }
    std::string getInstitutionType() const { return institutionType_; }
    std::string getInstitutionCity() const { return institutionCity_; }
    std::string getInstitutionState() const { return institutionState_; }
    std::string getInstitutionCountry() const { return institutionCountry_; }
    std::string getDegreeEarned() const { return degreeEarned_; }
    std::string getMajor() const { return major_; }
    std::string getMinor() const { return minor_; }
    double getGpa() const { return gpa_; }
    double getGpaScale() const { return gpaScale_; }
    std::string getStartDate() const { return startDate_; }
    std::string getEndDate() const { return endDate_; }
    std::string getGraduationDate() const { return graduationDate_; }
    bool isCurrentlyAttending() const { return isCurrentlyAttending_; }
    bool isTranscriptReceived() const { return transcriptReceived_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(const std::string& studentId) { studentId_ = studentId; }
    void setInstitutionName(const std::string& name) { institutionName_ = name; }
    void setInstitutionType(const std::string& type) { institutionType_ = type; }
    void setInstitutionCity(const std::string& city) { institutionCity_ = city; }
    void setInstitutionState(const std::string& state) { institutionState_ = state; }
    void setInstitutionCountry(const std::string& country) { institutionCountry_ = country; }
    void setDegreeEarned(const std::string& degree) { degreeEarned_ = degree; }
    void setMajor(const std::string& major) { major_ = major; }
    void setMinor(const std::string& minor) { minor_ = minor; }
    void setGpa(double gpa) { gpa_ = gpa; }
    void setGpaScale(double scale) { gpaScale_ = scale; }
    void setStartDate(const std::string& date) { startDate_ = date; }
    void setEndDate(const std::string& date) { endDate_ = date; }
    void setGraduationDate(const std::string& date) { graduationDate_ = date; }
    void setCurrentlyAttending(bool attending) { isCurrentlyAttending_ = attending; }
    void setTranscriptReceived(bool received) { transcriptReceived_ = received; }

    // JSON serialization
    nlohmann::json toJson() const;
    static AcademicHistory fromJson(const nlohmann::json& json);

    // Utility
    bool isEmpty() const;

private:
    std::string id_;
    std::string studentId_;
    std::string institutionName_;
    std::string institutionType_;  // "High School", "College", "University", "Trade School", etc.
    std::string institutionCity_;
    std::string institutionState_;
    std::string institutionCountry_;
    std::string degreeEarned_;
    std::string major_;
    std::string minor_;
    double gpa_;
    double gpaScale_;
    std::string startDate_;
    std::string endDate_;
    std::string graduationDate_;
    bool isCurrentlyAttending_;
    bool transcriptReceived_;
};

} // namespace Models
} // namespace StudentIntake

#endif // ACADEMIC_HISTORY_H
