#include "ConsentForm.h"
#include "utils/Logger.h"
#include <Wt/WLabel.h>
#include <Wt/WDate.h>

namespace StudentIntake {
namespace Forms {

ConsentForm::ConsentForm()
    : BaseForm("consent", "Terms and Consent") {
}

ConsentForm::~ConsentForm() {
}

void ConsentForm::createFormFields() {
    // Introduction
    auto intro = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    intro->addStyleClass("form-section");
    auto introText = intro->addWidget(std::make_unique<Wt::WText>(
        "<p>Please review and acknowledge the following terms and policies to complete your enrollment.</p>"));
    introText->setTextFormat(Wt::TextFormat::XHTML);

    // Consent checkboxes section
    auto checkboxSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    checkboxSection->addStyleClass("form-section consent-checkboxes");

    // Terms of Service
    auto termsRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    termsRow->addStyleClass("consent-item");
    termsCheckbox_ = termsRow->addWidget(std::make_unique<Wt::WCheckBox>(" I agree to the Terms of Service *"));
    termsCheckbox_->addStyleClass("consent-title");
    auto termsDesc = termsRow->addWidget(std::make_unique<Wt::WText>(
        "By enrolling, I agree to abide by all university policies, procedures, regulations, and applicable laws."));
    termsDesc->addStyleClass("consent-description");

    // Privacy Policy
    auto privacyRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    privacyRow->addStyleClass("consent-item");
    privacyCheckbox_ = privacyRow->addWidget(std::make_unique<Wt::WCheckBox>(" I agree to the Privacy Policy *"));
    privacyCheckbox_->addStyleClass("consent-title");
    auto privacyDesc = privacyRow->addWidget(std::make_unique<Wt::WText>(
        "My personal information will be collected, stored, and processed in accordance with the university's Privacy Policy."));
    privacyDesc->addStyleClass("consent-description");

    // FERPA Rights
    auto ferpaRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    ferpaRow->addStyleClass("consent-item");
    ferpaCheckbox_ = ferpaRow->addWidget(std::make_unique<Wt::WCheckBox>(" I acknowledge my FERPA rights *"));
    ferpaCheckbox_->addStyleClass("consent-title");
    auto ferpaDesc = ferpaRow->addWidget(std::make_unique<Wt::WText>(
        "I understand my rights under the Family Educational Rights and Privacy Act regarding my education records."));
    ferpaDesc->addStyleClass("consent-description");

    // Student Code of Conduct
    auto conductRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    conductRow->addStyleClass("consent-item");
    codeOfConductCheckbox_ = conductRow->addWidget(std::make_unique<Wt::WCheckBox>(" I agree to the Student Code of Conduct *"));
    codeOfConductCheckbox_->addStyleClass("consent-title");
    auto conductDesc = conductRow->addWidget(std::make_unique<Wt::WText>(
        "I will uphold academic integrity and ethical behavior standards, including refraining from cheating and plagiarism."));
    conductDesc->addStyleClass("consent-description");

    // Communication Consent
    auto commRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    commRow->addStyleClass("consent-item");
    communicationCheckbox_ = commRow->addWidget(std::make_unique<Wt::WCheckBox>(" I consent to receive communications *"));
    communicationCheckbox_->addStyleClass("consent-title");
    auto commDesc = commRow->addWidget(std::make_unique<Wt::WText>(
        "I agree to receive email, SMS, and mail from the university regarding enrollment, academics, and campus events."));
    commDesc->addStyleClass("consent-description");

    // Photo Release (Optional)
    auto photoRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    photoRow->addStyleClass("consent-item");
    photoReleaseCheckbox_ = photoRow->addWidget(std::make_unique<Wt::WCheckBox>(" I consent to photo/media release (optional)"));
    photoReleaseCheckbox_->addStyleClass("consent-title");
    auto photoDesc = photoRow->addWidget(std::make_unique<Wt::WText>(
        "I grant permission for photos and videos taken during university events to be used for promotional purposes."));
    photoDesc->addStyleClass("consent-description");

    // Accuracy Certification
    auto accuracyRow = checkboxSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    accuracyRow->addStyleClass("consent-item");
    accuracyCheckbox_ = accuracyRow->addWidget(std::make_unique<Wt::WCheckBox>(" I certify accuracy of information *"));
    accuracyCheckbox_->addStyleClass("consent-title");
    auto accuracyDesc = accuracyRow->addWidget(std::make_unique<Wt::WText>(
        "All information provided in this application is accurate and complete. I understand false information may result in disciplinary action."));
    accuracyDesc->addStyleClass("consent-description");

    // Electronic Signature section
    auto signatureSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    signatureSection->addStyleClass("form-section signature-section");
    auto signatureHeader = signatureSection->addWidget(std::make_unique<Wt::WText>("<h4>Electronic Signature</h4>"));
    signatureHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto sigText = signatureSection->addWidget(std::make_unique<Wt::WText>(
        "<p>By typing your full legal name below, you are providing an electronic signature that "
        "has the same legal effect as a handwritten signature.</p>"));
    sigText->setTextFormat(Wt::TextFormat::XHTML);

    auto sigRow = signatureSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    sigRow->addStyleClass("form-row");

    auto sigGroup = sigRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    sigGroup->addStyleClass("form-group col-md-8");
    sigGroup->addWidget(std::make_unique<Wt::WLabel>("Full Legal Name (Electronic Signature) *"));
    signatureInput_ = sigGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    signatureInput_->setPlaceholderText("Type your full legal name");
    signatureInput_->addStyleClass("form-control signature-input");

    auto dateGroup = sigRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateGroup->addStyleClass("form-group col-md-4");
    dateGroup->addWidget(std::make_unique<Wt::WLabel>("Date *"));
    signatureDateInput_ = dateGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    signatureDateInput_->setDate(Wt::WDate::currentDate());
    signatureDateInput_->addStyleClass("form-control");

    // Update button text
    setNextButtonText("Submit Application");

    // Load existing consent data from API
    loadConsentsFromApi();
}

bool ConsentForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    if (!termsCheckbox_->isChecked()) {
        validationErrors_.push_back("You must agree to the Terms of Service");
        isValid_ = false;
    }

    if (!privacyCheckbox_->isChecked()) {
        validationErrors_.push_back("You must agree to the Privacy Policy");
        isValid_ = false;
    }

    if (!ferpaCheckbox_->isChecked()) {
        validationErrors_.push_back("You must acknowledge FERPA rights");
        isValid_ = false;
    }

    if (!codeOfConductCheckbox_->isChecked()) {
        validationErrors_.push_back("You must agree to the Student Code of Conduct");
        isValid_ = false;
    }

    if (!communicationCheckbox_->isChecked()) {
        validationErrors_.push_back("You must consent to receive communications");
        isValid_ = false;
    }

    if (!accuracyCheckbox_->isChecked()) {
        validationErrors_.push_back("You must certify the accuracy of your information");
        isValid_ = false;
    }

    if (signatureInput_->text().empty()) {
        validationErrors_.push_back("Electronic signature is required");
        isValid_ = false;
    }

    if (!signatureDateInput_->date().isValid()) {
        validationErrors_.push_back("Signature date is required");
        isValid_ = false;
    }

    return isValid_;
}

void ConsentForm::collectFormData(Models::FormData& data) const {
    data.setField("termsAccepted", termsCheckbox_->isChecked());
    data.setField("privacyAccepted", privacyCheckbox_->isChecked());
    data.setField("ferpaAcknowledged", ferpaCheckbox_->isChecked());
    data.setField("codeOfConductAccepted", codeOfConductCheckbox_->isChecked());
    data.setField("communicationConsent", communicationCheckbox_->isChecked());
    data.setField("photoRelease", photoReleaseCheckbox_->isChecked());
    data.setField("accuracyCertified", accuracyCheckbox_->isChecked());
    data.setField("electronicSignature", signatureInput_->text().toUTF8());
    data.setField("signatureDate", signatureDateInput_->date().toString("yyyy-MM-dd").toUTF8());
}

void ConsentForm::populateFormFields(const Models::FormData& data) {
    if (data.hasField("termsAccepted"))
        termsCheckbox_->setChecked(data.getField("termsAccepted").boolValue);
    if (data.hasField("privacyAccepted"))
        privacyCheckbox_->setChecked(data.getField("privacyAccepted").boolValue);
    if (data.hasField("ferpaAcknowledged"))
        ferpaCheckbox_->setChecked(data.getField("ferpaAcknowledged").boolValue);
    if (data.hasField("codeOfConductAccepted"))
        codeOfConductCheckbox_->setChecked(data.getField("codeOfConductAccepted").boolValue);
    if (data.hasField("communicationConsent"))
        communicationCheckbox_->setChecked(data.getField("communicationConsent").boolValue);
    if (data.hasField("photoRelease"))
        photoReleaseCheckbox_->setChecked(data.getField("photoRelease").boolValue);
    if (data.hasField("accuracyCertified"))
        accuracyCheckbox_->setChecked(data.getField("accuracyCertified").boolValue);
    if (data.hasField("electronicSignature"))
        signatureInput_->setText(data.getField("electronicSignature").stringValue);
}

void ConsentForm::loadConsentsFromApi() {
    if (!apiService_ || !session_) {
        LOG_DEBUG("ConsentForm", "No apiService or session available, skipping API load");
        return;
    }

    std::string studentId = session_->getStudent().getId();
    if (studentId.empty()) {
        LOG_DEBUG("ConsentForm", "No student ID, skipping API load");
        return;
    }

    LOG_DEBUG("ConsentForm", "Loading consent data from API for student: " << studentId);

    auto consentData = apiService_->getStudentConsentsWithSignature(studentId);

    if (consentData.consents.empty()) {
        LOG_DEBUG("ConsentForm", "No existing consent records found");
        return;
    }

    // Map API consent_type values to form checkboxes
    if (consentData.consents.count("terms_of_service")) {
        termsCheckbox_->setChecked(consentData.consents["terms_of_service"]);
    }
    if (consentData.consents.count("privacy_policy")) {
        privacyCheckbox_->setChecked(consentData.consents["privacy_policy"]);
    }
    if (consentData.consents.count("ferpa_acknowledgment")) {
        ferpaCheckbox_->setChecked(consentData.consents["ferpa_acknowledgment"]);
    }
    if (consentData.consents.count("code_of_conduct")) {
        codeOfConductCheckbox_->setChecked(consentData.consents["code_of_conduct"]);
    }
    if (consentData.consents.count("communication_consent")) {
        communicationCheckbox_->setChecked(consentData.consents["communication_consent"]);
    }
    if (consentData.consents.count("photo_release")) {
        photoReleaseCheckbox_->setChecked(consentData.consents["photo_release"]);
    }
    if (consentData.consents.count("accuracy_certification")) {
        accuracyCheckbox_->setChecked(consentData.consents["accuracy_certification"]);
    }

    // Load signature if present
    if (!consentData.signature.empty()) {
        signatureInput_->setText(consentData.signature);
    }

    LOG_DEBUG("ConsentForm", "Loaded " << consentData.consents.size() << " consent values from API");
}

} // namespace Forms
} // namespace StudentIntake
