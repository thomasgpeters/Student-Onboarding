#ifndef EMERGENCY_CONTACT_H
#define EMERGENCY_CONTACT_H

#include <string>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

class EmergencyContact {
public:
    EmergencyContact();
    EmergencyContact(const std::string& studentId);

    // Getters
    std::string getId() const { return id_; }
    std::string getStudentId() const { return studentId_; }
    std::string getFirstName() const { return firstName_; }
    std::string getLastName() const { return lastName_; }
    std::string getRelationship() const { return relationship_; }
    std::string getPhone() const { return phone_; }
    std::string getAlternatePhone() const { return alternatePhone_; }
    std::string getEmail() const { return email_; }
    std::string getStreet1() const { return street1_; }
    std::string getStreet2() const { return street2_; }
    std::string getCity() const { return city_; }
    std::string getState() const { return state_; }
    std::string getPostalCode() const { return postalCode_; }
    std::string getCountry() const { return country_; }
    bool isPrimary() const { return isPrimary_; }
    int getPriority() const { return priority_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(const std::string& studentId) { studentId_ = studentId; }
    void setFirstName(const std::string& firstName) { firstName_ = firstName; }
    void setLastName(const std::string& lastName) { lastName_ = lastName; }
    void setRelationship(const std::string& relationship) { relationship_ = relationship; }
    void setPhone(const std::string& phone) { phone_ = phone; }
    void setAlternatePhone(const std::string& phone) { alternatePhone_ = phone; }
    void setEmail(const std::string& email) { email_ = email; }
    void setStreet1(const std::string& street1) { street1_ = street1; }
    void setStreet2(const std::string& street2) { street2_ = street2; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setPostalCode(const std::string& postalCode) { postalCode_ = postalCode; }
    void setCountry(const std::string& country) { country_ = country; }
    void setPrimary(bool isPrimary) { isPrimary_ = isPrimary; }
    void setPriority(int priority) { priority_ = priority; }

    // Serialization
    nlohmann::json toJson() const;
    static EmergencyContact fromJson(const nlohmann::json& json);

    // Check if contact has data
    bool isEmpty() const;

private:
    std::string id_;
    std::string studentId_;
    std::string firstName_;
    std::string lastName_;
    std::string relationship_;
    std::string phone_;
    std::string alternatePhone_;
    std::string email_;
    std::string street1_;
    std::string street2_;
    std::string city_;
    std::string state_;
    std::string postalCode_;
    std::string country_;
    bool isPrimary_;
    int priority_;
};

} // namespace Models
} // namespace StudentIntake

#endif // EMERGENCY_CONTACT_H
