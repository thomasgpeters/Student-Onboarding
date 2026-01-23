#ifndef VALIDATION_WIDGET_H
#define VALIDATION_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDoubleSpinBox.h>
#include <Wt/WDialog.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/InstructorService.h"
#include "models/TrainingSession.h"

namespace StudentIntake {
namespace Instructor {

/**
 * @brief Widget for validating student skills and achievements
 *
 * Allows instructors to:
 * - View skill categories and items
 * - Record skill validations for students
 * - Track student progress toward certification
 * - Record CDL test results
 */
class ValidationWidget : public Wt::WContainerWidget {
public:
    ValidationWidget();
    ~ValidationWidget() = default;

    void setInstructorService(std::shared_ptr<Api::InstructorService> service) { instructorService_ = service; }
    void setInstructorId(int instructorId) { instructorId_ = instructorId; }

    /**
     * @brief Load validation data
     */
    void loadData();

    /**
     * @brief Show validation dialog for a student
     */
    void showValidationDialog(int studentId = 0);

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }

private:
    void setupUI();
    void createSkillCategoriesSection();
    void createRecentValidationsSection();
    void createValidationDialog();
    void updateSkillCategories();
    void updateRecentValidations();
    void saveValidation();
    void populateStudentCombo();
    void populateSkillItems();
    void onStudentChanged();

    std::shared_ptr<Api::InstructorService> instructorService_;
    int instructorId_;
    int selectedStudentId_;
    int selectedEnrollmentId_;

    // Main UI
    Wt::WContainerWidget* headerSection_;
    Wt::WText* titleText_;
    Wt::WPushButton* backButton_;
    Wt::WPushButton* addValidationButton_;

    // Skill categories section
    Wt::WContainerWidget* categoriesSection_;
    Wt::WContainerWidget* categoriesGrid_;

    // Recent validations section
    Wt::WContainerWidget* validationsSection_;
    Wt::WTable* validationsTable_;
    Wt::WText* noValidationsText_;

    // Validation dialog
    Wt::WDialog* validationDialog_;
    Wt::WComboBox* studentCombo_;
    Wt::WComboBox* skillCombo_;
    Wt::WComboBox* resultCombo_;
    Wt::WDoubleSpinBox* scoreEdit_;
    Wt::WTextArea* errorsEdit_;
    Wt::WCheckBox* criticalErrorCheck_;
    Wt::WCheckBox* officialTestCheck_;
    Wt::WTextArea* notesEdit_;
    Wt::WPushButton* saveButton_;
    Wt::WPushButton* dialogCancelButton_;

    // Skill items cache
    std::vector<Models::SkillItem> allSkillItems_;
    std::vector<Models::SkillCategory> skillCategories_;

    // Signals
    Wt::Signal<> backClicked_;
};

} // namespace Instructor
} // namespace StudentIntake

#endif // VALIDATION_WIDGET_H
