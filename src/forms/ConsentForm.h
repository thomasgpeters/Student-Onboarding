#ifndef CONSENT_FORM_H
#define CONSENT_FORM_H

#include "BaseForm.h"
#include <Wt/WCheckBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WDateEdit.h>

namespace StudentIntake {
namespace Forms {

class ConsentForm : public BaseForm {
public:
    ConsentForm();
    ~ConsentForm() override;
    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;

private:
    // Consent checkboxes
    Wt::WCheckBox* termsCheckbox_;
    Wt::WCheckBox* privacyCheckbox_;
    Wt::WCheckBox* ferpaCheckbox_;
    Wt::WCheckBox* codeOfConductCheckbox_;
    Wt::WCheckBox* communicationCheckbox_;
    Wt::WCheckBox* photoReleaseCheckbox_;
    Wt::WCheckBox* accuracyCheckbox_;

    // Signature
    Wt::WLineEdit* signatureInput_;
    Wt::WDateEdit* signatureDateInput_;
};

} // namespace Forms
} // namespace StudentIntake

#endif // CONSENT_FORM_H
