#ifndef STUDENT_DETAIL_WIDGET_H
#define STUDENT_DETAIL_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "../../models/Student.h"
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

// Form submission record for display
struct StudentFormRecord {
    int id;
    int formTypeId;
    std::string formType;
    std::string formName;
    std::string status;
    std::string submittedAt;
    std::string reviewedAt;
    std::string reviewedBy;
};

class StudentDetailWidget : public Wt::WContainerWidget {
public:
    StudentDetailWidget();
    ~StudentDetailWidget();

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadStudent(int studentId);
    void clear();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<int>& revokeAccessClicked() { return revokeAccessClicked_; }
    Wt::Signal<int>& restoreAccessClicked() { return restoreAccessClicked_; }
    Wt::Signal<int>& previewFormClicked() { return previewFormClicked_; }
    Wt::Signal<int>& printAllFormsClicked() { return printAllFormsClicked_; }

private:
    void setupUI();
    void updateDisplay();
    void loadFormSubmissions();
    void loadStudentAddress();
    void updateFormSubmissionsTable();
    void onRevokeAccess();
    void onRestoreAccess();
    void approveSubmission(int submissionId);
    void rejectSubmission(int submissionId);
    std::string formatDate(const std::string& dateStr);
    std::string getStatusBadgeClass(const std::string& status);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    ::StudentIntake::Models::Student currentStudent_;
    bool isRevoked_;
    int currentStudentId_;

    // Form submissions data
    std::vector<StudentFormRecord> formSubmissions_;

    // UI Elements
    Wt::WContainerWidget* headerContainer_;
    Wt::WText* studentName_;
    Wt::WText* studentEmail_;
    Wt::WText* statusBadge_;

    Wt::WContainerWidget* infoContainer_;
    Wt::WText* programText_;
    Wt::WText* enrolledText_;
    Wt::WText* phoneText_;
    Wt::WText* addressText_;
    Wt::WText* intakeStatusText_;
    std::string intakeStatus_;

    Wt::WContainerWidget* actionsContainer_;
    Wt::WPushButton* revokeBtn_;
    Wt::WPushButton* restoreBtn_;
    Wt::WPushButton* backBtn_;

    // Form submissions section
    Wt::WContainerWidget* submissionsContainer_;
    Wt::WContainerWidget* submissionsHeader_;
    Wt::WText* submissionsTitle_;
    Wt::WPushButton* previewPdfBtn_;
    Wt::WPushButton* printAllBtn_;
    Wt::WTable* submissionsTable_;
    Wt::WText* noSubmissionsText_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<int> revokeAccessClicked_;
    Wt::Signal<int> restoreAccessClicked_;
    Wt::Signal<int> previewFormClicked_;
    Wt::Signal<int> printAllFormsClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // STUDENT_DETAIL_WIDGET_H
