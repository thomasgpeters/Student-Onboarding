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
    // Map form IDs to ApiLogicServer endpoints
    static const std::map<std::string, std::string> endpointMap = {
        {"personal_info", "/PersonalInfo"},
        {"emergency_contact", "/EmergencyContact"},
        {"medical_info", "/MedicalInfo"},
        {"academic_history", "/AcademicHistory"},
        {"financial_aid", "/FinancialAid"},
        {"documents", "/Documents"},
        {"consent", "/Consent"}
    };

    auto it = endpointMap.find(formId);
    if (it != endpointMap.end()) {
        return it->second;
    }
    return "/FormSubmission";  // Generic endpoint
}

nlohmann::json FormSubmissionService::prepareFormPayload(const std::string& studentId,
                                                          const Models::FormData& data) {
    nlohmann::json payload = data.toJson();
    payload["studentId"] = studentId;
    payload["submittedAt"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
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

    nlohmann::json payload = student.toJson();
    payload["password"] = password;

    std::cout << "[FormSubmissionService] Payload: " << payload.dump() << std::endl;
    std::cout.flush();

    ApiResponse response = apiClient_->post("/Student", payload);

    std::cout << "[FormSubmissionService] API response received - status: " << response.statusCode
              << ", success: " << response.success << std::endl;
    std::cout.flush();

    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::loginStudent(const std::string& email,
                                                      const std::string& password) {
    nlohmann::json payload;
    payload["email"] = email;
    payload["password"] = password;

    ApiResponse response = apiClient_->post("/auth/login", payload);
    SubmissionResult result = parseSubmissionResponse(response);

    if (result.success && result.responseData.contains("token")) {
        apiClient_->setAuthToken(result.responseData["token"].get<std::string>());
    }

    return result;
}

SubmissionResult FormSubmissionService::getStudentProfile(const std::string& studentId) {
    ApiResponse response = apiClient_->get("/Student/" + studentId);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::updateStudentProfile(const Models::Student& student) {
    nlohmann::json payload = student.toJson();
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
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/PersonalInfo", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitEmergencyContact(const std::string& studentId,
                                                                const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/EmergencyContact", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitMedicalInfo(const std::string& studentId,
                                                           const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/MedicalInfo", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitAcademicHistory(const std::string& studentId,
                                                               const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/AcademicHistory", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitFinancialAid(const std::string& studentId,
                                                            const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/FinancialAid", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitDocuments(const std::string& studentId,
                                                         const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/Documents", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::submitConsent(const std::string& studentId,
                                                       const Models::FormData& data) {
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post("/Consent", payload);
    return parseSubmissionResponse(response);
}

// Generic form submission
SubmissionResult FormSubmissionService::submitForm(const std::string& studentId,
                                                    const std::string& formId,
                                                    const Models::FormData& data) {
    std::string endpoint = getEndpointForForm(formId);
    nlohmann::json payload = prepareFormPayload(studentId, data);
    ApiResponse response = apiClient_->post(endpoint, payload);
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
    ApiResponse response = apiClient_->get(endpoint + "?studentId=" + studentId);

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

    ApiResponse response = apiClient_->get("/FormSubmission?studentId=" + studentId +
                                           "&status=submitted");
    if (response.isSuccess()) {
        auto json = response.getJson();
        auto items = json.is_array() ? json : json["data"];
        for (const auto& item : items) {
            if (item.contains("formId")) {
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
    nlohmann::json payload;
    payload["studentId"] = studentId;
    payload["status"] = "completed";
    payload["completedAt"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    ApiResponse response = apiClient_->post("/IntakeCompletion", payload);
    return parseSubmissionResponse(response);
}

// File upload
SubmissionResult FormSubmissionService::uploadDocument(const std::string& studentId,
                                                        const std::string& documentType,
                                                        const std::string& filePath) {
    std::map<std::string, std::string> fields;
    fields["studentId"] = studentId;
    fields["documentType"] = documentType;

    ApiResponse response = apiClient_->uploadFile("/Documents/upload", "file", filePath, fields);
    return parseSubmissionResponse(response);
}

} // namespace Api
} // namespace StudentIntake
