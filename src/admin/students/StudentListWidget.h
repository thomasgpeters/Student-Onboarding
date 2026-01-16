#ifndef STUDENT_LIST_WIDGET_H
#define STUDENT_LIST_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <vector>
#include <memory>
#include "../../models/Student.h"
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

class StudentListWidget : public Wt::WContainerWidget {
public:
    StudentListWidget();
    ~StudentListWidget();

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void refresh();

    // Signal emitted when a student is selected for viewing
    Wt::Signal<int>& studentSelected() { return studentSelected_; }

private:
    void setupUI();
    void setupFilters();
    void setupTable();
    void loadStudents();
    void applyFilters();
    void clearFilters();
    void updateTable(const std::vector<Models::Student>& students);
    void onStudentRowClicked(int studentId);
    std::string formatDate(const std::string& dateStr);
    std::string getStatusBadgeClass(const std::string& status);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::vector<Models::Student> allStudents_;

    // UI Elements
    Wt::WLineEdit* searchInput_;
    Wt::WComboBox* programFilter_;
    Wt::WComboBox* statusFilter_;
    Wt::WPushButton* searchButton_;
    Wt::WPushButton* clearButton_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* studentTable_;
    Wt::WText* resultCount_;

    // Signal
    Wt::Signal<int> studentSelected_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // STUDENT_LIST_WIDGET_H
