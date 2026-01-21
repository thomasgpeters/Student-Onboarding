#ifndef ACADEMIC_HISTORY_FORM_H
#define ACADEMIC_HISTORY_FORM_H

#include "BaseForm.h"
#include "models/AcademicHistory.h"
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WDoubleSpinBox.h>
#include <vector>

namespace StudentIntake {
namespace Forms {

class AcademicHistoryForm : public BaseForm {
public:
    AcademicHistoryForm();
    ~AcademicHistoryForm() override;

    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;
    void handleSubmit() override;

private:
    // Academic History List
    std::vector<Models::AcademicHistory> academicHistories_;
    Wt::WCheckBox* hasPreviousEducationCheckbox_;
    Wt::WContainerWidget* academicHistoryContainer_;
    Wt::WTable* academicHistoryTable_;
    Wt::WPushButton* addEducationBtn_;
    Wt::WText* noEducationText_;

    // Test scores
    Wt::WCheckBox* hasTestScoresCheckbox_;
    Wt::WContainerWidget* testScoresContainer_;
    Wt::WLineEdit* satScoreInput_;
    Wt::WLineEdit* actScoreInput_;
    Wt::WLineEdit* greScoreInput_;
    Wt::WLineEdit* gmatScoreInput_;
    Wt::WLineEdit* toeflScoreInput_;

    // Honors/Awards
    Wt::WTextArea* honorsAwardsInput_;

    // Academic interests
    Wt::WTextArea* academicInterestsInput_;

    // UI Toggle methods
    void togglePreviousEducation();
    void toggleTestScores();

    // Academic History list methods
    void updateAcademicHistoryTable();
    void showAddEducationDialog();
    void showEditEducationDialog(int index);
    void deleteEducation(int index);
    void saveEducationFromDialog(Wt::WDialog* dialog,
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
                                  int editIndex = -1);

    // Utility methods
    std::vector<std::string> getUSStates() const;
    std::string getInstitutionTypeLabel(const std::string& type) const;
    std::string getInstitutionTypeValue(const std::string& label) const;
    std::vector<std::string> getInstitutionTypes() const;
    std::vector<std::string> getDegreeTypesForInstitution(const std::string& institutionType) const;

    // API integration
    void loadHistoriesFromApi();
    void saveHistoriesToApi();
};

} // namespace Forms
} // namespace StudentIntake

#endif // ACADEMIC_HISTORY_FORM_H
