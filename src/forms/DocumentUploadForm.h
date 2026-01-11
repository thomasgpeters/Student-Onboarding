#ifndef DOCUMENT_UPLOAD_FORM_H
#define DOCUMENT_UPLOAD_FORM_H

#include "BaseForm.h"
#include <Wt/WFileUpload.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <vector>

namespace StudentIntake {
namespace Forms {

class DocumentUploadForm : public BaseForm {
public:
    DocumentUploadForm();
    ~DocumentUploadForm() override;
    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;

private:
    struct DocumentUpload {
        std::string documentType;
        std::string label;
        bool required;
        Wt::WContainerWidget* container;
        Wt::WFileUpload* upload;
        Wt::WText* statusText;
        std::string uploadedPath;
    };

    std::vector<DocumentUpload> documents_;
    Wt::WContainerWidget* uploadsContainer_;

    void addDocumentUpload(const std::string& type, const std::string& label, bool required);
    void handleUpload(size_t index);
    void handleUploadError(size_t index);
};

} // namespace Forms
} // namespace StudentIntake

#endif // DOCUMENT_UPLOAD_FORM_H
