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
    std::string getMiddleName() const { return middleName_; }
    std::string getLastName() const { return lastName_; }
    std::string getPreferredName() const { return preferredName_; }
    std::string getFullName() const;
    std::string getCurriculumId() const { return curriculumId_; }
    std::string getStudentType() const { return studentType_; }
    std::string getGender() const { return gender_; }
    std::string getPreferredPronouns() const { return preferredPronouns_; }
    std::string getPhoneNumber() const { return phoneNumber_; }
    std::string getAlternatePhone() const { return alternatePhone_; }
    std::string getAddressLine1() const { return addressLine1_; }
    std::string getAddressLine2() const { return addressLine2_; }
    std::string getCity() const { return city_; }
    std::string getState() const { return state_; }
    std::string getZipCode() const { return zipCode_; }
    std::string getSsn() const { return ssn_; }
    std::string getCitizenshipStatus() const { return citizenshipStatus_; }
    std::chrono::system_clock::time_point getEnrollmentDate() const { return enrollmentDate_; }
    std::chrono::system_clock::time_point getDateOfBirth() const { return dateOfBirth_; }
    std::string getCreatedAt() const { return createdAt_; }
    bool isInternational() const { return isInternational_; }
    bool isTransferStudent() const { return isTransferStudent_; }
    bool isVeteran() const { return isVeteran_; }
    bool requiresFinancialAid() const { return requiresFinancialAid_; }
    std::vector<std::string> getCompletedForms() const { return completedForms_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setEmail(const std::string& email) { email_ = email; }
    void setFirstName(const std::string& firstName) { firstName_ = firstName; }
    void setMiddleName(const std::string& middleName) { middleName_ = middleName; }
    void setLastName(const std::string& lastName) { lastName_ = lastName; }
    void setPreferredName(const std::string& preferredName) { preferredName_ = preferredName; }
    void setCurriculumId(const std::string& curriculumId) { curriculumId_ = curriculumId; }
    void setStudentType(const std::string& type) { studentType_ = type; }
    void setGender(const std::string& gender) { gender_ = gender; }
    void setPreferredPronouns(const std::string& pronouns) { preferredPronouns_ = pronouns; }
    void setPhoneNumber(const std::string& phone) { phoneNumber_ = phone; }
    void setAlternatePhone(const std::string& phone) { alternatePhone_ = phone; }
    void setAddressLine1(const std::string& address) { addressLine1_ = address; }
    void setAddressLine2(const std::string& address) { addressLine2_ = address; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setZipCode(const std::string& zip) { zipCode_ = zip; }
    void setSsn(const std::string& ssn) { ssn_ = ssn; }
    void setCitizenshipStatus(const std::string& status) { citizenshipStatus_ = status; }
    void setEnrollmentDate(const std::chrono::system_clock::time_point& date) { enrollmentDate_ = date; }
    void setDateOfBirth(const std::chrono::system_clock::time_point& date) { dateOfBirth_ = date; }
    void setCreatedAt(const std::string& createdAt) { createdAt_ = createdAt; }
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
    std::string middleName_;
    std::string lastName_;
    std::string preferredName_;
    std::string curriculumId_;
    std::string studentType_;  // "undergraduate", "graduate", "doctoral", "certificate"
    std::string gender_;
    std::string preferredPronouns_;
    std::string phoneNumber_;
    std::string alternatePhone_;
    std::string addressLine1_;
    std::string addressLine2_;
    std::string city_;
    std::string state_;
    std::string zipCode_;
    std::string ssn_;
    std::string citizenshipStatus_;
    std::chrono::system_clock::time_point enrollmentDate_;
    std::chrono::system_clock::time_point dateOfBirth_;
    std::string createdAt_;
    bool isInternational_;
    bool isTransferStudent_;
    bool isVeteran_;
    bool requiresFinancialAid_;
    std::vector<std::string> completedForms_;
};

} // namespace Models
} // namespace StudentIntake

#endif // STUDENT_H
