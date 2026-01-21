#ifndef STUDENT_DETAIL_WIDGET_H
#define STUDENT_DETAIL_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <Wt/WDialog.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WDoubleSpinBox.h>
#include <Wt/WCheckBox.h>
#include <Wt/WDateEdit.h>
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

// Academic history record for display
struct AcademicHistoryRecord {
    int id;
    std::string institutionName;
    std::string institutionType;
    std::string institutionCity;
    std::string institutionState;
    std::string degreeEarned;
    std::string major;
    double gpa;
    std::string startDate;
    std::string endDate;
    std::string graduationDate;
    bool isCurrentlyAttending;
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
    void loadAcademicHistory();
    void updateFormSubmissionsTable();
    void updateAcademicHistoryTable();
    void onRevokeAccess();
    void onRestoreAccess();
    void approveSubmission(int submissionId);
    void rejectSubmission(int submissionId);
    void showAddAcademicHistoryDialog();
    void saveAcademicHistory(Wt::WDialog* dialog);
    void deleteAcademicHistory(int historyId);
    std::string formatDate(const std::string& dateStr);
    std::string getStatusBadgeClass(const std::string& status);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    ::StudentIntake::Models::Student currentStudent_;
    bool isRevoked_;
    int currentStudentId_;

    // Form submissions data
    std::vector<StudentFormRecord> formSubmissions_;

    // Academic history data
    std::vector<AcademicHistoryRecord> academicHistory_;

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
    Wt::WText* intakeStatusLabel_;
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

    // Academic history section
    Wt::WContainerWidget* academicHistoryContainer_;
    Wt::WContainerWidget* academicHistoryHeader_;
    Wt::WText* academicHistoryTitle_;
    Wt::WPushButton* addAcademicHistoryBtn_;
    Wt::WTable* academicHistoryTable_;
    Wt::WText* noAcademicHistoryText_;

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
