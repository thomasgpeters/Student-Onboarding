#include "AcademicHistoryForm.h"
#include <Wt/WLabel.h>

namespace StudentIntake {
namespace Forms {

AcademicHistoryForm::AcademicHistoryForm()
    : BaseForm("academic_history", "Academic History") {
}

AcademicHistoryForm::~AcademicHistoryForm() {
}

void AcademicHistoryForm::createFormFields() {
    // High School Section
    auto hsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsSection->addStyleClass("form-section");
    hsSection->addWidget(std::make_unique<Wt::WText>("<h4>High School Education</h4>"));

    auto hsRow1 = hsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsRow1->addStyleClass("form-row");

    auto hsNameGroup = hsRow1->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsNameGroup->addStyleClass("form-group col-md-6");
    hsNameGroup->addWidget(std::make_unique<Wt::WLabel>("High School Name *"));
    highSchoolNameInput_ = hsNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    highSchoolNameInput_->addStyleClass("form-control");

    auto hsCityGroup = hsRow1->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsCityGroup->addStyleClass("form-group col-md-3");
    hsCityGroup->addWidget(std::make_unique<Wt::WLabel>("City"));
    highSchoolCityInput_ = hsCityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    highSchoolCityInput_->addStyleClass("form-control");

    auto hsStateGroup = hsRow1->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsStateGroup->addStyleClass("form-group col-md-3");
    hsStateGroup->addWidget(std::make_unique<Wt::WLabel>("State"));
    highSchoolStateSelect_ = hsStateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    highSchoolStateSelect_->addStyleClass("form-control");
    highSchoolStateSelect_->addItem("Select...");
    for (const auto& state : getUSStates()) {
        highSchoolStateSelect_->addItem(state);
    }

    auto hsRow2 = hsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsRow2->addStyleClass("form-row");

    auto hsGradGroup = hsRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsGradGroup->addStyleClass("form-group col-md-6");
    hsGradGroup->addWidget(std::make_unique<Wt::WLabel>("Graduation Date *"));
    highSchoolGradDateInput_ = hsGradGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    highSchoolGradDateInput_->addStyleClass("form-control");

    auto hsGpaGroup = hsRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    hsGpaGroup->addStyleClass("form-group col-md-6");
    hsGpaGroup->addWidget(std::make_unique<Wt::WLabel>("GPA (4.0 scale)"));
    highSchoolGpaInput_ = hsGpaGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    highSchoolGpaInput_->setPlaceholderText("e.g., 3.5");
    highSchoolGpaInput_->addStyleClass("form-control");

    gedCheckbox_ = hsSection->addWidget(std::make_unique<Wt::WCheckBox>(" I have a GED instead of high school diploma"));
    gedCheckbox_->addStyleClass("form-check");

    // Previous College Section
    auto collegeSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    collegeSection->addStyleClass("form-section");
    collegeSection->addWidget(std::make_unique<Wt::WText>("<h4>Previous College Education</h4>"));

    hasPreviousCollegeCheckbox_ = collegeSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have attended college previously"));
    hasPreviousCollegeCheckbox_->addStyleClass("form-check");
    hasPreviousCollegeCheckbox_->changed().connect(this, &AcademicHistoryForm::togglePreviousCollege);

    previousCollegeContainer_ = collegeSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    previousCollegeContainer_->addStyleClass("previous-college-fields");
    previousCollegeContainer_->hide();

    auto colRow1 = previousCollegeContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    colRow1->addStyleClass("form-row");

    auto colNameGroup = colRow1->addWidget(std::make_unique<Wt::WContainerWidget>());
    colNameGroup->addStyleClass("form-group col-md-6");
    colNameGroup->addWidget(std::make_unique<Wt::WLabel>("College/University Name"));
    collegeNameInput_ = colNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    collegeNameInput_->addStyleClass("form-control");

    auto colMajorGroup = colRow1->addWidget(std::make_unique<Wt::WContainerWidget>());
    colMajorGroup->addStyleClass("form-group col-md-6");
    colMajorGroup->addWidget(std::make_unique<Wt::WLabel>("Major/Field of Study"));
    collegeMajorInput_ = colMajorGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    collegeMajorInput_->addStyleClass("form-control");

    auto colRow2 = previousCollegeContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    colRow2->addStyleClass("form-row");

    auto colCityGroup = colRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    colCityGroup->addStyleClass("form-group col-md-4");
    colCityGroup->addWidget(std::make_unique<Wt::WLabel>("City"));
    collegeCityInput_ = colCityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    collegeCityInput_->addStyleClass("form-control");

    auto colStateGroup = colRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    colStateGroup->addStyleClass("form-group col-md-4");
    colStateGroup->addWidget(std::make_unique<Wt::WLabel>("State"));
    collegeStateSelect_ = colStateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    collegeStateSelect_->addStyleClass("form-control");
    collegeStateSelect_->addItem("Select...");
    for (const auto& state : getUSStates()) {
        collegeStateSelect_->addItem(state);
    }

    auto colGpaGroup = colRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    colGpaGroup->addStyleClass("form-group col-md-4");
    colGpaGroup->addWidget(std::make_unique<Wt::WLabel>("GPA"));
    collegeGpaInput_ = colGpaGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    collegeGpaInput_->addStyleClass("form-control");

    auto colRow3 = previousCollegeContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    colRow3->addStyleClass("form-row");

    auto startGroup = colRow3->addWidget(std::make_unique<Wt::WContainerWidget>());
    startGroup->addStyleClass("form-group col-md-4");
    startGroup->addWidget(std::make_unique<Wt::WLabel>("Start Date"));
    collegeStartDateInput_ = startGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    collegeStartDateInput_->addStyleClass("form-control");

    auto endGroup = colRow3->addWidget(std::make_unique<Wt::WContainerWidget>());
    endGroup->addStyleClass("form-group col-md-4");
    endGroup->addWidget(std::make_unique<Wt::WLabel>("End Date"));
    collegeEndDateInput_ = endGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    collegeEndDateInput_->addStyleClass("form-control");

    auto degreeGroup = colRow3->addWidget(std::make_unique<Wt::WContainerWidget>());
    degreeGroup->addStyleClass("form-group col-md-4");
    degreeGroup->addWidget(std::make_unique<Wt::WLabel>("Degree Type"));
    collegeDegreeSelect_ = degreeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    collegeDegreeSelect_->addStyleClass("form-control");
    collegeDegreeSelect_->addItem("Select...");
    collegeDegreeSelect_->addItem("Associate's");
    collegeDegreeSelect_->addItem("Bachelor's");
    collegeDegreeSelect_->addItem("Master's");
    collegeDegreeSelect_->addItem("Doctoral");
    collegeDegreeSelect_->addItem("Certificate");

    degreeCompletedCheckbox_ = previousCollegeContainer_->addWidget(
        std::make_unique<Wt::WCheckBox>(" Degree completed"));
    degreeCompletedCheckbox_->addStyleClass("form-check");

    // Transfer Credits
    hasTransferCreditsCheckbox_ = previousCollegeContainer_->addWidget(
        std::make_unique<Wt::WCheckBox>(" I would like to transfer credits"));
    hasTransferCreditsCheckbox_->addStyleClass("form-check");

    auto transferGroup = previousCollegeContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    transferGroup->addStyleClass("form-group");
    transferGroup->addWidget(std::make_unique<Wt::WLabel>("Transfer Credits Details"));
    transferCreditsInput_ = transferGroup->addWidget(std::make_unique<Wt::WTextArea>());
    transferCreditsInput_->setPlaceholderText("List courses you'd like to transfer...");
    transferCreditsInput_->addStyleClass("form-control");
    transferCreditsInput_->setRows(3);

    // Test Scores
    auto testSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    testSection->addStyleClass("form-section");
    testSection->addWidget(std::make_unique<Wt::WText>("<h4>Standardized Test Scores</h4>"));

    hasTestScoresCheckbox_ = testSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have standardized test scores to report"));
    hasTestScoresCheckbox_->addStyleClass("form-check");
    hasTestScoresCheckbox_->changed().connect(this, &AcademicHistoryForm::toggleTestScores);

    testScoresContainer_ = testSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    testScoresContainer_->addStyleClass("test-scores-fields");
    testScoresContainer_->hide();

    auto testRow = testScoresContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    testRow->addStyleClass("form-row");

    auto satGroup = testRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    satGroup->addStyleClass("form-group col-md-4");
    satGroup->addWidget(std::make_unique<Wt::WLabel>("SAT Score"));
    satScoreInput_ = satGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    satScoreInput_->addStyleClass("form-control");

    auto actGroup = testRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    actGroup->addStyleClass("form-group col-md-4");
    actGroup->addWidget(std::make_unique<Wt::WLabel>("ACT Score"));
    actScoreInput_ = actGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    actScoreInput_->addStyleClass("form-control");

    auto greGroup = testRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    greGroup->addStyleClass("form-group col-md-4");
    greGroup->addWidget(std::make_unique<Wt::WLabel>("GRE Score"));
    greScoreInput_ = greGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    greScoreInput_->addStyleClass("form-control");

    auto testRow2 = testScoresContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    testRow2->addStyleClass("form-row");

    auto gmatGroup = testRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    gmatGroup->addStyleClass("form-group col-md-6");
    gmatGroup->addWidget(std::make_unique<Wt::WLabel>("GMAT Score"));
    gmatScoreInput_ = gmatGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    gmatScoreInput_->addStyleClass("form-control");

    auto toeflGroup = testRow2->addWidget(std::make_unique<Wt::WContainerWidget>());
    toeflGroup->addStyleClass("form-group col-md-6");
    toeflGroup->addWidget(std::make_unique<Wt::WLabel>("TOEFL Score"));
    toeflScoreInput_ = toeflGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    toeflScoreInput_->addStyleClass("form-control");

    // Honors and Awards
    auto honorsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    honorsSection->addStyleClass("form-section");
    honorsSection->addWidget(std::make_unique<Wt::WText>("<h4>Honors & Awards</h4>"));

    auto honorsGroup = honorsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    honorsGroup->addStyleClass("form-group");
    honorsAwardsInput_ = honorsGroup->addWidget(std::make_unique<Wt::WTextArea>());
    honorsAwardsInput_->setPlaceholderText("List any academic honors, awards, or achievements...");
    honorsAwardsInput_->addStyleClass("form-control");
    honorsAwardsInput_->setRows(3);

    // Academic Interests
    auto interestsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    interestsSection->addStyleClass("form-section");
    interestsSection->addWidget(std::make_unique<Wt::WText>("<h4>Academic Interests</h4>"));

    auto interestsGroup = interestsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    interestsGroup->addStyleClass("form-group");
    academicInterestsInput_ = interestsGroup->addWidget(std::make_unique<Wt::WTextArea>());
    academicInterestsInput_->setPlaceholderText("Describe your academic interests and goals...");
    academicInterestsInput_->addStyleClass("form-control");
    academicInterestsInput_->setRows(3);
}

void AcademicHistoryForm::togglePreviousCollege() {
    previousCollegeContainer_->setHidden(!hasPreviousCollegeCheckbox_->isChecked());
}

void AcademicHistoryForm::toggleTestScores() {
    testScoresContainer_->setHidden(!hasTestScoresCheckbox_->isChecked());
}

bool AcademicHistoryForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    validateRequired(highSchoolNameInput_->text().toUTF8(), "High school name");

    if (!gedCheckbox_->isChecked() && !highSchoolGradDateInput_->date().isValid()) {
        validationErrors_.push_back("Please enter high school graduation date");
        isValid_ = false;
    }

    if (hasPreviousCollegeCheckbox_->isChecked()) {
        validateRequired(collegeNameInput_->text().toUTF8(), "College name");
    }

    return isValid_;
}

void AcademicHistoryForm::collectFormData(Models::FormData& data) const {
    data.setField("highSchoolName", highSchoolNameInput_->text().toUTF8());
    data.setField("highSchoolCity", highSchoolCityInput_->text().toUTF8());
    data.setField("highSchoolState", highSchoolStateSelect_->currentText().toUTF8());
    data.setField("highSchoolGradDate", highSchoolGradDateInput_->date().toString("yyyy-MM-dd").toUTF8());
    data.setField("highSchoolGpa", highSchoolGpaInput_->text().toUTF8());
    data.setField("hasGed", gedCheckbox_->isChecked());

    data.setField("hasPreviousCollege", hasPreviousCollegeCheckbox_->isChecked());
    data.setField("collegeName", collegeNameInput_->text().toUTF8());
    data.setField("collegeCity", collegeCityInput_->text().toUTF8());
    data.setField("collegeState", collegeStateSelect_->currentText().toUTF8());
    data.setField("collegeMajor", collegeMajorInput_->text().toUTF8());
    data.setField("collegeGpa", collegeGpaInput_->text().toUTF8());
    data.setField("collegeDegree", collegeDegreeSelect_->currentText().toUTF8());
    data.setField("degreeCompleted", degreeCompletedCheckbox_->isChecked());
    data.setField("hasTransferCredits", hasTransferCreditsCheckbox_->isChecked());
    data.setField("transferCredits", transferCreditsInput_->text().toUTF8());

    data.setField("hasTestScores", hasTestScoresCheckbox_->isChecked());
    data.setField("satScore", satScoreInput_->text().toUTF8());
    data.setField("actScore", actScoreInput_->text().toUTF8());
    data.setField("greScore", greScoreInput_->text().toUTF8());
    data.setField("gmatScore", gmatScoreInput_->text().toUTF8());
    data.setField("toeflScore", toeflScoreInput_->text().toUTF8());

    data.setField("honorsAwards", honorsAwardsInput_->text().toUTF8());
    data.setField("academicInterests", academicInterestsInput_->text().toUTF8());
}

void AcademicHistoryForm::populateFormFields(const Models::FormData& data) {
    if (data.hasField("highSchoolName"))
        highSchoolNameInput_->setText(data.getField("highSchoolName").stringValue);
    if (data.hasField("highSchoolCity"))
        highSchoolCityInput_->setText(data.getField("highSchoolCity").stringValue);
    if (data.hasField("highSchoolGpa"))
        highSchoolGpaInput_->setText(data.getField("highSchoolGpa").stringValue);
    if (data.hasField("hasGed"))
        gedCheckbox_->setChecked(data.getField("hasGed").boolValue);

    if (data.hasField("hasPreviousCollege")) {
        hasPreviousCollegeCheckbox_->setChecked(data.getField("hasPreviousCollege").boolValue);
        togglePreviousCollege();
    }

    if (data.hasField("collegeName"))
        collegeNameInput_->setText(data.getField("collegeName").stringValue);
    if (data.hasField("collegeCity"))
        collegeCityInput_->setText(data.getField("collegeCity").stringValue);
    if (data.hasField("collegeMajor"))
        collegeMajorInput_->setText(data.getField("collegeMajor").stringValue);
    if (data.hasField("collegeGpa"))
        collegeGpaInput_->setText(data.getField("collegeGpa").stringValue);

    if (data.hasField("hasTestScores")) {
        hasTestScoresCheckbox_->setChecked(data.getField("hasTestScores").boolValue);
        toggleTestScores();
    }

    if (data.hasField("satScore"))
        satScoreInput_->setText(data.getField("satScore").stringValue);
    if (data.hasField("actScore"))
        actScoreInput_->setText(data.getField("actScore").stringValue);
    if (data.hasField("greScore"))
        greScoreInput_->setText(data.getField("greScore").stringValue);

    if (data.hasField("honorsAwards"))
        honorsAwardsInput_->setText(data.getField("honorsAwards").stringValue);
    if (data.hasField("academicInterests"))
        academicInterestsInput_->setText(data.getField("academicInterests").stringValue);
}

std::vector<std::string> AcademicHistoryForm::getUSStates() const {
    return {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado",
        "Connecticut", "Delaware", "Florida", "Georgia", "Hawaii", "Idaho",
        "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky", "Louisiana",
        "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota",
        "Mississippi", "Missouri", "Montana", "Nebraska", "Nevada",
        "New Hampshire", "New Jersey", "New Mexico", "New York",
        "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon",
        "Pennsylvania", "Rhode Island", "South Carolina", "South Dakota",
        "Tennessee", "Texas", "Utah", "Vermont", "Virginia", "Washington",
        "West Virginia", "Wisconsin", "Wyoming", "District of Columbia",
        "International"
    };
}

} // namespace Forms
} // namespace StudentIntake
