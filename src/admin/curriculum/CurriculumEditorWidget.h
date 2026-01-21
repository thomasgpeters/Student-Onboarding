#ifndef CURRICULUM_EDITOR_WIDGET_H
#define CURRICULUM_EDITOR_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WComboBox.h>
#include <Wt/WCheckBox.h>
#include <Wt/WSpinBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include <map>
#include "../../api/FormSubmissionService.h"
#include "../../models/Curriculum.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Widget for viewing and editing curriculum/program details
 */
class CurriculumEditorWidget : public Wt::WContainerWidget {
public:
    CurriculumEditorWidget();
    ~CurriculumEditorWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadCurriculum(const std::string& curriculumId);
    void createNew();
    void clearForm();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<>& saveSuccess() { return saveSuccess_; }

private:
    void setupUI();
    void populateForm(const StudentIntake::Models::Curriculum& curriculum);
    void loadAvailableForms();
    void loadCurriculumFormRequirements(const std::string& curriculumId);
    void saveCurriculumFormRequirements(const std::string& curriculumId);
    void saveCurriculum();
    void updateRequiredFormsDisplay();
    void toggleFormRequirement(const std::string& formId, bool required);
    bool validateForm();
    void showError(const std::string& message);
    void showSuccess(const std::string& message);
    void clearMessages();

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    StudentIntake::Models::Curriculum currentCurriculum_;
    bool isNewCurriculum_;
    std::vector<std::string> selectedForms_;

    // Available form types
    struct FormTypeOption {
        std::string id;
        std::string name;
        std::string description;
    };
    std::vector<FormTypeOption> availableForms_;

    // Available departments (loaded from API)
    struct DepartmentOption {
        int id;
        std::string code;
        std::string name;
    };
    std::vector<DepartmentOption> departments_;
    void loadDepartments();

    // UI Elements - Header
    Wt::WContainerWidget* headerSection_;
    Wt::WText* headerTitle_;
    Wt::WPushButton* backBtn_;

    // Messages
    Wt::WContainerWidget* messageContainer_;
    Wt::WText* errorMessage_;
    Wt::WText* successMessage_;

    // Form fields - Basic Info
    Wt::WContainerWidget* formContainer_;
    Wt::WLineEdit* codeInput_;
    Wt::WLineEdit* nameInput_;
    Wt::WTextArea* descriptionInput_;
    Wt::WComboBox* departmentSelect_;
    Wt::WComboBox* degreeTypeSelect_;

    // Form fields - Program Details
    Wt::WSpinBox* creditHoursInput_;
    Wt::WSpinBox* durationInput_;
    Wt::WComboBox* durationIntervalSelect_;
    Wt::WCheckBox* isActiveCheck_;
    Wt::WCheckBox* isOnlineCheck_;

    // Form fields - Required Forms
    Wt::WContainerWidget* formsSection_;
    Wt::WContainerWidget* formsCheckboxContainer_;
    std::map<std::string, Wt::WCheckBox*> formCheckboxes_;

    // Action buttons
    Wt::WContainerWidget* actionsContainer_;
    Wt::WPushButton* saveBtn_;
    Wt::WPushButton* cancelBtn_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<> saveSuccess_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // CURRICULUM_EDITOR_WIDGET_H
