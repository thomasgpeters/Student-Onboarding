#include "FormSubmissionService.h"
#include <thread>
#include <iostream>

namespace StudentIntake {
namespace Api {

FormSubmissionService::FormSubmissionService()
    : apiClient_(std::make_shared<ApiClient>()) {
}

FormSubmissionService::FormSubmissionService(std::shared_ptr<ApiClient> apiClient)
    : apiClient_(apiClient) {
}

FormSubmissionService::~FormSubmissionService() {
}

std::string FormSubmissionService::getEndpointForForm(const std::string& formId) const {
    // Map form IDs to ApiLogicServer endpoints (capitalized resource names)
    static const std::map<std::string, std::string> endpointMap = {
        {"personal_info", "/Student"},           // Personal info updates student record
        {"emergency_contact", "/EmergencyContact"},
        {"medical_info", "/MedicalInfo"},
        {"academic_history", "/AcademicHistory"},
        {"financial_aid", "/FinancialAid"},
        {"documents", "/Document"},
        {"consent", "/Consent"}
    };

    auto it = endpointMap.find(formId);
    if (it != endpointMap.end()) {
        return it->second;
    }
    return "/FormSubmission";  // Generic endpoint
}

nlohmann::json FormSubmissionService::prepareFormPayload(const std::string& studentId,
                                                          const Models::FormData& data,
                                                          const std::string& resourceType) {
    // Extract field values from FormData
    nlohmann::json attributes;

    // student_id should be an integer for the database
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        // If conversion fails, use as string (shouldn't happen with valid IDs)
        attributes["student_id"] = studentId;
    }

    // Convert FormData fields to flat attributes for JSON:API
    auto fieldNames = data.getFieldNames();
    for (const auto& fieldName : fieldNames) {
        auto value = data.getField(fieldName);
        // Convert camelCase to snake_case for API
        std::string snakeName = fieldName;
        for (size_t i = 1; i < snakeName.size(); ++i) {
            if (std::isupper(snakeName[i])) {
                snakeName.insert(i, "_");
                snakeName[i + 1] = std::tolower(snakeName[i + 1]);
                ++i;
            }
        }
        // Make first char lowercase
        if (!snakeName.empty()) {
            snakeName[0] = std::tolower(snakeName[0]);
        }

        if (value.type == "bool") {
            attributes[snakeName] = value.boolValue;
        } else if (value.type == "int") {
            attributes[snakeName] = value.intValue;
        } else {
            attributes[snakeName] = value.stringValue;
        }
    }

    // Wrap in JSON:API format
    nlohmann::json payload;
    payload["data"] = {
        {"type", resourceType},
        {"attributes", attributes}
    };

    return payload;
}

SubmissionResult FormSubmissionService::parseSubmissionResponse(const ApiResponse& response) {
    SubmissionResult result;
    result.success = response.isSuccess();

    if (response.isSuccess()) {
        result.responseData = response.getJson();

        // Handle 'id' field - could be int or string
        if (result.responseData.contains("id")) {
            if (result.responseData["id"].is_string()) {
                result.submissionId = result.responseData["id"].get<std::string>();
            } else if (result.responseData["id"].is_number()) {
                result.submissionId = std::to_string(result.responseData["id"].get<int>());
            }
        }
        // Also check nested 'data' object (ApiLogicServer format)
        if (result.responseData.contains("data") && result.responseData["data"].is_object()) {
            auto& data = result.responseData["data"];
            if (data.contains("id")) {
                if (data["id"].is_string()) {
                    result.submissionId = data["id"].get<std::string>();
                } else if (data["id"].is_number()) {
                    result.submissionId = std::to_string(data["id"].get<int>());
                }
            }
        }

        if (result.responseData.contains("message")) {
            if (result.responseData["message"].is_string()) {
                result.message = result.responseData["message"].get<std::string>();
            }
        } else {
            result.message = "Form submitted successfully";
        }
    } else {
        result.message = response.errorMessage;
        auto json = response.getJson();
        if (json.contains("errors")) {
            if (json["errors"].is_array()) {
                for (const auto& err : json["errors"]) {
                    if (err.is_string()) {
                        result.errors.push_back(err.get<std::string>());
                    } else if (err.is_object() && err.contains("message")) {
                        result.errors.push_back(err["message"].get<std::string>());
                    }
                }
            }
        } else if (json.contains("message")) {
            if (json["message"].is_string()) {
                result.errors.push_back(json["message"].get<std::string>());
            }
        } else {
            result.errors.push_back("Submission failed with status: " +
                                    std::to_string(response.statusCode));
        }
    }

    return result;
}

// Student API endpoints
SubmissionResult FormSubmissionService::registerStudent(const Models::Student& student,
                                                         const std::string& password) {
    std::cout << "[FormSubmissionService] registerStudent called" << std::endl;

    // ApiLogicServer uses JSON:API format (SAFRS)
    nlohmann::json attributes = student.toJson();
    // Database column is password_hash (ideally should be hashed server-side)
    attributes["password_hash"] = password;

    // Wrap in JSON:API format
    nlohmann::json payload;
    payload["data"] = {
        {"type", "Student"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] Payload: " << payload.dump() << std::endl;
    std::cout.flush();

    // ApiLogicServer uses capitalized resource names
    ApiResponse response = apiClient_->post("/Student", payload);

    std::cout << "[FormSubmissionService] API response received - status: " << response.statusCode
              << ", success: " << response.success << std::endl;
    std::cout << "[FormSubmissionService] Response body: " << response.body << std::endl;
    std::cout.flush();

    SubmissionResult result = parseSubmissionResponse(response);
    std::cout << "[FormSubmissionService] Parsed submissionId: '" << result.submissionId << "'" << std::endl;
    std::cout.flush();

    return result;
}

SubmissionResult FormSubmissionService::loginStudent(const std::string& email,
                                                      const std::string& password) {
    // ApiLogicServer without auth - query Student by email
    // Use SAFRS filter syntax: filter[field]=value
    std::string endpoint = "/Student?filter[email]=" + email;

    ApiResponse response = apiClient_->get(endpoint);
    SubmissionResult result;

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json students;

        // Handle JSON:API format
        if (json.contains("data")) {
            students = json["data"];
        } else if (json.is_array()) {
            students = json;
        }

        if (students.is_array() && !students.empty()) {
            auto studentData = students[0];
            auto attributes = studentData.contains("attributes")
                ? studentData["attributes"]
                : studentData;

            // Check password (stored as password_hash in DB)
            std::string storedPassword = attributes.value("password_hash", "");
            if (storedPassword == password) {
                result.success = true;
                result.message = "Login successful";
                result.responseData = studentData;

                // Extract student ID
                if (studentData.contains("id")) {
                    if (studentData["id"].is_string()) {
                        result.submissionId = studentData["id"].get<std::string>();
                    } else if (studentData["id"].is_number()) {
                        result.submissionId = std::to_string(studentData["id"].get<int>());
                    }
                }
                std::cout << "[FormSubmissionService] Login successful, student ID: " << result.submissionId << std::endl;
                std::cout.flush();
            } else {
                result.success = false;
                result.message = "Invalid password";
            }
        } else {
            result.success = false;
            result.message = "Student not found";
        }
    } else {
        result.success = false;
        result.message = "Failed to connect to server";
    }

    return result;
}

SubmissionResult FormSubmissionService::getStudentProfile(const std::string& studentId) {
    ApiResponse response = apiClient_->get("/Student/" + studentId);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::updateStudentProfile(const Models::Student& student) {
    // Wrap in JSON:API format
    nlohmann::json attributes = student.toJson();
    nlohmann::json payload;
    payload["data"] = {
        {"type", "Student"},
        {"id", student.getId()},
        {"attributes", attributes}
    };
    ApiResponse response = apiClient_->patch("/Student/" + student.getId(), payload);
    return parseSubmissionResponse(response);
}

// Curriculum API endpoints
std::vector<Models::Curriculum> FormSubmissionService::getCurriculums() {
    std::vector<Models::Curriculum> curriculums;

    ApiResponse response = apiClient_->get("/Curriculum");
    if (response.isSuccess()) {
        auto json = response.getJson();
        if (json.is_array()) {
            for (const auto& item : json) {
                curriculums.push_back(Models::Curriculum::fromJson(item));
            }
        } else if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                curriculums.push_back(Models::Curriculum::fromJson(item));
            }
        }
    }

    return curriculums;
}

Models::Curriculum FormSubmissionService::getCurriculum(const std::string& curriculumId) {
    ApiResponse response = apiClient_->get("/Curriculum/" + curriculumId);
    if (response.isSuccess()) {
        return Models::Curriculum::fromJson(response.getJson());
    }
    return Models::Curriculum();
}

// Form type configuration
std::vector<Models::FormTypeInfo> FormSubmissionService::getFormTypes() {
    std::vector<Models::FormTypeInfo> formTypes;

    ApiResponse response = apiClient_->get("/FormType");
    if (response.isSuccess()) {
        auto json = response.getJson();
        if (json.is_array()) {
            for (const auto& item : json) {
                formTypes.push_back(Models::FormTypeInfo::fromJson(item));
            }
        } else if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                formTypes.push_back(Models::FormTypeInfo::fromJson(item));
            }
        }
    }

    return formTypes;
}

Models::FormTypeInfo FormSubmissionService::getFormType(const std::string& formTypeId) {
    ApiResponse response = apiClient_->get("/FormType/" + formTypeId);
    if (response.isSuccess()) {
        return Models::FormTypeInfo::fromJson(response.getJson());
    }
    return Models::FormTypeInfo{};
}

// Individual form submission methods
SubmissionResult FormSubmissionService::submitPersonalInfo(const std::string& studentId,
                                                            const Models::FormData& data) {
    // Personal info updates the student record (PATCH)
    nlohmann::json payload = prepareFormPayload(studentId, data, "Student");
    // Add student ID to the data for JSON:API PATCH
    payload["data"]["id"] = studentId;
    std::cout << "[FormSubmissionService] submitPersonalInfo payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->patch("/Student/" + studentId, payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitEmergencyContact(const std::string& studentId,
                                                                const Models::FormData& data) {
    // Transform form data to match EmergencyContact API schema (snake_case for database)
    nlohmann::json attributes;

    // student_id as integer
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        attributes["student_id"] = studentId;
    }

    // Debug: log what fields are available
    std::cout << "[FormSubmissionService] submitEmergencyContact - studentId: " << studentId << std::endl;
    std::cout << "[FormSubmissionService] Available fields:" << std::endl;
    for (const auto& fieldName : data.getFieldNames()) {
        std::cout << "  - " << fieldName << std::endl;
    }
    std::cout.flush();

    // Get primary contact data (contact1_) - use snake_case for API
    attributes["first_name"] = data.hasField("contact1_FirstName") ? data.getField("contact1_FirstName").stringValue : "";
    attributes["last_name"] = data.hasField("contact1_LastName") ? data.getField("contact1_LastName").stringValue : "";
    attributes["contact_relationship"] = data.hasField("contact1_Relationship") ? data.getField("contact1_Relationship").stringValue : "";
    attributes["phone"] = data.hasField("contact1_Phone") ? data.getField("contact1_Phone").stringValue : "";
    attributes["alternate_phone"] = data.hasField("contact1_AlternatePhone") ? data.getField("contact1_AlternatePhone").stringValue : "";
    attributes["email"] = data.hasField("contact1_Email") ? data.getField("contact1_Email").stringValue : "";
    attributes["street1"] = data.hasField("contact1_AddressLine1") ? data.getField("contact1_AddressLine1").stringValue : "";
    attributes["city"] = data.hasField("contact1_City") ? data.getField("contact1_City").stringValue : "";
    attributes["state"] = data.hasField("contact1_State") ? data.getField("contact1_State").stringValue : "";
    attributes["postal_code"] = data.hasField("contact1_ZipCode") ? data.getField("contact1_ZipCode").stringValue : "";
    attributes["is_primary"] = true;
    attributes["priority"] = 1;

    nlohmann::json payload;
    payload["data"] = {
        {"type", "EmergencyContact"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] submitEmergencyContact payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/EmergencyContact", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitMedicalInfo(const std::string& studentId,
                                                           const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data, "MedicalInfo");
    std::cout << "[FormSubmissionService] submitMedicalInfo payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/MedicalInfo", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitAcademicHistory(const std::string& studentId,
                                                               const Models::FormData& data) {
    // Transform form data to match AcademicHistory table schema (snake_case)
    nlohmann::json attributes;

    // student_id as integer
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        attributes["student_id"] = studentId;
    }

    // Debug: log what fields are available
    std::cout << "[FormSubmissionService] submitAcademicHistory - studentId: " << studentId << std::endl;
    std::cout.flush();

    // Map form fields to database columns
    // If has previous college, use college data; otherwise use high school data
    bool hasPreviousCollege = data.hasField("hasPreviousCollege") && data.getField("hasPreviousCollege").boolValue;

    if (hasPreviousCollege) {
        attributes["institution_name"] = data.hasField("collegeName") ? data.getField("collegeName").stringValue : "";
        attributes["institution_type"] = "College";
        attributes["institution_city"] = data.hasField("collegeCity") ? data.getField("collegeCity").stringValue : "";
        attributes["institution_state"] = data.hasField("collegeState") ? data.getField("collegeState").stringValue : "";
        attributes["degree_earned"] = data.hasField("collegeDegree") ? data.getField("collegeDegree").stringValue : "";
        attributes["major"] = data.hasField("collegeMajor") ? data.getField("collegeMajor").stringValue : "";
        std::string gpaStr = data.hasField("collegeGpa") ? data.getField("collegeGpa").stringValue : "";
        if (!gpaStr.empty()) {
            try {
                attributes["gpa"] = std::stod(gpaStr);
            } catch (...) {
                attributes["gpa"] = nullptr;
            }
        }
        attributes["is_currently_attending"] = data.hasField("degreeCompleted") ? !data.getField("degreeCompleted").boolValue : false;
    } else {
        attributes["institution_name"] = data.hasField("highSchoolName") ? data.getField("highSchoolName").stringValue : "";
        attributes["institution_type"] = "High School";
        attributes["institution_city"] = data.hasField("highSchoolCity") ? data.getField("highSchoolCity").stringValue : "";
        attributes["institution_state"] = data.hasField("highSchoolState") ? data.getField("highSchoolState").stringValue : "";
        attributes["degree_earned"] = data.hasField("hasGed") && data.getField("hasGed").boolValue ? "GED" : "High School Diploma";
        std::string gpaStr = data.hasField("highSchoolGpa") ? data.getField("highSchoolGpa").stringValue : "";
        if (!gpaStr.empty()) {
            try {
                attributes["gpa"] = std::stod(gpaStr);
            } catch (...) {
                attributes["gpa"] = nullptr;
            }
        }
        attributes["graduation_date"] = data.hasField("highSchoolGradDate") ? data.getField("highSchoolGradDate").stringValue : "";
        attributes["is_currently_attending"] = false;
    }

    attributes["gpa_scale"] = 4.0;
    attributes["transcript_received"] = false;

    nlohmann::json payload;
    payload["data"] = {
        {"type", "AcademicHistory"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] submitAcademicHistory payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/AcademicHistory", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitFinancialAid(const std::string& studentId,
                                                            const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data, "FinancialAid");
    std::cout << "[FormSubmissionService] submitFinancialAid payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/FinancialAid", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitDocuments(const std::string& studentId,
                                                         const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data, "Document");
    std::cout << "[FormSubmissionService] submitDocuments payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/Document", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitConsent(const std::string& studentId,
                                                       const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data, "Consent");
    std::cout << "[FormSubmissionService] submitConsent payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/Consent", payload);
    return parseSubmissionResponse(response);
}

// Generic form submission
SubmissionResult FormSubmissionService::submitForm(const std::string& studentId,
                                                    const std::string& formId,
                                                    const Models::FormData& data) {
    // Route to specialized submission functions for forms that need custom handling
    if (formId == "emergency_contact") {
        return submitEmergencyContact(studentId, data);
    }
    if (formId == "academic_history") {
        return submitAcademicHistory(studentId, data);
    }

    std::string endpoint = getEndpointForForm(formId);

    // Map formId to resource type for JSON:API
    static const std::map<std::string, std::string> resourceTypeMap = {
        {"personal_info", "Student"},
        {"emergency_contact", "EmergencyContact"},
        {"medical_info", "MedicalInfo"},
        {"academic_history", "AcademicHistory"},
        {"financial_aid", "FinancialAid"},
        {"documents", "Document"},
        {"consent", "Consent"}
    };

    std::string resourceType = "FormSubmission";
    auto it = resourceTypeMap.find(formId);
    if (it != resourceTypeMap.end()) {
        resourceType = it->second;
    }

    nlohmann::json payload = prepareFormPayload(studentId, data, resourceType);
    std::cout << "[FormSubmissionService] submitForm(" << formId << ") payload: " << payload.dump() << std::endl;

    // Personal info uses PATCH on student, others use POST
    ApiResponse response;
    if (formId == "personal_info") {
        payload["data"]["id"] = studentId;
        response = apiClient_->patch("/Student/" + studentId, payload);
    } else {
        response = apiClient_->post(endpoint, payload);
    }

    return parseSubmissionResponse(response);
}

void FormSubmissionService::submitFormAsync(const std::string& studentId,
                                             const std::string& formId,
                                             const Models::FormData& data,
                                             SubmissionCallback callback) {
    std::thread([this, studentId, formId, data, callback]() {
        SubmissionResult result = submitForm(studentId, formId, data);
        if (callback) {
            callback(result);
        }
    }).detach();
}

// Form data retrieval
Models::FormData FormSubmissionService::getFormData(const std::string& studentId,
                                                     const std::string& formId) {
    std::string endpoint = getEndpointForForm(formId);
    ApiResponse response = apiClient_->get(endpoint + "?student_id=" + studentId);

    if (response.isSuccess()) {
        auto json = response.getJson();
        if (json.is_array() && !json.empty()) {
            return Models::FormData::fromJson(json[0]);
        } else if (json.contains("data") && json["data"].is_array() && !json["data"].empty()) {
            return Models::FormData::fromJson(json["data"][0]);
        }
    }

    return Models::FormData(formId, studentId);
}

std::vector<Models::FormData> FormSubmissionService::getAllFormData(const std::string& studentId) {
    std::vector<Models::FormData> allData;

    std::vector<std::string> formIds = {
        "personal_info", "emergency_contact", "medical_info",
        "academic_history", "financial_aid", "documents", "consent"
    };

    for (const auto& formId : formIds) {
        Models::FormData data = getFormData(studentId, formId);
        if (!data.getFieldNames().empty()) {
            allData.push_back(data);
        }
    }

    return allData;
}

// Form status
std::string FormSubmissionService::getFormStatus(const std::string& studentId,
                                                  const std::string& formId) {
    Models::FormData data = getFormData(studentId, formId);
    return data.getStatus();
}

std::vector<std::string> FormSubmissionService::getCompletedFormIds(const std::string& studentId) {
    std::vector<std::string> completed;

    ApiResponse response = apiClient_->get("/FormSubmission?student_id=" + studentId +
                                           "&status=submitted");
    if (response.isSuccess()) {
        auto json = response.getJson();
        auto items = json.is_array() ? json : json["data"];
        for (const auto& item : items) {
            if (item.contains("form_id")) {
                completed.push_back(item["form_id"].get<std::string>());
            } else if (item.contains("formId")) {
                completed.push_back(item["formId"].get<std::string>());
            }
        }
    }

    return completed;
}

bool FormSubmissionService::isFormComplete(const std::string& studentId,
                                            const std::string& formId) {
    std::string status = getFormStatus(studentId, formId);
    return status == "submitted" || status == "approved";
}

// Batch operations
SubmissionResult FormSubmissionService::submitAllForms(const std::string& studentId,
                                                        const std::vector<Models::FormData>& forms) {
    SubmissionResult finalResult;
    finalResult.success = true;

    for (const auto& form : forms) {
        SubmissionResult result = submitForm(studentId, form.getFormId(), form);
        if (!result.success) {
            finalResult.success = false;
            finalResult.errors.insert(finalResult.errors.end(),
                                      result.errors.begin(), result.errors.end());
        }
    }

    if (finalResult.success) {
        finalResult.message = "All forms submitted successfully";
    } else {
        finalResult.message = "Some forms failed to submit";
    }

    return finalResult;
}

SubmissionResult FormSubmissionService::finalizeIntake(const std::string& studentId) {
    // Update student intake_status to completed
    nlohmann::json payload;
    payload["data"] = {
        {"type", "Student"},
        {"id", studentId},
        {"attributes", {
            {"intake_status", "completed"}
        }}
    };

    ApiResponse response = apiClient_->patch("/Student/" + studentId, payload);
    return parseSubmissionResponse(response);
}

// File upload
SubmissionResult FormSubmissionService::uploadDocument(const std::string& studentId,
                                                        const std::string& documentType,
                                                        const std::string& filePath) {
    std::map<std::string, std::string> fields;
    fields["student_id"] = studentId;
    fields["document_type"] = documentType;

    ApiResponse response = apiClient_->uploadFile("/Document/upload", "file", filePath, fields);
    return parseSubmissionResponse(response);
}

} // namespace Api
} // namespace StudentIntake
