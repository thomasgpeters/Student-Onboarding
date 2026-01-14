#include "DocumentUploadForm.h"
#include <Wt/WLabel.h>
#include <Wt/WProgressBar.h>

namespace StudentIntake {
namespace Forms {

DocumentUploadForm::DocumentUploadForm()
    : BaseForm("documents", "Document Upload")
    , uploadsContainer_(nullptr) {
}

DocumentUploadForm::~DocumentUploadForm() {
}

void DocumentUploadForm::createFormFields() {
    auto intro = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    intro->addStyleClass("form-section");
    auto introText = intro->addWidget(std::make_unique<Wt::WText>(
        "<p>Please upload the following required documents. Accepted formats: PDF, JPG, PNG. Maximum file size: 10MB.</p>"));
    introText->setTextFormat(Wt::TextFormat::XHTML);

    auto requirements = intro->addWidget(std::make_unique<Wt::WContainerWidget>());
    requirements->addStyleClass("alert alert-info");
    auto reqText = requirements->addWidget(std::make_unique<Wt::WText>(
        "<strong>Required Documents:</strong>"
        "<ul>"
        "<li>Government-issued photo ID</li>"
        "<li>Official high school transcript</li>"
        "</ul>"
        "<strong>Optional Documents (if applicable):</strong>"
        "<ul>"
        "<li>College transcripts</li>"
        "<li>Immunization records</li>"
        "<li>Visa/immigration documents (international students)</li>"
        "</ul>"));
    reqText->setTextFormat(Wt::TextFormat::XHTML);

    uploadsContainer_ = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    uploadsContainer_->addStyleClass("uploads-container");

    // Add required document uploads
    addDocumentUpload("photo_id", "Government-Issued Photo ID (Driver's License, Passport, or State ID)", true);
    addDocumentUpload("hs_transcript", "Official High School Transcript", true);

    // Add optional document uploads
    addDocumentUpload("college_transcript", "College Transcript(s)", false);
    addDocumentUpload("immunization", "Immunization Records", false);
    addDocumentUpload("visa_docs", "Visa/Immigration Documents", false);
    addDocumentUpload("other", "Other Supporting Documents", false);
}

void DocumentUploadForm::addDocumentUpload(const std::string& type, const std::string& label, bool required) {
    DocumentUpload doc;
    doc.documentType = type;
    doc.label = label;
    doc.required = required;

    doc.container = uploadsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    doc.container->addStyleClass("document-upload-item form-section");

    std::string labelText = label;
    if (required) {
        labelText += " *";
    }
    auto labelWidget = doc.container->addWidget(std::make_unique<Wt::WLabel>(labelText));
    labelWidget->addStyleClass("document-label");

    auto uploadContainer = doc.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    uploadContainer->addStyleClass("upload-container");

    doc.upload = uploadContainer->addWidget(std::make_unique<Wt::WFileUpload>());
    doc.upload->setFileTextSize(10 * 1024 * 1024); // 10MB limit
    doc.upload->addStyleClass("form-control-file");

    doc.statusText = uploadContainer->addWidget(std::make_unique<Wt::WText>());
    doc.statusText->addStyleClass("upload-status");

    size_t index = documents_.size();
    doc.upload->uploaded().connect([this, index]() {
        handleUpload(index);
    });
    doc.upload->fileTooLarge().connect([this, index]() {
        handleUploadError(index);
    });

    // Auto-upload when file is selected
    doc.upload->changed().connect([this, index]() {
        if (index < documents_.size() && !documents_[index].upload->empty()) {
            documents_[index].upload->upload();
            documents_[index].statusText->setText("Uploading...");
            documents_[index].statusText->addStyleClass("text-info");
        }
    });

    documents_.push_back(doc);
}

void DocumentUploadForm::handleUpload(size_t index) {
    if (index >= documents_.size()) return;

    auto& doc = documents_[index];
    if (!doc.upload->empty()) {
        doc.uploadedPath = doc.upload->spoolFileName();
        std::string filename = doc.upload->clientFileName().toUTF8();
        doc.statusText->setText("Uploaded: " + filename);
        doc.statusText->setStyleClass("upload-status text-success");
        doc.container->addStyleClass("upload-complete");
    }
}

void DocumentUploadForm::handleUploadError(size_t index) {
    if (index >= documents_.size()) return;

    auto& doc = documents_[index];
    doc.statusText->setText("Error: File too large (max 10MB)");
    doc.statusText->setStyleClass("upload-status text-danger");
}

bool DocumentUploadForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    for (const auto& doc : documents_) {
        if (doc.required && doc.uploadedPath.empty()) {
            validationErrors_.push_back(doc.label + " is required");
            isValid_ = false;
        }
    }

    return isValid_;
}

void DocumentUploadForm::collectFormData(Models::FormData& data) const {
    for (const auto& doc : documents_) {
        data.setField(doc.documentType + "_uploaded", !doc.uploadedPath.empty());
        if (!doc.uploadedPath.empty()) {
            data.setFileField(doc.documentType + "_path", doc.uploadedPath);
            data.setField(doc.documentType + "_filename", doc.upload->clientFileName().toUTF8());
        }
    }
}

void DocumentUploadForm::populateFormFields(const Models::FormData& data) {
    // File uploads typically can't be pre-populated for security reasons
    // But we can show what was previously uploaded
    for (auto& doc : documents_) {
        std::string uploadedField = doc.documentType + "_uploaded";
        std::string filenameField = doc.documentType + "_filename";

        if (data.hasField(uploadedField) && data.getField(uploadedField).boolValue) {
            if (data.hasField(filenameField)) {
                doc.statusText->setText("Previously uploaded: " + data.getField(filenameField).stringValue);
                doc.statusText->setStyleClass("upload-status text-muted");
            }
        }
    }
}

} // namespace Forms
} // namespace StudentIntake
