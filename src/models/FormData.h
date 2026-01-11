#ifndef FORMDATA_H
#define FORMDATA_H

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Represents a field value in a form
 */
struct FormFieldValue {
    std::string stringValue;
    int intValue = 0;
    double doubleValue = 0.0;
    bool boolValue = false;
    std::vector<std::string> arrayValue;
    std::string type = "string";  // "string", "int", "double", "bool", "array", "file"

    nlohmann::json toJson() const;
    static FormFieldValue fromJson(const nlohmann::json& json);
};

/**
 * @brief Represents form data submitted by a student
 */
class FormData {
public:
    FormData();
    FormData(const std::string& formId, const std::string& studentId);

    // Getters
    std::string getFormId() const { return formId_; }
    std::string getStudentId() const { return studentId_; }
    std::string getSessionId() const { return sessionId_; }
    std::string getStatus() const { return status_; }
    std::chrono::system_clock::time_point getSubmittedAt() const { return submittedAt_; }
    std::chrono::system_clock::time_point getLastModified() const { return lastModified_; }

    // Setters
    void setFormId(const std::string& formId) { formId_ = formId; }
    void setStudentId(const std::string& studentId) { studentId_ = studentId; }
    void setSessionId(const std::string& sessionId) { sessionId_ = sessionId; }
    void setStatus(const std::string& status) { status_ = status; }
    void setSubmittedAt(const std::chrono::system_clock::time_point& time) { submittedAt_ = time; }

    // Field operations
    void setField(const std::string& fieldName, const std::string& value);
    void setField(const std::string& fieldName, int value);
    void setField(const std::string& fieldName, double value);
    void setField(const std::string& fieldName, bool value);
    void setField(const std::string& fieldName, const std::vector<std::string>& value);
    void setFileField(const std::string& fieldName, const std::string& filePath);

    FormFieldValue getField(const std::string& fieldName) const;
    bool hasField(const std::string& fieldName) const;
    std::vector<std::string> getFieldNames() const;
    void clearFields();

    // Validation
    bool isValid() const { return isValid_; }
    void setValid(bool valid) { isValid_ = valid; }
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    void addValidationError(const std::string& error);
    void clearValidationErrors();

    // Serialization
    nlohmann::json toJson() const;
    static FormData fromJson(const nlohmann::json& json);

    // Update modification time
    void touch();

private:
    std::string formId_;
    std::string studentId_;
    std::string sessionId_;
    std::string status_;  // "draft", "submitted", "approved", "rejected"
    std::map<std::string, FormFieldValue> fields_;
    std::chrono::system_clock::time_point submittedAt_;
    std::chrono::system_clock::time_point lastModified_;
    bool isValid_;
    std::vector<std::string> validationErrors_;
};

/**
 * @brief Form type definition with metadata
 */
struct FormTypeInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string category;
    int displayOrder;
    bool isRequired;
    std::vector<std::string> requiredForStudentTypes;
    std::vector<std::string> requiredForCurriculums;
    bool requiredForInternational;
    bool requiredForTransfer;
    bool requiredForVeteran;
    bool requiredForFinancialAid;
    int minAge;
    int maxAge;

    nlohmann::json toJson() const;
    static FormTypeInfo fromJson(const nlohmann::json& json);
};

} // namespace Models
} // namespace StudentIntake

#endif // FORMDATA_H
