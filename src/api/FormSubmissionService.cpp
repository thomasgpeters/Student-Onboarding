#include "FormSubmissionService.h"
#include <thread>
#include <iostream>
#include <chrono>
#include <ctime>

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

            // Debug: Log curriculum_id from API response
            std::cout << "[FormSubmissionService] Login - curriculum_id in API response: ";
            if (attributes.contains("curriculum_id")) {
                std::cout << attributes["curriculum_id"].dump() << std::endl;
            } else {
                std::cout << "NOT PRESENT" << std::endl;
            }
            std::cout.flush();

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
    std::cout << "[FormSubmissionService] updateStudentProfile - student ID: " << student.getId() << std::endl;
    std::cout << "[FormSubmissionService] updateStudentProfile - curriculum_id in student: '"
              << student.getCurriculumId() << "'" << std::endl;
    std::cout << "[FormSubmissionService] updateStudentProfile - curriculum_id in payload: "
              << (attributes.contains("curriculum_id") ? attributes["curriculum_id"].dump() : "NOT PRESENT") << std::endl;

    nlohmann::json payload;
    payload["data"] = {
        {"type", "Student"},
        {"id", student.getId()},
        {"attributes", attributes}
    };
    ApiResponse response = apiClient_->patch("/Student/" + student.getId(), payload);
    std::cout << "[FormSubmissionService] updateStudentProfile - response status: " << response.statusCode << std::endl;
    if (!response.isSuccess()) {
        std::cout << "[FormSubmissionService] updateStudentProfile - error: " << response.errorMessage << std::endl;
    }
    std::cout.flush();
    return parseSubmissionResponse(response);
}

// StudentAddress API endpoints
std::vector<Models::StudentAddress> FormSubmissionService::getStudentAddresses(const std::string& studentId) {
    std::vector<Models::StudentAddress> addresses;

    std::string endpoint = "/StudentAddress?filter[student_id]=" + studentId;
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        for (const auto& item : items) {
            addresses.push_back(Models::StudentAddress::fromJson(item));
        }
    }

    return addresses;
}

Models::StudentAddress FormSubmissionService::getStudentAddress(const std::string& studentId,
                                                                  const std::string& addressType) {
    std::string endpoint = "/StudentAddress?filter[student_id]=" + studentId +
                           "&filter[address_type]=" + addressType;
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        if (!items.empty()) {
            return Models::StudentAddress::fromJson(items[0]);
        }
    }

    // Return empty address with the requested type
    return Models::StudentAddress(studentId, addressType);
}

SubmissionResult FormSubmissionService::createStudentAddress(const Models::StudentAddress& address) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "StudentAddress"},
        {"attributes", address.toJson()}
    };

    std::cout << "[FormSubmissionService] createStudentAddress payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/StudentAddress", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::updateStudentAddress(const Models::StudentAddress& address) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "StudentAddress"},
        {"id", address.getId()},
        {"attributes", address.toJson()}
    };

    std::cout << "[FormSubmissionService] updateStudentAddress payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->patch("/StudentAddress/" + address.getId(), payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::deleteStudentAddress(const std::string& addressId) {
    ApiResponse response = apiClient_->del("/StudentAddress/" + addressId);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::saveStudentAddress(const Models::StudentAddress& address) {
    // If address has an ID, update it; otherwise check if one exists for this student/type
    if (!address.getId().empty()) {
        return updateStudentAddress(address);
    }

    // Check if address already exists for this student and type
    Models::StudentAddress existing = getStudentAddress(address.getStudentId(), address.getAddressType());
    if (!existing.getId().empty()) {
        // Update existing address
        Models::StudentAddress updated = address;
        updated.setId(existing.getId());
        return updateStudentAddress(updated);
    }

    // Create new address
    return createStudentAddress(address);
}

// EmergencyContact API endpoints
std::vector<Models::EmergencyContact> FormSubmissionService::getEmergencyContacts(const std::string& studentId) {
    std::vector<Models::EmergencyContact> contacts;

    std::string endpoint = "/EmergencyContact?filter[student_id]=" + studentId;
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        for (const auto& item : items) {
            contacts.push_back(Models::EmergencyContact::fromJson(item));
        }
    }

    return contacts;
}

SubmissionResult FormSubmissionService::createEmergencyContact(const Models::EmergencyContact& contact) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "EmergencyContact"},
        {"attributes", contact.toJson()}
    };

    std::cout << "[FormSubmissionService] createEmergencyContact payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/EmergencyContact", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::updateEmergencyContact(const Models::EmergencyContact& contact) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "EmergencyContact"},
        {"id", contact.getId()},
        {"attributes", contact.toJson()}
    };

    std::cout << "[FormSubmissionService] updateEmergencyContact payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->patch("/EmergencyContact/" + contact.getId(), payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::deleteEmergencyContact(const std::string& contactId) {
    ApiResponse response = apiClient_->del("/EmergencyContact/" + contactId);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::saveEmergencyContact(const Models::EmergencyContact& contact) {
    // If contact has an ID, update it; otherwise create new
    if (!contact.getId().empty()) {
        return updateEmergencyContact(contact);
    }
    return createEmergencyContact(contact);
}

// AcademicHistory API endpoints
std::vector<Models::AcademicHistory> FormSubmissionService::getAcademicHistories(const std::string& studentId) {
    std::vector<Models::AcademicHistory> histories;

    std::string endpoint = "/AcademicHistory?filter[student_id]=" + studentId;
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        for (const auto& item : items) {
            histories.push_back(Models::AcademicHistory::fromJson(item));
        }
    }

    return histories;
}

Models::AcademicHistory FormSubmissionService::getAcademicHistoryByType(const std::string& studentId,
                                                                          const std::string& institutionType) {
    std::string endpoint = "/AcademicHistory?filter[student_id]=" + studentId +
                           "&filter[institution_type]=" + institutionType;
    ApiResponse response = apiClient_->get(endpoint);

    if (response.isSuccess()) {
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        if (!items.empty()) {
            return Models::AcademicHistory::fromJson(items[0]);
        }
    }

    // Return empty history with the requested type
    Models::AcademicHistory history(studentId);
    history.setInstitutionType(institutionType);
    return history;
}

SubmissionResult FormSubmissionService::createAcademicHistory(const Models::AcademicHistory& history) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "AcademicHistory"},
        {"attributes", history.toJson()}
    };

    std::cout << "[FormSubmissionService] createAcademicHistory payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/AcademicHistory", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::updateAcademicHistory(const Models::AcademicHistory& history) {
    nlohmann::json payload;
    payload["data"] = {
        {"type", "AcademicHistory"},
        {"id", history.getId()},
        {"attributes", history.toJson()}
    };

    std::cout << "[FormSubmissionService] updateAcademicHistory payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->patch("/AcademicHistory/" + history.getId(), payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::deleteAcademicHistory(const std::string& historyId) {
    ApiResponse response = apiClient_->del("/AcademicHistory/" + historyId);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::saveAcademicHistory(const Models::AcademicHistory& history) {
    // If history has an ID, update it
    if (!history.getId().empty()) {
        return updateAcademicHistory(history);
    }

    // Check if a record already exists for this student and institution type
    Models::AcademicHistory existing = getAcademicHistoryByType(history.getStudentId(), history.getInstitutionType());
    if (!existing.getId().empty()) {
        // Update existing record
        Models::AcademicHistory updated = history;
        updated.setId(existing.getId());
        return updateAcademicHistory(updated);
    }

    // Create new record
    return createAcademicHistory(history);
}

// Curriculum API endpoints
std::vector<Models::Curriculum> FormSubmissionService::getCurriculums() {
    std::vector<Models::Curriculum> curriculums;

    std::cout << "[FormSubmissionService] Fetching curriculums from /Curriculum" << std::endl;
    ApiResponse response = apiClient_->get("/Curriculum");
    if (response.isSuccess()) {
        auto json = response.getJson();
        std::cout << "[FormSubmissionService] Curriculum response received" << std::endl;

        if (json.is_array()) {
            std::cout << "[FormSubmissionService] Response is array with " << json.size() << " items" << std::endl;
            for (const auto& item : json) {
                auto curriculum = Models::Curriculum::fromJson(item);
                std::cout << "[FormSubmissionService] Parsed curriculum: id=" << curriculum.getId()
                          << ", name=" << curriculum.getName()
                          << ", credits=" << curriculum.getCreditHours() << std::endl;
                curriculums.push_back(curriculum);
            }
        } else if (json.contains("data") && json["data"].is_array()) {
            std::cout << "[FormSubmissionService] Response has 'data' array with " << json["data"].size() << " items" << std::endl;
            for (const auto& item : json["data"]) {
                auto curriculum = Models::Curriculum::fromJson(item);
                std::cout << "[FormSubmissionService] Parsed curriculum: id=" << curriculum.getId()
                          << ", name=" << curriculum.getName()
                          << ", credits=" << curriculum.getCreditHours() << std::endl;
                curriculums.push_back(curriculum);
            }
        } else {
            std::cout << "[FormSubmissionService] Unexpected response format" << std::endl;
        }
    } else {
        std::cout << "[FormSubmissionService] Failed to fetch curriculums: " << response.errorMessage << std::endl;
    }

    std::cout << "[FormSubmissionService] Returning " << curriculums.size() << " curriculums" << std::endl;
    std::cout.flush();
    return curriculums;
}

Models::Curriculum FormSubmissionService::getCurriculum(const std::string& curriculumId) {
    ApiResponse response = apiClient_->get("/Curriculum/" + curriculumId);
    if (response.isSuccess()) {
        return Models::Curriculum::fromJson(response.getJson());
    }
    return Models::Curriculum();
}

// Institution Settings API endpoints
Models::InstitutionSettings FormSubmissionService::getInstitutionSettings() {
    std::cout << "[FormSubmissionService] Loading institution settings" << std::endl;

    ApiResponse response = apiClient_->get("/InstitutionSettings");
    if (response.isSuccess()) {
        std::cout << "[FormSubmissionService] Institution settings loaded successfully" << std::endl;
        return Models::InstitutionSettings::fromApiResponse(response.getJson());
    }

    std::cerr << "[FormSubmissionService] Failed to load institution settings: " << response.errorMessage << std::endl;
    return Models::InstitutionSettings();
}

SubmissionResult FormSubmissionService::updateInstitutionSetting(const std::string& key, const std::string& value) {
    std::cout << "[FormSubmissionService] Updating setting: " << key << std::endl;

    // First, find the setting by key to get its ID
    std::string endpoint = "/InstitutionSettings?filter[setting_key]=" + key;
    ApiResponse getResponse = apiClient_->get(endpoint);

    if (!getResponse.isSuccess()) {
        SubmissionResult result;
        result.success = false;
        result.message = "Failed to find setting: " + key;
        return result;
    }

    // Parse to find the setting ID
    auto json = getResponse.getJson();
    nlohmann::json items;
    if (json.is_array()) {
        items = json;
    } else if (json.contains("data") && json["data"].is_array()) {
        items = json["data"];
    }

    if (items.empty()) {
        SubmissionResult result;
        result.success = false;
        result.message = "Setting not found: " + key;
        return result;
    }

    // Get the ID of the first matching setting
    std::string settingId;
    if (items[0].contains("id")) {
        if (items[0]["id"].is_number()) {
            settingId = std::to_string(items[0]["id"].get<int>());
        } else if (items[0]["id"].is_string()) {
            settingId = items[0]["id"].get<std::string>();
        }
    }

    if (settingId.empty()) {
        SubmissionResult result;
        result.success = false;
        result.message = "Could not determine setting ID for: " + key;
        return result;
    }

    // Update the setting using PATCH
    nlohmann::json payload;
    payload["data"] = {
        {"type", "InstitutionSettings"},
        {"id", settingId},
        {"attributes", {
            {"setting_value", value}
        }}
    };

    std::cout << "[FormSubmissionService] Updating setting " << settingId << " with value: " << value << std::endl;
    ApiResponse patchResponse = apiClient_->patch("/InstitutionSettings/" + settingId, payload);
    return parseSubmissionResponse(patchResponse);
}

SubmissionResult FormSubmissionService::updateInstitutionSettings(const Models::InstitutionSettings& settings) {
    std::cout << "[FormSubmissionService] Updating all institution settings" << std::endl;

    SubmissionResult finalResult;
    finalResult.success = true;
    int successCount = 0;
    int failCount = 0;

    for (const auto& [key, value] : settings.getAllSettings()) {
        auto result = updateInstitutionSetting(key, value);
        if (result.success) {
            successCount++;
        } else {
            failCount++;
            std::cerr << "[FormSubmissionService] Failed to update setting " << key << ": " << result.message << std::endl;
        }
    }

    finalResult.message = "Updated " + std::to_string(successCount) + " settings";
    if (failCount > 0) {
        finalResult.message += " (" + std::to_string(failCount) + " failed)";
        finalResult.success = (failCount == 0);
    }

    std::cout << "[FormSubmissionService] " << finalResult.message << std::endl;
    return finalResult;
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
    // Transform form data to match financial_aid table schema (snake_case)
    nlohmann::json attributes;

    // student_id as integer
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        attributes["student_id"] = studentId;
    }

    // Map form fields to database columns
    attributes["fafsa_completed"] = data.hasField("fafsaCompleted") ? data.getField("fafsaCompleted").boolValue : false;

    // fafsaId maps to efc (Expected Family Contribution)
    std::string fafsaId = data.hasField("fafsaId") ? data.getField("fafsaId").stringValue : "";
    if (!fafsaId.empty()) {
        try {
            attributes["efc"] = std::stod(fafsaId);
        } catch (...) {
            attributes["efc"] = nullptr;
        }
    }

    attributes["employment_status"] = data.hasField("employmentStatus") ? data.getField("employmentStatus").stringValue : "";
    attributes["employer_name"] = data.hasField("employer") ? data.getField("employer").stringValue : "";

    // householdIncome maps to annual_income - extract numeric value if possible
    std::string incomeStr = data.hasField("householdIncome") ? data.getField("householdIncome").stringValue : "";
    // Store the text description in aid_types for now since annual_income expects a number
    if (!incomeStr.empty() && incomeStr != "Select...") {
        // Just store it as text, backend can handle it
        attributes["aid_types"] = incomeStr;  // Store income range description
    }

    // dependents maps to dependents_count
    std::string dependentsStr = data.hasField("dependents") ? data.getField("dependents").stringValue : "";
    if (!dependentsStr.empty()) {
        try {
            attributes["dependents_count"] = std::stoi(dependentsStr);
        } catch (...) {
            attributes["dependents_count"] = 0;
        }
    }

    // Build aid_types from checkboxes
    std::vector<std::string> aidTypes;
    if (data.hasField("scholarshipInterest") && data.getField("scholarshipInterest").boolValue) {
        aidTypes.push_back("Scholarships/Grants");
    }
    if (data.hasField("workStudyInterest") && data.getField("workStudyInterest").boolValue) {
        aidTypes.push_back("Work-Study");
    }
    if (data.hasField("loanInterest") && data.getField("loanInterest").boolValue) {
        aidTypes.push_back("Student Loans");
    }
    if (data.hasField("veteranBenefits") && data.getField("veteranBenefits").boolValue) {
        aidTypes.push_back("Veteran Benefits");
    }
    if (!aidTypes.empty()) {
        std::string aidTypesStr;
        for (size_t i = 0; i < aidTypes.size(); ++i) {
            if (i > 0) aidTypesStr += ", ";
            aidTypesStr += aidTypes[i];
        }
        attributes["aid_types"] = aidTypesStr;
    }

    attributes["scholarship_applications"] = data.hasField("currentScholarships") ? data.getField("currentScholarships").stringValue : "";

    // Store special circumstances in scholarship_applications if no scholarships listed
    std::string specialCirc = data.hasField("specialCircumstances") ? data.getField("specialCircumstances").stringValue : "";
    if (!specialCirc.empty()) {
        std::string existing = attributes.value("scholarship_applications", "");
        if (!existing.empty()) {
            attributes["scholarship_applications"] = existing + "\n\nSpecial Circumstances: " + specialCirc;
        } else {
            attributes["scholarship_applications"] = "Special Circumstances: " + specialCirc;
        }
    }

    nlohmann::json payload;
    payload["data"] = {
        {"type", "FinancialAid"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] submitFinancialAid payload: " << payload.dump() << std::endl;

    // Check if a financial_aid record already exists for this student
    std::string checkEndpoint = "/FinancialAid?filter[student_id]=" + studentId;
    ApiResponse checkResponse = apiClient_->get(checkEndpoint);

    ApiResponse response;
    if (checkResponse.isSuccess()) {
        auto json = checkResponse.getJson();
        nlohmann::json items;
        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        if (!items.empty()) {
            // Record exists - update it with PATCH
            std::string existingId;
            if (items[0].contains("id")) {
                if (items[0]["id"].is_string()) {
                    existingId = items[0]["id"].get<std::string>();
                } else if (items[0]["id"].is_number()) {
                    existingId = std::to_string(items[0]["id"].get<int>());
                }
            }
            if (!existingId.empty()) {
                payload["data"]["id"] = existingId;
                std::cout << "[FormSubmissionService] Updating existing FinancialAid record: " << existingId << std::endl;
                response = apiClient_->patch("/FinancialAid/" + existingId, payload);
            } else {
                response = apiClient_->post("/FinancialAid", payload);
            }
        } else {
            // No existing record - create new
            response = apiClient_->post("/FinancialAid", payload);
        }
    } else {
        // Couldn't check - try POST anyway
        response = apiClient_->post("/FinancialAid", payload);
    }

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
    // Transform form data to match Consent table schema (snake_case)
    nlohmann::json attributes;

    // student_id as integer
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        attributes["student_id"] = studentId;
    }

    // Check if all required consents are accepted
    bool termsAccepted = data.hasField("termsAccepted") && data.getField("termsAccepted").boolValue;
    bool privacyAccepted = data.hasField("privacyAccepted") && data.getField("privacyAccepted").boolValue;
    bool ferpaAcknowledged = data.hasField("ferpaAcknowledged") && data.getField("ferpaAcknowledged").boolValue;
    bool accuracyCertified = data.hasField("accuracyCertified") && data.getField("accuracyCertified").boolValue;

    attributes["consent_type"] = "student_intake";
    attributes["consent_version"] = "1.0";
    attributes["is_accepted"] = termsAccepted && privacyAccepted && ferpaAcknowledged && accuracyCertified;
    attributes["electronic_signature"] = data.hasField("electronicSignature") ? data.getField("electronicSignature").stringValue : "";
    attributes["signature_date"] = data.hasField("signatureDate") ? data.getField("signatureDate").stringValue : "";

    nlohmann::json payload;
    payload["data"] = {
        {"type", "Consent"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] submitConsent payload: " << payload.dump() << std::endl;
    ApiResponse response = apiClient_->post("/Consent", payload);
    return parseSubmissionResponse(response);
}

SubmissionResult FormSubmissionService::deleteStudentConsents(const std::string& studentId) {
    std::cout << "[FormSubmissionService] deleteStudentConsents for student: " << studentId << std::endl;

    // First, get all consent records for this student
    std::string endpoint = "/Consent?filter[student_id]=" + studentId;
    ApiResponse getResponse = apiClient_->get(endpoint);

    if (!getResponse.isSuccess()) {
        SubmissionResult result;
        result.success = false;
        result.message = "Failed to get consent records: " + getResponse.errorMessage;
        return result;
    }

    // Parse consent records
    auto json = getResponse.getJson();
    nlohmann::json items;
    if (json.is_array()) {
        items = json;
    } else if (json.contains("data") && json["data"].is_array()) {
        items = json["data"];
    }

    int deletedCount = 0;
    std::string lastError;

    // Delete each consent record
    for (const auto& item : items) {
        std::string consentId;
        if (item.contains("id")) {
            if (item["id"].is_number()) {
                consentId = std::to_string(item["id"].get<int>());
            } else if (item["id"].is_string()) {
                consentId = item["id"].get<std::string>();
            }
        }

        if (!consentId.empty()) {
            ApiResponse delResponse = apiClient_->del("/Consent/" + consentId);
            if (delResponse.isSuccess()) {
                deletedCount++;
                std::cout << "[FormSubmissionService] Deleted consent record: " << consentId << std::endl;
            } else {
                lastError = delResponse.errorMessage;
                std::cerr << "[FormSubmissionService] Failed to delete consent " << consentId << ": " << lastError << std::endl;
            }
        }
    }

    SubmissionResult result;
    result.success = true;
    result.message = "Deleted " + std::to_string(deletedCount) + " consent record(s)";
    if (!lastError.empty()) {
        result.message += " (some errors occurred)";
    }
    std::cout << "[FormSubmissionService] " << result.message << std::endl;
    return result;
}

// Generic form submission
SubmissionResult FormSubmissionService::submitForm(const std::string& studentId,
                                                    const std::string& formId,
                                                    const Models::FormData& data) {
    SubmissionResult domainResult;

    // Route to specialized submission functions for forms that need custom handling
    if (formId == "emergency_contact") {
        domainResult = submitEmergencyContact(studentId, data);
    } else if (formId == "academic_history") {
        domainResult = submitAcademicHistory(studentId, data);
    } else if (formId == "financial_aid") {
        domainResult = submitFinancialAid(studentId, data);
    } else if (formId == "consent") {
        domainResult = submitConsent(studentId, data);
    } else {
        // Generic submission for other form types
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

        domainResult = parseSubmissionResponse(response);
    }

    // If domain-specific submission succeeded, also create a FormSubmission record
    // This ensures the admin can see all submitted forms in the FormSubmissions list
    if (domainResult.success) {
        std::cout << "[FormSubmissionService] Domain submission successful, creating FormSubmission record..." << std::endl;
        SubmissionResult formSubmissionResult = createFormSubmissionRecord(studentId, formId, "pending");
        if (!formSubmissionResult.success) {
            std::cerr << "[FormSubmissionService] Warning: Domain data saved but FormSubmission record creation failed: "
                      << formSubmissionResult.message << std::endl;
            // Don't fail the overall submission - domain data was saved successfully
            // Just log the warning
        }
    }

    return domainResult;
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

// Load form type cache from API
void FormSubmissionService::loadFormTypeCache() {
    if (!formTypeCache_.empty()) {
        return;  // Already loaded
    }

    std::cout << "[FormSubmissionService] Loading form type cache..." << std::endl;
    ApiResponse response = apiClient_->get("/FormType");
    if (response.isSuccess()) {
        std::cout << "[FormSubmissionService] FormType API response: " << response.body << std::endl;
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
            std::cout << "[FormSubmissionService] Response is array with " << items.size() << " items" << std::endl;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
            std::cout << "[FormSubmissionService] Response has data array with " << items.size() << " items" << std::endl;
        } else {
            std::cerr << "[FormSubmissionService] Unexpected FormType response format" << std::endl;
        }

        for (const auto& item : items) {
            std::string code;
            int id = 0;

            // Get code
            if (item.contains("attributes") && item["attributes"].contains("code")) {
                code = item["attributes"]["code"].get<std::string>();
            } else if (item.contains("code")) {
                code = item["code"].get<std::string>();
            }

            // Get id
            if (item.contains("id")) {
                if (item["id"].is_number()) {
                    id = item["id"].get<int>();
                } else if (item["id"].is_string()) {
                    try {
                        id = std::stoi(item["id"].get<std::string>());
                    } catch (...) {}
                }
            }

            if (!code.empty() && id > 0) {
                formTypeCache_[code] = id;
                std::cout << "[FormSubmissionService] Cached form type: " << code << " -> " << id << std::endl;
            } else {
                std::cerr << "[FormSubmissionService] Skipping item - code: '" << code << "', id: " << id << std::endl;
            }
        }
    } else {
        std::cerr << "[FormSubmissionService] Failed to load FormType data: " << response.errorMessage
                  << " (status: " << response.statusCode << ")" << std::endl;
    }
    std::cout << "[FormSubmissionService] Form type cache loaded with " << formTypeCache_.size() << " entries" << std::endl;
    std::cout.flush();

    // If cache is still empty, seed the FormType table
    if (formTypeCache_.empty()) {
        std::cout << "[FormSubmissionService] FormType table appears empty, seeding default values..." << std::endl;
        ensureFormTypesExist();
    }
}

// Create a single FormType record
int FormSubmissionService::createFormType(const std::string& code, const std::string& name,
                                           const std::string& category, int displayOrder) {
    nlohmann::json attributes;
    attributes["code"] = code;
    attributes["name"] = name;
    attributes["category"] = category;
    attributes["display_order"] = displayOrder;
    attributes["is_required"] = true;
    attributes["is_active"] = true;

    nlohmann::json payload;
    payload["data"] = {
        {"type", "FormType"},
        {"attributes", attributes}
    };

    std::cout << "[FormSubmissionService] Creating FormType: " << code << std::endl;
    ApiResponse response = apiClient_->post("/FormType", payload);

    if (response.isSuccess()) {
        auto json = response.getJson();
        int id = 0;

        // Extract ID from response
        if (json.contains("data") && json["data"].contains("id")) {
            if (json["data"]["id"].is_number()) {
                id = json["data"]["id"].get<int>();
            } else if (json["data"]["id"].is_string()) {
                try {
                    id = std::stoi(json["data"]["id"].get<std::string>());
                } catch (...) {}
            }
        } else if (json.contains("id")) {
            if (json["id"].is_number()) {
                id = json["id"].get<int>();
            } else if (json["id"].is_string()) {
                try {
                    id = std::stoi(json["id"].get<std::string>());
                } catch (...) {}
            }
        }

        if (id > 0) {
            formTypeCache_[code] = id;
            std::cout << "[FormSubmissionService] Created FormType: " << code << " -> " << id << std::endl;
            return id;
        }
    } else {
        std::cerr << "[FormSubmissionService] Failed to create FormType " << code
                  << ": " << response.errorMessage << std::endl;
    }

    return 0;
}

// Seed all required FormType records if they don't exist
void FormSubmissionService::ensureFormTypesExist() {
    // Define the required form types (matching schema.sql seed data)
    struct FormTypeDef {
        std::string code;
        std::string name;
        std::string category;
        int displayOrder;
    };

    std::vector<FormTypeDef> formTypes = {
        {"personal_info", "Personal Information", "core", 1},
        {"emergency_contact", "Emergency Contacts", "core", 2},
        {"medical_info", "Medical Information", "health", 3},
        {"academic_history", "Academic History", "academic", 4},
        {"financial_aid", "Financial Aid", "financial", 5},
        {"documents", "Document Upload", "documents", 6},
        {"consent", "Terms and Consent", "legal", 7}
    };

    for (const auto& ft : formTypes) {
        // Check if already exists in cache
        if (formTypeCache_.find(ft.code) == formTypeCache_.end()) {
            createFormType(ft.code, ft.name, ft.category, ft.displayOrder);
        }
    }

    std::cout << "[FormSubmissionService] FormType seeding complete. Cache now has "
              << formTypeCache_.size() << " entries" << std::endl;
}

// Get form type ID from code
int FormSubmissionService::getFormTypeId(const std::string& formCode) {
    std::cout << "[FormSubmissionService] getFormTypeId called for: " << formCode << std::endl;
    loadFormTypeCache();

    auto it = formTypeCache_.find(formCode);
    if (it != formTypeCache_.end()) {
        std::cout << "[FormSubmissionService] Found form type in cache: " << formCode << " -> " << it->second << std::endl;
        return it->second;
    }

    std::cout << "[FormSubmissionService] Form type not in cache, querying API..." << std::endl;

    // Fallback: query API directly
    std::string endpoint = "/FormType?filter[code]=" + formCode;
    ApiResponse response = apiClient_->get(endpoint);
    if (response.isSuccess()) {
        std::cout << "[FormSubmissionService] FormType query response: " << response.body << std::endl;
        auto json = response.getJson();
        nlohmann::json items;

        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        if (!items.empty()) {
            auto item = items[0];
            int id = 0;
            if (item.contains("id")) {
                if (item["id"].is_number()) {
                    id = item["id"].get<int>();
                } else if (item["id"].is_string()) {
                    try {
                        id = std::stoi(item["id"].get<std::string>());
                    } catch (...) {}
                }
            }
            if (id > 0) {
                formTypeCache_[formCode] = id;
                std::cout << "[FormSubmissionService] Found form type via query: " << formCode << " -> " << id << std::endl;
                return id;
            }
        } else {
            std::cerr << "[FormSubmissionService] FormType query returned empty results for: " << formCode << std::endl;
        }
    } else {
        std::cerr << "[FormSubmissionService] FormType query failed: " << response.errorMessage << std::endl;
    }

    std::cerr << "[FormSubmissionService] Could not find form type ID for code: " << formCode << std::endl;
    return 0;
}

// Create a form submission record in the database
SubmissionResult FormSubmissionService::createFormSubmissionRecord(const std::string& studentId,
                                                                     const std::string& formCode,
                                                                     const std::string& status) {
    std::cout << "[FormSubmissionService] Creating form submission record for student " << studentId
              << ", form: " << formCode << ", status: " << status << std::endl;

    int formTypeId = getFormTypeId(formCode);
    if (formTypeId == 0) {
        std::cerr << "[FormSubmissionService] Failed to get form type ID for: " << formCode << std::endl;
        return SubmissionResult{false, "", "Could not find form type: " + formCode, {"Unknown form type"}, {}};
    }

    // Check if a submission already exists for this student and form type
    std::string checkEndpoint = "/FormSubmission?filter[student_id]=" + studentId +
                                 "&filter[form_type_id]=" + std::to_string(formTypeId);
    ApiResponse checkResponse = apiClient_->get(checkEndpoint);

    nlohmann::json attributes;
    try {
        attributes["student_id"] = std::stoi(studentId);
    } catch (const std::exception&) {
        attributes["student_id"] = studentId;
    }
    attributes["form_type_id"] = formTypeId;
    attributes["status"] = status;

    // Get current timestamp in ISO format
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::gmtime(&time_t_now);
    char timestamp[32];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%SZ", &tm_now);
    attributes["submitted_at"] = std::string(timestamp);

    nlohmann::json payload;
    payload["data"] = {
        {"type", "FormSubmission"},
        {"attributes", attributes}
    };

    ApiResponse response;
    bool isUpdate = false;

    // Check if record already exists
    if (checkResponse.isSuccess()) {
        auto json = checkResponse.getJson();
        nlohmann::json items;
        if (json.is_array()) {
            items = json;
        } else if (json.contains("data") && json["data"].is_array()) {
            items = json["data"];
        }

        if (!items.empty()) {
            // Record exists - update it
            std::string existingId;
            if (items[0].contains("id")) {
                if (items[0]["id"].is_string()) {
                    existingId = items[0]["id"].get<std::string>();
                } else if (items[0]["id"].is_number()) {
                    existingId = std::to_string(items[0]["id"].get<int>());
                }
            }
            if (!existingId.empty()) {
                payload["data"]["id"] = existingId;
                std::cout << "[FormSubmissionService] Updating existing FormSubmission record: " << existingId << std::endl;
                response = apiClient_->patch("/FormSubmission/" + existingId, payload);
                isUpdate = true;
            }
        }
    }

    // Create new record if not updating
    if (!isUpdate) {
        std::cout << "[FormSubmissionService] Creating new FormSubmission record" << std::endl;
        std::cout << "[FormSubmissionService] FormSubmission payload: " << payload.dump() << std::endl;
        response = apiClient_->post("/FormSubmission", payload);
    }

    std::cout << "[FormSubmissionService] FormSubmission API response - status: " << response.statusCode
              << ", success: " << response.success << std::endl;
    if (!response.body.empty()) {
        std::cout << "[FormSubmissionService] FormSubmission response body: " << response.body << std::endl;
    }
    if (!response.errorMessage.empty()) {
        std::cerr << "[FormSubmissionService] FormSubmission error: " << response.errorMessage << std::endl;
    }

    SubmissionResult result = parseSubmissionResponse(response);
    if (result.success) {
        std::cout << "[FormSubmissionService] FormSubmission record " << (isUpdate ? "updated" : "created")
                  << " successfully, ID: " << result.submissionId << std::endl;
    } else {
        std::cerr << "[FormSubmissionService] Failed to " << (isUpdate ? "update" : "create")
                  << " FormSubmission record: " << result.message << std::endl;
        for (const auto& err : result.errors) {
            std::cerr << "[FormSubmissionService]   Error: " << err << std::endl;
        }
    }
    std::cout.flush();

    return result;
}

} // namespace Api
} // namespace StudentIntake
