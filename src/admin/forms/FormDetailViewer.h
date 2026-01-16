#ifndef FORM_DETAIL_VIEWER_H
#define FORM_DETAIL_VIEWER_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "../../api/FormSubmissionService.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Form field display structure
 */
struct FormFieldDisplay {
    std::string label;
    std::string value;
    std::string fieldType; // text, date, email, phone, select, textarea, file
};

/**
 * @brief Widget for viewing detailed form submission data
 */
class FormDetailViewer : public Wt::WContainerWidget {
public:
    FormDetailViewer();
    ~FormDetailViewer() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);
    void loadSubmission(int submissionId);
    void clear();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<int>& approveClicked() { return approveClicked_; }
    Wt::Signal<int>& rejectClicked() { return rejectClicked_; }
    Wt::Signal<int>& requestRevisionClicked() { return requestRevisionClicked_; }

private:
    void setupUI();
    void displayFormData(const std::vector<FormFieldDisplay>& fields);
    void updateStatusDisplay(const std::string& status);
    void handleApprove();
    void handleReject();
    void handleRequestRevision();
    std::string getStatusBadgeClass(const std::string& status);
    std::string formatValue(const std::string& value, const std::string& fieldType);
    std::vector<FormFieldDisplay> parseFormData(const std::string& formType, const nlohmann::json& data);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    int currentSubmissionId_;
    std::string currentStatus_;
    std::string currentFormType_;

    // UI Elements - Header
    Wt::WContainerWidget* headerSection_;
    Wt::WPushButton* backBtn_;
    Wt::WText* formTitle_;
    Wt::WText* statusBadge_;

    // Student info section
    Wt::WContainerWidget* studentInfoSection_;
    Wt::WText* studentName_;
    Wt::WText* studentEmail_;
    Wt::WText* studentProgram_;
    Wt::WText* submittedDate_;

    // Form data section
    Wt::WContainerWidget* formDataSection_;
    Wt::WContainerWidget* fieldsContainer_;

    // Review section
    Wt::WContainerWidget* reviewSection_;
    Wt::WText* reviewedByText_;
    Wt::WText* reviewedAtText_;
    Wt::WTextArea* reviewNotes_;

    // Actions section
    Wt::WContainerWidget* actionsSection_;
    Wt::WPushButton* approveBtn_;
    Wt::WPushButton* rejectBtn_;
    Wt::WPushButton* revisionBtn_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<int> approveClicked_;
    Wt::Signal<int> rejectClicked_;
    Wt::Signal<int> requestRevisionClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // FORM_DETAIL_VIEWER_H
