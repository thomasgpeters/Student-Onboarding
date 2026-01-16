#ifndef STUDENT_FORM_VIEWER_H
#define STUDENT_FORM_VIEWER_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

struct FormSubmission {
    int id;
    std::string formName;
    std::string status;
    std::string submittedAt;
    std::string reviewedAt;
    std::string reviewedBy;
};

class StudentFormViewer : public Wt::WContainerWidget {
public:
    StudentFormViewer();
    ~StudentFormViewer();

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadStudentForms(int studentId, const std::string& studentName);
    void clear();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<int>& viewFormDetailClicked() { return viewFormDetailClicked_; }

private:
    void setupUI();
    void updateTable();
    std::string getStatusBadgeClass(const std::string& status);
    std::string formatDate(const std::string& dateStr);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    int currentStudentId_;
    std::string currentStudentName_;
    std::vector<FormSubmission> forms_;

    // UI Elements
    Wt::WText* headerTitle_;
    Wt::WText* headerSubtitle_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* formsTable_;
    Wt::WText* noFormsMessage_;
    Wt::WPushButton* backBtn_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<int> viewFormDetailClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // STUDENT_FORM_VIEWER_H
