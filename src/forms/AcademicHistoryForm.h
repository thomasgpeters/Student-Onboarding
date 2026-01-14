#ifndef ACADEMIC_HISTORY_FORM_H
#define ACADEMIC_HISTORY_FORM_H

#include "BaseForm.h"
#include "models/AcademicHistory.h"
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDateEdit.h>

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
    // High School
    Wt::WLineEdit* highSchoolNameInput_;
    Wt::WLineEdit* highSchoolCityInput_;
    Wt::WComboBox* highSchoolStateSelect_;
    Wt::WDateEdit* highSchoolGradDateInput_;
    Wt::WLineEdit* highSchoolGpaInput_;
    Wt::WCheckBox* gedCheckbox_;

    // Previous College
    Wt::WCheckBox* hasPreviousCollegeCheckbox_;
    Wt::WContainerWidget* previousCollegeContainer_;
    Wt::WLineEdit* collegeNameInput_;
    Wt::WLineEdit* collegeCityInput_;
    Wt::WComboBox* collegeStateSelect_;
    Wt::WDateEdit* collegeStartDateInput_;
    Wt::WDateEdit* collegeEndDateInput_;
    Wt::WLineEdit* collegeMajorInput_;
    Wt::WLineEdit* collegeGpaInput_;
    Wt::WComboBox* collegeDegreeSelect_;
    Wt::WCheckBox* degreeCompletedCheckbox_;

    // Transfer credits
    Wt::WCheckBox* hasTransferCreditsCheckbox_;
    Wt::WTextArea* transferCreditsInput_;

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

    void togglePreviousCollege();
    void toggleTestScores();
    std::vector<std::string> getUSStates() const;

    // API integration
    void loadHistoriesFromApi();
    void saveHistoriesToApi();
    void populateHighSchoolFields(const Models::AcademicHistory& history);
    void populateCollegeFields(const Models::AcademicHistory& history);
    Models::AcademicHistory buildHighSchoolHistory() const;
    Models::AcademicHistory buildCollegeHistory() const;

    // Cached record IDs for updates
    std::string highSchoolRecordId_;
    std::string collegeRecordId_;
};

} // namespace Forms
} // namespace StudentIntake

#endif // ACADEMIC_HISTORY_FORM_H
