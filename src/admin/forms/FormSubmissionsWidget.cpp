#include "FormSubmissionsWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <map>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormSubmissionsWidget::FormSubmissionsWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , statsContainer_(nullptr)
    , todayCountText_(nullptr)
    , pendingCountText_(nullptr)
    , approvedCountText_(nullptr)
    , rejectedCountText_(nullptr)
    , revisionCountText_(nullptr)
    , filterContainer_(nullptr)
    , searchInput_(nullptr)
    , formTypeFilter_(nullptr)
    , statusFilter_(nullptr)
    , programFilter_(nullptr)
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
    statsContainer_->addStyleClass("admin-submission-stats admin-submission-stats-5");

    // Today card - first for prominence
    auto todayCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    todayCard->addStyleClass("admin-stat-mini-card today");
    auto todayIcon = todayCard->addWidget(std::make_unique<Wt::WText>("📅"));
    todayIcon->addStyleClass("admin-stat-mini-icon");
    todayCountText_ = todayCard->addWidget(std::make_unique<Wt::WText>("0"));
    todayCountText_->addStyleClass("admin-stat-mini-number");
    auto todayLabel = todayCard->addWidget(std::make_unique<Wt::WText>("Today"));
    todayLabel->addStyleClass("admin-stat-mini-label");

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

    // Needs Revision card
    auto revisionCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    revisionCard->addStyleClass("admin-stat-mini-card revision");
    auto revisionIcon = revisionCard->addWidget(std::make_unique<Wt::WText>("✎"));
    revisionIcon->addStyleClass("admin-stat-mini-icon");
    revisionCountText_ = revisionCard->addWidget(std::make_unique<Wt::WText>("0"));
    revisionCountText_->addStyleClass("admin-stat-mini-number");
    auto revisionLabel = revisionCard->addWidget(std::make_unique<Wt::WText>("Needs Revision"));
    revisionLabel->addStyleClass("admin-stat-mini-label");

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
    searchInput_->textInput().connect(this, &FormSubmissionsWidget::applyFilters);

    // Form type filter
    auto formGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formGroup->addStyleClass("admin-filter-group");

    auto formLabel = formGroup->addWidget(std::make_unique<Wt::WText>("Form Type"));
    formLabel->addStyleClass("admin-filter-label");

    formTypeFilter_ = formGroup->addWidget(std::make_unique<Wt::WComboBox>());
    formTypeFilter_->addStyleClass("admin-filter-select");
    formTypeFilter_->addItem("All Forms");  // Will be repopulated by loadFormTypes()
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
    programFilter_->addItem("All Programs");  // Will be repopulated by loadPrograms()
    programFilter_->changed().connect(this, &FormSubmissionsWidget::applyFilters);

    // Clear button only - filters apply automatically on change
    auto buttonGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons admin-filter-buttons-right");

    resetBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
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

void FormSubmissionsWidget::loadData() {
    // Load form types and programs first (for dropdowns and ID mapping)
    loadFormTypes();
    loadPrograms();
    loadSubmissions();
}

void FormSubmissionsWidget::clearData() {
    submissions_.clear();
    filteredSubmissions_.clear();
    if (submissionsTable_) {
        submissionsTable_->clear();
    }
}

void FormSubmissionsWidget::loadFormTypes() {
    formTypeIdToCode_.clear();
    formTypeCodeToName_.clear();

    if (!apiService_) return;

    try {
        auto response = apiService_->getApiClient()->get("/FormType");
        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json items;

            if (json.is_array()) {
                items = json;
            } else if (json.contains("data") && json["data"].is_array()) {
                items = json["data"];
            }

            std::cerr << "[FormSubmissionsWidget] Loading " << items.size() << " form types" << std::endl;

            for (const auto& item : items) {
                int id = 0;
                std::string code, name;

                // Get ID
                if (item.contains("id")) {
                    if (item["id"].is_string()) {
                        id = std::stoi(item["id"].get<std::string>());
                    } else {
                        id = item["id"].get<int>();
                    }
                }

                // Get attributes
                const auto& attrs = item.contains("attributes") ? item["attributes"] : item;
                code = attrs.value("code", "");
                name = attrs.value("name", code);  // Use code as fallback for name

                if (id > 0 && !code.empty()) {
                    formTypeIdToCode_[id] = code;
                    formTypeCodeToName_[code] = name;
                    std::cerr << "[FormSubmissionsWidget] FormType: id=" << id << ", code=" << code << ", name=" << name << std::endl;
                }
            }

            // Update the form type filter dropdown
            formTypeFilter_->clear();
            formTypeFilter_->addItem("All Forms");
            for (const auto& pair : formTypeCodeToName_) {
                formTypeFilter_->addItem(pair.second);  // Add display name
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FormSubmissionsWidget] Error loading form types: " << e.what() << std::endl;
    }
}

void FormSubmissionsWidget::loadPrograms() {
    programs_.clear();

    if (!apiService_) return;

    try {
        auto response = apiService_->getApiClient()->get("/Curriculum");
        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json items;

            if (json.is_array()) {
                items = json;
            } else if (json.contains("data") && json["data"].is_array()) {
                items = json["data"];
            }

            std::cerr << "[FormSubmissionsWidget] Loading " << items.size() << " programs" << std::endl;

            for (const auto& item : items) {
                int id = 0;
                std::string name;

                // Get ID
                if (item.contains("id")) {
                    if (item["id"].is_string()) {
                        id = std::stoi(item["id"].get<std::string>());
                    } else {
                        id = item["id"].get<int>();
                    }
                }

                // Get attributes
                const auto& attrs = item.contains("attributes") ? item["attributes"] : item;
                name = attrs.value("name", "");

                if (id > 0 && !name.empty()) {
                    programs_.push_back({id, name});
                    std::cerr << "[FormSubmissionsWidget] Program: id=" << id << ", name=" << name << std::endl;
                }
            }

            // Update the program filter dropdown
            programFilter_->clear();
            programFilter_->addItem("All Programs");
            for (const auto& program : programs_) {
                programFilter_->addItem(program.second);  // Add program name
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[FormSubmissionsWidget] Error loading programs: " << e.what() << std::endl;
    }
}

void FormSubmissionsWidget::loadSubmissions() {
    submissions_.clear();

    // Build a cache of student info for efficient lookups
    std::map<int, std::pair<std::string, std::string>> studentCache;  // id -> (name, email)
    std::map<int, std::string> curriculumCache;  // id -> name

    // Try to load from API if available
    if (apiService_) {
        try {
            // First, load students to get names and emails
            auto studentsResponse = apiService_->getApiClient()->get("/Student");
            if (studentsResponse.success) {
                auto studentsJson = nlohmann::json::parse(studentsResponse.body);
                nlohmann::json studentItems;
                if (studentsJson.is_array()) {
                    studentItems = studentsJson;
                } else if (studentsJson.contains("data") && studentsJson["data"].is_array()) {
                    studentItems = studentsJson["data"];
                }

                for (const auto& student : studentItems) {
                    int studentId = 0;
                    std::string name, email;
                    int curriculumId = 0;

                    if (student.contains("id")) {
                        if (student["id"].is_string()) {
                            studentId = std::stoi(student["id"].get<std::string>());
                        } else {
                            studentId = student["id"].get<int>();
                        }
                    }

                    const auto& attrs = student.contains("attributes") ? student["attributes"] : student;
                    std::string firstName = attrs.value("first_name", "");
                    std::string lastName = attrs.value("last_name", "");
                    name = firstName + " " + lastName;
                    email = attrs.value("email", "");
                    curriculumId = attrs.value("curriculum_id", 0);

                    if (studentId > 0) {
                        studentCache[studentId] = {name, email};
                    }
                }
            }

            // Load curricula for program names
            auto curriculumResponse = apiService_->getApiClient()->get("/Curriculum");
            if (curriculumResponse.success) {
                auto curriculumJson = nlohmann::json::parse(curriculumResponse.body);
                nlohmann::json curriculumItems;
                if (curriculumJson.is_array()) {
                    curriculumItems = curriculumJson;
                } else if (curriculumJson.contains("data") && curriculumJson["data"].is_array()) {
                    curriculumItems = curriculumJson["data"];
                }

                for (const auto& curr : curriculumItems) {
                    int currId = 0;
                    std::string currName;

                    if (curr.contains("id")) {
                        if (curr["id"].is_string()) {
                            currId = std::stoi(curr["id"].get<std::string>());
                        } else {
                            currId = curr["id"].get<int>();
                        }
                    }

                    const auto& attrs = curr.contains("attributes") ? curr["attributes"] : curr;
                    currName = attrs.value("name", "");

                    if (currId > 0) {
                        curriculumCache[currId] = currName;
                    }
                }
            }

            // Now load form submissions
            auto response = apiService_->getApiClient()->get("/FormSubmission");
            if (response.success) {
                auto jsonResponse = nlohmann::json::parse(response.body);
                nlohmann::json items;
                if (jsonResponse.is_array()) {
                    items = jsonResponse;
                } else if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                    items = jsonResponse["data"];
                }

                for (const auto& item : items) {
                    FormSubmissionRecord record;

                    // Handle id - can be string or int in JSON:API
                    if (item.contains("id")) {
                        if (item["id"].is_string()) {
                            record.id = std::stoi(item["id"].get<std::string>());
                        } else {
                            record.id = item["id"].get<int>();
                        }
                    }

                    // Get attributes
                    const auto& attrs = item.contains("attributes") ? item["attributes"] : item;
                    record.studentId = attrs.value("student_id", 0);
                    record.status = attrs.value("status", "pending");
                    record.submittedAt = attrs.value("submitted_at", "");
                    record.reviewedAt = attrs.value("approved_at", "");
                    record.reviewedBy = attrs.value("approved_by", "");

                    // Map form_type_id to form type name
                    int formTypeId = attrs.value("form_type_id", 0);
                    record.formType = getFormTypeFromId(formTypeId);
                    record.formName = getFormDisplayName(record.formType);

                    // Look up student info from cache
                    auto studentIt = studentCache.find(record.studentId);
                    if (studentIt != studentCache.end()) {
                        record.studentName = studentIt->second.first;
                        record.studentEmail = studentIt->second.second;
                    } else {
                        record.studentName = "Student #" + std::to_string(record.studentId);
                        record.studentEmail = "";
                    }

                    // For program name, we'd need to look up the student's curriculum_id
                    // For now, leave it empty or populate from a separate query if needed
                    record.programName = "";

                    submissions_.push_back(record);
                }
            }
            std::cerr << "[FormSubmissionsWidget] Loaded " << submissions_.size() << " submissions from API" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[FormSubmissionsWidget] Error loading from API: " << e.what() << std::endl;
        }
    }

    // Log when no submissions found (no mock data - this is a real system now)
    if (submissions_.empty()) {
        std::cerr << "[FormSubmissionsWidget] No form submissions found in database" << std::endl;
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
        // Get the display name from the dropdown
        std::string displayName = formTypeFilter_->currentText().toUTF8();
        // Find the code that matches this display name
        for (const auto& pair : formTypeCodeToName_) {
            if (pair.second == displayName) {
                selectedForm = pair.first;  // Use the code
                break;
            }
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
    int todayCount = 0, pendingCount = 0, approvedCount = 0, rejectedCount = 0, revisionCount = 0;

    for (const auto& submission : submissions_) {
        // Update counts
        if (isToday(submission.submittedAt)) todayCount++;
        if (submission.status == "pending") pendingCount++;
        else if (submission.status == "approved") approvedCount++;
        else if (submission.status == "rejected") rejectedCount++;
        else if (submission.status == "needs_revision") revisionCount++;

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
    todayCountText_->setText(std::to_string(todayCount));
    pendingCountText_->setText(std::to_string(pendingCount));
    approvedCountText_->setText(std::to_string(approvedCount));
    rejectedCountText_->setText(std::to_string(rejectedCount));
    revisionCountText_->setText(std::to_string(revisionCount));

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

    // Header row - icon column first
    submissionsTable_->setHeaderCount(1);
    int col = 0;
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>(""));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Student"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Form"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Program"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Submitted"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"));

    // Apply header styling to cells
    for (int i = 0; i < col; i++) {
        submissionsTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    int row = 1;
    for (const auto& submission : filteredSubmissions_) {
        col = 0;

        // Form icon
        auto iconCell = submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>("📋"));
        iconCell->addStyleClass("admin-row-icon form-icon");
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

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
    // Use dynamic map loaded from API
    auto it = formTypeCodeToName_.find(formType);
    if (it != formTypeCodeToName_.end()) {
        return it->second;
    }
    // Fallback to hardcoded values for backwards compatibility
    if (formType == "personal_info") return "Personal Information";
    if (formType == "emergency_contact") return "Emergency Contacts";
    if (formType == "medical_info") return "Medical Information";
    if (formType == "academic_history") return "Academic History";
    if (formType == "financial_aid") return "Financial Aid";
    if (formType == "documents") return "Document Upload";
    if (formType == "consent") return "Terms and Consent";
    return formType;
}

std::string FormSubmissionsWidget::getFormTypeFromId(int formTypeId) {
    // Use dynamic map loaded from API
    auto it = formTypeIdToCode_.find(formTypeId);
    if (it != formTypeIdToCode_.end()) {
        return it->second;
    }
    return "unknown";
}

std::string FormSubmissionsWidget::getTodayDateString() {
    // Get current date in YYYY-MM-DD format
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now = *std::localtime(&time_t_now);

    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &tm_now);
    return std::string(buffer);
}

bool FormSubmissionsWidget::isToday(const std::string& dateStr) {
    if (dateStr.empty() || dateStr.length() < 10) {
        return false;
    }

    // Extract YYYY-MM-DD from the ISO date string
    std::string submissionDate = dateStr.substr(0, 10);
    std::string today = getTodayDateString();

    return submissionDate == today;
}

} // namespace Admin
} // namespace StudentIntake
