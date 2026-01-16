#include "FormSubmissionsWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormSubmissionsWidget::FormSubmissionsWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , statsContainer_(nullptr)
    , pendingCountText_(nullptr)
    , approvedCountText_(nullptr)
    , rejectedCountText_(nullptr)
    , filterContainer_(nullptr)
    , searchInput_(nullptr)
    , formTypeFilter_(nullptr)
    , statusFilter_(nullptr)
    , programFilter_(nullptr)
    , searchBtn_(nullptr)
    , resetBtn_(nullptr)
    , resultCount_(nullptr)
    , tableContainer_(nullptr)
    , submissionsTable_(nullptr)
    , noDataMessage_(nullptr) {
    setupUI();
}

FormSubmissionsWidget::~FormSubmissionsWidget() {
}

void FormSubmissionsWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void FormSubmissionsWidget::setupUI() {
    addStyleClass("admin-form-submissions");

    // Header section
    auto headerSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection->addStyleClass("admin-section-header");

    headerTitle_ = headerSection->addWidget(std::make_unique<Wt::WText>("Form Submissions"));
    headerTitle_->addStyleClass("admin-section-title");

    headerSubtitle_ = headerSection->addWidget(std::make_unique<Wt::WText>("Review and approve student form submissions"));
    headerSubtitle_->addStyleClass("admin-section-subtitle");

    // Statistics cards
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Pending card
    auto pendingCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    pendingCard->addStyleClass("admin-stat-mini-card pending");
    auto pendingIcon = pendingCard->addWidget(std::make_unique<Wt::WText>("⏳"));
    pendingIcon->addStyleClass("admin-stat-mini-icon");
    pendingCountText_ = pendingCard->addWidget(std::make_unique<Wt::WText>("0"));
    pendingCountText_->addStyleClass("admin-stat-mini-number");
    auto pendingLabel = pendingCard->addWidget(std::make_unique<Wt::WText>("Pending Review"));
    pendingLabel->addStyleClass("admin-stat-mini-label");

    // Approved card
    auto approvedCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    approvedCard->addStyleClass("admin-stat-mini-card approved");
    auto approvedIcon = approvedCard->addWidget(std::make_unique<Wt::WText>("✓"));
    approvedIcon->addStyleClass("admin-stat-mini-icon");
    approvedCountText_ = approvedCard->addWidget(std::make_unique<Wt::WText>("0"));
    approvedCountText_->addStyleClass("admin-stat-mini-number");
    auto approvedLabel = approvedCard->addWidget(std::make_unique<Wt::WText>("Approved"));
    approvedLabel->addStyleClass("admin-stat-mini-label");

    // Rejected card
    auto rejectedCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    rejectedCard->addStyleClass("admin-stat-mini-card rejected");
    auto rejectedIcon = rejectedCard->addWidget(std::make_unique<Wt::WText>("✗"));
    rejectedIcon->addStyleClass("admin-stat-mini-icon");
    rejectedCountText_ = rejectedCard->addWidget(std::make_unique<Wt::WText>("0"));
    rejectedCountText_->addStyleClass("admin-stat-mini-number");
    auto rejectedLabel = rejectedCard->addWidget(std::make_unique<Wt::WText>("Rejected"));
    rejectedLabel->addStyleClass("admin-stat-mini-label");

    // Filter section
    filterContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer_->addStyleClass("admin-filter-container");

    // Search input
    auto searchGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group admin-filter-search");

    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WText>("Search"));
    searchLabel->addStyleClass("admin-filter-label");

    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search by student name or email...");
    searchInput_->addStyleClass("admin-filter-input");
    searchInput_->enterPressed().connect(this, &FormSubmissionsWidget::applyFilters);

    // Form type filter
    auto formGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formGroup->addStyleClass("admin-filter-group");

    auto formLabel = formGroup->addWidget(std::make_unique<Wt::WText>("Form Type"));
    formLabel->addStyleClass("admin-filter-label");

    formTypeFilter_ = formGroup->addWidget(std::make_unique<Wt::WComboBox>());
    formTypeFilter_->addStyleClass("admin-filter-select");
    formTypeFilter_->addItem("All Forms");
    formTypeFilter_->addItem("Personal Information");
    formTypeFilter_->addItem("Emergency Contact");
    formTypeFilter_->addItem("Medical Information");
    formTypeFilter_->addItem("Academic History");
    formTypeFilter_->addItem("Financial Aid");
    formTypeFilter_->addItem("Document Upload");
    formTypeFilter_->addItem("Consent Form");
    formTypeFilter_->changed().connect(this, &FormSubmissionsWidget::applyFilters);

    // Status filter
    auto statusGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusGroup->addStyleClass("admin-filter-group");

    auto statusLabel = statusGroup->addWidget(std::make_unique<Wt::WText>("Status"));
    statusLabel->addStyleClass("admin-filter-label");

    statusFilter_ = statusGroup->addWidget(std::make_unique<Wt::WComboBox>());
    statusFilter_->addStyleClass("admin-filter-select");
    statusFilter_->addItem("All Status");
    statusFilter_->addItem("Pending");
    statusFilter_->addItem("Approved");
    statusFilter_->addItem("Rejected");
    statusFilter_->addItem("Needs Revision");
    statusFilter_->changed().connect(this, &FormSubmissionsWidget::applyFilters);

    // Program filter
    auto programGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    programGroup->addStyleClass("admin-filter-group");

    auto programLabel = programGroup->addWidget(std::make_unique<Wt::WText>("Program"));
    programLabel->addStyleClass("admin-filter-label");

    programFilter_ = programGroup->addWidget(std::make_unique<Wt::WComboBox>());
    programFilter_->addStyleClass("admin-filter-select");
    programFilter_->addItem("All Programs");
    programFilter_->addItem("Computer Science");
    programFilter_->addItem("Business Administration");
    programFilter_->addItem("MBA Program");
    programFilter_->addItem("Nursing");
    programFilter_->changed().connect(this, &FormSubmissionsWidget::applyFilters);

    // Filter buttons
    auto buttonGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons");

    searchBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Search"));
    searchBtn_->addStyleClass("btn btn-primary");
    searchBtn_->clicked().connect(this, &FormSubmissionsWidget::applyFilters);

    resetBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Reset"));
    resetBtn_->addStyleClass("btn btn-secondary");
    resetBtn_->clicked().connect(this, &FormSubmissionsWidget::resetFilters);

    // Result count
    resultCount_ = addWidget(std::make_unique<Wt::WText>(""));
    resultCount_->addStyleClass("admin-result-count");

    // Table container
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("admin-table-container");

    submissionsTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    submissionsTable_->addStyleClass("admin-data-table");

    // No data message
    noDataMessage_ = addWidget(std::make_unique<Wt::WText>("No form submissions found matching your criteria."));
    noDataMessage_->addStyleClass("admin-no-data-message");
    noDataMessage_->hide();
}

void FormSubmissionsWidget::refresh() {
    loadSubmissions();
}

void FormSubmissionsWidget::clear() {
    submissions_.clear();
    filteredSubmissions_.clear();
    if (submissionsTable_) {
        submissionsTable_->clear();
    }
}

void FormSubmissionsWidget::loadSubmissions() {
    submissions_.clear();

    if (!apiService_) {
        std::cerr << "[FormSubmissionsWidget] API service not available, using mock data" << std::endl;

        // Mock data for testing
        submissions_.push_back({1, 101, "John Doe", "john.doe@email.com", "personal_info",
            "Personal Information", "pending", "2026-01-15T10:30:00Z", "", "", "Computer Science"});
        submissions_.push_back({2, 101, "John Doe", "john.doe@email.com", "academic_history",
            "Academic History", "pending", "2026-01-15T10:35:00Z", "", "", "Computer Science"});
        submissions_.push_back({3, 102, "Jane Smith", "jane.smith@email.com", "personal_info",
            "Personal Information", "approved", "2026-01-14T09:00:00Z", "2026-01-14T14:00:00Z", "Admin User", "Business Administration"});
        submissions_.push_back({4, 102, "Jane Smith", "jane.smith@email.com", "financial_aid",
            "Financial Aid", "pending", "2026-01-14T09:15:00Z", "", "", "Business Administration"});
        submissions_.push_back({5, 103, "Mike Johnson", "mike.j@email.com", "personal_info",
            "Personal Information", "rejected", "2026-01-13T11:00:00Z", "2026-01-13T15:30:00Z", "Admin User", "MBA Program"});
        submissions_.push_back({6, 103, "Mike Johnson", "mike.j@email.com", "consent",
            "Consent Form", "approved", "2026-01-13T11:10:00Z", "2026-01-13T15:35:00Z", "Admin User", "MBA Program"});
        submissions_.push_back({7, 104, "Sarah Wilson", "sarah.w@email.com", "medical_info",
            "Medical Information", "needs_revision", "2026-01-12T14:00:00Z", "2026-01-12T16:00:00Z", "Admin User", "Nursing"});
        submissions_.push_back({8, 104, "Sarah Wilson", "sarah.w@email.com", "emergency_contact",
            "Emergency Contact", "pending", "2026-01-12T14:05:00Z", "", "", "Nursing"});
        submissions_.push_back({9, 105, "Tom Brown", "tom.b@email.com", "document_upload",
            "Document Upload", "pending", "2026-01-11T08:30:00Z", "", "", "Computer Science"});
        submissions_.push_back({10, 105, "Tom Brown", "tom.b@email.com", "personal_info",
            "Personal Information", "approved", "2026-01-11T08:00:00Z", "2026-01-11T12:00:00Z", "Admin User", "Computer Science"});

        applyFilters();
        return;
    }

    try {
        auto response = apiService_->getApiClient()->get("/FormSubmission");
        if (response.success) {
            auto jsonResponse = nlohmann::json::parse(response.body);
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                for (const auto& item : jsonResponse["data"]) {
                    FormSubmissionRecord record;
                    record.id = item.value("id", 0);
                    record.studentId = item["attributes"].value("student_id", 0);
                    record.studentName = item["attributes"].value("student_name", "");
                    record.studentEmail = item["attributes"].value("student_email", "");
                    record.formType = item["attributes"].value("form_type", "");
                    record.formName = getFormDisplayName(record.formType);
                    record.status = item["attributes"].value("status", "pending");
                    record.submittedAt = item["attributes"].value("submitted_at", "");
                    record.reviewedAt = item["attributes"].value("reviewed_at", "");
                    record.reviewedBy = item["attributes"].value("reviewed_by", "");
                    record.programName = item["attributes"].value("program_name", "");
                    submissions_.push_back(record);
                }
            }
        }
        std::cerr << "[FormSubmissionsWidget] Loaded " << submissions_.size() << " submissions" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FormSubmissionsWidget] Error loading submissions: " << e.what() << std::endl;
    }

    applyFilters();
}

void FormSubmissionsWidget::applyFilters() {
    filteredSubmissions_.clear();

    std::string searchText = searchInput_->text().toUTF8();
    std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);

    int formIndex = formTypeFilter_->currentIndex();
    std::string selectedForm = "";
    if (formIndex > 0) {
        // Map display names to form types
        std::vector<std::string> formTypes = {"personal_info", "emergency_contact", "medical_info",
            "academic_history", "financial_aid", "document_upload", "consent"};
        if (formIndex - 1 < static_cast<int>(formTypes.size())) {
            selectedForm = formTypes[formIndex - 1];
        }
    }

    int statusIndex = statusFilter_->currentIndex();
    std::string selectedStatus = "";
    if (statusIndex > 0) {
        std::vector<std::string> statuses = {"pending", "approved", "rejected", "needs_revision"};
        if (statusIndex - 1 < static_cast<int>(statuses.size())) {
            selectedStatus = statuses[statusIndex - 1];
        }
    }

    int programIndex = programFilter_->currentIndex();
    std::string selectedProgram = (programIndex > 0) ? programFilter_->currentText().toUTF8() : "";

    // Count statistics
    int pendingCount = 0, approvedCount = 0, rejectedCount = 0;

    for (const auto& submission : submissions_) {
        // Update counts
        if (submission.status == "pending") pendingCount++;
        else if (submission.status == "approved") approvedCount++;
        else if (submission.status == "rejected" || submission.status == "needs_revision") rejectedCount++;

        // Search filter
        if (!searchText.empty()) {
            std::string name = submission.studentName;
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::string email = submission.studentEmail;
            std::transform(email.begin(), email.end(), email.begin(), ::tolower);

            if (name.find(searchText) == std::string::npos &&
                email.find(searchText) == std::string::npos) {
                continue;
            }
        }

        // Form type filter
        if (!selectedForm.empty() && submission.formType != selectedForm) {
            continue;
        }

        // Status filter
        if (!selectedStatus.empty() && submission.status != selectedStatus) {
            continue;
        }

        // Program filter
        if (!selectedProgram.empty() && submission.programName != selectedProgram) {
            continue;
        }

        filteredSubmissions_.push_back(submission);
    }

    // Update statistics display
    pendingCountText_->setText(std::to_string(pendingCount));
    approvedCountText_->setText(std::to_string(approvedCount));
    rejectedCountText_->setText(std::to_string(rejectedCount));

    updateTable();
}

void FormSubmissionsWidget::resetFilters() {
    searchInput_->setText("");
    formTypeFilter_->setCurrentIndex(0);
    statusFilter_->setCurrentIndex(0);
    programFilter_->setCurrentIndex(0);
    applyFilters();
}

void FormSubmissionsWidget::updateTable() {
    submissionsTable_->clear();

    if (filteredSubmissions_.empty()) {
        resultCount_->setText("No results");
        tableContainer_->hide();
        noDataMessage_->show();
        return;
    }

    noDataMessage_->hide();
    tableContainer_->show();
    resultCount_->setText("Showing " + std::to_string(filteredSubmissions_.size()) + " of " +
                          std::to_string(submissions_.size()) + " submissions");

    // Header row
    submissionsTable_->setHeaderCount(1);
    int col = 0;
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Student"))->addStyleClass("admin-table-header");
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Form"))->addStyleClass("admin-table-header");
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Program"))->addStyleClass("admin-table-header");
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Submitted"))->addStyleClass("admin-table-header");
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"))->addStyleClass("admin-table-header");
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"))->addStyleClass("admin-table-header");

    int row = 1;
    for (const auto& submission : filteredSubmissions_) {
        col = 0;

        // Student info
        auto studentContainer = submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto nameText = studentContainer->addWidget(std::make_unique<Wt::WText>(submission.studentName));
        nameText->addStyleClass("admin-student-name-cell");
        studentContainer->addWidget(std::make_unique<Wt::WBreak>());
        auto emailText = studentContainer->addWidget(std::make_unique<Wt::WText>(submission.studentEmail));
        emailText->addStyleClass("admin-student-email-cell");
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Form name
        submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(submission.formName));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Program
        submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(submission.programName));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Submitted date
        submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(formatDate(submission.submittedAt)));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Status
        std::string statusDisplay = submission.status;
        if (statusDisplay == "needs_revision") statusDisplay = "Needs Revision";
        else if (!statusDisplay.empty()) {
            statusDisplay[0] = std::toupper(statusDisplay[0]);
        }
        auto statusBadge = submissionsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(statusDisplay));
        statusBadge->addStyleClass("badge " + getStatusBadgeClass(submission.status));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Actions
        auto actionsContainer = submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");
        int submissionId = submission.id;
        viewBtn->clicked().connect([this, submissionId]() {
            viewSubmissionClicked_.emit(submissionId);
        });

        if (submission.status == "pending" || submission.status == "needs_revision") {
            auto approveBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Approve"));
            approveBtn->addStyleClass("btn btn-sm btn-success");
            approveBtn->clicked().connect([this, submissionId]() {
                approveSubmission(submissionId);
            });

            auto rejectBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Reject"));
            rejectBtn->addStyleClass("btn btn-sm btn-danger");
            rejectBtn->clicked().connect([this, submissionId]() {
                rejectSubmission(submissionId);
            });
        }

        submissionsTable_->elementAt(row, col)->addStyleClass("admin-table-cell");

        row++;
    }
}

void FormSubmissionsWidget::approveSubmission(int submissionId) {
    std::cerr << "[FormSubmissionsWidget] Approving submission: " << submissionId << std::endl;

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "form_submission";
            payload["data"]["id"] = std::to_string(submissionId);
            payload["data"]["attributes"]["status"] = "approved";

            auto response = apiService_->getApiClient()->patch("/FormSubmission/" + std::to_string(submissionId), payload.dump());
            if (response.success) {
                std::cerr << "[FormSubmissionsWidget] Submission approved successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormSubmissionsWidget] Error approving submission: " << e.what() << std::endl;
        }
    }

    // Update local state
    for (auto& submission : submissions_) {
        if (submission.id == submissionId) {
            submission.status = "approved";
            break;
        }
    }

    approveClicked_.emit(submissionId, "approved");
    applyFilters();
}

void FormSubmissionsWidget::rejectSubmission(int submissionId) {
    std::cerr << "[FormSubmissionsWidget] Rejecting submission: " << submissionId << std::endl;

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "form_submission";
            payload["data"]["id"] = std::to_string(submissionId);
            payload["data"]["attributes"]["status"] = "rejected";

            auto response = apiService_->getApiClient()->patch("/FormSubmission/" + std::to_string(submissionId), payload.dump());
            if (response.success) {
                std::cerr << "[FormSubmissionsWidget] Submission rejected successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "[FormSubmissionsWidget] Error rejecting submission: " << e.what() << std::endl;
        }
    }

    // Update local state
    for (auto& submission : submissions_) {
        if (submission.id == submissionId) {
            submission.status = "rejected";
            break;
        }
    }

    rejectClicked_.emit(submissionId, "rejected");
    applyFilters();
}

std::string FormSubmissionsWidget::getStatusBadgeClass(const std::string& status) {
    if (status == "pending") return "badge-warning";
    if (status == "approved") return "badge-success";
    if (status == "rejected") return "badge-danger";
    if (status == "needs_revision") return "badge-info";
    return "badge-secondary";
}

std::string FormSubmissionsWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    // Parse ISO date and format for display
    // Input: 2026-01-15T10:30:00Z
    // Output: Jan 15, 2026
    try {
        if (dateStr.length() >= 10) {
            std::string year = dateStr.substr(0, 4);
            std::string month = dateStr.substr(5, 2);
            std::string day = dateStr.substr(8, 2);

            std::vector<std::string> months = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

            int monthIdx = std::stoi(month) - 1;
            if (monthIdx >= 0 && monthIdx < 12) {
                return months[monthIdx] + " " + std::to_string(std::stoi(day)) + ", " + year;
            }
        }
    } catch (...) {
        // Fall through to return original
    }

    return dateStr;
}

std::string FormSubmissionsWidget::getFormDisplayName(const std::string& formType) {
    if (formType == "personal_info") return "Personal Information";
    if (formType == "emergency_contact") return "Emergency Contact";
    if (formType == "medical_info") return "Medical Information";
    if (formType == "academic_history") return "Academic History";
    if (formType == "financial_aid") return "Financial Aid";
    if (formType == "document_upload") return "Document Upload";
    if (formType == "consent") return "Consent Form";
    return formType;
}

} // namespace Admin
} // namespace StudentIntake
