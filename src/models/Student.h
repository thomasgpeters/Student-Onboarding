#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Represents a student in the intake system
 */
class Student {
public:
    Student();
    Student(const std::string& id, const std::string& email);

    // Getters
    std::string getId() const { return id_; }
    std::string getEmail() const { return email_; }
    std::string getFirstName() const { return firstName_; }
    std::string getLastName() const { return lastName_; }
    std::string getFullName() const;
    std::string getCurriculumId() const { return curriculumId_; }
    std::string getStudentType() const { return studentType_; }
    std::chrono::system_clock::time_point getEnrollmentDate() const { return enrollmentDate_; }
    std::chrono::system_clock::time_point getDateOfBirth() const { return dateOfBirth_; }
    bool isInternational() const { return isInternational_; }
    bool isTransferStudent() const { return isTransferStudent_; }
    bool isVeteran() const { return isVeteran_; }
    bool requiresFinancialAid() const { return requiresFinancialAid_; }
    std::vector<std::string> getCompletedForms() const { return completedForms_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setEmail(const std::string& email) { email_ = email; }
    void setFirstName(const std::string& firstName) { firstName_ = firstName; }
    void setLastName(const std::string& lastName) { lastName_ = lastName; }
    void setCurriculumId(const std::string& curriculumId) { curriculumId_ = curriculumId; }
    void setStudentType(const std::string& type) { studentType_ = type; }
    void setEnrollmentDate(const std::chrono::system_clock::time_point& date) { enrollmentDate_ = date; }
    void setDateOfBirth(const std::chrono::system_clock::time_point& date) { dateOfBirth_ = date; }
    void setInternational(bool isInternational) { isInternational_ = isInternational; }
    void setTransferStudent(bool isTransfer) { isTransferStudent_ = isTransfer; }
    void setVeteran(bool isVeteran) { isVeteran_ = isVeteran; }
    void setRequiresFinancialAid(bool requires) { requiresFinancialAid_ = requires; }

    // Form tracking
    void markFormCompleted(const std::string& formId);
    bool hasCompletedForm(const std::string& formId) const;
    void resetCompletedForms();

    // Serialization
    nlohmann::json toJson() const;
    static Student fromJson(const nlohmann::json& json);

    // Age calculation for form requirements
    int getAge() const;

private:
    std::string id_;
    std::string email_;
    std::string firstName_;
    std::string lastName_;
    std::string curriculumId_;
    std::string studentType_;  // "undergraduate", "graduate", "doctoral", "certificate"
    std::chrono::system_clock::time_point enrollmentDate_;
    std::chrono::system_clock::time_point dateOfBirth_;
    bool isInternational_;
    bool isTransferStudent_;
    bool isVeteran_;
    bool requiresFinancialAid_;
    std::vector<std::string> completedForms_;
};

} // namespace Models
} // namespace StudentIntake

#endif // STUDENT_H
