#ifndef BASE_FORM_H
#define BASE_FORM_H

#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <memory>
#include <string>
#include <vector>
#include "models/FormData.h"
#include "session/StudentSession.h"
#include "api/FormSubmissionService.h"

namespace StudentIntake {
namespace Forms {

/**
 * @brief Base class for all intake forms
 * Provides common functionality for form validation, submission, and navigation
 */
class BaseForm : public Wt::WContainerWidget {
public:
    BaseForm(const std::string& formId, const std::string& formTitle);
    virtual ~BaseForm();

    // Form identification
    std::string getFormId() const { return formId_; }
    std::string getFormTitle() const { return formTitle_; }
    std::string getFormDescription() const { return formDescription_; }
    void setFormDescription(const std::string& desc) { formDescription_ = desc; }

    // Dependencies
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }
    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) { apiService_ = apiService; }

    // Signals for navigation
    Wt::Signal<>& formSubmitted() { return formSubmitted_; }
    Wt::Signal<>& formCancelled() { return formCancelled_; }
    Wt::Signal<>& previousRequested() { return previousRequested_; }
    Wt::Signal<std::string>& formError() { return formError_; }

    // Form lifecycle
    virtual void initialize();
    virtual void reset();
    virtual void loadData();
    virtual void loadData(const Models::FormData& data);

    // Validation
    virtual bool validate();
    virtual std::vector<std::string> getValidationErrors() const;
    bool isValid() const { return isValid_; }

    // Data access
    virtual Models::FormData getFormData() const;

    // Form state
    bool isDirty() const { return isDirty_; }
    void setDirty(bool dirty) { isDirty_ = dirty; }
    bool isSubmitting() const { return isSubmitting_; }

    // Show/hide navigation buttons
    void setShowPreviousButton(bool show);
    void setShowNextButton(bool show);
    void setNextButtonText(const std::string& text);
    void setPreviousButtonText(const std::string& text);

protected:
    // Override these in derived classes to create form-specific UI
    virtual void createFormFields() = 0;
    virtual void collectFormData(Models::FormData& data) const = 0;
    virtual void populateFormFields(const Models::FormData& data) = 0;

    // Common UI helpers
    void setupLayout();
    void createHeader();
    void createFooter();
    void showError(const std::string& message);
    void showErrors(const std::vector<std::string>& errors);
    void clearErrors();
    void showSuccess(const std::string& message);
    void markFieldInvalid(Wt::WFormWidget* field, const std::string& message);
    void clearFieldValidation(Wt::WFormWidget* field);

    // Common validation helpers
    bool validateRequired(const std::string& value, const std::string& fieldName);
    bool validateEmail(const std::string& email);
    bool validatePhone(const std::string& phone);
    bool validateDate(const std::string& date);
    bool validateSSN(const std::string& ssn);
    bool validateZipCode(const std::string& zipCode);

    // Submit handling
    virtual void handleSubmit();
    virtual void handlePrevious();
    virtual void handleCancel();
    virtual void onSubmitSuccess(const Api::SubmissionResult& result);
    virtual void onSubmitError(const Api::SubmissionResult& result);

    // Protected members
    std::string formId_;
    std::string formTitle_;
    std::string formDescription_;
    std::shared_ptr<Session::StudentSession> session_;
    std::shared_ptr<Api::FormSubmissionService> apiService_;

    // UI containers
    Wt::WContainerWidget* headerContainer_;
    Wt::WContainerWidget* formFieldsContainer_;
    Wt::WContainerWidget* errorContainer_;
    Wt::WContainerWidget* successContainer_;
    Wt::WContainerWidget* footerContainer_;

    // Navigation buttons
    Wt::WPushButton* previousButton_;
    Wt::WPushButton* nextButton_;
    Wt::WPushButton* saveButton_;

    // State
    bool isValid_;
    bool isDirty_;
    bool isSubmitting_;
    std::vector<std::string> validationErrors_;

private:
    Wt::Signal<> formSubmitted_;
    Wt::Signal<> formCancelled_;
    Wt::Signal<> previousRequested_;
    Wt::Signal<std::string> formError_;
};

} // namespace Forms
} // namespace StudentIntake

#endif // BASE_FORM_H
