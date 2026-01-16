#include "FormDetailViewer.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormDetailViewer::FormDetailViewer()
    : WContainerWidget()
    , apiService_(nullptr)
    , currentSubmissionId_(0)
    , headerSection_(nullptr)
    , backBtn_(nullptr)
    , formTitle_(nullptr)
    , statusBadge_(nullptr)
    , studentInfoSection_(nullptr)
    , studentName_(nullptr)
    , studentEmail_(nullptr)
    , studentProgram_(nullptr)
    , submittedDate_(nullptr)
    , formDataSection_(nullptr)
    , fieldsContainer_(nullptr)
    , reviewSection_(nullptr)
    , reviewedByText_(nullptr)
    , reviewedAtText_(nullptr)
    , reviewNotes_(nullptr)
    , actionsSection_(nullptr)
    , approveBtn_(nullptr)
    , rejectBtn_(nullptr)
    , revisionBtn_(nullptr) {
    setupUI();
}

FormDetailViewer::~FormDetailViewer() {
}

void FormDetailViewer::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void FormDetailViewer::setupUI() {
    addStyleClass("admin-form-detail-viewer");

    // Header section with back button
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->addStyleClass("admin-detail-topbar");

    backBtn_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("< Back to Submissions"));
    backBtn_->addStyleClass("btn btn-link");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    // Title section
    auto titleSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    titleSection->addStyleClass("admin-form-detail-header");

    auto titleRow = titleSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleRow->addStyleClass("admin-title-row");

    formTitle_ = titleRow->addWidget(std::make_unique<Wt::WText>("<h2>Form Submission Details</h2>"));
    formTitle_->setTextFormat(Wt::TextFormat::XHTML);

    statusBadge_ = titleRow->addWidget(std::make_unique<Wt::WText>("Pending"));
    statusBadge_->addStyleClass("badge badge-warning admin-status-badge");

    // Student info section
    studentInfoSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    studentInfoSection_->addStyleClass("admin-student-info-section card");

    auto studentInfoHeader = studentInfoSection_->addWidget(std::make_unique<Wt::WText>("<h4>Student Information</h4>"));
    studentInfoHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto studentInfoGrid = studentInfoSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentInfoGrid->addStyleClass("admin-info-grid");

    auto nameCard = studentInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameCard->addStyleClass("admin-info-card");
    auto nameLabel = nameCard->addWidget(std::make_unique<Wt::WText>("Student Name"));
    nameLabel->addStyleClass("admin-info-label");
    studentName_ = nameCard->addWidget(std::make_unique<Wt::WText>("-"));
    studentName_->addStyleClass("admin-info-value");

    auto emailCard = studentInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailCard->addStyleClass("admin-info-card");
    auto emailLabel = emailCard->addWidget(std::make_unique<Wt::WText>("Email"));
    emailLabel->addStyleClass("admin-info-label");
    studentEmail_ = emailCard->addWidget(std::make_unique<Wt::WText>("-"));
    studentEmail_->addStyleClass("admin-info-value");

    auto programCard = studentInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    programCard->addStyleClass("admin-info-card");
    auto programLabel = programCard->addWidget(std::make_unique<Wt::WText>("Program"));
    programLabel->addStyleClass("admin-info-label");
    studentProgram_ = programCard->addWidget(std::make_unique<Wt::WText>("-"));
    studentProgram_->addStyleClass("admin-info-value");

    auto dateCard = studentInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateCard->addStyleClass("admin-info-card");
    auto dateLabel = dateCard->addWidget(std::make_unique<Wt::WText>("Submitted"));
    dateLabel->addStyleClass("admin-info-label");
    submittedDate_ = dateCard->addWidget(std::make_unique<Wt::WText>("-"));
    submittedDate_->addStyleClass("admin-info-value");

    // Form data section
    formDataSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    formDataSection_->addStyleClass("admin-form-data-section card");

    auto formDataHeader = formDataSection_->addWidget(std::make_unique<Wt::WText>("<h4>Submitted Data</h4>"));
    formDataHeader->setTextFormat(Wt::TextFormat::XHTML);

    fieldsContainer_ = formDataSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsContainer_->addStyleClass("admin-fields-container");

    // Review section
    reviewSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    reviewSection_->addStyleClass("admin-review-section card");

    auto reviewHeader = reviewSection_->addWidget(std::make_unique<Wt::WText>("<h4>Review Information</h4>"));
    reviewHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto reviewInfoGrid = reviewSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    reviewInfoGrid->addStyleClass("admin-review-info");

    auto reviewByCard = reviewInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    reviewByCard->addStyleClass("admin-info-card");
    auto reviewByLabel = reviewByCard->addWidget(std::make_unique<Wt::WText>("Reviewed By"));
    reviewByLabel->addStyleClass("admin-info-label");
    reviewedByText_ = reviewByCard->addWidget(std::make_unique<Wt::WText>("Not yet reviewed"));
    reviewedByText_->addStyleClass("admin-info-value");

    auto reviewAtCard = reviewInfoGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    reviewAtCard->addStyleClass("admin-info-card");
    auto reviewAtLabel = reviewAtCard->addWidget(std::make_unique<Wt::WText>("Reviewed At"));
    reviewAtLabel->addStyleClass("admin-info-label");
    reviewedAtText_ = reviewAtCard->addWidget(std::make_unique<Wt::WText>("-"));
    reviewedAtText_->addStyleClass("admin-info-value");

    // Review notes
    auto notesGroup = reviewSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    notesGroup->addStyleClass("form-group");
    auto notesLabel = notesGroup->addWidget(std::make_unique<Wt::WText>("Review Notes (optional)"));
    notesLabel->addStyleClass("form-label");
    reviewNotes_ = notesGroup->addWidget(std::make_unique<Wt::WTextArea>());
    reviewNotes_->setPlaceholderText("Add notes about this submission...");
    reviewNotes_->addStyleClass("form-control");
    reviewNotes_->setRows(3);

    // Actions section
    actionsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsSection_->addStyleClass("admin-form-detail-actions");

    approveBtn_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Approve Submission"));
    approveBtn_->addStyleClass("btn btn-success");
    approveBtn_->clicked().connect(this, &FormDetailViewer::handleApprove);

    revisionBtn_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Request Revision"));
    revisionBtn_->addStyleClass("btn btn-warning");
    revisionBtn_->clicked().connect(this, &FormDetailViewer::handleRequestRevision);

    rejectBtn_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Reject Submission"));
    rejectBtn_->addStyleClass("btn btn-danger");
    rejectBtn_->clicked().connect(this, &FormDetailViewer::handleReject);
}

void FormDetailViewer::loadSubmission(int submissionId) {
    currentSubmissionId_ = submissionId;
    fieldsContainer_->clear();

    std::cerr << "[FormDetailViewer] Loading submission: " << submissionId << std::endl;

    // Try to load from API
    if (apiService_) {
        try {
            auto response = apiService_->getApiClient()->get("/FormSubmission/" + std::to_string(submissionId));
            if (response.success) {
                auto jsonResponse = nlohmann::json::parse(response.body);
                if (jsonResponse.contains("data")) {
                    auto& data = jsonResponse["data"];
                    auto& attrs = data["attributes"];

                    studentName_->setText(attrs.value("student_name", "-"));
                    studentEmail_->setText(attrs.value("student_email", "-"));
                    studentProgram_->setText(attrs.value("program_name", "-"));
                    submittedDate_->setText(attrs.value("submitted_at", "-"));

                    currentStatus_ = attrs.value("status", "pending");
                    currentFormType_ = attrs.value("form_type", "");
                    updateStatusDisplay(currentStatus_);

                    if (!attrs.value("reviewed_by", "").empty()) {
                        reviewedByText_->setText(attrs.value("reviewed_by", ""));
                        reviewedAtText_->setText(attrs.value("reviewed_at", "-"));
                    }

                    if (attrs.contains("form_data") && attrs["form_data"].is_object()) {
                        auto fields = parseFormData(currentFormType_, attrs["form_data"]);
                        displayFormData(fields);
                    }

                    return;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormDetailViewer] Error loading submission: " << e.what() << std::endl;
        }
    }

    // Mock data for testing
    studentName_->setText("John Doe");
    studentEmail_->setText("john.doe@email.com");
    studentProgram_->setText("Computer Science");
    submittedDate_->setText("Jan 15, 2026");
    currentStatus_ = "pending";
    currentFormType_ = "personal_info";
    updateStatusDisplay(currentStatus_);

    // Mock form fields
    std::vector<FormFieldDisplay> mockFields = {
        {"First Name", "John", "text"},
        {"Last Name", "Doe", "text"},
        {"Date of Birth", "1998-05-15", "date"},
        {"Email Address", "john.doe@email.com", "email"},
        {"Phone Number", "(555) 123-4567", "phone"},
        {"Street Address", "123 Main Street", "text"},
        {"City", "Springfield", "text"},
        {"State", "IL", "text"},
        {"ZIP Code", "62701", "text"},
        {"Gender", "Male", "select"},
        {"Preferred Pronouns", "He/Him", "select"},
        {"Citizenship Status", "US Citizen", "select"}
    };

    displayFormData(mockFields);
}

void FormDetailViewer::displayFormData(const std::vector<FormFieldDisplay>& fields) {
    fieldsContainer_->clear();

    for (const auto& field : fields) {
        auto fieldRow = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        fieldRow->addStyleClass("admin-field-row");

        auto labelText = fieldRow->addWidget(std::make_unique<Wt::WText>(field.label));
        labelText->addStyleClass("admin-field-label");

        auto valueText = fieldRow->addWidget(std::make_unique<Wt::WText>(formatValue(field.value, field.fieldType)));
        valueText->addStyleClass("admin-field-value");
    }
}

void FormDetailViewer::updateStatusDisplay(const std::string& status) {
    currentStatus_ = status;

    std::string displayStatus = status;
    if (displayStatus == "needs_revision") displayStatus = "Needs Revision";
    else if (!displayStatus.empty()) {
        displayStatus[0] = std::toupper(displayStatus[0]);
    }

    statusBadge_->setText(displayStatus);
    statusBadge_->removeStyleClass("badge-warning");
    statusBadge_->removeStyleClass("badge-success");
    statusBadge_->removeStyleClass("badge-danger");
    statusBadge_->removeStyleClass("badge-info");
    statusBadge_->addStyleClass("badge " + getStatusBadgeClass(status));

    // Show/hide action buttons based on status
    bool canReview = (status == "pending" || status == "needs_revision");
    approveBtn_->setHidden(!canReview);
    rejectBtn_->setHidden(!canReview);
    revisionBtn_->setHidden(!canReview);

    if (!canReview) {
        reviewNotes_->setDisabled(true);
    } else {
        reviewNotes_->setDisabled(false);
    }
}

void FormDetailViewer::handleApprove() {
    std::cerr << "[FormDetailViewer] Approving submission: " << currentSubmissionId_ << std::endl;

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "form_submission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "approved";
            payload["data"]["attributes"]["review_notes"] = reviewNotes_->text().toUTF8();

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload.dump());

            if (response.success) {
                std::cerr << "[FormDetailViewer] Submission approved successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormDetailViewer] Error approving submission: " << e.what() << std::endl;
        }
    }

    updateStatusDisplay("approved");
    approveClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::handleReject() {
    std::cerr << "[FormDetailViewer] Rejecting submission: " << currentSubmissionId_ << std::endl;

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "form_submission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "rejected";
            payload["data"]["attributes"]["review_notes"] = reviewNotes_->text().toUTF8();

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload.dump());

            if (response.success) {
                std::cerr << "[FormDetailViewer] Submission rejected successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormDetailViewer] Error rejecting submission: " << e.what() << std::endl;
        }
    }

    updateStatusDisplay("rejected");
    rejectClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::handleRequestRevision() {
    std::cerr << "[FormDetailViewer] Requesting revision for submission: " << currentSubmissionId_ << std::endl;

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "form_submission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "needs_revision";
            payload["data"]["attributes"]["review_notes"] = reviewNotes_->text().toUTF8();

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload.dump());

            if (response.success) {
                std::cerr << "[FormDetailViewer] Revision requested successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormDetailViewer] Error requesting revision: " << e.what() << std::endl;
        }
    }

    updateStatusDisplay("needs_revision");
    requestRevisionClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::clearForm() {
    currentSubmissionId_ = 0;
    currentStatus_ = "";
    currentFormType_ = "";
    studentName_->setText("-");
    studentEmail_->setText("-");
    studentProgram_->setText("-");
    submittedDate_->setText("-");
    fieldsContainer_->clear();
    reviewNotes_->setText("");
    reviewedByText_->setText("Not yet reviewed");
    reviewedAtText_->setText("-");
}

std::string FormDetailViewer::getStatusBadgeClass(const std::string& status) {
    if (status == "pending") return "badge-warning";
    if (status == "approved") return "badge-success";
    if (status == "rejected") return "badge-danger";
    if (status == "needs_revision") return "badge-info";
    return "badge-secondary";
}

std::string FormDetailViewer::formatValue(const std::string& value, const std::string& fieldType) {
    if (value.empty()) return "-";

    if (fieldType == "date" && value.length() >= 10) {
        // Format ISO date to readable format
        try {
            std::string year = value.substr(0, 4);
            std::string month = value.substr(5, 2);
            std::string day = value.substr(8, 2);

            std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

            int monthIdx = std::stoi(month) - 1;
            if (monthIdx >= 0 && monthIdx < 12) {
                return months[monthIdx] + " " + std::to_string(std::stoi(day)) + ", " + year;
            }
        } catch (...) {}
    }

    return value;
}

std::vector<FormFieldDisplay> FormDetailViewer::parseFormData(const std::string& formType, const nlohmann::json& data) {
    std::vector<FormFieldDisplay> fields;

    // Map JSON keys to display labels based on form type
    std::map<std::string, std::string> labelMap;

    if (formType == "personal_info") {
        labelMap = {
            {"first_name", "First Name"},
            {"last_name", "Last Name"},
            {"middle_name", "Middle Name"},
            {"date_of_birth", "Date of Birth"},
            {"email", "Email Address"},
            {"phone", "Phone Number"},
            {"address", "Street Address"},
            {"city", "City"},
            {"state", "State"},
            {"zip_code", "ZIP Code"},
            {"gender", "Gender"},
            {"pronouns", "Preferred Pronouns"},
            {"citizenship", "Citizenship Status"},
            {"ssn", "SSN (Last 4)"}
        };
    } else if (formType == "emergency_contact") {
        labelMap = {
            {"contact_name", "Contact Name"},
            {"relationship", "Relationship"},
            {"phone", "Phone Number"},
            {"alt_phone", "Alternate Phone"},
            {"email", "Email Address"},
            {"address", "Address"}
        };
    } else if (formType == "medical_info") {
        labelMap = {
            {"insurance_provider", "Insurance Provider"},
            {"policy_number", "Policy Number"},
            {"primary_physician", "Primary Physician"},
            {"physician_phone", "Physician Phone"},
            {"allergies", "Allergies"},
            {"medications", "Current Medications"},
            {"conditions", "Medical Conditions"},
            {"emergency_instructions", "Emergency Instructions"}
        };
    } else if (formType == "academic_history") {
        labelMap = {
            {"high_school", "High School"},
            {"graduation_year", "Graduation Year"},
            {"gpa", "GPA"},
            {"previous_college", "Previous College"},
            {"degree_earned", "Degree Earned"},
            {"major", "Major"},
            {"credits_earned", "Credits Earned"}
        };
    } else if (formType == "financial_aid") {
        labelMap = {
            {"applying_for_aid", "Applying for Aid"},
            {"fafsa_completed", "FAFSA Completed"},
            {"household_income", "Household Income Range"},
            {"dependents", "Number of Dependents"},
            {"employment_status", "Employment Status"},
            {"scholarship_interest", "Scholarship Interest"}
        };
    }

    // Parse JSON data
    for (auto& [key, value] : data.items()) {
        FormFieldDisplay field;
        field.label = labelMap.count(key) ? labelMap[key] : key;

        if (value.is_string()) {
            field.value = value.get<std::string>();
        } else if (value.is_number()) {
            field.value = std::to_string(value.get<int>());
        } else if (value.is_boolean()) {
            field.value = value.get<bool>() ? "Yes" : "No";
        } else {
            field.value = value.dump();
        }

        // Determine field type
        if (key.find("date") != std::string::npos || key.find("birth") != std::string::npos) {
            field.fieldType = "date";
        } else if (key.find("email") != std::string::npos) {
            field.fieldType = "email";
        } else if (key.find("phone") != std::string::npos) {
            field.fieldType = "phone";
        } else {
            field.fieldType = "text";
        }

        fields.push_back(field);
    }

    return fields;
}

} // namespace Admin
} // namespace StudentIntake
