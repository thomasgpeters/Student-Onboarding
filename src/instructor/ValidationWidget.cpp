#include "ValidationWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Instructor {

ValidationWidget::ValidationWidget()
    : instructorId_(0)
    , selectedStudentId_(0)
    , selectedEnrollmentId_(0)
    , validationDialog_(nullptr)
{
    setStyleClass("validation-widget");
    setupUI();
}

void ValidationWidget::setupUI() {
    // Header
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->setStyleClass("widget-header");

    backButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Back"));
    backButton_->setStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this] { backClicked_.emit(); });

    titleText_ = headerSection_->addWidget(std::make_unique<Wt::WText>("Skill Validations"));
    titleText_->setStyleClass("widget-title");

    addValidationButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Record Validation"));
    addValidationButton_->setStyleClass("btn btn-primary");
    addValidationButton_->clicked().connect([this] { showValidationDialog(); });

    // Skill categories section
    createSkillCategoriesSection();

    // Recent validations section
    createRecentValidationsSection();
}

void ValidationWidget::createSkillCategoriesSection() {
    categoriesSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    categoriesSection_->setStyleClass("categories-section");

    auto title = categoriesSection_->addWidget(std::make_unique<Wt::WText>("CDL Skill Categories"));
    title->setStyleClass("section-title");

    categoriesGrid_ = categoriesSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    categoriesGrid_->setStyleClass("categories-grid");
}

void ValidationWidget::createRecentValidationsSection() {
    validationsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    validationsSection_->setStyleClass("validations-section");

    auto title = validationsSection_->addWidget(std::make_unique<Wt::WText>("Recent Validations"));
    title->setStyleClass("section-title");

    validationsTable_ = validationsSection_->addWidget(std::make_unique<Wt::WTable>());
    validationsTable_->setStyleClass("validations-table");
    validationsTable_->setHeaderCount(1);

    validationsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Date"));
    validationsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Student"));
    validationsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Skill"));
    validationsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Result"));
    validationsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Score"));
    validationsTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Official"));

    noValidationsText_ = validationsSection_->addWidget(std::make_unique<Wt::WText>("No validations recorded yet"));
    noValidationsText_->setStyleClass("no-data-text");
    noValidationsText_->hide();
}

void ValidationWidget::loadData() {
    if (!instructorService_) return;

    // Load skill categories
    skillCategories_ = instructorService_->getSkillCategories();
    allSkillItems_ = instructorService_->getAllSkillItems();

    updateSkillCategories();
    updateRecentValidations();
}

void ValidationWidget::updateSkillCategories() {
    categoriesGrid_->clear();

    for (const auto& category : skillCategories_) {
        auto card = categoriesGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
        card->setStyleClass("category-card");

        auto cardHeader = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        cardHeader->setStyleClass("card-header");

        auto name = cardHeader->addWidget(std::make_unique<Wt::WText>(category.getName()));
        name->setStyleClass("category-name");

        auto code = cardHeader->addWidget(std::make_unique<Wt::WText>(category.getCode()));
        code->setStyleClass("category-code");

        if (!category.getDescription().empty()) {
            auto desc = card->addWidget(std::make_unique<Wt::WText>(category.getDescription()));
            desc->setStyleClass("category-description");
        }

        // Count skills in this category
        int skillCount = 0;
        int categoryId = std::stoi(category.getId());
        for (const auto& skill : allSkillItems_) {
            if (skill.getCategoryId() == categoryId) {
                skillCount++;
            }
        }

        auto skillsInfo = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        skillsInfo->setStyleClass("skills-info");

        std::ostringstream infoStr;
        infoStr << skillCount << " skills";
        if (category.getMinimumPracticeHours() > 0) {
            infoStr << " | Min " << std::fixed << std::setprecision(1)
                   << category.getMinimumPracticeHours() << " hours";
        }
        skillsInfo->addWidget(std::make_unique<Wt::WText>(infoStr.str()));
    }
}

void ValidationWidget::updateRecentValidations() {
    // Clear existing rows
    while (validationsTable_->rowCount() > 1) {
        validationsTable_->removeRow(1);
    }

    if (!instructorService_) {
        noValidationsText_->show();
        validationsTable_->hide();
        return;
    }

    auto validations = instructorService_->getInstructorValidations(instructorId_);

    if (validations.empty()) {
        noValidationsText_->show();
        validationsTable_->hide();
        return;
    }

    noValidationsText_->hide();
    validationsTable_->show();

    // Show max 20 recent validations
    int count = 0;
    for (const auto& validation : validations) {
        if (count >= 20) break;

        int row = validationsTable_->rowCount();

        // Date
        std::string dateStr = validation.getValidatedAt();
        if (dateStr.length() > 10) dateStr = dateStr.substr(0, 10);
        validationsTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(dateStr));

        // Student
        validationsTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>("Student #" + std::to_string(validation.getStudentId())));

        // Skill - find skill name
        std::string skillName = "Skill #" + std::to_string(validation.getSkillItemId());
        for (const auto& skill : allSkillItems_) {
            if (std::stoi(skill.getId()) == validation.getSkillItemId()) {
                skillName = skill.getName();
                break;
            }
        }
        validationsTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(skillName));

        // Result
        auto resultText = validationsTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(validation.getResultString()));
        resultText->setStyleClass("result-badge result-" + validation.getResultString());

        // Score
        std::ostringstream scoreStr;
        scoreStr << std::fixed << std::setprecision(0) << validation.getScorePercentage() << "%";
        validationsTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(scoreStr.str()));

        // Official
        validationsTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WText>(validation.isOfficialTest() ? "Yes" : "No"));

        count++;
    }
}

void ValidationWidget::createValidationDialog() {
    validationDialog_ = addChild(std::make_unique<Wt::WDialog>("Record Skill Validation"));
    validationDialog_->setStyleClass("validation-dialog");
    validationDialog_->setModal(true);
    validationDialog_->setClosable(true);

    auto content = validationDialog_->contents();
    content->setStyleClass("dialog-content");

    // Student selection
    auto studentRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentRow->setStyleClass("form-row");
    studentRow->addWidget(std::make_unique<Wt::WText>("Student:"))->setStyleClass("form-label");
    studentCombo_ = studentRow->addWidget(std::make_unique<Wt::WComboBox>());
    studentCombo_->setStyleClass("form-input");
    studentCombo_->changed().connect([this] { onStudentChanged(); });

    // Skill selection
    auto skillRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    skillRow->setStyleClass("form-row");
    skillRow->addWidget(std::make_unique<Wt::WText>("Skill:"))->setStyleClass("form-label");
    skillCombo_ = skillRow->addWidget(std::make_unique<Wt::WComboBox>());
    skillCombo_->setStyleClass("form-input");

    // Result
    auto resultRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    resultRow->setStyleClass("form-row");
    resultRow->addWidget(std::make_unique<Wt::WText>("Result:"))->setStyleClass("form-label");
    resultCombo_ = resultRow->addWidget(std::make_unique<Wt::WComboBox>());
    resultCombo_->addItem("Pass");
    resultCombo_->addItem("Fail");
    resultCombo_->addItem("Needs Practice");
    resultCombo_->addItem("Deferred");
    resultCombo_->addItem("Incomplete");
    resultCombo_->setStyleClass("form-input");

    // Score
    auto scoreRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    scoreRow->setStyleClass("form-row");
    scoreRow->addWidget(std::make_unique<Wt::WText>("Score (0-100):"))->setStyleClass("form-label");
    scoreEdit_ = scoreRow->addWidget(std::make_unique<Wt::WDoubleSpinBox>());
    scoreEdit_->setRange(0, 100);
    scoreEdit_->setValue(0);
    scoreEdit_->setStyleClass("form-input");

    // Errors noted
    auto errorsRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    errorsRow->setStyleClass("form-row");
    errorsRow->addWidget(std::make_unique<Wt::WText>("Errors Noted:"))->setStyleClass("form-label");
    errorsEdit_ = errorsRow->addWidget(std::make_unique<Wt::WTextArea>());
    errorsEdit_->setStyleClass("form-input");
    errorsEdit_->setRows(2);

    // Options
    auto optionsRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    optionsRow->setStyleClass("form-row options-row");

    criticalErrorCheck_ = optionsRow->addWidget(std::make_unique<Wt::WCheckBox>("Critical Error"));
    criticalErrorCheck_->setStyleClass("option-checkbox");

    officialTestCheck_ = optionsRow->addWidget(std::make_unique<Wt::WCheckBox>("Official Test"));
    officialTestCheck_->setStyleClass("option-checkbox");

    // Notes
    auto notesRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    notesRow->setStyleClass("form-row");
    notesRow->addWidget(std::make_unique<Wt::WText>("Notes:"))->setStyleClass("form-label");
    notesEdit_ = notesRow->addWidget(std::make_unique<Wt::WTextArea>());
    notesEdit_->setStyleClass("form-input");
    notesEdit_->setRows(3);

    // Dialog buttons
    auto footer = validationDialog_->footer();
    footer->setStyleClass("dialog-footer");

    dialogCancelButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    dialogCancelButton_->setStyleClass("btn btn-secondary");
    dialogCancelButton_->clicked().connect([this] { validationDialog_->reject(); });

    saveButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Save Validation"));
    saveButton_->setStyleClass("btn btn-primary");
    saveButton_->clicked().connect([this] { saveValidation(); });
}

void ValidationWidget::showValidationDialog(int studentId) {
    if (!validationDialog_) {
        createValidationDialog();
    }

    populateStudentCombo();
    populateSkillItems();

    // Select student if provided
    if (studentId > 0) {
        for (int i = 0; i < studentCombo_->count(); ++i) {
            if (studentCombo_->itemText(i).toUTF8().find(std::to_string(studentId)) != std::string::npos) {
                studentCombo_->setCurrentIndex(i);
                break;
            }
        }
    }

    // Reset form
    resultCombo_->setCurrentIndex(0);
    scoreEdit_->setValue(0);
    errorsEdit_->setText("");
    criticalErrorCheck_->setChecked(false);
    officialTestCheck_->setChecked(false);
    notesEdit_->setText("");

    validationDialog_->show();
}

void ValidationWidget::saveValidation() {
    if (!instructorService_) return;

    Models::SkillValidation validation;

    validation.setInstructorId(instructorId_);
    validation.setStudentId(selectedStudentId_);
    validation.setEnrollmentId(selectedEnrollmentId_);

    // Get selected skill
    int skillIndex = skillCombo_->currentIndex();
    if (skillIndex >= 0 && skillIndex < static_cast<int>(allSkillItems_.size())) {
        validation.setSkillItemId(std::stoi(allSkillItems_[skillIndex].getId()));
    }

    // Get result
    int resultIndex = resultCombo_->currentIndex();
    Models::ValidationResult results[] = {
        Models::ValidationResult::Pass,
        Models::ValidationResult::Fail,
        Models::ValidationResult::NeedsPractice,
        Models::ValidationResult::Deferred,
        Models::ValidationResult::Incomplete
    };
    if (resultIndex >= 0 && resultIndex < 5) {
        validation.setResult(results[resultIndex]);
    }

    validation.setScore(scoreEdit_->value());
    validation.setMaxScore(100.0);
    validation.setErrorsNoted(errorsEdit_->text().toUTF8());
    validation.setCriticalError(criticalErrorCheck_->isChecked());
    validation.setOfficialTest(officialTestCheck_->isChecked());
    validation.setNotes(notesEdit_->text().toUTF8());

    auto result = instructorService_->createValidation(validation);

    if (result.success) {
        validationDialog_->accept();
        updateRecentValidations();
    } else {
        Wt::WMessageBox::show("Error", "Failed to save validation: " + result.message,
                              Wt::StandardButton::Ok);
    }
}

void ValidationWidget::populateStudentCombo() {
    studentCombo_->clear();

    if (!instructorService_) return;

    auto students = instructorService_->getAssignedStudentProgress(instructorId_);
    for (const auto& student : students) {
        std::string label = student.studentName.empty() ?
            "Student #" + std::to_string(student.studentId) : student.studentName;
        studentCombo_->addItem(label);
    }

    if (!students.empty()) {
        selectedStudentId_ = students[0].studentId;
        selectedEnrollmentId_ = students[0].enrollmentId;
    }
}

void ValidationWidget::populateSkillItems() {
    skillCombo_->clear();

    for (const auto& skill : allSkillItems_) {
        // Find category name
        std::string categoryName;
        for (const auto& cat : skillCategories_) {
            if (std::stoi(cat.getId()) == skill.getCategoryId()) {
                categoryName = cat.getCode();
                break;
            }
        }

        std::string label = "[" + categoryName + "] " + skill.getName();
        skillCombo_->addItem(label);
    }
}

void ValidationWidget::onStudentChanged() {
    if (!instructorService_) return;

    // Update selected student info
    auto students = instructorService_->getAssignedStudentProgress(instructorId_);
    int selectedIndex = studentCombo_->currentIndex();

    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(students.size())) {
        selectedStudentId_ = students[selectedIndex].studentId;
        selectedEnrollmentId_ = students[selectedIndex].enrollmentId;
    }
}

} // namespace Instructor
} // namespace StudentIntake
