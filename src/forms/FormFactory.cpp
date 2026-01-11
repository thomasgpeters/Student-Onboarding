#include "FormFactory.h"
#include "PersonalInfoForm.h"
#include "EmergencyContactForm.h"
#include "MedicalInfoForm.h"
#include "AcademicHistoryForm.h"
#include "FinancialAidForm.h"
#include "DocumentUploadForm.h"
#include "ConsentForm.h"
#include <algorithm>

namespace StudentIntake {
namespace Forms {

FormFactory::FormFactory() {
    registerDefaultFormTypes();
}

FormFactory::~FormFactory() {
}

void FormFactory::registerDefaultFormTypes() {
    // Personal Information Form
    registerFormType("personal_info",
                     "Personal Information",
                     "Please provide your personal details and contact information.",
                     1,
                     []() { return std::make_unique<PersonalInfoForm>(); });

    // Emergency Contact Form
    registerFormType("emergency_contact",
                     "Emergency Contacts",
                     "Provide emergency contact information for at least one person.",
                     2,
                     []() { return std::make_unique<EmergencyContactForm>(); });

    // Medical Information Form
    registerFormType("medical_info",
                     "Medical Information",
                     "Health and medical information for campus health services.",
                     3,
                     []() { return std::make_unique<MedicalInfoForm>(); });

    // Academic History Form
    registerFormType("academic_history",
                     "Academic History",
                     "Provide information about your previous education.",
                     4,
                     []() { return std::make_unique<AcademicHistoryForm>(); });

    // Financial Aid Form
    registerFormType("financial_aid",
                     "Financial Aid Information",
                     "Information for financial aid and scholarship consideration.",
                     5,
                     []() { return std::make_unique<FinancialAidForm>(); });

    // Document Upload Form
    registerFormType("documents",
                     "Document Upload",
                     "Upload required documents such as transcripts and identification.",
                     6,
                     []() { return std::make_unique<DocumentUploadForm>(); });

    // Consent Form
    registerFormType("consent",
                     "Terms and Consent",
                     "Review and accept required terms, policies, and consent forms.",
                     7,
                     []() { return std::make_unique<ConsentForm>(); });
}

void FormFactory::registerFormType(const std::string& formId,
                                    const std::string& title,
                                    const std::string& description,
                                    int order,
                                    FormCreator creator) {
    formTypes_[formId] = {title, description, order, creator};
}

bool FormFactory::hasFormType(const std::string& formId) const {
    return formTypes_.find(formId) != formTypes_.end();
}

std::unique_ptr<BaseForm> FormFactory::createForm(const std::string& formId) {
    auto it = formTypes_.find(formId);
    if (it == formTypes_.end()) {
        return nullptr;
    }

    auto form = it->second.creator();
    if (form) {
        configureForm(form.get());
        form->setFormDescription(it->second.description);
        form->initialize();
    }

    return form;
}

void FormFactory::configureForm(BaseForm* form) {
    if (form) {
        form->setSession(session_);
        form->setApiService(apiService_);
    }
}

std::vector<std::unique_ptr<BaseForm>> FormFactory::createRequiredForms(
    const std::vector<std::string>& formIds) {
    std::vector<std::unique_ptr<BaseForm>> forms;

    // Sort form IDs by their order
    std::vector<std::string> sortedIds = formIds;
    std::sort(sortedIds.begin(), sortedIds.end(),
              [this](const std::string& a, const std::string& b) {
                  return getFormOrder(a) < getFormOrder(b);
              });

    for (const auto& formId : sortedIds) {
        auto form = createForm(formId);
        if (form) {
            forms.push_back(std::move(form));
        }
    }

    return forms;
}

std::vector<std::string> FormFactory::getAvailableFormIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : formTypes_) {
        ids.push_back(pair.first);
    }

    // Sort by order
    std::sort(ids.begin(), ids.end(),
              [this](const std::string& a, const std::string& b) {
                  auto itA = formTypes_.find(a);
                  auto itB = formTypes_.find(b);
                  return itA->second.order < itB->second.order;
              });

    return ids;
}

std::string FormFactory::getFormTitle(const std::string& formId) const {
    auto it = formTypes_.find(formId);
    if (it != formTypes_.end()) {
        return it->second.title;
    }
    return "";
}

std::string FormFactory::getFormDescription(const std::string& formId) const {
    auto it = formTypes_.find(formId);
    if (it != formTypes_.end()) {
        return it->second.description;
    }
    return "";
}

int FormFactory::getFormOrder(const std::string& formId) const {
    auto it = formTypes_.find(formId);
    if (it != formTypes_.end()) {
        return it->second.order;
    }
    return 999;
}

} // namespace Forms
} // namespace StudentIntake
