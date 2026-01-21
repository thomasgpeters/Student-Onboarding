#include "AcademicHistoryForm.h"
#include "api/FormSubmissionService.h"
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Forms {

AcademicHistoryForm::AcademicHistoryForm()
    : BaseForm("academic_history", "Academic History") {
}

AcademicHistoryForm::~AcademicHistoryForm() {
}

void AcademicHistoryForm::createFormFields() {
    // Previous Education Section (checkbox toggles visibility)
    auto eduSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    eduSection->addStyleClass("form-section");
    auto eduHeader = eduSection->addWidget(std::make_unique<Wt::WText>("<h4>Previous Education</h4>"));
    eduHeader->setTextFormat(Wt::TextFormat::XHTML);

    hasPreviousEducationCheckbox_ = eduSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have previous education to report"));
    hasPreviousEducationCheckbox_->addStyleClass("form-check");
    hasPreviousEducationCheckbox_->changed().connect(this, &AcademicHistoryForm::togglePreviousEducation);

    // Container for academic history list (hidden by default)
    academicHistoryContainer_ = eduSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    academicHistoryContainer_->addStyleClass("academic-history-list mt-3");
    academicHistoryContainer_->hide();

    // Header with Add button
    auto listHeader = academicHistoryContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    listHeader->addStyleClass("d-flex justify-content-between align-items-center mb-3");

    auto listTitle = listHeader->addWidget(std::make_unique<Wt::WText>("<strong>Education History</strong>"));
    listTitle->setTextFormat(Wt::TextFormat::XHTML);

    addEducationBtn_ = listHeader->addWidget(std::make_unique<Wt::WPushButton>("+ Add Education"));
    addEducationBtn_->addStyleClass("btn btn-primary btn-sm");
    addEducationBtn_->clicked().connect(this, &AcademicHistoryForm::showAddEducationDialog);

    // Table for displaying academic history entries
    academicHistoryTable_ = academicHistoryContainer_->addWidget(std::make_unique<Wt::WTable>());
    academicHistoryTable_->addStyleClass("table table-striped table-hover");
    academicHistoryTable_->setHeaderCount(1);

    // No education text (shown when list is empty)
    noEducationText_ = academicHistoryContainer_->addWidget(
        std::make_unique<Wt::WText>("No education history added. Click '+ Add Education' to add your educational background."));
    noEducationText_->addStyleClass("text-muted");

    // Test Scores Section
    auto testSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    testSection->addStyleClass("form-section");
    auto testHeader = testSection->addWidget(std::make_unique<Wt::WText>("<h4>Standardized Test Scores</h4>"));
    testHeader->setTextFormat(Wt::TextFormat::XHTML);

    hasTestScoresCheckbox_ = testSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I have standardized test scores to report"));
    hasTestScoresCheckbox_->addStyleClass("form-check");
    hasTestScoresCheckbox_->changed().connect(this, &AcademicHistoryForm::toggleTestScores);

    testScoresContainer_ = testSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    testScoresContainer_->addStyleClass("test-scores-fields mt-3");
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
    auto honorsHeader = honorsSection->addWidget(std::make_unique<Wt::WText>("<h4>Honors &amp; Awards</h4>"));
    honorsHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto honorsGroup = honorsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    honorsGroup->addStyleClass("form-group");
    honorsAwardsInput_ = honorsGroup->addWidget(std::make_unique<Wt::WTextArea>());
    honorsAwardsInput_->setPlaceholderText("List any academic honors, awards, or achievements...");
    honorsAwardsInput_->addStyleClass("form-control");
    honorsAwardsInput_->setRows(3);

    // Academic Interests
    auto interestsSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    interestsSection->addStyleClass("form-section");
    auto interestsHeader = interestsSection->addWidget(std::make_unique<Wt::WText>("<h4>Academic Interests</h4>"));
    interestsHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto interestsGroup = interestsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    interestsGroup->addStyleClass("form-group");
    academicInterestsInput_ = interestsGroup->addWidget(std::make_unique<Wt::WTextArea>());
    academicInterestsInput_->setPlaceholderText("Describe your academic interests and goals...");
    academicInterestsInput_->addStyleClass("form-control");
    academicInterestsInput_->setRows(3);

    // Load existing data from API
    loadHistoriesFromApi();
}

void AcademicHistoryForm::togglePreviousEducation() {
    academicHistoryContainer_->setHidden(!hasPreviousEducationCheckbox_->isChecked());
}

void AcademicHistoryForm::toggleTestScores() {
    testScoresContainer_->setHidden(!hasTestScoresCheckbox_->isChecked());
}

void AcademicHistoryForm::updateAcademicHistoryTable() {
    // Clear existing table content
    academicHistoryTable_->clear();

    if (academicHistories_.empty()) {
        academicHistoryTable_->hide();
        noEducationText_->show();
        return;
    }

    noEducationText_->hide();
    academicHistoryTable_->show();

    // Add header row
    academicHistoryTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>(""));  // Icon column
    academicHistoryTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Type"));
    academicHistoryTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Institution"));
    academicHistoryTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Location"));
    academicHistoryTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Degree/Major"));
    academicHistoryTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("GPA"));
    academicHistoryTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));

    // Add data rows
    int row = 1;
    for (size_t i = 0; i < academicHistories_.size(); ++i) {
        const auto& history = academicHistories_[i];

        // Icon
        auto iconText = academicHistoryTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>("🎓"));
        iconText->addStyleClass("education-icon");

        // Type
        academicHistoryTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>(getInstitutionTypeLabel(history.getInstitutionType())));

        // Institution name
        academicHistoryTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(history.getInstitutionName()));

        // Location
        std::string location = history.getInstitutionCity();
        if (!history.getInstitutionState().empty()) {
            if (!location.empty()) location += ", ";
            location += history.getInstitutionState();
        }
        academicHistoryTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(location));

        // Degree/Major
        std::string degreeMajor = history.getDegreeEarned();
        if (!history.getMajor().empty()) {
            if (!degreeMajor.empty()) degreeMajor += " - ";
            degreeMajor += history.getMajor();
        }
        academicHistoryTable_->elementAt(row, 4)->addWidget(
            std::make_unique<Wt::WText>(degreeMajor));

        // GPA
        std::string gpaStr = "-";
        if (history.getGpa() > 0) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << history.getGpa();
            gpaStr = oss.str();
        }
        academicHistoryTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WText>(gpaStr));

        // Actions (Edit/Delete buttons)
        auto actionsCell = academicHistoryTable_->elementAt(row, 6);
        actionsCell->addStyleClass("actions-cell");

        auto editBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
        editBtn->addStyleClass("btn btn-sm btn-outline-primary mr-1");
        int index = static_cast<int>(i);
        editBtn->clicked().connect([this, index]() {
            showEditEducationDialog(index);
        });

        auto deleteBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
        deleteBtn->addStyleClass("btn btn-sm btn-outline-danger");
        deleteBtn->clicked().connect([this, index]() {
            deleteEducation(index);
        });

        ++row;
    }
}

void AcademicHistoryForm::showAddEducationDialog() {
    showEditEducationDialog(-1);  // -1 indicates new entry
}

void AcademicHistoryForm::showEditEducationDialog(int index) {
    bool isEdit = (index >= 0 && index < static_cast<int>(academicHistories_.size()));
    std::string title = isEdit ? "🎓 Edit Education" : "🎓 Add Education";

    auto dialog = this->addChild(std::make_unique<Wt::WDialog>(title));
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->addStyleClass("education-dialog");
    dialog->setWidth(Wt::WLength(600, Wt::LengthUnit::Pixel));

    auto content = dialog->contents();
    content->addStyleClass("p-3");

    // Institution Type (show human-friendly labels)
    auto typeGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    typeGroup->addStyleClass("form-group");
    typeGroup->addWidget(std::make_unique<Wt::WLabel>("Institution Type *"));
    auto typeSelect = typeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    typeSelect->addStyleClass("form-control");
    typeSelect->addItem("Select...");
    for (const auto& type : getInstitutionTypes()) {
        typeSelect->addItem(getInstitutionTypeLabel(type));
    }

    // Institution Name
    auto nameGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group");
    nameGroup->addWidget(std::make_unique<Wt::WLabel>("Institution Name *"));
    auto nameInput = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    nameInput->addStyleClass("form-control");
    nameInput->setPlaceholderText("e.g., Lincoln High School, State University");

    // Location Row
    auto locationRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    locationRow->addStyleClass("form-row");

    auto cityGroup = locationRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityGroup->addStyleClass("form-group col-md-6");
    cityGroup->addWidget(std::make_unique<Wt::WLabel>("City"));
    auto cityInput = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    cityInput->addStyleClass("form-control");

    auto stateGroup = locationRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    stateGroup->addStyleClass("form-group col-md-6");
    stateGroup->addWidget(std::make_unique<Wt::WLabel>("State"));
    auto stateSelect = stateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    stateSelect->addStyleClass("form-control");
    stateSelect->addItem("Select...");
    for (const auto& state : getUSStates()) {
        stateSelect->addItem(state);
    }

    // Degree Type (changes based on institution type)
    auto degreeGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    degreeGroup->addStyleClass("form-group");
    degreeGroup->addWidget(std::make_unique<Wt::WLabel>("Degree/Credential Earned"));
    auto degreeSelect = degreeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    degreeSelect->addStyleClass("form-control");
    degreeSelect->addItem("Select...");

    // Update degree options when institution type changes
    typeSelect->changed().connect([this, typeSelect, degreeSelect]() {
        std::string currentType = typeSelect->currentText().toUTF8();
        degreeSelect->clear();
        degreeSelect->addItem("Select...");
        for (const auto& degree : getDegreeTypesForInstitution(currentType)) {
            degreeSelect->addItem(degree);
        }
    });

    // Major/Field of Study
    auto majorGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    majorGroup->addStyleClass("form-group");
    majorGroup->addWidget(std::make_unique<Wt::WLabel>("Major/Field of Study"));
    auto majorInput = majorGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    majorInput->addStyleClass("form-control");

    // GPA
    auto gpaGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    gpaGroup->addStyleClass("form-group");
    gpaGroup->addWidget(std::make_unique<Wt::WLabel>("GPA (4.0 scale)"));
    auto gpaInput = gpaGroup->addWidget(std::make_unique<Wt::WDoubleSpinBox>());
    gpaInput->addStyleClass("form-control");
    gpaInput->setRange(0.0, 4.0);
    gpaInput->setSingleStep(0.1);
    gpaInput->setDecimals(2);
    gpaInput->setValue(0.0);

    // Dates Row
    auto datesRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    datesRow->addStyleClass("form-row");

    auto startGroup = datesRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    startGroup->addStyleClass("form-group col-md-6");
    startGroup->addWidget(std::make_unique<Wt::WLabel>("Start Date"));
    auto startDateInput = startGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    startDateInput->addStyleClass("form-control");

    auto endGroup = datesRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    endGroup->addStyleClass("form-group col-md-6");
    endGroup->addWidget(std::make_unique<Wt::WLabel>("End Date / Graduation Date"));
    auto endDateInput = endGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    endDateInput->addStyleClass("form-control");

    // Currently Attending
    auto currentlyAttendingCheckbox = content->addWidget(
        std::make_unique<Wt::WCheckBox>(" I am currently attending this institution"));
    currentlyAttendingCheckbox->addStyleClass("form-check mb-3");

    // Populate fields if editing
    if (isEdit) {
        const auto& history = academicHistories_[index];

        // Set institution type (convert stored value to label for display)
        std::string instType = history.getInstitutionType();
        std::string instTypeLabel = getInstitutionTypeLabel(instType);
        for (int i = 0; i < typeSelect->count(); ++i) {
            if (typeSelect->itemText(i).toUTF8() == instTypeLabel) {
                typeSelect->setCurrentIndex(i);
                break;
            }
        }

        // Update degree options for this institution type
        degreeSelect->clear();
        degreeSelect->addItem("Select...");
        for (const auto& degree : getDegreeTypesForInstitution(instType)) {
            degreeSelect->addItem(degree);
        }

        nameInput->setText(history.getInstitutionName());
        cityInput->setText(history.getInstitutionCity());

        // Set state
        std::string state = history.getInstitutionState();
        if (!state.empty()) {
            for (int i = 0; i < stateSelect->count(); ++i) {
                if (stateSelect->itemText(i).toUTF8() == state) {
                    stateSelect->setCurrentIndex(i);
                    break;
                }
            }
        }

        // Set degree
        std::string degree = history.getDegreeEarned();
        if (!degree.empty()) {
            for (int i = 0; i < degreeSelect->count(); ++i) {
                if (degreeSelect->itemText(i).toUTF8() == degree) {
                    degreeSelect->setCurrentIndex(i);
                    break;
                }
            }
        }

        majorInput->setText(history.getMajor());

        if (history.getGpa() > 0) {
            gpaInput->setValue(history.getGpa());
        }

        // Set dates
        if (!history.getStartDate().empty()) {
            Wt::WDate startDate = Wt::WDate::fromString(history.getStartDate(), "yyyy-MM-dd");
            if (startDate.isValid()) {
                startDateInput->setDate(startDate);
            }
        }

        std::string endDateStr = history.getEndDate();
        if (endDateStr.empty()) {
            endDateStr = history.getGraduationDate();
        }
        if (!endDateStr.empty()) {
            Wt::WDate endDate = Wt::WDate::fromString(endDateStr, "yyyy-MM-dd");
            if (endDate.isValid()) {
                endDateInput->setDate(endDate);
            }
        }

        currentlyAttendingCheckbox->setChecked(history.isCurrentlyAttending());
    }

    // Footer buttons
    auto footer = dialog->footer();
    footer->addStyleClass("p-2");

    auto cancelBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn->addStyleClass("btn btn-secondary mr-2");
    cancelBtn->clicked().connect(dialog, &Wt::WDialog::reject);

    auto saveBtn = footer->addWidget(std::make_unique<Wt::WPushButton>(isEdit ? "Update" : "Add"));
    saveBtn->addStyleClass("btn btn-primary");
    saveBtn->clicked().connect([this, dialog, typeSelect, nameInput, cityInput, stateSelect,
                                degreeSelect, majorInput, gpaInput, startDateInput, endDateInput,
                                currentlyAttendingCheckbox, index]() {
        saveEducationFromDialog(dialog, typeSelect, nameInput, cityInput, stateSelect,
                               degreeSelect, majorInput, gpaInput, startDateInput, endDateInput,
                               currentlyAttendingCheckbox, index);
    });

    dialog->finished().connect([dialog]() {
        dialog->removeFromParent();
    });

    dialog->show();
}

void AcademicHistoryForm::saveEducationFromDialog(Wt::WDialog* dialog,
                                                   Wt::WComboBox* typeSelect,
                                                   Wt::WLineEdit* nameInput,
                                                   Wt::WLineEdit* cityInput,
                                                   Wt::WComboBox* stateSelect,
                                                   Wt::WComboBox* degreeSelect,
                                                   Wt::WLineEdit* majorInput,
                                                   Wt::WDoubleSpinBox* gpaInput,
                                                   Wt::WDateEdit* startDateInput,
                                                   Wt::WDateEdit* endDateInput,
                                                   Wt::WCheckBox* currentlyAttendingCheckbox,
                                                   int editIndex) {
    // Validate required fields
    std::string instTypeLabel = typeSelect->currentText().toUTF8();
    std::string instName = nameInput->text().toUTF8();

    if (instTypeLabel == "Select..." || instTypeLabel.empty()) {
        // Show validation error
        return;
    }

    if (instName.empty()) {
        // Show validation error
        return;
    }

    // Convert label back to value for storage
    std::string instType = getInstitutionTypeValue(instTypeLabel);

    // Build the academic history record
    Models::AcademicHistory history;

    if (editIndex >= 0 && editIndex < static_cast<int>(academicHistories_.size())) {
        // Preserve existing record for updates
        history = academicHistories_[editIndex];
    }

    history.setInstitutionType(instType);
    history.setInstitutionName(instName);
    history.setInstitutionCity(cityInput->text().toUTF8());

    std::string state = stateSelect->currentText().toUTF8();
    if (state != "Select...") {
        history.setInstitutionState(state);
    } else {
        history.setInstitutionState("");
    }

    std::string degree = degreeSelect->currentText().toUTF8();
    if (degree != "Select...") {
        history.setDegreeEarned(degree);
    } else {
        history.setDegreeEarned("");
    }

    history.setMajor(majorInput->text().toUTF8());

    if (gpaInput->value() > 0) {
        history.setGpa(gpaInput->value());
    }
    history.setGpaScale(4.0);

    if (startDateInput->date().isValid()) {
        history.setStartDate(startDateInput->date().toString("yyyy-MM-dd").toUTF8());
    }

    if (endDateInput->date().isValid()) {
        std::string endDateStr = endDateInput->date().toString("yyyy-MM-dd").toUTF8();
        history.setEndDate(endDateStr);
        if (!currentlyAttendingCheckbox->isChecked()) {
            history.setGraduationDate(endDateStr);
        }
    }

    history.setCurrentlyAttending(currentlyAttendingCheckbox->isChecked());

    // Set student ID if available
    if (session_) {
        history.setStudentId(session_->getStudent().getId());
    }

    // Add or update the record
    if (editIndex >= 0 && editIndex < static_cast<int>(academicHistories_.size())) {
        academicHistories_[editIndex] = history;
    } else {
        academicHistories_.push_back(history);
    }

    // Update the table display
    updateAcademicHistoryTable();

    // Close the dialog
    dialog->accept();
}

void AcademicHistoryForm::deleteEducation(int index) {
    if (index >= 0 && index < static_cast<int>(academicHistories_.size())) {
        // If the record has an ID, we'll mark it for deletion on save
        // For now, just remove from local list
        academicHistories_.erase(academicHistories_.begin() + index);
        updateAcademicHistoryTable();
    }
}

bool AcademicHistoryForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    // If checkbox is checked, at least one education entry is required
    if (hasPreviousEducationCheckbox_->isChecked() && academicHistories_.empty()) {
        validationErrors_.push_back("Please add at least one education history entry");
        isValid_ = false;
    }

    return isValid_;
}

void AcademicHistoryForm::collectFormData(Models::FormData& data) const {
    data.setField("hasPreviousEducation", hasPreviousEducationCheckbox_->isChecked());

    // Store count of academic histories
    data.setField("academicHistoryCount", static_cast<int>(academicHistories_.size()));

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
    if (data.hasField("hasPreviousEducation")) {
        hasPreviousEducationCheckbox_->setChecked(data.getField("hasPreviousEducation").boolValue);
        togglePreviousEducation();
    }

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
    if (data.hasField("gmatScore"))
        gmatScoreInput_->setText(data.getField("gmatScore").stringValue);
    if (data.hasField("toeflScore"))
        toeflScoreInput_->setText(data.getField("toeflScore").stringValue);

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

std::vector<std::string> AcademicHistoryForm::getInstitutionTypes() const {
    return {
        "high_school",
        "undergraduate",
        "graduate",
        "vocational_certificate"
    };
}

std::string AcademicHistoryForm::getInstitutionTypeLabel(const std::string& type) const {
    if (type == "high_school") return "High School";
    if (type == "undergraduate") return "Undergraduate";
    if (type == "graduate") return "Graduate";
    if (type == "vocational_certificate") return "Vocational/Certificate";
    // Legacy types
    if (type == "High School") return "High School";
    if (type == "College" || type == "University") return "Undergraduate";
    return type;
}

std::string AcademicHistoryForm::getInstitutionTypeValue(const std::string& label) const {
    if (label == "High School") return "high_school";
    if (label == "Undergraduate") return "undergraduate";
    if (label == "Graduate") return "graduate";
    if (label == "Vocational/Certificate") return "vocational_certificate";
    // Return as-is if already a value or unknown
    return label;
}

std::vector<std::string> AcademicHistoryForm::getDegreeTypesForInstitution(const std::string& institutionType) const {
    // Handle both raw values and labels
    std::string type = institutionType;
    if (type == "High School") type = "high_school";
    else if (type == "Undergraduate") type = "undergraduate";
    else if (type == "Graduate") type = "graduate";
    else if (type == "Vocational/Certificate") type = "vocational_certificate";

    if (type == "high_school") {
        return {"High School Diploma", "GED"};
    }
    if (type == "undergraduate") {
        return {"Associate's", "Bachelor's", "In Progress"};
    }
    if (type == "graduate") {
        return {"Master's", "Doctoral/PhD", "Professional (JD, MD, etc.)", "In Progress"};
    }
    if (type == "vocational_certificate") {
        return {"Certificate", "Diploma", "License", "In Progress"};
    }
    // Default
    return {"Certificate", "Diploma", "Associate's", "Bachelor's", "Master's", "Doctoral"};
}

void AcademicHistoryForm::handleSubmit() {
    if (!validate()) {
        // Show validation errors
        return;
    }

    // Save academic histories to API (the new list-based approach)
    saveHistoriesToApi();

    // Mark form as completed and navigate to next
    // (Don't call BaseForm::handleSubmit as it would try to use the old FormData submission)
    if (session_) {
        // Save form data to session
        Models::FormData data = getFormData();
        data.setStatus("submitted");
        session_->setFormData(formId_, data);

        // Mark form as completed
        session_->getStudent().markFormCompleted(formId_);

        // Update student profile to persist completion status
        if (apiService_) {
            apiService_->updateStudentProfile(session_->getStudent());
        }
    }

    // Emit success signal to move to next form
    formSubmitted_.emit();
}

void AcademicHistoryForm::loadHistoriesFromApi() {
    if (!apiService_ || !session_) {
        return;
    }

    std::string studentId = session_->getStudent().getId();
    if (studentId.empty()) {
        return;
    }

    std::cout << "[AcademicHistoryForm] Loading academic histories for student: " << studentId << std::endl;

    auto histories = apiService_->getAcademicHistories(studentId);
    std::cout << "[AcademicHistoryForm] Found " << histories.size() << " history records" << std::endl;

    academicHistories_.clear();
    for (const auto& history : histories) {
        std::cout << "[AcademicHistoryForm] Record type: " << history.getInstitutionType()
                  << ", name: " << history.getInstitutionName() << std::endl;
        academicHistories_.push_back(history);
    }

    // If there are histories, show the section
    if (!academicHistories_.empty()) {
        hasPreviousEducationCheckbox_->setChecked(true);
        togglePreviousEducation();
        updateAcademicHistoryTable();
    }
}

void AcademicHistoryForm::saveHistoriesToApi() {
    if (!apiService_ || !session_) {
        return;
    }

    std::string studentId = session_->getStudent().getId();
    if (studentId.empty()) {
        return;
    }

    std::cout << "[AcademicHistoryForm] Saving " << academicHistories_.size()
              << " academic histories for student: " << studentId << std::endl;

    for (auto& history : academicHistories_) {
        history.setStudentId(studentId);

        auto result = apiService_->saveAcademicHistory(history);
        if (result.success) {
            std::cout << "[AcademicHistoryForm] Saved record: " << history.getInstitutionName()
                      << " (" << history.getInstitutionType() << ")" << std::endl;
        } else {
            std::cout << "[AcademicHistoryForm] Failed to save record: " << result.message << std::endl;
        }
    }
}

} // namespace Forms
} // namespace StudentIntake
