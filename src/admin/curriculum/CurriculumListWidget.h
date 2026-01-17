#ifndef CURRICULUM_LIST_WIDGET_H
#define CURRICULUM_LIST_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "../../api/FormSubmissionService.h"
#include "../../models/Curriculum.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Widget for displaying and managing curriculum/program list
 */
class CurriculumListWidget : public Wt::WContainerWidget {
public:
    CurriculumListWidget();
    ~CurriculumListWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadData();
    void clearData();

    // Signals
    Wt::Signal<std::string>& curriculumSelected() { return curriculumSelected_; }
    Wt::Signal<>& addCurriculumClicked() { return addCurriculumClicked_; }

private:
    void setupUI();
    void loadCurriculums();
    void updateTable();
    void applyFilters();
    void resetFilters();
    std::string getStatusBadgeClass(bool isActive);
    std::string getDegreeTypeBadgeClass(const std::string& degreeType);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::vector<StudentIntake::Models::Curriculum> curriculums_;
    std::vector<StudentIntake::Models::Curriculum> filteredCurriculums_;

    // UI Elements
    Wt::WText* headerTitle_;
    Wt::WText* headerSubtitle_;
    Wt::WContainerWidget* filterContainer_;
    Wt::WLineEdit* searchInput_;
    Wt::WComboBox* departmentFilter_;
    Wt::WComboBox* degreeTypeFilter_;
    Wt::WComboBox* statusFilter_;
    Wt::WPushButton* resetBtn_;
    Wt::WPushButton* addBtn_;
    Wt::WText* resultCount_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* curriculumTable_;
    Wt::WText* noDataMessage_;

    // Signals
    Wt::Signal<std::string> curriculumSelected_;
    Wt::Signal<> addCurriculumClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // CURRICULUM_LIST_WIDGET_H
