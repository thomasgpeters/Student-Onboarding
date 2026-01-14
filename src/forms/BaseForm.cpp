#include "BaseForm.h"
#include <Wt/WBreak.h>
#include <Wt/WLabel.h>
#include <regex>

namespace StudentIntake {
namespace Forms {

BaseForm::BaseForm(const std::string& formId, const std::string& formTitle)
    : WContainerWidget()
    , formId_(formId)
    , formTitle_(formTitle)
    , formDescription_("")
    , headerContainer_(nullptr)
    , formFieldsContainer_(nullptr)
    , errorContainer_(nullptr)
    , successContainer_(nullptr)
    , footerContainer_(nullptr)
    , previousButton_(nullptr)
    , nextButton_(nullptr)
    , saveButton_(nullptr)
    , isValid_(false)
    , isDirty_(false)
    , isSubmitting_(false) {
}

BaseForm::~BaseForm() {
}

void BaseForm::initialize() {
    setupLayout();
    createHeader();
    createFormFields();
    createFooter();
}

void BaseForm::setupLayout() {
    addStyleClass("intake-form");
    addStyleClass("form-" + formId_);

    // Header section
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("form-header");

    // Error container (hidden by default)
    errorContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->addStyleClass("alert alert-danger");
    errorContainer_->hide();

    // Success container (hidden by default)
    successContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    successContainer_->addStyleClass("alert alert-success");
    successContainer_->hide();

    // Main form fields container
    formFieldsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    formFieldsContainer_->addStyleClass("form-fields");

    // Footer with navigation
    footerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    footerContainer_->addStyleClass("form-footer");
}

void BaseForm::createHeader() {
    auto title = headerContainer_->addWidget(
        std::make_unique<Wt::WText>("<h3>" + formTitle_ + "</h3>"));
    title->addStyleClass("form-title");

    if (!formDescription_.empty()) {
        auto desc = headerContainer_->addWidget(
            std::make_unique<Wt::WText>("<p>" + formDescription_ + "</p>"));
        desc->addStyleClass("form-description");
    }

    // Required fields note
    auto requiredNote = headerContainer_->addWidget(
        std::make_unique<Wt::WText>("<p class='required-note'>Fields marked with * are required</p>"));
}

void BaseForm::createFooter() {
    auto buttonRow = footerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonRow->addStyleClass("button-row");

    // Previous button
    previousButton_ = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Previous"));
    previousButton_->addStyleClass("btn btn-secondary");
    previousButton_->clicked().connect(this, &BaseForm::handlePrevious);

    // Save draft button
    saveButton_ = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Save Draft"));
    saveButton_->addStyleClass("btn btn-outline-primary");
    saveButton_->clicked().connect([this]() {
        Models::FormData data = getFormData();
        data.setStatus("draft");
        if (session_) {
            session_->setFormData(formId_, data);
        }
        showSuccess("Draft saved successfully");
    });

    // Next/Submit button
    nextButton_ = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Next"));
    nextButton_->addStyleClass("btn btn-primary");
    nextButton_->clicked().connect(this, &BaseForm::handleSubmit);
}

void BaseForm::reset() {
    validationErrors_.clear();
    isValid_ = false;
    isDirty_ = false;
    clearErrors();
}

void BaseForm::loadData() {
    if (session_ && session_->hasFormData(formId_)) {
        loadData(session_->getFormData(formId_));
    }
}

void BaseForm::loadData(const Models::FormData& data) {
    populateFormFields(data);
    isDirty_ = false;
}

bool BaseForm::validate() {
    validationErrors_.clear();
    isValid_ = true;
    // Derived classes will add specific validation
    return isValid_;
}

std::vector<std::string> BaseForm::getValidationErrors() const {
    return validationErrors_;
}

Models::FormData BaseForm::getFormData() const {
    Models::FormData data(formId_, session_ ? session_->getStudent().getId() : "");
    collectFormData(data);
    return data;
}

void BaseForm::setShowPreviousButton(bool show) {
    if (previousButton_) {
        previousButton_->setHidden(!show);
    }
}

void BaseForm::setShowNextButton(bool show) {
    if (nextButton_) {
        nextButton_->setHidden(!show);
    }
}

void BaseForm::setNextButtonText(const std::string& text) {
    if (nextButton_) {
        nextButton_->setText(text);
    }
}

void BaseForm::setPreviousButtonText(const std::string& text) {
    if (previousButton_) {
        previousButton_->setText(text);
    }
}

void BaseForm::showError(const std::string& message) {
    errorContainer_->clear();
    errorContainer_->addWidget(std::make_unique<Wt::WText>(message));
    errorContainer_->show();
    successContainer_->hide();
}

void BaseForm::showErrors(const std::vector<std::string>& errors) {
    errorContainer_->clear();
    std::string html = "<ul>";
    for (const auto& error : errors) {
        html += "<li>" + error + "</li>";
    }
    html += "</ul>";
    errorContainer_->addWidget(std::make_unique<Wt::WText>(html));
    errorContainer_->show();
    successContainer_->hide();
}

void BaseForm::clearErrors() {
    errorContainer_->clear();
    errorContainer_->hide();
}

void BaseForm::showSuccess(const std::string& message) {
    successContainer_->clear();
    successContainer_->addWidget(std::make_unique<Wt::WText>(message));
    successContainer_->show();
    errorContainer_->hide();
}

void BaseForm::markFieldInvalid(Wt::WFormWidget* field, const std::string& message) {
    if (field) {
        field->addStyleClass("is-invalid");
        // You could also add a validation message element here
    }
}

void BaseForm::clearFieldValidation(Wt::WFormWidget* field) {
    if (field) {
        field->removeStyleClass("is-invalid");
        field->removeStyleClass("is-valid");
    }
}

bool BaseForm::validateRequired(const std::string& value, const std::string& fieldName) {
    if (value.empty()) {
        validationErrors_.push_back(fieldName + " is required");
        isValid_ = false;
        return false;
    }
    return true;
}

bool BaseForm::validateEmail(const std::string& email) {
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        validationErrors_.push_back("Please enter a valid email address");
        isValid_ = false;
        return false;
    }
    return true;
}

bool BaseForm::validatePhone(const std::string& phone) {
    // Allow various phone formats
    std::regex phoneRegex(R"(^[\d\s\-\(\)\+\.]{10,20}$)");
    if (!std::regex_match(phone, phoneRegex)) {
        validationErrors_.push_back("Please enter a valid phone number");
        isValid_ = false;
        return false;
    }
    return true;
}

bool BaseForm::validateDate(const std::string& date) {
    // Basic date format validation (YYYY-MM-DD)
    std::regex dateRegex(R"(\d{4}-\d{2}-\d{2})");
    if (!std::regex_match(date, dateRegex)) {
        validationErrors_.push_back("Please enter a valid date (YYYY-MM-DD)");
        isValid_ = false;
        return false;
    }
    return true;
}

bool BaseForm::validateSSN(const std::string& ssn) {
    // SSN format: XXX-XX-XXXX
    std::regex ssnRegex(R"(\d{3}-\d{2}-\d{4})");
    if (!std::regex_match(ssn, ssnRegex)) {
        validationErrors_.push_back("Please enter a valid SSN (XXX-XX-XXXX)");
        isValid_ = false;
        return false;
    }
    return true;
}

bool BaseForm::validateZipCode(const std::string& zipCode) {
    // US ZIP code: 5 digits or 5+4 format
    std::regex zipRegex(R"(\d{5}(-\d{4})?)");
    if (!std::regex_match(zipCode, zipRegex)) {
        validationErrors_.push_back("Please enter a valid ZIP code");
        isValid_ = false;
        return false;
    }
    return true;
}

void BaseForm::handleSubmit() {
    clearErrors();

    if (!validate()) {
        showErrors(validationErrors_);
        return;
    }

    isSubmitting_ = true;
    nextButton_->setEnabled(false);
    nextButton_->setText("Submitting...");

    Models::FormData data = getFormData();
    data.setStatus("submitted");

    // Save to session
    if (session_) {
        session_->setFormData(formId_, data);
    }

    // Submit to API
    if (apiService_ && session_) {
        Api::SubmissionResult result = apiService_->submitForm(
            session_->getStudent().getId(), formId_, data);

        if (result.success) {
            onSubmitSuccess(result);
        } else {
            onSubmitError(result);
        }
    } else {
        // No API service, just emit success
        onSubmitSuccess(Api::SubmissionResult{true, "", "Form data saved", {}, {}});
    }
}

void BaseForm::handlePrevious() {
    // Save current data as draft
    if (session_ && isDirty_) {
        Models::FormData data = getFormData();
        data.setStatus("draft");
        session_->setFormData(formId_, data);
    }

    previousRequested_.emit();
}

void BaseForm::handleCancel() {
    formCancelled_.emit();
}

void BaseForm::onSubmitSuccess(const Api::SubmissionResult& result) {
    isSubmitting_ = false;
    nextButton_->setEnabled(true);
    nextButton_->setText("Next");

    // Mark form as completed in session
    if (session_) {
        session_->getStudent().markFormCompleted(formId_);

        // Save completed_forms to API so it persists across sessions
        if (apiService_) {
            apiService_->updateStudentProfile(session_->getStudent());
        }
    }

    formSubmitted_.emit();
}

void BaseForm::onSubmitError(const Api::SubmissionResult& result) {
    isSubmitting_ = false;
    nextButton_->setEnabled(true);
    nextButton_->setText("Next");

    if (!result.errors.empty()) {
        showErrors(result.errors);
    } else {
        showError(result.message);
    }

    formError_.emit(result.message);
}

} // namespace Forms
} // namespace StudentIntake
