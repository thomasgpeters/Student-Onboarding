#ifndef STUDENT_DETAIL_WIDGET_H
#define STUDENT_DETAIL_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include "../../models/Student.h"
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

class StudentDetailWidget : public Wt::WContainerWidget {
public:
    StudentDetailWidget();
    ~StudentDetailWidget();

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadStudent(int studentId);
    void clear();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<int>& viewFormsClicked() { return viewFormsClicked_; }
    Wt::Signal<int>& revokeAccessClicked() { return revokeAccessClicked_; }
    Wt::Signal<int>& restoreAccessClicked() { return restoreAccessClicked_; }

private:
    void setupUI();
    void updateDisplay();
    void onRevokeAccess();
    void onRestoreAccess();
    std::string formatDate(const std::string& dateStr);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    Models::Student currentStudent_;
    bool isRevoked_;

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

    Wt::WContainerWidget* actionsContainer_;
    Wt::WPushButton* viewFormsBtn_;
    Wt::WPushButton* revokeBtn_;
    Wt::WPushButton* restoreBtn_;
    Wt::WPushButton* backBtn_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<int> viewFormsClicked_;
    Wt::Signal<int> revokeAccessClicked_;
    Wt::Signal<int> restoreAccessClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // STUDENT_DETAIL_WIDGET_H
