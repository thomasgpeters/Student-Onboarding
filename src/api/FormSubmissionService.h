#ifndef FORM_SUBMISSION_SERVICE_H
#define FORM_SUBMISSION_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include "ApiClient.h"
#include "models/FormData.h"
#include "models/Student.h"
#include "models/StudentAddress.h"
#include "models/EmergencyContact.h"
#include "models/AcademicHistory.h"
#include "models/Curriculum.h"
#include "models/InstitutionSettings.h"

namespace StudentIntake {
namespace Api {

/**
 * @brief Result of a form submission
 */
struct SubmissionResult {
    bool success;
    std::string submissionId;
    std::string message;
    std::vector<std::string> errors;
    nlohmann::json responseData;
};

/**
 * @brief Callback for async submissions
 */
using SubmissionCallback = std::function<void(const SubmissionResult&)>;

/**
 * @brief Student consent data including signature
 */
struct StudentConsentData {
    std::map<std::string, bool> consents;  // consent_type -> is_accepted
    std::string signature;
    std::string signatureDate;
};

/**
 * @brief Service for submitting forms to the ApiLogicServer backend
 */
class FormSubmissionService {
public:
    FormSubmissionService();
    explicit FormSubmissionService(std::shared_ptr<ApiClient> apiClient);
    ~FormSubmissionService();

    // API client management
    void setApiClient(std::shared_ptr<ApiClient> client) { apiClient_ = client; }
    std::shared_ptr<ApiClient> getApiClient() const { return apiClient_; }

    // Student API endpoints
    SubmissionResult registerStudent(const Models::Student& student, const std::string& password);
    SubmissionResult loginStudent(const std::string& email, const std::string& password);
    SubmissionResult getStudentProfile(const std::string& studentId);
    SubmissionResult updateStudentProfile(const Models::Student& student);

    // StudentAddress API endpoints
    std::vector<Models::StudentAddress> getStudentAddresses(const std::string& studentId);
    Models::StudentAddress getStudentAddress(const std::string& studentId, const std::string& addressType);
    SubmissionResult createStudentAddress(const Models::StudentAddress& address);
    SubmissionResult updateStudentAddress(const Models::StudentAddress& address);
    SubmissionResult deleteStudentAddress(const std::string& addressId);
    SubmissionResult saveStudentAddress(const Models::StudentAddress& address);  // Create or update

    // EmergencyContact API endpoints (uses compound key: student_id, contact_relationship, phone)
    std::vector<Models::EmergencyContact> getEmergencyContacts(const std::string& studentId);
    Models::EmergencyContact getEmergencyContactByKey(const std::string& studentId,
                                                       const std::string& relationship,
                                                       const std::string& phone);
    SubmissionResult createEmergencyContact(const Models::EmergencyContact& contact);
    SubmissionResult updateEmergencyContact(const Models::EmergencyContact& contact);
    SubmissionResult deleteEmergencyContact(const std::string& studentId,
                                             const std::string& relationship,
                                             const std::string& phone);
    SubmissionResult saveEmergencyContact(const Models::EmergencyContact& contact);  // Create or update

    // AcademicHistory API endpoints (uses compound key: student_id, institution_name, institution_type)
    std::vector<Models::AcademicHistory> getAcademicHistories(const std::string& studentId);
    Models::AcademicHistory getAcademicHistoryByType(const std::string& studentId, const std::string& institutionType);
    Models::AcademicHistory getAcademicHistoryByKey(const std::string& studentId,
                                                     const std::string& institutionName,
                                                     const std::string& institutionType);
    SubmissionResult createAcademicHistory(const Models::AcademicHistory& history);
    SubmissionResult updateAcademicHistory(const Models::AcademicHistory& history);
    SubmissionResult deleteAcademicHistory(const std::string& studentId,
                                            const std::string& institutionName,
                                            const std::string& institutionType);
    SubmissionResult saveAcademicHistory(const Models::AcademicHistory& history);  // Create or update

    // Curriculum API endpoints
    std::vector<Models::Curriculum> getCurriculums();
    Models::Curriculum getCurriculum(const std::string& curriculumId);

    // Institution settings API endpoints
    Models::InstitutionSettings getInstitutionSettings();
    SubmissionResult updateInstitutionSetting(const std::string& key, const std::string& value);
    SubmissionResult updateInstitutionSettings(const Models::InstitutionSettings& settings);

    // Form type configuration
    std::vector<Models::FormTypeInfo> getFormTypes();
    Models::FormTypeInfo getFormType(const std::string& formTypeId);

    // Form submission endpoints - one for each form type
    SubmissionResult submitPersonalInfo(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitEmergencyContact(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitMedicalInfo(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitAcademicHistory(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitFinancialAid(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitDocuments(const std::string& studentId, const Models::FormData& data);
    SubmissionResult submitConsent(const std::string& studentId, const Models::FormData& data);

    // Consent record management
    SubmissionResult deleteStudentConsents(const std::string& studentId);
    std::map<std::string, bool> getStudentConsents(const std::string& studentId);
    StudentConsentData getStudentConsentsWithSignature(const std::string& studentId);

    // Generic form submission (determines endpoint from form ID)
    SubmissionResult submitForm(const std::string& studentId,
                                const std::string& formId,
                                const Models::FormData& data);

    // Async form submission
    void submitFormAsync(const std::string& studentId,
                         const std::string& formId,
                         const Models::FormData& data,
                         SubmissionCallback callback);

    // Form data retrieval (for resuming incomplete forms)
    Models::FormData getFormData(const std::string& studentId, const std::string& formId);
    std::vector<Models::FormData> getAllFormData(const std::string& studentId);

    // Form status
    std::string getFormStatus(const std::string& studentId, const std::string& formId);
    std::vector<std::string> getCompletedFormIds(const std::string& studentId);
    bool isFormComplete(const std::string& studentId, const std::string& formId);

    // Batch operations
    SubmissionResult submitAllForms(const std::string& studentId,
                                    const std::vector<Models::FormData>& forms);
    SubmissionResult finalizeIntake(const std::string& studentId);

    // File upload
    SubmissionResult uploadDocument(const std::string& studentId,
                                    const std::string& documentType,
                                    const std::string& filePath);

    // Form submission record management
    SubmissionResult createFormSubmissionRecord(const std::string& studentId,
                                                 const std::string& formCode,
                                                 const std::string& status = "pending");
    int getFormTypeId(const std::string& formCode);

private:
    std::shared_ptr<ApiClient> apiClient_;
    std::map<std::string, int> formTypeCache_;  // Cache form_type code -> id mapping

    // Helper methods
    std::string getEndpointForForm(const std::string& formId) const;
    SubmissionResult parseSubmissionResponse(const ApiResponse& response);
    nlohmann::json prepareFormPayload(const std::string& studentId, const Models::FormData& data,
                                       const std::string& resourceType);
    void loadFormTypeCache();
    void ensureFormTypesExist();  // Seed FormType table if empty
    int createFormType(const std::string& code, const std::string& name,
                       const std::string& category, int displayOrder);
};

} // namespace Api
} // namespace StudentIntake

#endif // FORM_SUBMISSION_SERVICE_H
