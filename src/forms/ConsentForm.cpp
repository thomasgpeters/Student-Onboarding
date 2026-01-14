#include "ConsentForm.h"
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
    auto intro = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    intro->addStyleClass("form-section");
    intro->addWidget(std::make_unique<Wt::WText>(
        "<p>Please review and acknowledge the following terms and policies to complete your enrollment.</p>"));

    // Terms of Service
    auto termsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    termsSection->addStyleClass("form-section consent-section");
    auto termsHeader = termsSection->addWidget(std::make_unique<Wt::WText>("<h4>Terms of Service</h4>"));
    termsHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto termsText = termsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    termsText->addStyleClass("consent-text");
    termsText->addWidget(std::make_unique<Wt::WText>(
        "<p>By enrolling at this institution, you agree to abide by all university policies, "
        "procedures, and regulations. This includes academic integrity policies, student conduct "
        "codes, and all applicable local, state, and federal laws.</p>"));

    termsCheckbox_ = termsSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have read and agree to the Terms of Service *"));
    termsCheckbox_->addStyleClass("form-check consent-checkbox");

    // Privacy Policy
    auto privacySection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    privacySection->addStyleClass("form-section consent-section");
    auto privacyHeader = privacySection->addWidget(std::make_unique<Wt::WText>("<h4>Privacy Policy</h4>"));
    privacyHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto privacyText = privacySection->addWidget(std::make_unique<Wt::WContainerWidget>());
    privacyText->addStyleClass("consent-text");
    privacyText->addWidget(std::make_unique<Wt::WText>(
        "<p>Your personal information will be collected, stored, and processed in accordance with "
        "our Privacy Policy. We are committed to protecting your privacy and will only use your "
        "information for educational purposes and as required by law.</p>"));

    privacyCheckbox_ = privacySection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have read and agree to the Privacy Policy *"));
    privacyCheckbox_->addStyleClass("form-check consent-checkbox");

    // FERPA Rights
    auto ferpaSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    ferpaSection->addStyleClass("form-section consent-section");
    auto ferpaHeader = ferpaSection->addWidget(std::make_unique<Wt::WText>("<h4>FERPA Rights Acknowledgment</h4>"));
    ferpaHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto ferpaText = ferpaSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    ferpaText->addStyleClass("consent-text");
    ferpaText->addWidget(std::make_unique<Wt::WText>(
        "<p>The Family Educational Rights and Privacy Act (FERPA) protects the privacy of student "
        "education records. You have the right to inspect and review your education records, "
        "request amendments, and control disclosure of personally identifiable information.</p>"));

    ferpaCheckbox_ = ferpaSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I acknowledge my FERPA rights and responsibilities *"));
    ferpaCheckbox_->addStyleClass("form-check consent-checkbox");

    // Student Code of Conduct
    auto conductSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    conductSection->addStyleClass("form-section consent-section");
    auto conductHeader = conductSection->addWidget(std::make_unique<Wt::WText>("<h4>Student Code of Conduct</h4>"));
    conductHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto conductText = conductSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    conductText->addStyleClass("consent-text");
    conductText->addWidget(std::make_unique<Wt::WText>(
        "<p>As a member of our academic community, you agree to uphold the highest standards of "
        "academic integrity and ethical behavior. This includes refraining from cheating, plagiarism, "
        "and other forms of academic dishonesty.</p>"));

    codeOfConductCheckbox_ = conductSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I agree to abide by the Student Code of Conduct *"));
    codeOfConductCheckbox_->addStyleClass("form-check consent-checkbox");

    // Communication Consent
    auto commSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    commSection->addStyleClass("form-section consent-section");
    auto commHeader = commSection->addWidget(std::make_unique<Wt::WText>("<h4>Communication Preferences</h4>"));
    commHeader->setTextFormat(Wt::TextFormat::XHTML);

    communicationCheckbox_ = commSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I consent to receive communications (email, SMS, mail) from the university regarding "
        "my enrollment, academics, and campus events *"));
    communicationCheckbox_->addStyleClass("form-check consent-checkbox");

    // Photo Release (Optional)
    auto photoSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    photoSection->addStyleClass("form-section consent-section");
    auto photoHeader = photoSection->addWidget(std::make_unique<Wt::WText>("<h4>Photo/Media Release (Optional)</h4>"));
    photoHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto photoText = photoSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    photoText->addStyleClass("consent-text");
    photoText->addWidget(std::make_unique<Wt::WText>(
        "<p>I grant permission for photographs, videos, or other media of me taken during university "
        "events to be used for promotional and educational purposes.</p>"));

    photoReleaseCheckbox_ = photoSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I consent to the photo/media release (optional)"));
    photoReleaseCheckbox_->addStyleClass("form-check consent-checkbox");

    // Accuracy Certification
    auto accuracySection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    accuracySection->addStyleClass("form-section consent-section");
    auto accuracyHeader = accuracySection->addWidget(std::make_unique<Wt::WText>("<h4>Certification of Accuracy</h4>"));
    accuracyHeader->setTextFormat(Wt::TextFormat::XHTML);

    accuracyCheckbox_ = accuracySection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I certify that all information provided in this application is accurate and complete "
        "to the best of my knowledge. I understand that providing false information may result "
        "in dismissal or other disciplinary action. *"));
    accuracyCheckbox_->addStyleClass("form-check consent-checkbox");

    // Electronic Signature
    auto signatureSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    signatureSection->addStyleClass("form-section signature-section");
    auto signatureHeader = signatureSection->addWidget(std::make_unique<Wt::WText>("<h4>Electronic Signature</h4>"));
    signatureHeader->setTextFormat(Wt::TextFormat::XHTML);

    signatureSection->addWidget(std::make_unique<Wt::WText>(
        "<p>By typing your full legal name below, you are providing an electronic signature that "
        "has the same legal effect as a handwritten signature.</p>"));

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

} // namespace Forms
} // namespace StudentIntake
