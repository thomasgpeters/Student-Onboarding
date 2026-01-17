#include "FormPdfPreviewWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WImage.h>
#include <Wt/WApplication.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormPdfPreviewWidget::FormPdfPreviewWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , toolbar_(nullptr)
    , backBtn_(nullptr)
    , printBtn_(nullptr)
    , downloadBtn_(nullptr)
    , previewContainer_(nullptr)
    , documentContent_(nullptr) {
    setupUI();
}

FormPdfPreviewWidget::~FormPdfPreviewWidget() {
}

void FormPdfPreviewWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void FormPdfPreviewWidget::setupUI() {
    addStyleClass("form-pdf-preview-widget");

    // Toolbar
    toolbar_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    toolbar_->addStyleClass("pdf-preview-toolbar");

    backBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("← Back"));
    backBtn_->addStyleClass("btn btn-secondary");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    auto spacer = toolbar_->addWidget(std::make_unique<Wt::WContainerWidget>());
    spacer->addStyleClass("pdf-toolbar-spacer");

    printBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("🖨️ Print"));
    printBtn_->addStyleClass("btn btn-primary");
    printBtn_->clicked().connect([this]() {
        // Trigger browser print dialog
        Wt::WApplication::instance()->doJavaScript(
            "window.print();"
        );
        printClicked_.emit();
    });

    downloadBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("⬇️ Download PDF"));
    downloadBtn_->addStyleClass("btn btn-success");
    downloadBtn_->clicked().connect([this]() {
        // For now, print to PDF is the download mechanism
        // In the future, this could use a server-side PDF generation library
        Wt::WApplication::instance()->doJavaScript(
            "window.print();"
        );
        downloadClicked_.emit();
    });

    // Preview container with paper styling
    previewContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    previewContainer_->addStyleClass("pdf-preview-container");

    // The document content (styled to look like a paper document)
    documentContent_ = previewContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    documentContent_->addStyleClass("pdf-document");
}

void FormPdfPreviewWidget::setFormData(const std::string& formType,
                                        const std::string& formTitle,
                                        const std::string& studentName,
                                        const std::string& studentEmail,
                                        const std::string& submissionDate,
                                        const std::vector<FormFieldData>& fields) {
    formType_ = formType;
    formTitle_ = formTitle;
    studentName_ = studentName;
    studentEmail_ = studentEmail;
    submissionDate_ = submissionDate;
    fields_ = fields;

    buildPreview();
}

void FormPdfPreviewWidget::loadFormSubmission(int submissionId) {
    if (!apiService_) {
        std::cerr << "[FormPdfPreviewWidget] No API service available" << std::endl;
        return;
    }

    try {
        // Load the form submission
        auto response = apiService_->getApiClient()->get("/FormSubmission/" + std::to_string(submissionId));
        if (response.success) {
            auto json = nlohmann::json::parse(response.body);

            // Handle JSON:API format
            nlohmann::json data = json.contains("data") ? json["data"] : json;
            nlohmann::json attrs = data.contains("attributes") ? data["attributes"] : data;

            int studentId = attrs.value("student_id", 0);
            int formTypeId = attrs.value("form_type_id", 0);
            std::string status = attrs.value("status", "pending");
            std::string submittedAt = attrs.value("submitted_at", "");

            // Map form_type_id to form type name
            std::map<int, std::pair<std::string, std::string>> formTypeMap = {
                {1, {"personal_info", "Personal Information Form"}},
                {2, {"emergency_contact", "Emergency Contact Form"}},
                {3, {"medical_info", "Medical Information Form"}},
                {4, {"academic_history", "Academic History Form"}},
                {5, {"financial_aid", "Financial Aid Form"}},
                {6, {"documents", "Document Upload Form"}},
                {7, {"consent", "Consent Form"}}
            };

            auto formIt = formTypeMap.find(formTypeId);
            std::string formType = formIt != formTypeMap.end() ? formIt->second.first : "unknown";
            std::string formTitle = formIt != formTypeMap.end() ? formIt->second.second : "Form";

            // Load student data
            std::string studentName = "Unknown Student";
            std::string studentEmail = "";

            auto studentResponse = apiService_->getApiClient()->get("/Student/" + std::to_string(studentId));
            if (studentResponse.success) {
                auto studentJson = nlohmann::json::parse(studentResponse.body);
                nlohmann::json studentData = studentJson.contains("data") ? studentJson["data"] : studentJson;
                nlohmann::json studentAttrs = studentData.contains("attributes") ? studentData["attributes"] : studentData;

                std::string firstName = studentAttrs.value("first_name", "");
                std::string lastName = studentAttrs.value("last_name", "");
                studentName = firstName + " " + lastName;
                studentEmail = studentAttrs.value("email", "");
            }

            // Build form fields based on form type
            std::vector<FormFieldData> fields;

            // Load form-specific data based on formType
            if (formType == "personal_info") {
                // Load from Student table
                auto personalResponse = apiService_->getApiClient()->get("/Student/" + std::to_string(studentId));
                if (personalResponse.success) {
                    auto pJson = nlohmann::json::parse(personalResponse.body);
                    nlohmann::json pData = pJson.contains("data") ? pJson["data"] : pJson;
                    nlohmann::json pAttrs = pData.contains("attributes") ? pData["attributes"] : pData;

                    fields.push_back({"First Name", pAttrs.value("first_name", ""), "text"});
                    fields.push_back({"Last Name", pAttrs.value("last_name", ""), "text"});
                    fields.push_back({"Email", pAttrs.value("email", ""), "email"});
                    fields.push_back({"Phone", pAttrs.value("phone_number", ""), "phone"});
                    fields.push_back({"Date of Birth", pAttrs.value("date_of_birth", ""), "date"});
                    fields.push_back({"Gender", pAttrs.value("gender", ""), "text"});
                    fields.push_back({"Preferred Pronouns", pAttrs.value("preferred_pronouns", ""), "text"});
                    fields.push_back({"Citizenship Status", pAttrs.value("citizenship_status", ""), "text"});
                }
            } else if (formType == "emergency_contact") {
                auto ecResponse = apiService_->getApiClient()->get("/EmergencyContact?filter[student_id]=" + std::to_string(studentId));
                if (ecResponse.success) {
                    auto ecJson = nlohmann::json::parse(ecResponse.body);
                    nlohmann::json ecItems = ecJson.is_array() ? ecJson : (ecJson.contains("data") ? ecJson["data"] : nlohmann::json::array());

                    if (!ecItems.empty()) {
                        nlohmann::json ec = ecItems[0];
                        nlohmann::json ecAttrs = ec.contains("attributes") ? ec["attributes"] : ec;

                        fields.push_back({"Contact Name", ecAttrs.value("first_name", "") + " " + ecAttrs.value("last_name", ""), "text"});
                        fields.push_back({"Relationship", ecAttrs.value("contact_relationship", ""), "text"});
                        fields.push_back({"Phone", ecAttrs.value("phone", ""), "phone"});
                        fields.push_back({"Email", ecAttrs.value("email", ""), "email"});
                        fields.push_back({"Address", ecAttrs.value("street1", ""), "text"});
                        fields.push_back({"City", ecAttrs.value("city", ""), "text"});
                        fields.push_back({"State", ecAttrs.value("state", ""), "text"});
                        fields.push_back({"Postal Code", ecAttrs.value("postal_code", ""), "text"});
                    }
                }
            } else if (formType == "academic_history") {
                auto ahResponse = apiService_->getApiClient()->get("/AcademicHistory?filter[student_id]=" + std::to_string(studentId));
                if (ahResponse.success) {
                    auto ahJson = nlohmann::json::parse(ahResponse.body);
                    nlohmann::json ahItems = ahJson.is_array() ? ahJson : (ahJson.contains("data") ? ahJson["data"] : nlohmann::json::array());

                    if (!ahItems.empty()) {
                        nlohmann::json ah = ahItems[0];
                        nlohmann::json ahAttrs = ah.contains("attributes") ? ah["attributes"] : ah;

                        fields.push_back({"Institution Name", ahAttrs.value("institution_name", ""), "text"});
                        fields.push_back({"Institution Type", ahAttrs.value("institution_type", ""), "text"});
                        fields.push_back({"City", ahAttrs.value("institution_city", ""), "text"});
                        fields.push_back({"State", ahAttrs.value("institution_state", ""), "text"});
                        fields.push_back({"Degree Earned", ahAttrs.value("degree_earned", ""), "text"});
                        fields.push_back({"Major", ahAttrs.value("major", ""), "text"});
                        if (ahAttrs.contains("gpa") && !ahAttrs["gpa"].is_null()) {
                            fields.push_back({"GPA", std::to_string(ahAttrs.value("gpa", 0.0)), "text"});
                        }
                        fields.push_back({"Graduation Date", ahAttrs.value("graduation_date", ""), "date"});
                    }
                }
            } else if (formType == "financial_aid") {
                auto faResponse = apiService_->getApiClient()->get("/FinancialAid?filter[student_id]=" + std::to_string(studentId));
                if (faResponse.success) {
                    auto faJson = nlohmann::json::parse(faResponse.body);
                    nlohmann::json faItems = faJson.is_array() ? faJson : (faJson.contains("data") ? faJson["data"] : nlohmann::json::array());

                    if (!faItems.empty()) {
                        nlohmann::json fa = faItems[0];
                        nlohmann::json faAttrs = fa.contains("attributes") ? fa["attributes"] : fa;

                        fields.push_back({"FAFSA Completed", faAttrs.value("fafsa_completed", false) ? "Yes" : "No", "text"});
                        fields.push_back({"Employment Status", faAttrs.value("employment_status", ""), "text"});
                        fields.push_back({"Employer", faAttrs.value("employer_name", ""), "text"});
                        fields.push_back({"Aid Types Requested", faAttrs.value("aid_types", ""), "text"});
                        if (faAttrs.contains("dependents_count") && !faAttrs["dependents_count"].is_null()) {
                            fields.push_back({"Number of Dependents", std::to_string(faAttrs.value("dependents_count", 0)), "text"});
                        }
                    }
                }
            } else if (formType == "consent") {
                auto cResponse = apiService_->getApiClient()->get("/Consent?filter[student_id]=" + std::to_string(studentId));
                if (cResponse.success) {
                    auto cJson = nlohmann::json::parse(cResponse.body);
                    nlohmann::json cItems = cJson.is_array() ? cJson : (cJson.contains("data") ? cJson["data"] : nlohmann::json::array());

                    if (!cItems.empty()) {
                        nlohmann::json c = cItems[0];
                        nlohmann::json cAttrs = c.contains("attributes") ? c["attributes"] : c;

                        fields.push_back({"Consent Type", cAttrs.value("consent_type", ""), "text"});
                        fields.push_back({"Consent Accepted", cAttrs.value("is_accepted", false) ? "Yes" : "No", "text"});
                        fields.push_back({"Electronic Signature", cAttrs.value("electronic_signature", ""), "text"});
                        fields.push_back({"Signature Date", cAttrs.value("signature_date", ""), "date"});
                    }
                }
            }

            // Set the form data and build preview
            setFormData(formType, formTitle, studentName, studentEmail, submittedAt, fields);

        }
    } catch (const std::exception& e) {
        std::cerr << "[FormPdfPreviewWidget] Error loading submission: " << e.what() << std::endl;
    }
}

void FormPdfPreviewWidget::loadStudentForms(int studentId) {
    // Implementation for loading all forms for a student
    // This could be expanded to show a multi-page PDF with all forms
    std::cerr << "[FormPdfPreviewWidget] loadStudentForms not yet implemented for student: " << studentId << std::endl;
}

void FormPdfPreviewWidget::clearPreview() {
    if (documentContent_) {
        documentContent_->clear();
    }
}

void FormPdfPreviewWidget::buildPreview() {
    clearPreview();

    // Document header with logo and institution info
    auto header = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("pdf-header");

    auto logoContainer = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    logoContainer->addStyleClass("pdf-logo-container");

    auto institutionName = logoContainer->addWidget(std::make_unique<Wt::WText>("University Student Onboarding System"));
    institutionName->addStyleClass("pdf-institution-name");

    auto formTitleText = header->addWidget(std::make_unique<Wt::WText>(formTitle_));
    formTitleText->addStyleClass("pdf-form-title");

    // Submission info bar
    auto infoBar = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    infoBar->addStyleClass("pdf-info-bar");

    auto studentInfoContainer = infoBar->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentInfoContainer->addStyleClass("pdf-student-info");

    auto studentNameLabel = studentInfoContainer->addWidget(std::make_unique<Wt::WText>("Student: "));
    studentNameLabel->addStyleClass("pdf-label");
    auto studentNameValue = studentInfoContainer->addWidget(std::make_unique<Wt::WText>(studentName_));
    studentNameValue->addStyleClass("pdf-value");

    studentInfoContainer->addWidget(std::make_unique<Wt::WBreak>());

    auto emailLabel = studentInfoContainer->addWidget(std::make_unique<Wt::WText>("Email: "));
    emailLabel->addStyleClass("pdf-label");
    auto emailValue = studentInfoContainer->addWidget(std::make_unique<Wt::WText>(studentEmail_));
    emailValue->addStyleClass("pdf-value");

    auto dateContainer = infoBar->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateContainer->addStyleClass("pdf-date-info");

    auto dateLabel = dateContainer->addWidget(std::make_unique<Wt::WText>("Submitted: "));
    dateLabel->addStyleClass("pdf-label");
    auto dateValue = dateContainer->addWidget(std::make_unique<Wt::WText>(formatDate(submissionDate_)));
    dateValue->addStyleClass("pdf-value");

    // Form fields table
    auto fieldsSection = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsSection->addStyleClass("pdf-fields-section");

    auto fieldsTitle = fieldsSection->addWidget(std::make_unique<Wt::WText>("Form Data"));
    fieldsTitle->addStyleClass("pdf-section-title");

    auto fieldsTable = fieldsSection->addWidget(std::make_unique<Wt::WTable>());
    fieldsTable->addStyleClass("pdf-fields-table");

    // Table header
    fieldsTable->setHeaderCount(1);
    fieldsTable->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Field"));
    fieldsTable->elementAt(0, 0)->addStyleClass("pdf-table-header");
    fieldsTable->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Value"));
    fieldsTable->elementAt(0, 1)->addStyleClass("pdf-table-header");

    // Field rows
    int row = 1;
    for (const auto& field : fields_) {
        if (!field.value.empty()) {
            fieldsTable->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(field.label));
            fieldsTable->elementAt(row, 0)->addStyleClass("pdf-field-label");
            fieldsTable->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(formatValue(field.value, field.type)));
            fieldsTable->elementAt(row, 1)->addStyleClass("pdf-field-value");
            row++;
        }
    }

    // Footer
    auto footer = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("pdf-footer");

    // Get current date/time for generation timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::localtime(&time_t_now);
    char timestamp[64];
    std::strftime(timestamp, sizeof(timestamp), "%B %d, %Y at %I:%M %p", &tm_now);

    auto generatedText = footer->addWidget(std::make_unique<Wt::WText>(
        "Document generated on " + std::string(timestamp)));
    generatedText->addStyleClass("pdf-generated-text");

    auto confidentialText = footer->addWidget(std::make_unique<Wt::WText>(
        "CONFIDENTIAL - This document contains sensitive student information"));
    confidentialText->addStyleClass("pdf-confidential-text");
}

std::string FormPdfPreviewWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    try {
        if (dateStr.length() >= 10) {
            std::string year = dateStr.substr(0, 4);
            std::string month = dateStr.substr(5, 2);
            std::string day = dateStr.substr(8, 2);

            std::vector<std::string> months = {"January", "February", "March", "April", "May", "June",
                "July", "August", "September", "October", "November", "December"};

            int monthIdx = std::stoi(month) - 1;
            if (monthIdx >= 0 && monthIdx < 12) {
                return months[monthIdx] + " " + std::to_string(std::stoi(day)) + ", " + year;
            }
        }
    } catch (...) {
        // Fall through
    }

    return dateStr;
}

std::string FormPdfPreviewWidget::formatValue(const std::string& value, const std::string& type) {
    if (value.empty()) return "-";

    if (type == "date") {
        return formatDate(value);
    }

    if (type == "phone") {
        // Simple phone formatting
        std::string digits;
        for (char c : value) {
            if (std::isdigit(c)) {
                digits += c;
            }
        }
        if (digits.length() == 10) {
            return "(" + digits.substr(0, 3) + ") " + digits.substr(3, 3) + "-" + digits.substr(6, 4);
        }
    }

    return value;
}

} // namespace Admin
} // namespace StudentIntake
