#include "FormPdfPreviewWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WImage.h>
#include <Wt/WApplication.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormPdfPreviewWidget::FormPdfPreviewWidget()
    : WDialog("Form Preview")
    , apiService_(nullptr)
    , pdfGenerator_(std::make_unique<Api::PdfGenerator>())
    , toolbar_(nullptr)
    , printBtn_(nullptr)
    , downloadBtn_(nullptr)
    , closeBtn_(nullptr)
    , previewContainer_(nullptr)
    , documentContent_(nullptr) {
    setupUI();

    if (Api::PdfGenerator::isAvailable()) {
        std::cerr << "[FormPdfPreviewWidget] PDF generation is available (libharu found)" << std::endl;
    } else {
        std::cerr << "[FormPdfPreviewWidget] PDF generation not available - using HTML fallback" << std::endl;
    }
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
        if (Api::PdfGenerator::isAvailable()) {
            // Generate PDF and open in new tab for printing
            std::string pdfPath = generatePdf();
            if (!pdfPath.empty()) {
                // Create a file resource and store as member to keep it alive
                auto app = Wt::WApplication::instance();
                pdfResource_ = std::make_shared<Wt::WFileResource>("application/pdf", pdfPath);
                pdfResource_->suggestFileName(formTitle_ + ".pdf");
                // Open in new tab for printing
                app->doJavaScript(
                    "window.open('" + pdfResource_->url() + "', '_blank');"
                );
            }
        } else {
            // Fallback: Use CSS to hide everything except PDF content and trigger print
            Wt::WApplication::instance()->doJavaScript(
                "var style = document.createElement('style');"
                "style.id = 'print-style';"
                "style.innerHTML = '@media print { body * { visibility: hidden; } .pdf-document, .pdf-document * { visibility: visible; } .pdf-document { position: absolute; left: 0; top: 0; width: 100%; transform: none !important; margin: 0 !important; box-shadow: none !important; } }';"
                "document.head.appendChild(style);"
                "window.print();"
                "setTimeout(function() { var ps = document.getElementById(\"print-style\"); if(ps) ps.remove(); }, 1000);"
            );
        }
        printClicked_.emit();
    });

    downloadBtn_ = toolbar_->addWidget(std::make_unique<Wt::WPushButton>("Download PDF"));
    downloadBtn_->addStyleClass("btn btn-success");
    downloadBtn_->clicked().connect([this]() {
        if (Api::PdfGenerator::isAvailable()) {
            // Generate PDF and trigger download
            downloadPdf();
        } else {
            // Fallback: Use browser print-to-PDF
            Wt::WApplication::instance()->doJavaScript(
                "var style = document.createElement('style');"
                "style.id = 'print-style';"
                "style.innerHTML = '@media print { body * { visibility: hidden; } .pdf-document, .pdf-document * { visibility: visible; } .pdf-document { position: absolute; left: 0; top: 0; width: 100%; transform: none !important; margin: 0 !important; box-shadow: none !important; } }';"
                "document.head.appendChild(style);"
                "window.print();"
                "setTimeout(function() { var ps = document.getElementById(\"print-style\"); if(ps) ps.remove(); }, 1000);"
            );
        }
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

    // Clear previous content before loading new data
    clearPreview();

    std::cerr << "[FormPdfPreviewWidget] Loading form submission: " << submissionId << std::endl;

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

            std::cerr << "[FormPdfPreviewWidget] Submission " << submissionId
                      << " - student_id: " << studentId
                      << ", form_type_id: " << formTypeId
                      << ", status: " << status << std::endl;

            // Map form_type_id to form type name and display title
            // Each entry: {form_type_key, {document_title, window_title}}
            std::map<int, std::tuple<std::string, std::string, std::string>> formTypeMap = {
                {1, {"personal_info", "Personal Information Form", "Personal Information"}},
                {2, {"emergency_contact", "Emergency Contact Form", "Emergency Contact"}},
                {3, {"medical_info", "Medical Information Form", "Medical Information"}},
                {4, {"academic_history", "Academic History Form", "Academic History"}},
                {5, {"financial_aid", "Financial Aid Form", "Financial Aid"}},
                {6, {"documents", "Document Upload Form", "Document Upload"}},
                {7, {"consent", "Consent Form", "Consent"}}
            };

            auto formIt = formTypeMap.find(formTypeId);
            std::string formType = formIt != formTypeMap.end() ? std::get<0>(formIt->second) : "unknown";
            std::string formTitle = formIt != formTypeMap.end() ? std::get<1>(formIt->second) : "Form";
            std::string windowTitle = formIt != formTypeMap.end() ? std::get<2>(formIt->second) : "Form Preview";

            // Set the window title to the form name with icon
            setWindowTitle("📋 " + windowTitle);

            std::cerr << "[FormPdfPreviewWidget] Form type: " << formType << ", title: " << formTitle << std::endl;

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
                std::string ecUrl = "/EmergencyContact?filter[student_id]=" + std::to_string(studentId);
                std::cerr << "[FormPdfPreviewWidget] Loading emergency contacts from: " << ecUrl << std::endl;
                auto ecResponse = apiService_->getApiClient()->get(ecUrl);
                std::cerr << "[FormPdfPreviewWidget] EC response success: " << ecResponse.success
                          << ", status: " << ecResponse.statusCode
                          << ", body length: " << ecResponse.body.length() << std::endl;
                if (ecResponse.success && !ecResponse.body.empty()) {
                    std::cerr << "[FormPdfPreviewWidget] EC response body: " << ecResponse.body.substr(0, 500) << std::endl;
                    auto ecJson = nlohmann::json::parse(ecResponse.body);
                    nlohmann::json ecItems = ecJson.is_array() ? ecJson : (ecJson.contains("data") ? ecJson["data"] : nlohmann::json::array());
                    std::cerr << "[FormPdfPreviewWidget] Found " << ecItems.size() << " emergency contacts" << std::endl;

                    int contactNum = 1;
                    for (const auto& ec : ecItems) {
                        nlohmann::json ecAttrs = ec.contains("attributes") ? ec["attributes"] : ec;

                        // Determine contact label
                        std::string contactLabel;
                        bool isPrimary = ecAttrs.value("is_primary", false);
                        int priority = ecAttrs.value("priority", 0);

                        if (isPrimary || priority == 1) {
                            contactLabel = "Primary Contact";
                        } else if (priority == 2) {
                            contactLabel = "Secondary Contact";
                        } else {
                            contactLabel = "Contact #" + std::to_string(contactNum);
                        }

                        if (contactNum > 1) {
                            fields.push_back({"", "", "text"});
                        }
                        fields.push_back({contactLabel, "", "header"});

                        std::string fullName = ecAttrs.value("first_name", "") + " " + ecAttrs.value("last_name", "");
                        fields.push_back({"Name", fullName, "text"});
                        fields.push_back({"Relationship", ecAttrs.value("contact_relationship", ""), "text"});
                        fields.push_back({"Phone", ecAttrs.value("phone", ""), "phone"});

                        std::string altPhone = ecAttrs.value("alternate_phone", "");
                        if (!altPhone.empty()) {
                            fields.push_back({"Alternate Phone", altPhone, "phone"});
                        }
                        fields.push_back({"Email", ecAttrs.value("email", ""), "email"});

                        // Build address
                        std::string street1 = ecAttrs.value("street1", "");
                        std::string street2 = ecAttrs.value("street2", "");
                        std::string city = ecAttrs.value("city", "");
                        std::string state = ecAttrs.value("state", "");
                        std::string postalCode = ecAttrs.value("postal_code", "");
                        std::string country = ecAttrs.value("country", "");

                        if (!street1.empty()) {
                            std::string fullAddress = street1;
                            if (!street2.empty()) fullAddress += ", " + street2;
                            fields.push_back({"Street Address", fullAddress, "text"});
                        }
                        if (!city.empty() || !state.empty() || !postalCode.empty()) {
                            std::string cityStateZip = city;
                            if (!state.empty()) cityStateZip += (!cityStateZip.empty() ? ", " : "") + state;
                            if (!postalCode.empty()) cityStateZip += " " + postalCode;
                            fields.push_back({"City, State, Zip", cityStateZip, "text"});
                        }
                        if (!country.empty() && country != "USA" && country != "US" && country != "United States") {
                            fields.push_back({"Country", country, "text"});
                        }

                        contactNum++;
                    }
                } else {
                    std::cerr << "[FormPdfPreviewWidget] EC: No emergency contacts found in response" << std::endl;
                }
            } else if (formType == "academic_history") {
                std::string ahUrl = "/AcademicHistory?filter[student_id]=" + std::to_string(studentId);
                std::cerr << "[FormPdfPreviewWidget] Loading academic history from: " << ahUrl << std::endl;
                auto ahResponse = apiService_->getApiClient()->get(ahUrl);
                std::cerr << "[FormPdfPreviewWidget] AH response success: " << ahResponse.success
                          << ", status: " << ahResponse.statusCode << std::endl;
                if (ahResponse.success && !ahResponse.body.empty()) {
                    auto ahJson = nlohmann::json::parse(ahResponse.body);
                    nlohmann::json ahItems = ahJson.is_array() ? ahJson : (ahJson.contains("data") ? ahJson["data"] : nlohmann::json::array());

                    // Group by institution type
                    std::vector<std::string> institutionTypes = {"High School", "College", "University", "Trade School"};
                    std::map<std::string, std::vector<nlohmann::json>> groupedRecords;

                    for (const auto& ah : ahItems) {
                        nlohmann::json ahAttrs = ah.contains("attributes") ? ah["attributes"] : ah;
                        std::string instType = ahAttrs.value("institution_type", "Other");
                        groupedRecords[instType].push_back(ahAttrs);
                    }

                    bool firstGroup = true;
                    for (const auto& instType : institutionTypes) {
                        if (groupedRecords.find(instType) != groupedRecords.end()) {
                            if (!firstGroup) {
                                fields.push_back({"", "", "text"}); // Separator
                            }
                            fields.push_back({instType + " Education", "", "header"});
                            firstGroup = false;

                            int recordNum = 1;
                            for (const auto& ahAttrs : groupedRecords[instType]) {
                                if (recordNum > 1) {
                                    fields.push_back({"", "", "text"});
                                }

                                fields.push_back({"Institution Name", ahAttrs.value("institution_name", ""), "text"});

                                // Location
                                std::string city = ahAttrs.value("institution_city", "");
                                std::string state = ahAttrs.value("institution_state", "");
                                std::string country = ahAttrs.value("institution_country", "");
                                std::string location;
                                if (!city.empty()) location = city;
                                if (!state.empty()) location += (!location.empty() ? ", " : "") + state;
                                if (!country.empty() && country != "USA" && country != "US" && country != "United States") {
                                    location += (!location.empty() ? ", " : "") + country;
                                }
                                if (!location.empty()) {
                                    fields.push_back({"Location", location, "text"});
                                }

                                // Degree and major/minor
                                std::string degree = ahAttrs.value("degree_earned", "");
                                if (!degree.empty()) {
                                    fields.push_back({"Degree Earned", degree, "text"});
                                }
                                std::string major = ahAttrs.value("major", "");
                                if (!major.empty()) {
                                    fields.push_back({"Major", major, "text"});
                                }
                                std::string minor = ahAttrs.value("minor", "");
                                if (!minor.empty()) {
                                    fields.push_back({"Minor", minor, "text"});
                                }

                                // GPA
                                if (ahAttrs.contains("gpa") && !ahAttrs["gpa"].is_null()) {
                                    double gpa = ahAttrs.value("gpa", 0.0);
                                    double gpaScale = ahAttrs.value("gpa_scale", 4.0);
                                    std::ostringstream gpaStr;
                                    gpaStr << std::fixed << std::setprecision(2) << gpa << " / " << gpaScale;
                                    fields.push_back({"GPA", gpaStr.str(), "text"});
                                }

                                // Dates
                                std::string startDate = ahAttrs.value("start_date", "");
                                std::string endDate = ahAttrs.value("end_date", "");
                                bool currentlyAttending = ahAttrs.value("is_currently_attending", false);

                                if (!startDate.empty() || !endDate.empty()) {
                                    std::string dateRange = formatDate(startDate) + " - ";
                                    if (currentlyAttending) {
                                        dateRange += "Present";
                                    } else if (!endDate.empty()) {
                                        dateRange += formatDate(endDate);
                                    }
                                    fields.push_back({"Attendance Period", dateRange, "text"});
                                }

                                std::string gradDate = ahAttrs.value("graduation_date", "");
                                if (!gradDate.empty()) {
                                    fields.push_back({"Graduation Date", gradDate, "date"});
                                }

                                // Transcript status
                                bool transcriptReceived = ahAttrs.value("transcript_received", false);
                                fields.push_back({"Transcript Received", transcriptReceived ? "Yes" : "No", "text"});

                                recordNum++;
                            }
                        }
                    }

                    // Handle any "Other" types not in the standard list
                    for (const auto& pair : groupedRecords) {
                        if (std::find(institutionTypes.begin(), institutionTypes.end(), pair.first) == institutionTypes.end()) {
                            if (!firstGroup) {
                                fields.push_back({"", "", "text"});
                            }
                            fields.push_back({pair.first + " Education", "", "header"});
                            firstGroup = false;

                            for (const auto& ahAttrs : pair.second) {
                                fields.push_back({"Institution Name", ahAttrs.value("institution_name", ""), "text"});
                                fields.push_back({"Degree Earned", ahAttrs.value("degree_earned", ""), "text"});
                                fields.push_back({"Graduation Date", ahAttrs.value("graduation_date", ""), "date"});
                            }
                        }
                    }
                }
            } else if (formType == "medical_info") {
                std::string medUrl = "/MedicalInfo?filter[student_id]=" + std::to_string(studentId);
                std::cerr << "[FormPdfPreviewWidget] Loading medical info from: " << medUrl << std::endl;
                auto medResponse = apiService_->getApiClient()->get(medUrl);
                std::cerr << "[FormPdfPreviewWidget] Med response success: " << medResponse.success
                          << ", status: " << medResponse.statusCode << std::endl;
                if (medResponse.success && !medResponse.body.empty()) {
                    auto medJson = nlohmann::json::parse(medResponse.body);
                    nlohmann::json medItems = medJson.is_array() ? medJson : (medJson.contains("data") ? medJson["data"] : nlohmann::json::array());
                    std::cerr << "[FormPdfPreviewWidget] Found " << medItems.size() << " medical info records" << std::endl;
                    if (!medItems.empty()) {
                        nlohmann::json med = medItems[0];
                        nlohmann::json medAttrs = med.contains("attributes") ? med["attributes"] : med;

                        // Basic medical info
                        fields.push_back({"Medical Information", "", "header"});
                        fields.push_back({"Blood Type", medAttrs.value("blood_type", ""), "text"});

                        // Allergies
                        bool hasAllergies = medAttrs.value("has_allergies", false);
                        fields.push_back({"Has Allergies", hasAllergies ? "Yes" : "No", "text"});
                        if (hasAllergies) {
                            fields.push_back({"Allergies", medAttrs.value("allergies", ""), "text"});
                        }

                        // Medications
                        bool hasMedications = medAttrs.value("has_medications", false);
                        fields.push_back({"Currently on Medications", hasMedications ? "Yes" : "No", "text"});
                        if (hasMedications) {
                            fields.push_back({"Medications", medAttrs.value("medications", ""), "text"});
                        }

                        // Chronic conditions
                        bool hasChronicConditions = medAttrs.value("has_chronic_conditions", false);
                        fields.push_back({"Has Chronic Conditions", hasChronicConditions ? "Yes" : "No", "text"});
                        if (hasChronicConditions) {
                            fields.push_back({"Chronic Conditions", medAttrs.value("chronic_conditions", ""), "text"});
                        }

                        // Disabilities
                        bool hasDisabilities = medAttrs.value("has_disabilities", false);
                        fields.push_back({"Has Disabilities", hasDisabilities ? "Yes" : "No", "text"});
                        if (hasDisabilities) {
                            fields.push_back({"Disabilities", medAttrs.value("disabilities", ""), "text"});
                        }

                        // Accommodations
                        bool requiresAccommodations = medAttrs.value("requires_accommodations", false);
                        fields.push_back({"Requires Accommodations", requiresAccommodations ? "Yes" : "No", "text"});
                        if (requiresAccommodations) {
                            fields.push_back({"Accommodations Needed", medAttrs.value("accommodations_needed", ""), "text"});
                        }

                        // Immunizations
                        bool immunizationsUpToDate = medAttrs.value("immunizations_up_to_date", false);
                        fields.push_back({"Immunizations Up To Date", immunizationsUpToDate ? "Yes" : "No", "text"});

                        // Insurance information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Insurance Information", "", "header"});
                        fields.push_back({"Insurance Provider", medAttrs.value("insurance_provider", ""), "text"});
                        fields.push_back({"Policy Number", medAttrs.value("insurance_policy_number", ""), "text"});
                        fields.push_back({"Group Number", medAttrs.value("insurance_group_number", ""), "text"});
                        fields.push_back({"Insurance Phone", medAttrs.value("insurance_phone", ""), "phone"});

                        // Physician info
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Primary Care Physician", "", "header"});
                        fields.push_back({"Physician Name", medAttrs.value("primary_physician", ""), "text"});
                        fields.push_back({"Physician Phone", medAttrs.value("physician_phone", ""), "phone"});
                    }
                }
            } else if (formType == "financial_aid") {
                std::string faUrl = "/FinancialAid?filter[student_id]=" + std::to_string(studentId);
                std::cerr << "[FormPdfPreviewWidget] Loading financial aid from: " << faUrl << std::endl;
                auto faResponse = apiService_->getApiClient()->get(faUrl);
                std::cerr << "[FormPdfPreviewWidget] FA response success: " << faResponse.success
                          << ", status: " << faResponse.statusCode << std::endl;
                if (faResponse.success && !faResponse.body.empty()) {
                    auto faJson = nlohmann::json::parse(faResponse.body);
                    nlohmann::json faItems = faJson.is_array() ? faJson : (faJson.contains("data") ? faJson["data"] : nlohmann::json::array());
                    std::cerr << "[FormPdfPreviewWidget] Found " << faItems.size() << " financial aid records" << std::endl;
                    if (!faItems.empty()) {
                        nlohmann::json fa = faItems[0];
                        nlohmann::json faAttrs = fa.contains("attributes") ? fa["attributes"] : fa;

                        // FAFSA Information
                        fields.push_back({"FAFSA Information", "", "header"});
                        fields.push_back({"FAFSA Completed", faAttrs.value("fafsa_completed", false) ? "Yes" : "No", "text"});
                        fields.push_back({"FAFSA Year", faAttrs.value("fafsa_year", ""), "text"});
                        if (faAttrs.contains("efc") && !faAttrs["efc"].is_null()) {
                            std::ostringstream efcStr;
                            efcStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("efc", 0.0);
                            fields.push_back({"Expected Family Contribution (EFC)", efcStr.str(), "text"});
                        }
                        fields.push_back({"Aid Types Requested", faAttrs.value("aid_types", ""), "text"});
                        if (faAttrs.contains("requested_aid_amount") && !faAttrs["requested_aid_amount"].is_null()) {
                            std::ostringstream aidStr;
                            aidStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("requested_aid_amount", 0.0);
                            fields.push_back({"Requested Aid Amount", aidStr.str(), "text"});
                        }

                        // Employment Information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Employment Information", "", "header"});
                        fields.push_back({"Employment Status", faAttrs.value("employment_status", ""), "text"});
                        fields.push_back({"Employer Name", faAttrs.value("employer_name", ""), "text"});
                        if (faAttrs.contains("annual_income") && !faAttrs["annual_income"].is_null()) {
                            std::ostringstream incomeStr;
                            incomeStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("annual_income", 0.0);
                            fields.push_back({"Annual Income", incomeStr.str(), "text"});
                        }

                        // Household Information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Household Information", "", "header"});
                        fields.push_back({"Independent Student", faAttrs.value("is_independent", false) ? "Yes" : "No", "text"});
                        if (faAttrs.contains("household_size") && !faAttrs["household_size"].is_null()) {
                            fields.push_back({"Household Size", std::to_string(faAttrs.value("household_size", 0)), "text"});
                        }
                        if (faAttrs.contains("dependents_count") && !faAttrs["dependents_count"].is_null()) {
                            fields.push_back({"Number of Dependents", std::to_string(faAttrs.value("dependents_count", 0)), "text"});
                        }

                        // Loan Information
                        bool hasLoans = faAttrs.value("has_outstanding_loans", false);
                        fields.push_back({"Has Outstanding Loans", hasLoans ? "Yes" : "No", "text"});
                        if (hasLoans && faAttrs.contains("outstanding_loan_amount") && !faAttrs["outstanding_loan_amount"].is_null()) {
                            std::ostringstream loanStr;
                            loanStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("outstanding_loan_amount", 0.0);
                            fields.push_back({"Outstanding Loan Amount", loanStr.str(), "text"});
                        }

                        // Scholarships
                        std::string scholarships = faAttrs.value("scholarship_applications", "");
                        if (!scholarships.empty()) {
                            fields.push_back({"", "", "text"});
                            fields.push_back({"Scholarship Applications", "", "header"});
                            fields.push_back({"Scholarships", scholarships, "text"});
                        }
                    }
                }
            } else if (formType == "documents") {
                auto docResponse = apiService_->getApiClient()->get("/Document?filter[student_id]=" + std::to_string(studentId));
                if (docResponse.success) {
                    auto docJson = nlohmann::json::parse(docResponse.body);
                    nlohmann::json docItems = docJson.is_array() ? docJson : (docJson.contains("data") ? docJson["data"] : nlohmann::json::array());

                    if (!docItems.empty()) {
                        int docNum = 1;
                        for (const auto& doc : docItems) {
                            nlohmann::json docAttrs = doc.contains("attributes") ? doc["attributes"] : doc;

                            if (docNum > 1) {
                                fields.push_back({"", "", "text"});
                            }
                            fields.push_back({"Document #" + std::to_string(docNum), "", "header"});

                            fields.push_back({"Document Type", docAttrs.value("document_type", ""), "text"});
                            fields.push_back({"File Name", docAttrs.value("file_name", ""), "text"});

                            // File size in KB/MB
                            int fileSize = docAttrs.value("file_size", 0);
                            std::string sizeStr;
                            if (fileSize >= 1048576) {
                                std::ostringstream ss;
                                ss << std::fixed << std::setprecision(2) << (fileSize / 1048576.0) << " MB";
                                sizeStr = ss.str();
                            } else if (fileSize >= 1024) {
                                std::ostringstream ss;
                                ss << std::fixed << std::setprecision(1) << (fileSize / 1024.0) << " KB";
                                sizeStr = ss.str();
                            } else {
                                sizeStr = std::to_string(fileSize) + " bytes";
                            }
                            fields.push_back({"File Size", sizeStr, "text"});

                            fields.push_back({"Status", docAttrs.value("status", ""), "text"});

                            std::string verifiedAt = docAttrs.value("verified_at", "");
                            if (!verifiedAt.empty()) {
                                fields.push_back({"Verified On", verifiedAt, "date"});
                                fields.push_back({"Verified By", docAttrs.value("verified_by", ""), "text"});
                            }

                            std::string notes = docAttrs.value("notes", "");
                            if (!notes.empty()) {
                                fields.push_back({"Notes", notes, "text"});
                            }

                            docNum++;
                        }
                    } else {
                        fields.push_back({"No Documents", "No documents uploaded yet", "text"});
                    }
                }
            } else if (formType == "consent") {
                auto cResponse = apiService_->getApiClient()->get("/Consent?filter[student_id]=" + std::to_string(studentId));
                if (cResponse.success) {
                    auto cJson = nlohmann::json::parse(cResponse.body);
                    nlohmann::json cItems = cJson.is_array() ? cJson : (cJson.contains("data") ? cJson["data"] : nlohmann::json::array());

                    // Display all consent records
                    int consentNum = 1;
                    for (const auto& c : cItems) {
                        nlohmann::json cAttrs = c.contains("attributes") ? c["attributes"] : c;

                        if (consentNum > 1) {
                            fields.push_back({"", "", "text"});
                        }

                        std::string consentType = cAttrs.value("consent_type", "");
                        std::string consentVersion = cAttrs.value("consent_version", "");
                        std::string consentLabel = !consentType.empty() ? consentType : "Consent Agreement";
                        if (!consentVersion.empty()) {
                            consentLabel += " (v" + consentVersion + ")";
                        }
                        fields.push_back({consentLabel, "", "header"});

                        // Acceptance status
                        bool isAccepted = cAttrs.value("is_accepted", false);
                        fields.push_back({"Status", isAccepted ? "Accepted" : "Not Accepted", "text"});

                        // Acceptance timestamp
                        std::string acceptedAt = cAttrs.value("accepted_at", "");
                        if (!acceptedAt.empty()) {
                            fields.push_back({"Accepted On", acceptedAt, "date"});
                        }

                        // Signature information
                        std::string signature = cAttrs.value("electronic_signature", "");
                        if (!signature.empty()) {
                            fields.push_back({"Electronic Signature", signature, "text"});
                        }

                        std::string sigDate = cAttrs.value("signature_date", "");
                        if (!sigDate.empty()) {
                            fields.push_back({"Signature Date", sigDate, "date"});
                        }

                        // IP address for audit
                        std::string ipAddress = cAttrs.value("ip_address", "");
                        if (!ipAddress.empty()) {
                            fields.push_back({"IP Address", ipAddress, "text"});
                        }

                        consentNum++;
                    }

                    // If we have consent records, add a summary of what was agreed to
                    if (!cItems.empty()) {
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Acknowledgments", "", "header"});
                        fields.push_back({"Terms of Service", "Acknowledged", "text"});
                        fields.push_back({"Privacy Policy", "Acknowledged", "text"});
                        fields.push_back({"FERPA Rights", "Acknowledged", "text"});
                        fields.push_back({"Information Accuracy", "Certified", "text"});
                    }
                }
            }

            // If no fields were loaded, add a fallback message with student basic info
            if (fields.empty()) {
                std::cerr << "[FormPdfPreviewWidget] WARNING: No fields loaded for form type: " << formType << std::endl;
                fields.push_back({"Student Information", "", "header"});
                fields.push_back({"Student Name", studentName, "text"});
                fields.push_back({"Email", studentEmail, "text"});
                fields.push_back({"", "", "text"});
                fields.push_back({"Form Status", "", "header"});
                fields.push_back({"Data Status", "No detailed data has been entered for this form yet", "text"});
                fields.push_back({"Submission Status", status, "text"});
            }

            // Set the form data and build preview
            std::cerr << "[FormPdfPreviewWidget] Setting form data - fields count: " << fields.size() << std::endl;
            setFormData(formType, formTitle, studentName, studentEmail, submittedAt, fields);

        } else {
            std::cerr << "[FormPdfPreviewWidget] Failed to load form submission: " << response.errorMessage << std::endl;
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

        // Calculate total pages for footer
        int totalPages = static_cast<int>(submissions.size());

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

            // Form section container (each form is a separate page)
            auto formSection = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
            formSection->addStyleClass("pdf-form-section");
            if (formIndex > 0) {
                formSection->addStyleClass("pdf-page-break");
            }

            // Add logo and header to each form section
            auto formPageHeader = formSection->addWidget(std::make_unique<Wt::WContainerWidget>());
            formPageHeader->addStyleClass("pdf-header");

            auto logoRow = formPageHeader->addWidget(std::make_unique<Wt::WContainerWidget>());
            logoRow->addStyleClass("pdf-logo-row");

            auto formLogo = logoRow->addWidget(std::make_unique<Wt::WImage>(
                "https://media.licdn.com/dms/image/v2/D4E0BAQFNqqJ59i1lgQ/company-logo_200_200/company-logo_200_200/0/1733939002925/imagery_business_systems_llc_logo?e=2147483647&v=beta&t=s_hATe0kqIDc64S79VJYXNS4N_UwrcnUA1x7VCb3sFA"));
            formLogo->addStyleClass("pdf-logo-img");

            auto formInstitutionName = logoRow->addWidget(std::make_unique<Wt::WText>("Student Intake System"));
            formInstitutionName->addStyleClass("pdf-institution-name");

            // Form title
            auto formTitleContainer = formPageHeader->addWidget(std::make_unique<Wt::WContainerWidget>());
            formTitleContainer->addStyleClass("pdf-form-title-row");

            auto formTitleText = formTitleContainer->addWidget(std::make_unique<Wt::WText>(formTitle));
            formTitleText->addStyleClass("pdf-form-title");

            // Status badge
            std::string statusClass = "badge-secondary";
            if (status == "approved") statusClass = "badge-success";
            else if (status == "pending") statusClass = "badge-warning";
            else if (status == "rejected") statusClass = "badge-danger";
            else if (status == "needs_revision") statusClass = "badge-info";

            std::string statusDisplay = status;
            if (!statusDisplay.empty()) statusDisplay[0] = std::toupper(statusDisplay[0]);

            auto statusBadge = formTitleContainer->addWidget(std::make_unique<Wt::WText>(
                "<span class='badge " + statusClass + "'>" + statusDisplay + "</span>"));
            statusBadge->setTextFormat(Wt::TextFormat::XHTML);

            // Submission date info only
            auto formDateInfo = formSection->addWidget(std::make_unique<Wt::WContainerWidget>());
            formDateInfo->addStyleClass("pdf-info-bar");
            auto formDateDetails = formDateInfo->addWidget(std::make_unique<Wt::WText>(
                "<strong>Submitted:</strong> " + formatDate(submittedAt)));
            formDateDetails->setTextFormat(Wt::TextFormat::XHTML);

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

                    int contactNum = 1;
                    for (const auto& ec : ecItems) {
                        nlohmann::json ecAttrs = ec.contains("attributes") ? ec["attributes"] : ec;

                        // Determine contact label based on priority or is_primary
                        std::string contactLabel;
                        bool isPrimary = ecAttrs.value("is_primary", false);
                        int priority = ecAttrs.value("priority", 0);

                        if (isPrimary || priority == 1) {
                            contactLabel = "Primary Contact";
                        } else if (priority == 2) {
                            contactLabel = "Secondary Contact";
                        } else {
                            contactLabel = "Contact #" + std::to_string(contactNum);
                        }

                        // Add section header for each contact
                        if (contactNum > 1) {
                            fields.push_back({"", "", "text"}); // Empty row as separator
                        }
                        fields.push_back({contactLabel, "", "header"});

                        // Name and relationship
                        std::string fullName = ecAttrs.value("first_name", "") + " " + ecAttrs.value("last_name", "");
                        fields.push_back({"Name", fullName, "text"});
                        fields.push_back({"Relationship", ecAttrs.value("contact_relationship", ""), "text"});

                        // Contact info
                        fields.push_back({"Phone", ecAttrs.value("phone", ""), "phone"});
                        std::string altPhone = ecAttrs.value("alternate_phone", "");
                        if (!altPhone.empty()) {
                            fields.push_back({"Alternate Phone", altPhone, "phone"});
                        }
                        fields.push_back({"Email", ecAttrs.value("email", ""), "email"});

                        // Address
                        std::string street1 = ecAttrs.value("street1", "");
                        std::string street2 = ecAttrs.value("street2", "");
                        std::string city = ecAttrs.value("city", "");
                        std::string state = ecAttrs.value("state", "");
                        std::string postalCode = ecAttrs.value("postal_code", "");
                        std::string country = ecAttrs.value("country", "");

                        if (!street1.empty()) {
                            std::string fullAddress = street1;
                            if (!street2.empty()) fullAddress += ", " + street2;
                            fields.push_back({"Street Address", fullAddress, "text"});
                        }
                        if (!city.empty() || !state.empty() || !postalCode.empty()) {
                            std::string cityStateZip = city;
                            if (!state.empty()) cityStateZip += (!cityStateZip.empty() ? ", " : "") + state;
                            if (!postalCode.empty()) cityStateZip += " " + postalCode;
                            fields.push_back({"City, State, Zip", cityStateZip, "text"});
                        }
                        if (!country.empty() && country != "USA" && country != "US" && country != "United States") {
                            fields.push_back({"Country", country, "text"});
                        }

                        contactNum++;
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

                        // Basic medical info
                        fields.push_back({"Medical Information", "", "header"});
                        fields.push_back({"Blood Type", medAttrs.value("blood_type", ""), "text"});

                        bool hasAllergies = medAttrs.value("has_allergies", false);
                        fields.push_back({"Has Allergies", hasAllergies ? "Yes" : "No", "text"});
                        if (hasAllergies) {
                            fields.push_back({"Allergies", medAttrs.value("allergies", ""), "text"});
                        }

                        bool hasMedications = medAttrs.value("has_medications", false);
                        fields.push_back({"Currently on Medications", hasMedications ? "Yes" : "No", "text"});
                        if (hasMedications) {
                            fields.push_back({"Medications", medAttrs.value("medications", ""), "text"});
                        }

                        bool hasChronicConditions = medAttrs.value("has_chronic_conditions", false);
                        fields.push_back({"Has Chronic Conditions", hasChronicConditions ? "Yes" : "No", "text"});
                        if (hasChronicConditions) {
                            fields.push_back({"Chronic Conditions", medAttrs.value("chronic_conditions", ""), "text"});
                        }

                        bool hasDisabilities = medAttrs.value("has_disabilities", false);
                        fields.push_back({"Has Disabilities", hasDisabilities ? "Yes" : "No", "text"});
                        if (hasDisabilities) {
                            fields.push_back({"Disabilities", medAttrs.value("disabilities", ""), "text"});
                        }

                        bool requiresAccommodations = medAttrs.value("requires_accommodations", false);
                        fields.push_back({"Requires Accommodations", requiresAccommodations ? "Yes" : "No", "text"});
                        if (requiresAccommodations) {
                            fields.push_back({"Accommodations Needed", medAttrs.value("accommodations_needed", ""), "text"});
                        }

                        bool immunizationsUpToDate = medAttrs.value("immunizations_up_to_date", false);
                        fields.push_back({"Immunizations Up To Date", immunizationsUpToDate ? "Yes" : "No", "text"});

                        // Insurance information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Insurance Information", "", "header"});
                        fields.push_back({"Insurance Provider", medAttrs.value("insurance_provider", ""), "text"});
                        fields.push_back({"Policy Number", medAttrs.value("insurance_policy_number", ""), "text"});
                        fields.push_back({"Group Number", medAttrs.value("insurance_group_number", ""), "text"});
                        fields.push_back({"Insurance Phone", medAttrs.value("insurance_phone", ""), "phone"});

                        // Physician info
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Primary Care Physician", "", "header"});
                        fields.push_back({"Physician Name", medAttrs.value("primary_physician", ""), "text"});
                        fields.push_back({"Physician Phone", medAttrs.value("physician_phone", ""), "phone"});
                    }
                }
            } else if (formType == "academic_history") {
                auto ahResponse = apiService_->getApiClient()->get("/AcademicHistory?filter[student_id]=" + std::to_string(studentId));
                if (ahResponse.success) {
                    auto ahJson = nlohmann::json::parse(ahResponse.body);
                    nlohmann::json ahItems = ahJson.is_array() ? ahJson : (ahJson.contains("data") ? ahJson["data"] : nlohmann::json::array());

                    // Group by institution type
                    std::vector<std::string> institutionTypes = {"High School", "College", "University", "Trade School"};
                    std::map<std::string, std::vector<nlohmann::json>> groupedRecords;

                    for (const auto& ah : ahItems) {
                        nlohmann::json ahAttrs = ah.contains("attributes") ? ah["attributes"] : ah;
                        std::string instType = ahAttrs.value("institution_type", "Other");
                        groupedRecords[instType].push_back(ahAttrs);
                    }

                    bool firstGroup = true;
                    for (const auto& instType : institutionTypes) {
                        if (groupedRecords.find(instType) != groupedRecords.end()) {
                            if (!firstGroup) {
                                fields.push_back({"", "", "text"});
                            }
                            fields.push_back({instType + " Education", "", "header"});
                            firstGroup = false;

                            int recordNum = 1;
                            for (const auto& ahAttrs : groupedRecords[instType]) {
                                if (recordNum > 1) {
                                    fields.push_back({"", "", "text"});
                                }

                                fields.push_back({"Institution Name", ahAttrs.value("institution_name", ""), "text"});

                                std::string city = ahAttrs.value("institution_city", "");
                                std::string state = ahAttrs.value("institution_state", "");
                                std::string country = ahAttrs.value("institution_country", "");
                                std::string location;
                                if (!city.empty()) location = city;
                                if (!state.empty()) location += (!location.empty() ? ", " : "") + state;
                                if (!country.empty() && country != "USA" && country != "US" && country != "United States") {
                                    location += (!location.empty() ? ", " : "") + country;
                                }
                                if (!location.empty()) {
                                    fields.push_back({"Location", location, "text"});
                                }

                                std::string degree = ahAttrs.value("degree_earned", "");
                                if (!degree.empty()) {
                                    fields.push_back({"Degree Earned", degree, "text"});
                                }
                                std::string major = ahAttrs.value("major", "");
                                if (!major.empty()) {
                                    fields.push_back({"Major", major, "text"});
                                }
                                std::string minor = ahAttrs.value("minor", "");
                                if (!minor.empty()) {
                                    fields.push_back({"Minor", minor, "text"});
                                }

                                if (ahAttrs.contains("gpa") && !ahAttrs["gpa"].is_null()) {
                                    double gpa = ahAttrs.value("gpa", 0.0);
                                    double gpaScale = ahAttrs.value("gpa_scale", 4.0);
                                    std::ostringstream gpaStr;
                                    gpaStr << std::fixed << std::setprecision(2) << gpa << " / " << gpaScale;
                                    fields.push_back({"GPA", gpaStr.str(), "text"});
                                }

                                std::string startDate = ahAttrs.value("start_date", "");
                                std::string endDate = ahAttrs.value("end_date", "");
                                bool currentlyAttending = ahAttrs.value("is_currently_attending", false);
                                if (!startDate.empty() || !endDate.empty()) {
                                    std::string dateRange = formatDate(startDate) + " - ";
                                    if (currentlyAttending) {
                                        dateRange += "Present";
                                    } else if (!endDate.empty()) {
                                        dateRange += formatDate(endDate);
                                    }
                                    fields.push_back({"Attendance Period", dateRange, "text"});
                                }

                                std::string gradDate = ahAttrs.value("graduation_date", "");
                                if (!gradDate.empty()) {
                                    fields.push_back({"Graduation Date", gradDate, "date"});
                                }

                                bool transcriptReceived = ahAttrs.value("transcript_received", false);
                                fields.push_back({"Transcript Received", transcriptReceived ? "Yes" : "No", "text"});

                                recordNum++;
                            }
                        }
                    }

                    // Handle any "Other" types
                    for (const auto& pair : groupedRecords) {
                        if (std::find(institutionTypes.begin(), institutionTypes.end(), pair.first) == institutionTypes.end()) {
                            if (!firstGroup) {
                                fields.push_back({"", "", "text"});
                            }
                            fields.push_back({pair.first + " Education", "", "header"});
                            for (const auto& ahAttrs : pair.second) {
                                fields.push_back({"Institution Name", ahAttrs.value("institution_name", ""), "text"});
                                fields.push_back({"Degree Earned", ahAttrs.value("degree_earned", ""), "text"});
                                fields.push_back({"Graduation Date", ahAttrs.value("graduation_date", ""), "date"});
                            }
                        }
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

                        // FAFSA Information
                        fields.push_back({"FAFSA Information", "", "header"});
                        fields.push_back({"FAFSA Completed", faAttrs.value("fafsa_completed", false) ? "Yes" : "No", "text"});
                        fields.push_back({"FAFSA Year", faAttrs.value("fafsa_year", ""), "text"});
                        if (faAttrs.contains("efc") && !faAttrs["efc"].is_null()) {
                            std::ostringstream efcStr;
                            efcStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("efc", 0.0);
                            fields.push_back({"Expected Family Contribution (EFC)", efcStr.str(), "text"});
                        }
                        fields.push_back({"Aid Types Requested", faAttrs.value("aid_types", ""), "text"});
                        if (faAttrs.contains("requested_aid_amount") && !faAttrs["requested_aid_amount"].is_null()) {
                            std::ostringstream aidStr;
                            aidStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("requested_aid_amount", 0.0);
                            fields.push_back({"Requested Aid Amount", aidStr.str(), "text"});
                        }

                        // Employment Information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Employment Information", "", "header"});
                        fields.push_back({"Employment Status", faAttrs.value("employment_status", ""), "text"});
                        fields.push_back({"Employer Name", faAttrs.value("employer_name", ""), "text"});
                        if (faAttrs.contains("annual_income") && !faAttrs["annual_income"].is_null()) {
                            std::ostringstream incomeStr;
                            incomeStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("annual_income", 0.0);
                            fields.push_back({"Annual Income", incomeStr.str(), "text"});
                        }

                        // Household Information
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Household Information", "", "header"});
                        fields.push_back({"Independent Student", faAttrs.value("is_independent", false) ? "Yes" : "No", "text"});
                        if (faAttrs.contains("household_size") && !faAttrs["household_size"].is_null()) {
                            fields.push_back({"Household Size", std::to_string(faAttrs.value("household_size", 0)), "text"});
                        }
                        if (faAttrs.contains("dependents_count") && !faAttrs["dependents_count"].is_null()) {
                            fields.push_back({"Number of Dependents", std::to_string(faAttrs.value("dependents_count", 0)), "text"});
                        }

                        // Loan Information
                        bool hasLoans = faAttrs.value("has_outstanding_loans", false);
                        fields.push_back({"Has Outstanding Loans", hasLoans ? "Yes" : "No", "text"});
                        if (hasLoans && faAttrs.contains("outstanding_loan_amount") && !faAttrs["outstanding_loan_amount"].is_null()) {
                            std::ostringstream loanStr;
                            loanStr << "$" << std::fixed << std::setprecision(2) << faAttrs.value("outstanding_loan_amount", 0.0);
                            fields.push_back({"Outstanding Loan Amount", loanStr.str(), "text"});
                        }

                        // Scholarships
                        std::string scholarships = faAttrs.value("scholarship_applications", "");
                        if (!scholarships.empty()) {
                            fields.push_back({"", "", "text"});
                            fields.push_back({"Scholarship Applications", "", "header"});
                            fields.push_back({"Scholarships", scholarships, "text"});
                        }
                    }
                }
            } else if (formType == "documents") {
                auto docResponse = apiService_->getApiClient()->get("/Document?filter[student_id]=" + std::to_string(studentId));
                if (docResponse.success) {
                    auto docJson = nlohmann::json::parse(docResponse.body);
                    nlohmann::json docItems = docJson.is_array() ? docJson : (docJson.contains("data") ? docJson["data"] : nlohmann::json::array());

                    if (!docItems.empty()) {
                        int docNum = 1;
                        for (const auto& doc : docItems) {
                            nlohmann::json docAttrs = doc.contains("attributes") ? doc["attributes"] : doc;

                            if (docNum > 1) {
                                fields.push_back({"", "", "text"});
                            }
                            fields.push_back({"Document #" + std::to_string(docNum), "", "header"});

                            fields.push_back({"Document Type", docAttrs.value("document_type", ""), "text"});
                            fields.push_back({"File Name", docAttrs.value("file_name", ""), "text"});

                            int fileSize = docAttrs.value("file_size", 0);
                            std::string sizeStr;
                            if (fileSize >= 1048576) {
                                std::ostringstream ss;
                                ss << std::fixed << std::setprecision(2) << (fileSize / 1048576.0) << " MB";
                                sizeStr = ss.str();
                            } else if (fileSize >= 1024) {
                                std::ostringstream ss;
                                ss << std::fixed << std::setprecision(1) << (fileSize / 1024.0) << " KB";
                                sizeStr = ss.str();
                            } else {
                                sizeStr = std::to_string(fileSize) + " bytes";
                            }
                            fields.push_back({"File Size", sizeStr, "text"});

                            fields.push_back({"Status", docAttrs.value("status", ""), "text"});

                            std::string verifiedAt = docAttrs.value("verified_at", "");
                            if (!verifiedAt.empty()) {
                                fields.push_back({"Verified On", verifiedAt, "date"});
                                fields.push_back({"Verified By", docAttrs.value("verified_by", ""), "text"});
                            }

                            std::string notes = docAttrs.value("notes", "");
                            if (!notes.empty()) {
                                fields.push_back({"Notes", notes, "text"});
                            }

                            docNum++;
                        }
                    } else {
                        fields.push_back({"No Documents", "No documents uploaded yet", "text"});
                    }
                }
            } else if (formType == "consent") {
                auto cResponse = apiService_->getApiClient()->get("/Consent?filter[student_id]=" + std::to_string(studentId));
                if (cResponse.success) {
                    auto cJson = nlohmann::json::parse(cResponse.body);
                    nlohmann::json cItems = cJson.is_array() ? cJson : (cJson.contains("data") ? cJson["data"] : nlohmann::json::array());

                    int consentNum = 1;
                    for (const auto& c : cItems) {
                        nlohmann::json cAttrs = c.contains("attributes") ? c["attributes"] : c;

                        if (consentNum > 1) {
                            fields.push_back({"", "", "text"});
                        }

                        std::string consentType = cAttrs.value("consent_type", "");
                        std::string consentVersion = cAttrs.value("consent_version", "");
                        std::string consentLabel = !consentType.empty() ? consentType : "Consent Agreement";
                        if (!consentVersion.empty()) {
                            consentLabel += " (v" + consentVersion + ")";
                        }
                        fields.push_back({consentLabel, "", "header"});

                        bool isAccepted = cAttrs.value("is_accepted", false);
                        fields.push_back({"Status", isAccepted ? "Accepted" : "Not Accepted", "text"});

                        std::string acceptedAt = cAttrs.value("accepted_at", "");
                        if (!acceptedAt.empty()) {
                            fields.push_back({"Accepted On", acceptedAt, "date"});
                        }

                        std::string signature = cAttrs.value("electronic_signature", "");
                        if (!signature.empty()) {
                            fields.push_back({"Electronic Signature", signature, "text"});
                        }

                        std::string sigDate = cAttrs.value("signature_date", "");
                        if (!sigDate.empty()) {
                            fields.push_back({"Signature Date", sigDate, "date"});
                        }

                        std::string ipAddress = cAttrs.value("ip_address", "");
                        if (!ipAddress.empty()) {
                            fields.push_back({"IP Address", ipAddress, "text"});
                        }

                        consentNum++;
                    }

                    if (!cItems.empty()) {
                        fields.push_back({"", "", "text"});
                        fields.push_back({"Acknowledgments", "", "header"});
                        fields.push_back({"Terms of Service", "Acknowledged", "text"});
                        fields.push_back({"Privacy Policy", "Acknowledged", "text"});
                        fields.push_back({"FERPA Rights", "Acknowledged", "text"});
                        fields.push_back({"Information Accuracy", "Certified", "text"});
                    }
                }
            }

            // Build form data table
            if (!fields.empty()) {
                auto table = formSection->addWidget(std::make_unique<Wt::WTable>());
                table->addStyleClass("pdf-field-table");

                for (const auto& field : fields) {
                    // Handle header type (section headers within forms)
                    if (field.type == "header") {
                        auto row = table->insertRow(table->rowCount());
                        auto headerCell = row->elementAt(0);
                        headerCell->setColumnSpan(2);
                        headerCell->addWidget(std::make_unique<Wt::WText>(field.label));
                        headerCell->addStyleClass("pdf-section-header");
                    } else if (!field.value.empty()) {
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

            // Page footer with student name and page number
            auto pageFooter = formSection->addWidget(std::make_unique<Wt::WContainerWidget>());
            pageFooter->addStyleClass("pdf-page-footer");

            int currentPage = formIndex + 1;
            auto pageFooterText = pageFooter->addWidget(std::make_unique<Wt::WText>(
                studentName + " &nbsp;&nbsp;|&nbsp;&nbsp; Page " + std::to_string(currentPage) + " of " + std::to_string(totalPages)));
            pageFooterText->setTextFormat(Wt::TextFormat::XHTML);

            formIndex++;
        }

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

    // Submission date bar
    auto infoBar = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    infoBar->addStyleClass("pdf-info-bar");

    auto dateContainer = infoBar->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateContainer->addStyleClass("pdf-date-info");

    auto dateLabel = dateContainer->addWidget(std::make_unique<Wt::WText>("Submitted: "));
    dateLabel->addStyleClass("pdf-label");
    auto dateValue = dateContainer->addWidget(std::make_unique<Wt::WText>(formatDate(submissionDate_)));
    dateValue->addStyleClass("pdf-value");

    // Form fields table
    auto fieldsSection = documentContent_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsSection->addStyleClass("pdf-fields-section");

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

Api::PdfFormData FormPdfPreviewWidget::buildPdfFormData() {
    Api::PdfFormData pdfData;
    pdfData.formTitle = formTitle_;
    pdfData.studentName = studentName_;
    pdfData.studentEmail = studentEmail_;
    pdfData.submissionDate = submissionDate_;
    pdfData.institutionName = "Greenfield Technical College";
    pdfData.institutionTagline = "Official Student Records";

    // Convert FormFieldData to PdfFormField
    for (const auto& field : fields_) {
        pdfData.fields.emplace_back(field.label, field.value, field.type);
    }

    return pdfData;
}

std::string FormPdfPreviewWidget::generatePdf() {
    if (!pdfGenerator_ || !Api::PdfGenerator::isAvailable()) {
        std::cerr << "[FormPdfPreviewWidget] PDF generation not available" << std::endl;
        return "";
    }

    Api::PdfFormData pdfData = buildPdfFormData();
    std::string pdfPath = pdfGenerator_->generateFormPdf(pdfData);

    if (pdfPath.empty()) {
        std::cerr << "[FormPdfPreviewWidget] Failed to generate PDF: "
                  << pdfGenerator_->getLastError() << std::endl;
    } else {
        currentPdfPath_ = pdfPath;
        std::cerr << "[FormPdfPreviewWidget] Generated PDF at: " << pdfPath << std::endl;
    }

    return pdfPath;
}

void FormPdfPreviewWidget::downloadPdf() {
    std::string pdfPath = generatePdf();
    if (pdfPath.empty()) {
        // Show error message
        Wt::WApplication::instance()->doJavaScript(
            "alert('Failed to generate PDF. Please try again.');"
        );
        return;
    }

    // Create a file resource for download and store as member to keep it alive
    auto app = Wt::WApplication::instance();

    // Generate a clean filename from form title
    std::string filename = formTitle_;
    std::replace(filename.begin(), filename.end(), ' ', '_');
    filename += ".pdf";

    // Use WFileResource to serve the PDF - store as member to persist
    pdfResource_ = std::make_shared<Wt::WFileResource>("application/pdf", pdfPath);
    pdfResource_->suggestFileName(filename);

    // Trigger download by opening the resource URL
    app->doJavaScript(
        "window.open('" + pdfResource_->url() + "', '_blank');"
    );
}

} // namespace Admin
} // namespace StudentIntake
