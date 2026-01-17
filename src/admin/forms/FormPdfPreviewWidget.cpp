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
    : WDialog("Form Preview")
    , apiService_(nullptr)
    , toolbar_(nullptr)
    , printBtn_(nullptr)
    , downloadBtn_(nullptr)
    , closeBtn_(nullptr)
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
    // Configure dialog properties
    setModal(true);
    setClosable(true);
    setResizable(false);
    setMovable(true);
    rejectWhenEscapePressed();

    // Set title with document icon
    setWindowTitle("Form Preview");

    // Add custom style class for sizing
    addStyleClass("pdf-preview-dialog");

    // Style the title bar with icon via CSS (icon added via ::before pseudo-element)

    // Contents container
    auto content = contents();
    content->addStyleClass("pdf-preview-dialog-content");

    // Toolbar with action buttons
    toolbar_ = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    toolbar_->addStyleClass("pdf-preview-toolbar");

    printBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("Print"));
    printBtn_->addStyleClass("btn btn-primary");
    printBtn_->clicked().connect([this]() {
        // Trigger browser print dialog for the preview container
        Wt::WApplication::instance()->doJavaScript(
            "var content = document.querySelector('.pdf-preview-container');"
            "if (content) {"
            "  var printWindow = window.open('', '_blank');"
            "  printWindow.document.write('<html><head><title>Print Preview</title>');"
            "  printWindow.document.write('<link rel=\"stylesheet\" href=\"admin-styles.css\">');"
            "  printWindow.document.write('</head><body>');"
            "  printWindow.document.write(content.innerHTML);"
            "  printWindow.document.write('</body></html>');"
            "  printWindow.document.close();"
            "  printWindow.onload = function() { printWindow.print(); printWindow.close(); };"
            "}"
        );
        printClicked_.emit();
    });

    downloadBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("Download PDF"));
    downloadBtn_->addStyleClass("btn btn-success");
    downloadBtn_->clicked().connect([this]() {
        // Same as print for now - uses browser's save as PDF
        Wt::WApplication::instance()->doJavaScript(
            "var content = document.querySelector('.pdf-preview-container');"
            "if (content) {"
            "  var printWindow = window.open('', '_blank');"
            "  printWindow.document.write('<html><head><title>Download Preview</title>');"
            "  printWindow.document.write('<link rel=\"stylesheet\" href=\"admin-styles.css\">');"
            "  printWindow.document.write('</head><body>');"
            "  printWindow.document.write(content.innerHTML);"
            "  printWindow.document.write('</body></html>');"
            "  printWindow.document.close();"
            "  printWindow.onload = function() { printWindow.print(); };"
            "}"
        );
        downloadClicked_.emit();
    });

    // Spacer
    auto spacer = toolbar_->addWidget(std::make_unique<Wt::WContainerWidget>());
    spacer->addStyleClass("pdf-toolbar-spacer");

    closeBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("Close"));
    closeBtn_->addStyleClass("btn btn-secondary");
    closeBtn_->clicked().connect([this]() {
        reject();
    });

    // Preview container with paper styling
    previewContainer_ = content->addWidget(std::make_unique<Wt::WContainerWidget>());
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

void FormPdfPreviewWidget::showFormSubmission(int submissionId) {
    setWindowTitle("Form Preview");
    loadFormSubmissionData(submissionId);
    show();
}

void FormPdfPreviewWidget::showStudentForms(int studentId) {
    setWindowTitle("All Student Forms");
    loadStudentFormsData(studentId);
    show();
}

void FormPdfPreviewWidget::loadFormSubmissionData(int submissionId) {
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

void FormPdfPreviewWidget::loadStudentFormsData(int studentId) {
    std::cerr << "[FormPdfPreviewWidget] Loading all forms for student: " << studentId << std::endl;

    if (!apiService_) {
        std::cerr << "[FormPdfPreviewWidget] No API service available" << std::endl;
        return;
    }

    clearPreview();

    try {
        // First, load student information
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

        // Load all form submissions for this student
        auto submissionsResponse = apiService_->getApiClient()->get(
            "/FormSubmission?filter[student_id]=" + std::to_string(studentId));

        if (!submissionsResponse.success) {
            std::cerr << "[FormPdfPreviewWidget] Failed to load submissions for student" << std::endl;
            return;
        }

        auto submissionsJson = nlohmann::json::parse(submissionsResponse.body);
        nlohmann::json submissions = submissionsJson.is_array() ? submissionsJson :
            (submissionsJson.contains("data") ? submissionsJson["data"] : nlohmann::json::array());

        if (submissions.empty()) {
            // No submissions found
            auto noDataMsg = documentContent_->addWidget(
                std::make_unique<Wt::WText>("<p class='text-muted'>No form submissions found for this student.</p>"));
            noDataMsg->setTextFormat(Wt::TextFormat::XHTML);
            return;
        }

        std::cerr << "[FormPdfPreviewWidget] Found " << submissions.size() << " submissions" << std::endl;

        // Form type mapping
        std::map<int, std::pair<std::string, std::string>> formTypeMap = {
            {1, {"personal_info", "Personal Information Form"}},
            {2, {"emergency_contact", "Emergency Contact Form"}},
            {3, {"medical_info", "Medical Information Form"}},
            {4, {"academic_history", "Academic History Form"}},
            {5, {"financial_aid", "Financial Aid Form"}},
            {6, {"documents", "Document Upload Form"}},
            {7, {"consent", "Consent Form"}}
        };

        // Build a combined document for all forms
        // Document header
        auto header = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
        header->addStyleClass("pdf-header");

        auto logoRow = header->addWidget(std::make_unique<Wt::WContainerWidget>());
        logoRow->addStyleClass("pdf-logo-row");

        // Company Logo from Imagery Business Systems
        auto logo = logoRow->addWidget(std::make_unique<Wt::WImage>(
            "https://media.licdn.com/dms/image/v2/D4E0BAQFNqqJ59i1lgQ/company-logo_200_200/company-logo_200_200/0/1733939002925/imagery_business_systems_llc_logo?e=2147483647&v=beta&t=s_hATe0kqIDc64S79VJYXNS4N_UwrcnUA1x7VCb3sFA"));
        logo->addStyleClass("pdf-logo-img");

        auto institutionName = logoRow->addWidget(std::make_unique<Wt::WText>("Student Intake System"));
        institutionName->addStyleClass("pdf-institution-name");

        // Main title for all forms
        auto mainTitle = header->addWidget(std::make_unique<Wt::WText>(
            "<h1 class='pdf-main-title'>Complete Student Intake Forms</h1>"));
        mainTitle->setTextFormat(Wt::TextFormat::XHTML);

        // Student info banner
        auto studentInfo = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
        studentInfo->addStyleClass("pdf-student-info-banner");

        auto studentDetails = studentInfo->addWidget(std::make_unique<Wt::WText>(
            "<strong>Student:</strong> " + studentName + " &nbsp;|&nbsp; <strong>Email:</strong> " + studentEmail +
            " &nbsp;|&nbsp; <strong>Forms:</strong> " + std::to_string(submissions.size()) + " submission(s)"));
        studentDetails->setTextFormat(Wt::TextFormat::XHTML);

        // Process each form submission
        int formIndex = 0;
        for (const auto& submission : submissions) {
            nlohmann::json attrs = submission.contains("attributes") ? submission["attributes"] : submission;

            int formTypeId = attrs.value("form_type_id", 0);
            std::string submittedAt = attrs.value("submitted_at", "");
            std::string status = attrs.value("status", "pending");

            auto formIt = formTypeMap.find(formTypeId);
            std::string formType = formIt != formTypeMap.end() ? formIt->second.first : "unknown";
            std::string formTitle = formIt != formTypeMap.end() ? formIt->second.second : "Form";

            // Add a page break separator between forms (except first)
            if (formIndex > 0) {
                auto pageBreak = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
                pageBreak->addStyleClass("pdf-page-break");
            }

            // Form section header
            auto formSection = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
            formSection->addStyleClass("pdf-form-section");

            auto formHeader = formSection->addWidget(std::make_unique<Wt::WContainerWidget>());
            formHeader->addStyleClass("pdf-form-header");

            auto formTitleText = formHeader->addWidget(std::make_unique<Wt::WText>(
                "<h2>" + formTitle + "</h2>"));
            formTitleText->setTextFormat(Wt::TextFormat::XHTML);

            // Status badge
            std::string statusClass = "badge-secondary";
            if (status == "approved") statusClass = "badge-success";
            else if (status == "pending") statusClass = "badge-warning";
            else if (status == "rejected") statusClass = "badge-danger";
            else if (status == "needs_revision") statusClass = "badge-info";

            std::string statusDisplay = status;
            if (!statusDisplay.empty()) statusDisplay[0] = std::toupper(statusDisplay[0]);

            auto statusBadge = formHeader->addWidget(std::make_unique<Wt::WText>(
                "<span class='badge " + statusClass + "'>" + statusDisplay + "</span>"));
            statusBadge->setTextFormat(Wt::TextFormat::XHTML);

            // Submission date
            auto dateInfo = formSection->addWidget(std::make_unique<Wt::WText>(
                "<p class='pdf-date-info'>Submitted: " + formatDate(submittedAt) + "</p>"));
            dateInfo->setTextFormat(Wt::TextFormat::XHTML);

            // Load form fields for this submission
            std::vector<FormFieldData> fields;

            // Load data based on form type (similar to loadFormSubmissionData)
            if (formType == "personal_info") {
                // Personal info is in the Student table
                if (studentResponse.success) {
                    auto studentJson = nlohmann::json::parse(studentResponse.body);
                    nlohmann::json studentData = studentJson.contains("data") ? studentJson["data"] : studentJson;
                    nlohmann::json sAttrs = studentData.contains("attributes") ? studentData["attributes"] : studentData;

                    fields.push_back({"First Name", sAttrs.value("first_name", ""), "text"});
                    fields.push_back({"Last Name", sAttrs.value("last_name", ""), "text"});
                    fields.push_back({"Email", sAttrs.value("email", ""), "email"});
                    fields.push_back({"Date of Birth", sAttrs.value("date_of_birth", ""), "date"});
                    fields.push_back({"Gender", sAttrs.value("gender", ""), "text"});
                    fields.push_back({"Citizenship", sAttrs.value("citizenship_status", ""), "text"});
                }
            } else if (formType == "emergency_contact") {
                auto ecResponse = apiService_->getApiClient()->get("/EmergencyContact?filter[student_id]=" + std::to_string(studentId));
                if (ecResponse.success) {
                    auto ecJson = nlohmann::json::parse(ecResponse.body);
                    nlohmann::json ecItems = ecJson.is_array() ? ecJson : (ecJson.contains("data") ? ecJson["data"] : nlohmann::json::array());
                    for (size_t i = 0; i < ecItems.size() && i < 2; i++) {
                        nlohmann::json ec = ecItems[i];
                        nlohmann::json ecAttrs = ec.contains("attributes") ? ec["attributes"] : ec;
                        std::string prefix = (i == 0) ? "Primary " : "Secondary ";
                        fields.push_back({prefix + "Contact Name", ecAttrs.value("first_name", "") + " " + ecAttrs.value("last_name", ""), "text"});
                        fields.push_back({prefix + "Relationship", ecAttrs.value("relationship", ""), "text"});
                        fields.push_back({prefix + "Phone", ecAttrs.value("phone_number", ""), "phone"});
                    }
                }
            } else if (formType == "medical_info") {
                auto medResponse = apiService_->getApiClient()->get("/MedicalInfo?filter[student_id]=" + std::to_string(studentId));
                if (medResponse.success) {
                    auto medJson = nlohmann::json::parse(medResponse.body);
                    nlohmann::json medItems = medJson.is_array() ? medJson : (medJson.contains("data") ? medJson["data"] : nlohmann::json::array());
                    if (!medItems.empty()) {
                        nlohmann::json med = medItems[0];
                        nlohmann::json medAttrs = med.contains("attributes") ? med["attributes"] : med;
                        fields.push_back({"Insurance Provider", medAttrs.value("insurance_provider", ""), "text"});
                        fields.push_back({"Policy Number", medAttrs.value("policy_number", ""), "text"});
                        fields.push_back({"Primary Physician", medAttrs.value("primary_physician_name", ""), "text"});
                        fields.push_back({"Known Allergies", medAttrs.value("allergies", ""), "text"});
                    }
                }
            } else if (formType == "academic_history") {
                auto ahResponse = apiService_->getApiClient()->get("/AcademicHistory?filter[student_id]=" + std::to_string(studentId));
                if (ahResponse.success) {
                    auto ahJson = nlohmann::json::parse(ahResponse.body);
                    nlohmann::json ahItems = ahJson.is_array() ? ahJson : (ahJson.contains("data") ? ahJson["data"] : nlohmann::json::array());
                    for (const auto& ah : ahItems) {
                        nlohmann::json ahAttrs = ah.contains("attributes") ? ah["attributes"] : ah;
                        fields.push_back({"Institution", ahAttrs.value("institution_name", ""), "text"});
                        fields.push_back({"Type", ahAttrs.value("institution_type", ""), "text"});
                        fields.push_back({"Degree", ahAttrs.value("degree_earned", ""), "text"});
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
                        fields.push_back({"Consent Accepted", cAttrs.value("is_accepted", false) ? "Yes" : "No", "text"});
                        fields.push_back({"Electronic Signature", cAttrs.value("electronic_signature", ""), "text"});
                        fields.push_back({"Signature Date", cAttrs.value("signature_date", ""), "date"});
                    }
                }
            }

            // Build form data table
            if (!fields.empty()) {
                auto table = formSection->addWidget(std::make_unique<Wt::WTable>());
                table->addStyleClass("pdf-field-table");

                for (const auto& field : fields) {
                    if (!field.value.empty()) {
                        auto row = table->insertRow(table->rowCount());
                        auto labelCell = row->elementAt(0);
                        auto valueCell = row->elementAt(1);

                        labelCell->addWidget(std::make_unique<Wt::WText>(field.label));
                        labelCell->addStyleClass("pdf-field-label");

                        valueCell->addWidget(std::make_unique<Wt::WText>(formatValue(field.value, field.type)));
                        valueCell->addStyleClass("pdf-field-value");
                    }
                }
            } else {
                auto noFields = formSection->addWidget(std::make_unique<Wt::WText>(
                    "<p class='text-muted'>No data available for this form.</p>"));
                noFields->setTextFormat(Wt::TextFormat::XHTML);
            }

            formIndex++;
        }

        // Footer
        auto footer = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
        footer->addStyleClass("pdf-footer");

        auto footerText = footer->addWidget(std::make_unique<Wt::WText>(
            "This document was generated from the Student Intake System. " +
            std::to_string(submissions.size()) + " form(s) included."));
        footerText->addStyleClass("pdf-footer-text");

    } catch (const std::exception& e) {
        std::cerr << "[FormPdfPreviewWidget] Error loading student forms: " << e.what() << std::endl;
        auto errorMsg = documentContent_->addWidget(std::make_unique<Wt::WText>(
            "<p class='text-danger'>Error loading forms: " + std::string(e.what()) + "</p>"));
        errorMsg->setTextFormat(Wt::TextFormat::XHTML);
    }
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

    auto logoRow = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    logoRow->addStyleClass("pdf-logo-row");

    // Company Logo from Imagery Business Systems
    auto logo = logoRow->addWidget(std::make_unique<Wt::WImage>(
        "https://media.licdn.com/dms/image/v2/D4E0BAQFNqqJ59i1lgQ/company-logo_200_200/company-logo_200_200/0/1733939002925/imagery_business_systems_llc_logo?e=2147483647&v=beta&t=s_hATe0kqIDc64S79VJYXNS4N_UwrcnUA1x7VCb3sFA"));
    logo->addStyleClass("pdf-logo-img");

    auto institutionInfo = logoRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    institutionInfo->addStyleClass("pdf-institution-info");

    auto institutionName = institutionInfo->addWidget(std::make_unique<Wt::WText>("Student Intake System"));
    institutionName->addStyleClass("pdf-institution-name");

    auto institutionTagline = institutionInfo->addWidget(std::make_unique<Wt::WText>("Official Student Records"));
    institutionTagline->addStyleClass("pdf-institution-tagline");

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
