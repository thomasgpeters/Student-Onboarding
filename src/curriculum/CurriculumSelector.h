#ifndef CURRICULUM_SELECTOR_H
#define CURRICULUM_SELECTOR_H

#include <Wt/WContainerWidget.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDialog.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "CurriculumManager.h"
#include "models/Curriculum.h"
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Curriculum {

/**
 * @brief Widget for selecting a curriculum/program of study
 */
class CurriculumSelector : public Wt::WContainerWidget {
public:
    CurriculumSelector();
    ~CurriculumSelector();

    // Dependencies
    void setCurriculumManager(std::shared_ptr<CurriculumManager> manager);
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }

    // Signals
    Wt::Signal<Models::Curriculum>& curriculumSelected() { return curriculumSelected_; }
    Wt::Signal<Models::Curriculum, std::vector<Models::Curriculum>>& selectionComplete() { return selectionComplete_; }
    Wt::Signal<>& backRequested() { return backRequested_; }

    // Refresh the curriculum list
    void refresh();

    // Pre-select a curriculum
    void selectCurriculum(const std::string& curriculumId);

    // Get selected endorsements
    std::vector<Models::Curriculum> getSelectedEndorsements() const { return selectedEndorsements_; }

private:
    void setupUI();
    void populateDepartments();
    void populateDegreeTypes();
    void updateCurriculumList();
    void handleDepartmentChange();
    void handleDegreeTypeChange();
    void handleSearchChange();
    void handleSelectProgram(const Models::Curriculum& curriculum);
    void showSyllabusDialog(const Models::Curriculum& curriculum);

    // Endorsement selection (vocational mode)
    void showEndorsementSelection(const Models::Curriculum& baseProgram);
    void updateEndorsementList();
    void handleEndorsementToggle(const Models::Curriculum& endorsement, bool selected);
    void handleContinueWithEndorsements();
    void handleBackToBaseSelection();
    bool isVocationalMode() const;

    std::shared_ptr<CurriculumManager> curriculumManager_;
    std::shared_ptr<Session::StudentSession> session_;

    // UI Components - Base selection
    Wt::WContainerWidget* baseSelectionContainer_;
    Wt::WLineEdit* searchInput_;
    Wt::WComboBox* departmentSelect_;
    Wt::WComboBox* degreeTypeSelect_;
    Wt::WContainerWidget* curriculumCardsContainer_;

    // UI Components - Endorsement selection
    Wt::WContainerWidget* endorsementSelectionContainer_;
    Wt::WContainerWidget* endorsementCardsContainer_;
    Wt::WText* selectedBaseText_;

    // State
    Models::Curriculum selectedBaseProgram_;
    std::vector<Models::Curriculum> selectedEndorsements_;
    std::vector<Models::Curriculum> availableEndorsements_;

    Wt::Signal<Models::Curriculum> curriculumSelected_;
    Wt::Signal<Models::Curriculum, std::vector<Models::Curriculum>> selectionComplete_;
    Wt::Signal<> backRequested_;
};

} // namespace Curriculum
} // namespace StudentIntake

#endif // CURRICULUM_SELECTOR_H
