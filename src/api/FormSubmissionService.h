#ifndef FORM_SUBMISSION_SERVICE_H
#define FORM_SUBMISSION_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "ApiClient.h"
#include "models/FormData.h"
#include "models/Student.h"
#include "models/Curriculum.h"

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

    // Curriculum API endpoints
    std::vector<Models::Curriculum> getCurriculums();
    Models::Curriculum getCurriculum(const std::string& curriculumId);

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

private:
    std::shared_ptr<ApiClient> apiClient_;

    // Helper methods
    std::string getEndpointForForm(const std::string& formId) const;
    SubmissionResult parseSubmissionResponse(const ApiResponse& response);
    nlohmann::json prepareFormPayload(const std::string& studentId, const Models::FormData& data,
                                       const std::string& resourceType);
};

} // namespace Api
} // namespace StudentIntake

#endif // FORM_SUBMISSION_SERVICE_H
