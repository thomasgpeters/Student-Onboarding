#ifndef FORM_FACTORY_H
#define FORM_FACTORY_H

#include <memory>
#include <string>
#include <map>
#include <functional>
#include "BaseForm.h"
#include "session/StudentSession.h"
#include "api/FormSubmissionService.h"

namespace StudentIntake {
namespace Forms {

// Forward declarations
class PersonalInfoForm;
class EmergencyContactForm;
class MedicalInfoForm;
class AcademicHistoryForm;
class FinancialAidForm;
class DocumentUploadForm;
class ConsentForm;

/**
 * @brief Factory for creating form instances
 */
class FormFactory {
public:
    // Form creation function type
    using FormCreator = std::function<std::unique_ptr<BaseForm>()>;

    FormFactory();
    ~FormFactory();

    // Dependency injection
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }
    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) { apiService_ = apiService; }

    // Create a specific form by ID
    std::unique_ptr<BaseForm> createForm(const std::string& formId);

    // Create all required forms for a student
    std::vector<std::unique_ptr<BaseForm>> createRequiredForms(
        const std::vector<std::string>& formIds);

    // Get information about available forms
    std::vector<std::string> getAvailableFormIds() const;
    std::string getFormTitle(const std::string& formId) const;
    std::string getFormDescription(const std::string& formId) const;
    int getFormOrder(const std::string& formId) const;

    // Register custom form types
    void registerFormType(const std::string& formId,
                          const std::string& title,
                          const std::string& description,
                          int order,
                          FormCreator creator);

    // Check if a form type exists
    bool hasFormType(const std::string& formId) const;

private:
    struct FormTypeRegistration {
        std::string title;
        std::string description;
        int order;
        FormCreator creator;
    };

    std::shared_ptr<Session::StudentSession> session_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::map<std::string, FormTypeRegistration> formTypes_;

    void registerDefaultFormTypes();
    void configureForm(BaseForm* form);
};

} // namespace Forms
} // namespace StudentIntake

#endif // FORM_FACTORY_H
