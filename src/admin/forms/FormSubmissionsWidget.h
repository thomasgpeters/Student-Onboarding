#ifndef FORM_SUBMISSIONS_WIDGET_H
#define FORM_SUBMISSIONS_WIDGET_H

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

namespace StudentIntake {
namespace Admin {

/**
 * @brief Represents a form submission for admin review
 */
struct FormSubmissionRecord {
    int id;
    int studentId;
    std::string studentName;
    std::string studentEmail;
    std::string formType;
    std::string formName;
    std::string status;      // pending, approved, rejected, needs_revision
    std::string submittedAt;
    std::string reviewedAt;
    std::string reviewedBy;
    std::string programName;
};

/**
 * @brief Widget for displaying and managing form submissions
 */
class FormSubmissionsWidget : public Wt::WContainerWidget {
public:
    FormSubmissionsWidget();
    ~FormSubmissionsWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadData();
    void clearData();

    // Signals
    Wt::Signal<int>& viewSubmissionClicked() { return viewSubmissionClicked_; }
    Wt::Signal<int, std::string>& approveClicked() { return approveClicked_; }
    Wt::Signal<int, std::string>& rejectClicked() { return rejectClicked_; }

private:
    void setupUI();
    void loadSubmissions();
    void updateTable();
    void applyFilters();
    void resetFilters();
    void approveSubmission(int submissionId);
    void rejectSubmission(int submissionId);
    std::string getStatusBadgeClass(const std::string& status);
    std::string formatDate(const std::string& dateStr);
    std::string getFormDisplayName(const std::string& formType);
    std::string getFormTypeFromId(int formTypeId);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::vector<FormSubmissionRecord> submissions_;
    std::vector<FormSubmissionRecord> filteredSubmissions_;

    // UI Elements
    Wt::WText* headerTitle_;
    Wt::WText* headerSubtitle_;
    Wt::WContainerWidget* statsContainer_;
    Wt::WText* pendingCountText_;
    Wt::WText* approvedCountText_;
    Wt::WText* rejectedCountText_;
    Wt::WContainerWidget* filterContainer_;
    Wt::WLineEdit* searchInput_;
    Wt::WComboBox* formTypeFilter_;
    Wt::WComboBox* statusFilter_;
    Wt::WComboBox* programFilter_;
    Wt::WPushButton* searchBtn_;
    Wt::WPushButton* resetBtn_;
    Wt::WText* resultCount_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* submissionsTable_;
    Wt::WText* noDataMessage_;

    // Signals
    Wt::Signal<int> viewSubmissionClicked_;
    Wt::Signal<int, std::string> approveClicked_;
    Wt::Signal<int, std::string> rejectClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // FORM_SUBMISSIONS_WIDGET_H
