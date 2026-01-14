#include "FinancialAidForm.h"
#include <Wt/WLabel.h>

namespace StudentIntake {
namespace Forms {

FinancialAidForm::FinancialAidForm()
    : BaseForm("financial_aid", "Financial Aid Information") {
}

FinancialAidForm::~FinancialAidForm() {
}

void FinancialAidForm::createFormFields() {
    auto introSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    introSection->addStyleClass("form-section");
    auto introText = introSection->addWidget(std::make_unique<Wt::WText>(
        "<p>Complete this form if you are interested in financial aid, scholarships, or work-study programs.</p>"));
    introText->setTextFormat(Wt::TextFormat::XHTML);

    applyingForAidCheckbox_ = introSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I am interested in applying for financial aid"));
    applyingForAidCheckbox_->addStyleClass("form-check");
    applyingForAidCheckbox_->setChecked(true);

    // FAFSA Section
    auto fafsaSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fafsaSection->addStyleClass("form-section");
    auto fafsaHeader = fafsaSection->addWidget(std::make_unique<Wt::WText>("<h4>FAFSA Information</h4>"));
    fafsaHeader->setTextFormat(Wt::TextFormat::XHTML);

    fafsaCompletedCheckbox_ = fafsaSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have completed the FAFSA (Free Application for Federal Student Aid)"));
    fafsaCompletedCheckbox_->addStyleClass("form-check");

    auto fafsaIdGroup = fafsaSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    fafsaIdGroup->addStyleClass("form-group");
    fafsaIdGroup->addWidget(std::make_unique<Wt::WLabel>("FAFSA Student Aid Index (SAI) or Expected Family Contribution (EFC)"));
    fafsaIdInput_ = fafsaIdGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fafsaIdInput_->setPlaceholderText("Enter if known");
    fafsaIdInput_->addStyleClass("form-control");

    // Employment Section
    auto employmentSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    employmentSection->addStyleClass("form-section");
    auto empHeader = employmentSection->addWidget(std::make_unique<Wt::WText>("<h4>Employment Information</h4>"));
    empHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto empRow = employmentSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    empRow->addStyleClass("form-row");

    auto statusGroup = empRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusGroup->addStyleClass("form-group col-md-6");
    statusGroup->addWidget(std::make_unique<Wt::WLabel>("Employment Status"));
    employmentStatusSelect_ = statusGroup->addWidget(std::make_unique<Wt::WComboBox>());
    employmentStatusSelect_->addStyleClass("form-control");
    employmentStatusSelect_->addItem("Select...");
    employmentStatusSelect_->addItem("Not employed");
    employmentStatusSelect_->addItem("Part-time employed");
    employmentStatusSelect_->addItem("Full-time employed");
    employmentStatusSelect_->addItem("Self-employed");

    auto employerGroup = empRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    employerGroup->addStyleClass("form-group col-md-6");
    employerGroup->addWidget(std::make_unique<Wt::WLabel>("Employer Name (if applicable)"));
    employerInput_ = employerGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    employerInput_->addStyleClass("form-control");

    // Household Section
    auto householdSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    householdSection->addStyleClass("form-section");
    auto householdHeader = householdSection->addWidget(std::make_unique<Wt::WText>("<h4>Household Information</h4>"));
    householdHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto householdRow = householdSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    householdRow->addStyleClass("form-row");

    auto incomeGroup = householdRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    incomeGroup->addStyleClass("form-group col-md-6");
    incomeGroup->addWidget(std::make_unique<Wt::WLabel>("Household Income Range"));
    householdIncomeSelect_ = incomeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    householdIncomeSelect_->addStyleClass("form-control");
    householdIncomeSelect_->addItem("Select...");
    householdIncomeSelect_->addItem("Less than $25,000");
    householdIncomeSelect_->addItem("$25,000 - $50,000");
    householdIncomeSelect_->addItem("$50,000 - $75,000");
    householdIncomeSelect_->addItem("$75,000 - $100,000");
    householdIncomeSelect_->addItem("$100,000 - $150,000");
    householdIncomeSelect_->addItem("More than $150,000");
    householdIncomeSelect_->addItem("Prefer not to say");

    auto dependentsGroup = householdRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    dependentsGroup->addStyleClass("form-group col-md-6");
    dependentsGroup->addWidget(std::make_unique<Wt::WLabel>("Number of Dependents"));
    dependentsInput_ = dependentsGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dependentsInput_->setPlaceholderText("0");
    dependentsInput_->addStyleClass("form-control");

    veteranBenefitsCheckbox_ = householdSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I am eligible for veteran education benefits (GI Bill, etc.)"));
    veteranBenefitsCheckbox_->addStyleClass("form-check");

    // Aid Types Section
    auto aidSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    aidSection->addStyleClass("form-section");
    auto aidHeader = aidSection->addWidget(std::make_unique<Wt::WText>("<h4>Types of Aid Interested In</h4>"));
    aidHeader->setTextFormat(Wt::TextFormat::XHTML);

    scholarshipInterestCheckbox_ = aidSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " Scholarships and Grants"));
    scholarshipInterestCheckbox_->addStyleClass("form-check");

    auto scholarshipsGroup = aidSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    scholarshipsGroup->addStyleClass("form-group");
    scholarshipsGroup->addWidget(std::make_unique<Wt::WLabel>("Current Scholarships (if any)"));
    scholarshipsInput_ = scholarshipsGroup->addWidget(std::make_unique<Wt::WTextArea>());
    scholarshipsInput_->setPlaceholderText("List any scholarships you currently have or have applied for...");
    scholarshipsInput_->addStyleClass("form-control");
    scholarshipsInput_->setRows(2);

    workStudyInterestCheckbox_ = aidSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " Federal Work-Study Program"));
    workStudyInterestCheckbox_->addStyleClass("form-check");

    loanInterestCheckbox_ = aidSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " Student Loans"));
    loanInterestCheckbox_->addStyleClass("form-check");

    // Special Circumstances
    auto specialSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    specialSection->addStyleClass("form-section");
    auto specialHeader = specialSection->addWidget(std::make_unique<Wt::WText>("<h4>Special Circumstances</h4>"));
    specialHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto specialGroup = specialSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    specialGroup->addStyleClass("form-group");
    specialGroup->addWidget(std::make_unique<Wt::WLabel>(
        "Describe any special financial circumstances (job loss, medical expenses, etc.)"));
    specialCircumstancesInput_ = specialGroup->addWidget(std::make_unique<Wt::WTextArea>());
    specialCircumstancesInput_->setPlaceholderText("Optional - describe any circumstances that affect your ability to pay for education...");
    specialCircumstancesInput_->addStyleClass("form-control");
    specialCircumstancesInput_->setRows(3);
}

bool FinancialAidForm::validate() {
    validationErrors_.clear();
    isValid_ = true;
    // Financial aid form has no strictly required fields
    return isValid_;
}

void FinancialAidForm::collectFormData(Models::FormData& data) const {
    data.setField("applyingForAid", applyingForAidCheckbox_->isChecked());
    data.setField("fafsaCompleted", fafsaCompletedCheckbox_->isChecked());
    data.setField("fafsaId", fafsaIdInput_->text().toUTF8());
    data.setField("employmentStatus", employmentStatusSelect_->currentText().toUTF8());
    data.setField("employer", employerInput_->text().toUTF8());
    data.setField("householdIncome", householdIncomeSelect_->currentText().toUTF8());
    data.setField("dependents", dependentsInput_->text().toUTF8());
    data.setField("veteranBenefits", veteranBenefitsCheckbox_->isChecked());
    data.setField("scholarshipInterest", scholarshipInterestCheckbox_->isChecked());
    data.setField("currentScholarships", scholarshipsInput_->text().toUTF8());
    data.setField("workStudyInterest", workStudyInterestCheckbox_->isChecked());
    data.setField("loanInterest", loanInterestCheckbox_->isChecked());
    data.setField("specialCircumstances", specialCircumstancesInput_->text().toUTF8());
}

void FinancialAidForm::populateFormFields(const Models::FormData& data) {
    if (data.hasField("applyingForAid"))
        applyingForAidCheckbox_->setChecked(data.getField("applyingForAid").boolValue);
    if (data.hasField("fafsaCompleted"))
        fafsaCompletedCheckbox_->setChecked(data.getField("fafsaCompleted").boolValue);
    if (data.hasField("fafsaId"))
        fafsaIdInput_->setText(data.getField("fafsaId").stringValue);
    if (data.hasField("employer"))
        employerInput_->setText(data.getField("employer").stringValue);
    if (data.hasField("dependents"))
        dependentsInput_->setText(data.getField("dependents").stringValue);
    if (data.hasField("veteranBenefits"))
        veteranBenefitsCheckbox_->setChecked(data.getField("veteranBenefits").boolValue);
    if (data.hasField("scholarshipInterest"))
        scholarshipInterestCheckbox_->setChecked(data.getField("scholarshipInterest").boolValue);
    if (data.hasField("currentScholarships"))
        scholarshipsInput_->setText(data.getField("currentScholarships").stringValue);
    if (data.hasField("workStudyInterest"))
        workStudyInterestCheckbox_->setChecked(data.getField("workStudyInterest").boolValue);
    if (data.hasField("loanInterest"))
        loanInterestCheckbox_->setChecked(data.getField("loanInterest").boolValue);
    if (data.hasField("specialCircumstances"))
        specialCircumstancesInput_->setText(data.getField("specialCircumstances").stringValue);
}

} // namespace Forms
} // namespace StudentIntake
