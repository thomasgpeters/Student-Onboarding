#include "FormDetailViewer.h"
#include <Wt/WBreak.h>
#include "utils/Logger.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

// Helper to get current ISO timestamp
static std::string getCurrentIsoTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::gmtime(&time_t_now);
    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// Helper for safe JSON string extraction
static std::string safeGetString(const nlohmann::json& obj, const std::string& key, const std::string& defaultVal = "") {
    if (obj.contains(key) && !obj[key].is_null() && obj[key].is_string()) {
        return obj[key].get<std::string>();
    }
    return defaultVal;
}

// Helper for safe JSON int extraction
static int safeGetInt(const nlohmann::json& obj, const std::string& key, int defaultVal = 0) {
    if (obj.contains(key) && !obj[key].is_null()) {
        if (obj[key].is_number()) {
            return obj[key].get<int>();
        } else if (obj[key].is_string()) {
            try {
                return std::stoi(obj[key].get<std::string>());
            } catch (...) {}
        }
    }
    return defaultVal;
}

FormDetailViewer::FormDetailViewer()
    : WContainerWidget()
    , apiService_(nullptr)
    , currentSubmissionId_(0)
    , currentStudentId_(0)
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
    , revisionBtn_(nullptr)
    , previewPdfBtn_(nullptr)
    , printAllFormsBtn_(nullptr) {
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

    previewPdfBtn_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Preview PDF"));
    previewPdfBtn_->addStyleClass("btn btn-primary");
    previewPdfBtn_->clicked().connect([this]() {
        previewPdfClicked_.emit(currentSubmissionId_);
    });

    printAllFormsBtn_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Print All Student Forms"));
    printAllFormsBtn_->addStyleClass("btn btn-info");
    printAllFormsBtn_->clicked().connect([this]() {
        if (currentStudentId_ > 0) {
            printAllFormsClicked_.emit(currentStudentId_);
        }
    });

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
    currentStudentId_ = 0;
    fieldsContainer_->clear();

    LOG_DEBUG("FormDetailViewer", "Loading submission: " << submissionId);

    // Try to load from API
    if (apiService_) {
        try {
            auto response = apiService_->getApiClient()->get("/FormSubmission/" + std::to_string(submissionId));
            LOG_DEBUG("FormDetailViewer", "FormSubmission response - success: " << response.success);

            if (response.success) {
                auto jsonResponse = nlohmann::json::parse(response.body);
                nlohmann::json attrs;

                // Handle JSON:API format or plain format
                if (jsonResponse.contains("data")) {
                    auto& data = jsonResponse["data"];
                    attrs = data.contains("attributes") ? data["attributes"] : data;
                } else {
                    attrs = jsonResponse;
                }

                // Extract student_id and form_type_id
                int studentId = safeGetInt(attrs, "student_id", 0);
                int formTypeId = safeGetInt(attrs, "form_type_id", 0);
                currentStudentId_ = studentId;

                LOG_DEBUG("FormDetailViewer", "student_id: " << studentId
                          << ", form_type_id: " << formTypeId);

                // Get submission details
                currentStatus_ = safeGetString(attrs, "status", "pending");
                std::string submittedAt = safeGetString(attrs, "submitted_at", "");
                std::string approvedBy = safeGetString(attrs, "approved_by", "");
                std::string approvedAt = safeGetString(attrs, "approved_at", "");

                submittedDate_->setText(submittedAt.empty() ? "-" : formatDate(submittedAt));
                updateStatusDisplay(currentStatus_);

                if (!approvedBy.empty()) {
                    reviewedByText_->setText(approvedBy);
                    reviewedAtText_->setText(approvedAt.empty() ? "-" : formatDate(approvedAt));
                } else {
                    reviewedByText_->setText("Not yet reviewed");
                    reviewedAtText_->setText("-");
                }

                // Fetch student information separately
                if (studentId > 0) {
                    loadStudentInfo(studentId);
                } else {
                    studentName_->setText("-");
                    studentEmail_->setText("-");
                    studentProgram_->setText("-");
                }

                // Fetch form type information
                if (formTypeId > 0) {
                    loadFormTypeInfo(formTypeId);
                }

                // Load form field data
                loadFormFieldData(submissionId);

                return;
            }
        } catch (const std::exception& e) {
            LOG_ERROR("FormDetailViewer", "Error loading submission: " << e.what());
        }
    }

    // Fallback - display empty
    studentName_->setText("-");
    studentEmail_->setText("-");
    studentProgram_->setText("-");
    submittedDate_->setText("-");
    currentStatus_ = "pending";
    currentFormType_ = "";
    updateStatusDisplay(currentStatus_);
}

void FormDetailViewer::loadStudentInfo(int studentId) {
    if (!apiService_) return;

    try {
        auto response = apiService_->getApiClient()->get("/Student/" + std::to_string(studentId));
        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json attrs;

            if (json.contains("data")) {
                auto& data = json["data"];
                attrs = data.contains("attributes") ? data["attributes"] : data;
            } else {
                attrs = json;
            }

            std::string firstName = safeGetString(attrs, "first_name", "");
            std::string lastName = safeGetString(attrs, "last_name", "");
            std::string email = safeGetString(attrs, "email", "");
            int curriculumId = safeGetInt(attrs, "curriculum_id", 0);

            std::string fullName = firstName + " " + lastName;
            studentName_->setText(fullName.empty() || fullName == " " ? "-" : fullName);
            studentEmail_->setText(email.empty() ? "-" : email);

            // Fetch program name from curriculum
            if (curriculumId > 0) {
                auto currResponse = apiService_->getApiClient()->get("/Curriculum/" + std::to_string(curriculumId));
                if (currResponse.success) {
                    auto currJson = nlohmann::json::parse(currResponse.body);
                    nlohmann::json currAttrs;

                    if (currJson.contains("data")) {
                        auto& currData = currJson["data"];
                        currAttrs = currData.contains("attributes") ? currData["attributes"] : currData;
                    } else {
                        currAttrs = currJson;
                    }

                    std::string programName = safeGetString(currAttrs, "name", "");
                    studentProgram_->setText(programName.empty() ? "-" : programName);
                } else {
                    studentProgram_->setText("-");
                }
            } else {
                studentProgram_->setText("-");
            }

            LOG_DEBUG("FormDetailViewer", "Loaded student: " << fullName << ", " << email);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("FormDetailViewer", "Error loading student info: " << e.what());
        studentName_->setText("-");
        studentEmail_->setText("-");
        studentProgram_->setText("-");
    }
}

void FormDetailViewer::loadFormTypeInfo(int formTypeId) {
    if (!apiService_) return;

    try {
        auto response = apiService_->getApiClient()->get("/FormType/" + std::to_string(formTypeId));
        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json attrs;

            if (json.contains("data")) {
                auto& data = json["data"];
                attrs = data.contains("attributes") ? data["attributes"] : data;
            } else {
                attrs = json;
            }

            currentFormType_ = safeGetString(attrs, "code", "");
            std::string formName = safeGetString(attrs, "name", currentFormType_);

            formTitle_->setText("<h2>" + formName + " Submission</h2>");
            LOG_DEBUG("FormDetailViewer", "Form type: " << currentFormType_ << " (" << formName << ")");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("FormDetailViewer", "Error loading form type: " << e.what());
    }
}

void FormDetailViewer::loadFormFieldData(int submissionId) {
    if (!apiService_) return;

    std::vector<FormFieldDisplay> fields;

    try {
        // Query FormField records for this submission
        auto response = apiService_->getApiClient()->get(
            "/FormField?filter[form_submission_id]=" + std::to_string(submissionId));

        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json items;

            if (json.is_array()) {
                items = json;
            } else if (json.contains("data") && json["data"].is_array()) {
                items = json["data"];
            }

            LOG_DEBUG("FormDetailViewer", "Found " << items.size() << " form fields");

            for (const auto& item : items) {
                nlohmann::json attrs = item.contains("attributes") ? item["attributes"] : item;

                FormFieldDisplay field;
                field.label = safeGetString(attrs, "field_name", "Unknown");
                field.fieldType = safeGetString(attrs, "field_type", "string");

                // Get value based on field type
                if (field.fieldType == "boolean" || field.fieldType == "bool") {
                    bool boolVal = false;
                    if (attrs.contains("bool_value") && !attrs["bool_value"].is_null()) {
                        boolVal = attrs["bool_value"].get<bool>();
                    }
                    field.value = boolVal ? "Yes" : "No";
                } else if (field.fieldType == "integer" || field.fieldType == "int") {
                    field.value = std::to_string(safeGetInt(attrs, "int_value", 0));
                } else if (field.fieldType == "double" || field.fieldType == "number") {
                    if (attrs.contains("double_value") && !attrs["double_value"].is_null()) {
                        field.value = std::to_string(attrs["double_value"].get<double>());
                    } else {
                        field.value = "0";
                    }
                } else {
                    field.value = safeGetString(attrs, "string_value", "");
                }

                // Format field label for display
                field.label = formatFieldLabel(field.label);

                fields.push_back(field);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("FormDetailViewer", "Error loading form fields: " << e.what());
    }

    if (fields.empty()) {
        // No form field data - show message
        auto noDataText = fieldsContainer_->addWidget(
            std::make_unique<Wt::WText>("No form data available for this submission."));
        noDataText->addStyleClass("text-muted");
    } else {
        displayFormData(fields);
    }
}

std::string FormDetailViewer::formatFieldLabel(const std::string& fieldName) {
    // Convert snake_case to Title Case
    std::string result;
    bool capitalizeNext = true;

    for (char c : fieldName) {
        if (c == '_') {
            result += ' ';
            capitalizeNext = true;
        } else if (capitalizeNext) {
            result += std::toupper(c);
            capitalizeNext = false;
        } else {
            result += c;
        }
    }

    return result;
}

std::string FormDetailViewer::formatDate(const std::string& dateStr) {
    if (dateStr.empty() || dateStr.length() < 10) return dateStr;

    try {
        std::string year = dateStr.substr(0, 4);
        std::string month = dateStr.substr(5, 2);
        std::string day = dateStr.substr(8, 2);

        std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        int monthIdx = std::stoi(month) - 1;
        if (monthIdx >= 0 && monthIdx < 12) {
            return months[monthIdx] + " " + std::to_string(std::stoi(day)) + ", " + year;
        }
    } catch (...) {}

    return dateStr;
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
    LOG_INFO("FormDetailViewer", "Approving submission: " << currentSubmissionId_);

    bool success = false;
    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "FormSubmission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "approved";
            payload["data"]["attributes"]["approved_at"] = getCurrentIsoTimestamp();
            payload["data"]["attributes"]["approved_by"] = "Admin";  // TODO: Use actual admin user name

            std::string notes = reviewNotes_->text().toUTF8();
            if (!notes.empty()) {
                payload["data"]["attributes"]["rejection_reason"] = notes;  // Using rejection_reason field for notes
            }

            LOG_DEBUG("FormDetailViewer", "PATCH payload: " << payload.dump());

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload);

            if (response.success) {
                LOG_INFO("FormDetailViewer", "Submission approved successfully");
                success = true;
            } else {
                LOG_ERROR("FormDetailViewer", "Failed to approve: " << response.errorMessage);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("FormDetailViewer", "Error approving submission: " << e.what());
        }
    }

    if (success) {
        updateStatusDisplay("approved");
        reviewedByText_->setText("Admin");
        reviewedAtText_->setText(formatDate(getCurrentIsoTimestamp()));
    }
    approveClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::handleReject() {
    LOG_INFO("FormDetailViewer", "Rejecting submission: " << currentSubmissionId_);

    bool success = false;
    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "FormSubmission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "rejected";
            payload["data"]["attributes"]["approved_at"] = getCurrentIsoTimestamp();
            payload["data"]["attributes"]["approved_by"] = "Admin";

            std::string notes = reviewNotes_->text().toUTF8();
            if (!notes.empty()) {
                payload["data"]["attributes"]["rejection_reason"] = notes;
            }

            LOG_DEBUG("FormDetailViewer", "PATCH payload: " << payload.dump());

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload);

            if (response.success) {
                LOG_INFO("FormDetailViewer", "Submission rejected successfully");
                success = true;
            } else {
                LOG_ERROR("FormDetailViewer", "Failed to reject: " << response.errorMessage);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("FormDetailViewer", "Error rejecting submission: " << e.what());
        }
    }

    if (success) {
        updateStatusDisplay("rejected");
        reviewedByText_->setText("Admin");
        reviewedAtText_->setText(formatDate(getCurrentIsoTimestamp()));
    }
    rejectClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::handleRequestRevision() {
    LOG_INFO("FormDetailViewer", "Requesting revision for submission: " << currentSubmissionId_);

    bool success = false;
    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "FormSubmission";
            payload["data"]["id"] = std::to_string(currentSubmissionId_);
            payload["data"]["attributes"]["status"] = "needs_revision";
            payload["data"]["attributes"]["approved_at"] = getCurrentIsoTimestamp();
            payload["data"]["attributes"]["approved_by"] = "Admin";

            std::string notes = reviewNotes_->text().toUTF8();
            if (!notes.empty()) {
                payload["data"]["attributes"]["rejection_reason"] = notes;
            }

            LOG_DEBUG("FormDetailViewer", "PATCH payload: " << payload.dump());

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(currentSubmissionId_), payload);

            if (response.success) {
                LOG_INFO("FormDetailViewer", "Revision requested successfully");
                success = true;
            } else {
                LOG_ERROR("FormDetailViewer", "Failed to request revision: " << response.errorMessage);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("FormDetailViewer", "Error requesting revision: " << e.what());
        }
    }

    if (success) {
        updateStatusDisplay("needs_revision");
        reviewedByText_->setText("Admin");
        reviewedAtText_->setText(formatDate(getCurrentIsoTimestamp()));
    }
    requestRevisionClicked_.emit(currentSubmissionId_);
}

void FormDetailViewer::clearForm() {
    currentSubmissionId_ = 0;
    currentStudentId_ = 0;
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
    formTitle_->setText("<h2>Form Submission Details</h2>");
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
