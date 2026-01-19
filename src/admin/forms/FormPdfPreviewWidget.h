#ifndef FORM_PDF_PREVIEW_WIDGET_H
#define FORM_PDF_PREVIEW_WIDGET_H

#include <Wt/WDialog.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WFileResource.h>
#include <Wt/WAnchor.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "../../api/FormSubmissionService.h"
#include "../../api/PdfGenerator.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Form field data for PDF preview
 */
struct FormFieldData {
    std::string label;
    std::string value;
    std::string type;  // text, date, email, phone, etc.
};

/**
 * @brief Modal dialog for previewing form submissions as printable PDF-style documents
 */
class FormPdfPreviewWidget : public Wt::WDialog {
public:
    FormPdfPreviewWidget();
    ~FormPdfPreviewWidget() override;

    void setApiService(std::shared_ptr<Api::FormSubmissionService> apiService);

    // Load form data for preview and show dialog
    void showFormSubmission(int submissionId);
    void showStudentForms(int studentId);

    // Set form data directly
    void setFormData(const std::string& formType,
                     const std::string& formTitle,
                     const std::string& studentName,
                     const std::string& studentEmail,
                     const std::string& submissionDate,
                     const std::vector<FormFieldData>& fields);

    // Signals
    Wt::Signal<>& printClicked() { return printClicked_; }
    Wt::Signal<>& downloadClicked() { return downloadClicked_; }

private:
    void setupUI();
    void buildPreview();
    void clearPreview();
    void loadFormSubmissionData(int submissionId);
    void loadStudentFormsData(int studentId);
    std::string formatDate(const std::string& dateStr);
    std::string formatValue(const std::string& value, const std::string& type);

    // PDF generation methods
    std::string generatePdf();
    void downloadPdf();
    Api::PdfFormData buildPdfFormData();

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    std::unique_ptr<Api::PdfGenerator> pdfGenerator_;
    std::string currentPdfPath_;  // Path to most recently generated PDF

    // Form data
    std::string formType_;
    std::string formTitle_;
    std::string studentName_;
    std::string studentEmail_;
    std::string submissionDate_;
    std::vector<FormFieldData> fields_;

    // UI Elements
    Wt::WContainerWidget* toolbar_;
    Wt::WPushButton* printBtn_;
    Wt::WPushButton* downloadBtn_;
    Wt::WPushButton* closeBtn_;
    Wt::WContainerWidget* previewContainer_;
    Wt::WContainerWidget* documentContent_;

    // Signals
    Wt::Signal<> printClicked_;
    Wt::Signal<> downloadClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // FORM_PDF_PREVIEW_WIDGET_H
