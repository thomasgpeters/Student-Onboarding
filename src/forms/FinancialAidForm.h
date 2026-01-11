#ifndef FINANCIAL_AID_FORM_H
#define FINANCIAL_AID_FORM_H

#include "BaseForm.h"
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>

namespace StudentIntake {
namespace Forms {

class FinancialAidForm : public BaseForm {
public:
    FinancialAidForm();
    ~FinancialAidForm() override;
    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;

private:
    Wt::WCheckBox* applyingForAidCheckbox_;
    Wt::WCheckBox* fafsaCompletedCheckbox_;
    Wt::WLineEdit* fafsaIdInput_;
    Wt::WComboBox* employmentStatusSelect_;
    Wt::WLineEdit* employerInput_;
    Wt::WComboBox* householdIncomeSelect_;
    Wt::WLineEdit* dependentsInput_;
    Wt::WCheckBox* veteranBenefitsCheckbox_;
    Wt::WCheckBox* scholarshipInterestCheckbox_;
    Wt::WTextArea* scholarshipsInput_;
    Wt::WCheckBox* workStudyInterestCheckbox_;
    Wt::WCheckBox* loanInterestCheckbox_;
    Wt::WTextArea* specialCircumstancesInput_;
};

} // namespace Forms
} // namespace StudentIntake

#endif // FINANCIAL_AID_FORM_H
