#include "MedicalInfoForm.h"
#include <Wt/WLabel.h>

namespace StudentIntake {
namespace Forms {

MedicalInfoForm::MedicalInfoForm()
    : BaseForm("medical_info", "Medical Information") {
}

MedicalInfoForm::~MedicalInfoForm() {
}

void MedicalInfoForm::createFormFields() {
    // Privacy notice
    auto notice = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    notice->addStyleClass("alert alert-info");
    notice->addWidget(std::make_unique<Wt::WText>(
        "Your medical information is kept confidential and is only shared with "
        "campus health services and authorized personnel as needed for your care."));

    // Insurance Section
    auto insuranceSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    insuranceSection->addStyleClass("form-section");
    auto insuranceHeader = insuranceSection->addWidget(std::make_unique<Wt::WText>("<h4>Health Insurance</h4>"));
    insuranceHeader->setTextFormat(Wt::TextFormat::XHTML);

    hasInsuranceCheckbox_ = insuranceSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have health insurance"));
    hasInsuranceCheckbox_->addStyleClass("form-check");
    hasInsuranceCheckbox_->changed().connect(this, &MedicalInfoForm::toggleInsurance);

    insuranceContainer_ = insuranceSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    insuranceContainer_->addStyleClass("insurance-fields");
    insuranceContainer_->hide();

    auto row1 = insuranceContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    row1->addStyleClass("form-row");

    auto providerGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    providerGroup->addStyleClass("form-group col-md-6");
    providerGroup->addWidget(std::make_unique<Wt::WLabel>("Insurance Provider"));
    insuranceProviderInput_ = providerGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    insuranceProviderInput_->addStyleClass("form-control");

    auto policyGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    policyGroup->addStyleClass("form-group col-md-6");
    policyGroup->addWidget(std::make_unique<Wt::WLabel>("Policy Number"));
    policyNumberInput_ = policyGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    policyNumberInput_->addStyleClass("form-control");

    auto row2 = insuranceContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    row2->addStyleClass("form-row");

    auto groupGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    groupGroup->addStyleClass("form-group col-md-6");
    groupGroup->addWidget(std::make_unique<Wt::WLabel>("Group Number"));
    groupNumberInput_ = groupGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    groupNumberInput_->addStyleClass("form-control");

    auto holderGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    holderGroup->addStyleClass("form-group col-md-6");
    holderGroup->addWidget(std::make_unique<Wt::WLabel>("Policy Holder Name"));
    policyHolderInput_ = holderGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    policyHolderInput_->addStyleClass("form-control");

    // Physician Section
    auto physicianSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    physicianSection->addStyleClass("form-section");
    auto physicianHeader = physicianSection->addWidget(std::make_unique<Wt::WText>("<h4>Primary Care Physician</h4>"));
    physicianHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto physicianRow = physicianSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    physicianRow->addStyleClass("form-row");

    auto nameGroup = physicianRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group col-md-6");
    nameGroup->addWidget(std::make_unique<Wt::WLabel>("Physician Name"));
    physicianNameInput_ = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    physicianNameInput_->addStyleClass("form-control");

    auto phoneGroup = physicianRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneGroup->addStyleClass("form-group col-md-6");
    phoneGroup->addWidget(std::make_unique<Wt::WLabel>("Phone Number"));
    physicianPhoneInput_ = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    physicianPhoneInput_->addStyleClass("form-control");

    auto addrGroup = physicianSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    addrGroup->addStyleClass("form-group");
    addrGroup->addWidget(std::make_unique<Wt::WLabel>("Address"));
    physicianAddressInput_ = addrGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    physicianAddressInput_->addStyleClass("form-control");

    // Medical Conditions
    auto conditionsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    conditionsSection->addStyleClass("form-section");
    auto conditionsHeader = conditionsSection->addWidget(std::make_unique<Wt::WText>("<h4>Medical History</h4>"));
    conditionsHeader->setTextFormat(Wt::TextFormat::XHTML);

    hasConditionsCheckbox_ = conditionsSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have existing medical conditions"));
    hasConditionsCheckbox_->addStyleClass("form-check");
    hasConditionsCheckbox_->changed().connect(this, &MedicalInfoForm::toggleConditions);

    auto conditionsContainer = conditionsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    conditionsContainer->addStyleClass("form-group conditional-field");
    conditionsInput_ = conditionsContainer->addWidget(std::make_unique<Wt::WTextArea>());
    conditionsInput_->setPlaceholderText("Please list any medical conditions...");
    conditionsInput_->addStyleClass("form-control");
    conditionsInput_->setRows(3);
    conditionsInput_->hide();

    // Allergies
    hasAllergiesCheckbox_ = conditionsSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have allergies (food, medication, environmental)"));
    hasAllergiesCheckbox_->addStyleClass("form-check");
    hasAllergiesCheckbox_->changed().connect(this, &MedicalInfoForm::toggleAllergies);

    auto allergiesContainer = conditionsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    allergiesContainer->addStyleClass("form-group conditional-field");
    allergiesInput_ = allergiesContainer->addWidget(std::make_unique<Wt::WTextArea>());
    allergiesInput_->setPlaceholderText("Please list all allergies and reactions...");
    allergiesInput_->addStyleClass("form-control");
    allergiesInput_->setRows(3);
    allergiesInput_->hide();

    // Medications
    hasMedicationsCheckbox_ = conditionsSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I take regular medications"));
    hasMedicationsCheckbox_->addStyleClass("form-check");
    hasMedicationsCheckbox_->changed().connect(this, &MedicalInfoForm::toggleMedications);

    auto medsContainer = conditionsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    medsContainer->addStyleClass("form-group conditional-field");
    medicationsInput_ = medsContainer->addWidget(std::make_unique<Wt::WTextArea>());
    medicationsInput_->setPlaceholderText("Please list medications and dosages...");
    medicationsInput_->addStyleClass("form-control");
    medicationsInput_->setRows(3);
    medicationsInput_->hide();

    // Immunizations
    auto immunSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    immunSection->addStyleClass("form-section");
    auto immunHeader = immunSection->addWidget(std::make_unique<Wt::WText>("<h4>Immunizations</h4>"));
    immunHeader->setTextFormat(Wt::TextFormat::XHTML);

    immunizationsUpToDateCheckbox_ = immunSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " My immunizations are up to date"));
    immunizationsUpToDateCheckbox_->addStyleClass("form-check");

    auto immunNotesGroup = immunSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    immunNotesGroup->addStyleClass("form-group");
    immunNotesGroup->addWidget(std::make_unique<Wt::WLabel>("Immunization Notes"));
    immunizationNotesInput_ = immunNotesGroup->addWidget(std::make_unique<Wt::WTextArea>());
    immunizationNotesInput_->setPlaceholderText("Any notes about immunization status...");
    immunizationNotesInput_->addStyleClass("form-control");
    immunizationNotesInput_->setRows(2);

    // Accommodations
    auto accommodationsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    accommodationsSection->addStyleClass("form-section");
    auto accommodationsHeader = accommodationsSection->addWidget(std::make_unique<Wt::WText>("<h4>Disability & Accommodations</h4>"));
    accommodationsHeader->setTextFormat(Wt::TextFormat::XHTML);

    needsAccommodationsCheckbox_ = accommodationsSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have a disability or need accommodations"));
    needsAccommodationsCheckbox_->addStyleClass("form-check");
    needsAccommodationsCheckbox_->changed().connect(this, &MedicalInfoForm::toggleAccommodations);

    auto accommContainer = accommodationsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    accommContainer->addStyleClass("form-group conditional-field");
    accommodationsInput_ = accommContainer->addWidget(std::make_unique<Wt::WTextArea>());
    accommodationsInput_->setPlaceholderText("Please describe accommodations needed...");
    accommodationsInput_->addStyleClass("form-control");
    accommodationsInput_->setRows(3);
    accommodationsInput_->hide();

    // Dietary restrictions
    auto dietarySection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    dietarySection->addStyleClass("form-section");
    auto dietaryHeader = dietarySection->addWidget(std::make_unique<Wt::WText>("<h4>Dietary Information</h4>"));
    dietaryHeader->setTextFormat(Wt::TextFormat::XHTML);

    hasDietaryRestrictionsCheckbox_ = dietarySection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have dietary restrictions or preferences"));
    hasDietaryRestrictionsCheckbox_->addStyleClass("form-check");
    hasDietaryRestrictionsCheckbox_->changed().connect(this, &MedicalInfoForm::toggleDietary);

    auto dietaryContainer = dietarySection->addWidget(std::make_unique<Wt::WContainerWidget>());
    dietaryContainer->addStyleClass("form-group conditional-field");
    dietaryRestrictionsInput_ = dietaryContainer->addWidget(std::make_unique<Wt::WTextArea>());
    dietaryRestrictionsInput_->setPlaceholderText("Please describe dietary restrictions...");
    dietaryRestrictionsInput_->addStyleClass("form-control");
    dietaryRestrictionsInput_->setRows(2);
    dietaryRestrictionsInput_->hide();
}

void MedicalInfoForm::toggleInsurance() {
    insuranceContainer_->setHidden(!hasInsuranceCheckbox_->isChecked());
}

void MedicalInfoForm::toggleConditions() {
    conditionsInput_->setHidden(!hasConditionsCheckbox_->isChecked());
}

void MedicalInfoForm::toggleAllergies() {
    allergiesInput_->setHidden(!hasAllergiesCheckbox_->isChecked());
}

void MedicalInfoForm::toggleMedications() {
    medicationsInput_->setHidden(!hasMedicationsCheckbox_->isChecked());
}

void MedicalInfoForm::toggleAccommodations() {
    accommodationsInput_->setHidden(!needsAccommodationsCheckbox_->isChecked());
}

void MedicalInfoForm::toggleDietary() {
    dietaryRestrictionsInput_->setHidden(!hasDietaryRestrictionsCheckbox_->isChecked());
}

bool MedicalInfoForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    // Insurance details required if has insurance
    if (hasInsuranceCheckbox_->isChecked()) {
        validateRequired(insuranceProviderInput_->text().toUTF8(), "Insurance provider");
        validateRequired(policyNumberInput_->text().toUTF8(), "Policy number");
    }

    // Conditions required if checkbox checked
    if (hasConditionsCheckbox_->isChecked()) {
        validateRequired(conditionsInput_->text().toUTF8(), "Medical conditions description");
    }

    // Allergies required if checkbox checked
    if (hasAllergiesCheckbox_->isChecked()) {
        validateRequired(allergiesInput_->text().toUTF8(), "Allergies description");
    }

    return isValid_;
}

void MedicalInfoForm::collectFormData(Models::FormData& data) const {
    data.setField("hasInsurance", hasInsuranceCheckbox_->isChecked());
    data.setField("insuranceProvider", insuranceProviderInput_->text().toUTF8());
    data.setField("policyNumber", policyNumberInput_->text().toUTF8());
    data.setField("groupNumber", groupNumberInput_->text().toUTF8());
    data.setField("policyHolder", policyHolderInput_->text().toUTF8());

    data.setField("physicianName", physicianNameInput_->text().toUTF8());
    data.setField("physicianPhone", physicianPhoneInput_->text().toUTF8());
    data.setField("physicianAddress", physicianAddressInput_->text().toUTF8());

    data.setField("hasConditions", hasConditionsCheckbox_->isChecked());
    data.setField("conditions", conditionsInput_->text().toUTF8());

    data.setField("hasAllergies", hasAllergiesCheckbox_->isChecked());
    data.setField("allergies", allergiesInput_->text().toUTF8());

    data.setField("hasMedications", hasMedicationsCheckbox_->isChecked());
    data.setField("medications", medicationsInput_->text().toUTF8());

    data.setField("immunizationsUpToDate", immunizationsUpToDateCheckbox_->isChecked());
    data.setField("immunizationNotes", immunizationNotesInput_->text().toUTF8());

    data.setField("needsAccommodations", needsAccommodationsCheckbox_->isChecked());
    data.setField("accommodations", accommodationsInput_->text().toUTF8());

    data.setField("hasDietaryRestrictions", hasDietaryRestrictionsCheckbox_->isChecked());
    data.setField("dietaryRestrictions", dietaryRestrictionsInput_->text().toUTF8());
}

void MedicalInfoForm::populateFormFields(const Models::FormData& data) {
    if (data.hasField("hasInsurance")) {
        hasInsuranceCheckbox_->setChecked(data.getField("hasInsurance").boolValue);
        toggleInsurance();
    }
    if (data.hasField("insuranceProvider"))
        insuranceProviderInput_->setText(data.getField("insuranceProvider").stringValue);
    if (data.hasField("policyNumber"))
        policyNumberInput_->setText(data.getField("policyNumber").stringValue);
    if (data.hasField("groupNumber"))
        groupNumberInput_->setText(data.getField("groupNumber").stringValue);
    if (data.hasField("policyHolder"))
        policyHolderInput_->setText(data.getField("policyHolder").stringValue);

    if (data.hasField("physicianName"))
        physicianNameInput_->setText(data.getField("physicianName").stringValue);
    if (data.hasField("physicianPhone"))
        physicianPhoneInput_->setText(data.getField("physicianPhone").stringValue);
    if (data.hasField("physicianAddress"))
        physicianAddressInput_->setText(data.getField("physicianAddress").stringValue);

    if (data.hasField("hasConditions")) {
        hasConditionsCheckbox_->setChecked(data.getField("hasConditions").boolValue);
        toggleConditions();
    }
    if (data.hasField("conditions"))
        conditionsInput_->setText(data.getField("conditions").stringValue);

    if (data.hasField("hasAllergies")) {
        hasAllergiesCheckbox_->setChecked(data.getField("hasAllergies").boolValue);
        toggleAllergies();
    }
    if (data.hasField("allergies"))
        allergiesInput_->setText(data.getField("allergies").stringValue);

    if (data.hasField("hasMedications")) {
        hasMedicationsCheckbox_->setChecked(data.getField("hasMedications").boolValue);
        toggleMedications();
    }
    if (data.hasField("medications"))
        medicationsInput_->setText(data.getField("medications").stringValue);

    if (data.hasField("immunizationsUpToDate"))
        immunizationsUpToDateCheckbox_->setChecked(data.getField("immunizationsUpToDate").boolValue);
    if (data.hasField("immunizationNotes"))
        immunizationNotesInput_->setText(data.getField("immunizationNotes").stringValue);

    if (data.hasField("needsAccommodations")) {
        needsAccommodationsCheckbox_->setChecked(data.getField("needsAccommodations").boolValue);
        toggleAccommodations();
    }
    if (data.hasField("accommodations"))
        accommodationsInput_->setText(data.getField("accommodations").stringValue);

    if (data.hasField("hasDietaryRestrictions")) {
        hasDietaryRestrictionsCheckbox_->setChecked(data.getField("hasDietaryRestrictions").boolValue);
        toggleDietary();
    }
    if (data.hasField("dietaryRestrictions"))
        dietaryRestrictionsInput_->setText(data.getField("dietaryRestrictions").stringValue);
}

} // namespace Forms
} // namespace StudentIntake
