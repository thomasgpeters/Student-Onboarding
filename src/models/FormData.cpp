#include "FormData.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Models {

// FormFieldValue implementation
nlohmann::json FormFieldValue::toJson() const {
    nlohmann::json j;
    j["type"] = type;

    if (type == "string" || type == "file") {
        j["value"] = stringValue;
    } else if (type == "int") {
        j["value"] = intValue;
    } else if (type == "double") {
        j["value"] = doubleValue;
    } else if (type == "bool") {
        j["value"] = boolValue;
    } else if (type == "array") {
        j["value"] = arrayValue;
    }

    return j;
}

FormFieldValue FormFieldValue::fromJson(const nlohmann::json& json) {
    FormFieldValue field;

    if (json.contains("type")) {
        field.type = json["type"].get<std::string>();
    }

    if (json.contains("value")) {
        if (field.type == "string" || field.type == "file") {
            field.stringValue = json["value"].get<std::string>();
        } else if (field.type == "int") {
            field.intValue = json["value"].get<int>();
        } else if (field.type == "double") {
            field.doubleValue = json["value"].get<double>();
        } else if (field.type == "bool") {
            field.boolValue = json["value"].get<bool>();
        } else if (field.type == "array") {
            field.arrayValue = json["value"].get<std::vector<std::string>>();
        }
    }

    return field;
}

// FormData implementation
FormData::FormData()
    : formId_("")
    , studentId_("")
    , sessionId_("")
    , status_("draft")
    , isValid_(false) {
    submittedAt_ = std::chrono::system_clock::time_point{};
    lastModified_ = std::chrono::system_clock::now();
}

FormData::FormData(const std::string& formId, const std::string& studentId)
    : formId_(formId)
    , studentId_(studentId)
    , sessionId_("")
    , status_("draft")
    , isValid_(false) {
    submittedAt_ = std::chrono::system_clock::time_point{};
    lastModified_ = std::chrono::system_clock::now();
}

void FormData::setField(const std::string& fieldName, const std::string& value) {
    FormFieldValue field;
    field.type = "string";
    field.stringValue = value;
    fields_[fieldName] = field;
    touch();
}

void FormData::setField(const std::string& fieldName, int value) {
    FormFieldValue field;
    field.type = "int";
    field.intValue = value;
    fields_[fieldName] = field;
    touch();
}

void FormData::setField(const std::string& fieldName, double value) {
    FormFieldValue field;
    field.type = "double";
    field.doubleValue = value;
    fields_[fieldName] = field;
    touch();
}

void FormData::setField(const std::string& fieldName, bool value) {
    FormFieldValue field;
    field.type = "bool";
    field.boolValue = value;
    fields_[fieldName] = field;
    touch();
}

void FormData::setField(const std::string& fieldName, const std::vector<std::string>& value) {
    FormFieldValue field;
    field.type = "array";
    field.arrayValue = value;
    fields_[fieldName] = field;
    touch();
}

void FormData::setFileField(const std::string& fieldName, const std::string& filePath) {
    FormFieldValue field;
    field.type = "file";
    field.stringValue = filePath;
    fields_[fieldName] = field;
    touch();
}

FormFieldValue FormData::getField(const std::string& fieldName) const {
    auto it = fields_.find(fieldName);
    if (it != fields_.end()) {
        return it->second;
    }
    return FormFieldValue{};
}

bool FormData::hasField(const std::string& fieldName) const {
    return fields_.find(fieldName) != fields_.end();
}

std::vector<std::string> FormData::getFieldNames() const {
    std::vector<std::string> names;
    for (const auto& pair : fields_) {
        names.push_back(pair.first);
    }
    return names;
}

void FormData::clearFields() {
    fields_.clear();
    touch();
}

void FormData::addValidationError(const std::string& error) {
    validationErrors_.push_back(error);
    isValid_ = false;
}

void FormData::clearValidationErrors() {
    validationErrors_.clear();
    isValid_ = true;
}

void FormData::touch() {
    lastModified_ = std::chrono::system_clock::now();
}

nlohmann::json FormData::toJson() const {
    nlohmann::json j;
    j["formId"] = formId_;
    j["studentId"] = studentId_;
    j["sessionId"] = sessionId_;
    j["status"] = status_;
    j["isValid"] = isValid_;
    j["validationErrors"] = validationErrors_;

    // Fields
    nlohmann::json fieldsJson;
    for (const auto& pair : fields_) {
        fieldsJson[pair.first] = pair.second.toJson();
    }
    j["fields"] = fieldsJson;

    // Timestamps
    auto submittedTime = std::chrono::system_clock::to_time_t(submittedAt_);
    std::ostringstream submittedStream;
    submittedStream << std::put_time(std::gmtime(&submittedTime), "%Y-%m-%dT%H:%M:%SZ");
    j["submittedAt"] = submittedStream.str();

    auto modifiedTime = std::chrono::system_clock::to_time_t(lastModified_);
    std::ostringstream modifiedStream;
    modifiedStream << std::put_time(std::gmtime(&modifiedTime), "%Y-%m-%dT%H:%M:%SZ");
    j["lastModified"] = modifiedStream.str();

    return j;
}

FormData FormData::fromJson(const nlohmann::json& json) {
    FormData data;

    if (json.contains("formId")) data.formId_ = json["formId"].get<std::string>();
    if (json.contains("studentId")) data.studentId_ = json["studentId"].get<std::string>();
    if (json.contains("sessionId")) data.sessionId_ = json["sessionId"].get<std::string>();
    if (json.contains("status")) data.status_ = json["status"].get<std::string>();
    if (json.contains("isValid")) data.isValid_ = json["isValid"].get<bool>();
    if (json.contains("validationErrors")) {
        data.validationErrors_ = json["validationErrors"].get<std::vector<std::string>>();
    }

    if (json.contains("fields")) {
        for (auto& [key, value] : json["fields"].items()) {
            data.fields_[key] = FormFieldValue::fromJson(value);
        }
    }

    return data;
}

// FormTypeInfo implementation
nlohmann::json FormTypeInfo::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["description"] = description;
    j["category"] = category;
    j["displayOrder"] = displayOrder;
    j["isRequired"] = isRequired;
    j["requiredForStudentTypes"] = requiredForStudentTypes;
    j["requiredForCurriculums"] = requiredForCurriculums;
    j["requiredForInternational"] = requiredForInternational;
    j["requiredForTransfer"] = requiredForTransfer;
    j["requiredForVeteran"] = requiredForVeteran;
    j["requiredForFinancialAid"] = requiredForFinancialAid;
    j["minAge"] = minAge;
    j["maxAge"] = maxAge;
    return j;
}

FormTypeInfo FormTypeInfo::fromJson(const nlohmann::json& json) {
    FormTypeInfo info;
    if (json.contains("id")) info.id = json["id"].get<std::string>();
    if (json.contains("name")) info.name = json["name"].get<std::string>();
    if (json.contains("description")) info.description = json["description"].get<std::string>();
    if (json.contains("category")) info.category = json["category"].get<std::string>();
    if (json.contains("displayOrder")) info.displayOrder = json["displayOrder"].get<int>();
    if (json.contains("isRequired")) info.isRequired = json["isRequired"].get<bool>();
    if (json.contains("requiredForStudentTypes")) {
        info.requiredForStudentTypes = json["requiredForStudentTypes"].get<std::vector<std::string>>();
    }
    if (json.contains("requiredForCurriculums")) {
        info.requiredForCurriculums = json["requiredForCurriculums"].get<std::vector<std::string>>();
    }
    if (json.contains("requiredForInternational")) {
        info.requiredForInternational = json["requiredForInternational"].get<bool>();
    }
    if (json.contains("requiredForTransfer")) {
        info.requiredForTransfer = json["requiredForTransfer"].get<bool>();
    }
    if (json.contains("requiredForVeteran")) {
        info.requiredForVeteran = json["requiredForVeteran"].get<bool>();
    }
    if (json.contains("requiredForFinancialAid")) {
        info.requiredForFinancialAid = json["requiredForFinancialAid"].get<bool>();
    }
    if (json.contains("minAge")) info.minAge = json["minAge"].get<int>();
    if (json.contains("maxAge")) info.maxAge = json["maxAge"].get<int>();
    return info;
}

} // namespace Models
} // namespace StudentIntake
