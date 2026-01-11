#ifndef MEDICAL_INFO_FORM_H
#define MEDICAL_INFO_FORM_H

#include "BaseForm.h"
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WComboBox.h>
#include <Wt/WCheckBox.h>

namespace StudentIntake {
namespace Forms {

class MedicalInfoForm : public BaseForm {
public:
    MedicalInfoForm();
    ~MedicalInfoForm() override;

    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;

private:
    // Insurance information
    Wt::WCheckBox* hasInsuranceCheckbox_;
    Wt::WContainerWidget* insuranceContainer_;
    Wt::WLineEdit* insuranceProviderInput_;
    Wt::WLineEdit* policyNumberInput_;
    Wt::WLineEdit* groupNumberInput_;
    Wt::WLineEdit* policyHolderInput_;

    // Primary care physician
    Wt::WLineEdit* physicianNameInput_;
    Wt::WLineEdit* physicianPhoneInput_;
    Wt::WLineEdit* physicianAddressInput_;

    // Medical conditions
    Wt::WCheckBox* hasConditionsCheckbox_;
    Wt::WTextArea* conditionsInput_;

    // Allergies
    Wt::WCheckBox* hasAllergiesCheckbox_;
    Wt::WTextArea* allergiesInput_;

    // Medications
    Wt::WCheckBox* hasMedicationsCheckbox_;
    Wt::WTextArea* medicationsInput_;

    // Immunizations
    Wt::WCheckBox* immunizationsUpToDateCheckbox_;
    Wt::WTextArea* immunizationNotesInput_;

    // Disabilities/Accommodations
    Wt::WCheckBox* needsAccommodationsCheckbox_;
    Wt::WTextArea* accommodationsInput_;

    // Dietary restrictions
    Wt::WCheckBox* hasDietaryRestrictionsCheckbox_;
    Wt::WTextArea* dietaryRestrictionsInput_;

    void toggleInsurance();
    void toggleConditions();
    void toggleAllergies();
    void toggleMedications();
    void toggleAccommodations();
    void toggleDietary();
};

} // namespace Forms
} // namespace StudentIntake

#endif // MEDICAL_INFO_FORM_H
