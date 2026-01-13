#ifndef STUDENT_ADDRESS_H
#define STUDENT_ADDRESS_H

#include <string>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

class StudentAddress {
public:
    StudentAddress();
    StudentAddress(const std::string& studentId, const std::string& addressType);

    // Getters
    std::string getId() const { return id_; }
    std::string getStudentId() const { return studentId_; }
    std::string getAddressType() const { return addressType_; }
    std::string getStreet1() const { return street1_; }
    std::string getStreet2() const { return street2_; }
    std::string getCity() const { return city_; }
    std::string getState() const { return state_; }
    std::string getPostalCode() const { return postalCode_; }
    std::string getCountry() const { return country_; }
    bool isPrimary() const { return isPrimary_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setStudentId(const std::string& studentId) { studentId_ = studentId; }
    void setAddressType(const std::string& addressType) { addressType_ = addressType; }
    void setStreet1(const std::string& street1) { street1_ = street1; }
    void setStreet2(const std::string& street2) { street2_ = street2; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setPostalCode(const std::string& postalCode) { postalCode_ = postalCode; }
    void setCountry(const std::string& country) { country_ = country; }
    void setPrimary(bool isPrimary) { isPrimary_ = isPrimary; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentAddress fromJson(const nlohmann::json& json);

    // Check if address has data
    bool isEmpty() const;

private:
    std::string id_;
    std::string studentId_;
    std::string addressType_;  // "permanent", "mailing", "billing"
    std::string street1_;
    std::string street2_;
    std::string city_;
    std::string state_;
    std::string postalCode_;
    std::string country_;
    bool isPrimary_;
};

} // namespace Models
} // namespace StudentIntake

#endif // STUDENT_ADDRESS_H
