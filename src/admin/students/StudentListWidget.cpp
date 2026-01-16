#include "StudentListWidget.h"
#include <Wt/WTemplate.h>
#include <Wt/WBreak.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Admin {

StudentListWidget::StudentListWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , searchInput_(nullptr)
    , programFilter_(nullptr)
    , statusFilter_(nullptr)
    , searchButton_(nullptr)
    , clearButton_(nullptr)
    , tableContainer_(nullptr)
    , studentTable_(nullptr)
    , resultCount_(nullptr) {
    setupUI();
}

StudentListWidget::~StudentListWidget() {
}

void StudentListWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void StudentListWidget::setupUI() {
    addStyleClass("admin-student-list");

    // Header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("admin-section-header");

    auto title = header->addWidget(std::make_unique<Wt::WText>("Student Management"));
    title->addStyleClass("admin-section-title");

    auto subtitle = header->addWidget(std::make_unique<Wt::WText>(
        "View and manage all enrolled students"));
    subtitle->addStyleClass("admin-section-subtitle");

    // Filters section
    setupFilters();

    // Results count
    resultCount_ = addWidget(std::make_unique<Wt::WText>(""));
    resultCount_->addStyleClass("admin-result-count");

    // Table container
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("admin-table-container");

    setupTable();
}

void StudentListWidget::setupFilters() {
    auto filterContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer->addStyleClass("admin-filter-container");

    // Search input
    auto searchGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group");

    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WText>("Search"));
    searchLabel->addStyleClass("admin-filter-label");

    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search by name or email...");
    searchInput_->addStyleClass("admin-filter-input");
    searchInput_->enterPressed().connect([this]() {
        applyFilters();
    });

    // Program filter
    auto programGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    programGroup->addStyleClass("admin-filter-group");

    auto programLabel = programGroup->addWidget(std::make_unique<Wt::WText>("Program"));
    programLabel->addStyleClass("admin-filter-label");

    programFilter_ = programGroup->addWidget(std::make_unique<Wt::WComboBox>());
    programFilter_->addStyleClass("admin-filter-select");
    programFilter_->addItem("All Programs");
    programFilter_->addItem("Computer Science - BS");
    programFilter_->addItem("Information Technology - BS");
    programFilter_->addItem("Software Engineering - BS");
    programFilter_->addItem("Data Science - MS");

    // Status filter
    auto statusGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusGroup->addStyleClass("admin-filter-group");

    auto statusLabel = statusGroup->addWidget(std::make_unique<Wt::WText>("Status"));
    statusLabel->addStyleClass("admin-filter-label");

    statusFilter_ = statusGroup->addWidget(std::make_unique<Wt::WComboBox>());
    statusFilter_->addStyleClass("admin-filter-select");
    statusFilter_->addItem("All Status");
    statusFilter_->addItem("Active");
    statusFilter_->addItem("Pending");
    statusFilter_->addItem("Completed");
    statusFilter_->addItem("Revoked");

    // Buttons
    auto buttonGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons");

    searchButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Search"));
    searchButton_->addStyleClass("btn btn-primary");
    searchButton_->clicked().connect([this]() {
        applyFilters();
    });

    clearButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
    clearButton_->addStyleClass("btn btn-secondary");
    clearButton_->clicked().connect([this]() {
        clearFilters();
    });
}

void StudentListWidget::setupTable() {
    studentTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    studentTable_->addStyleClass("admin-data-table");
    studentTable_->setHeaderCount(1);

    // Table headers
    studentTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("ID"));
    studentTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Name"));
    studentTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Email"));
    studentTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Program"));
    studentTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Status"));
    studentTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Enrolled"));
    studentTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < 7; i++) {
        studentTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }
}

void StudentListWidget::refresh() {
    loadStudents();
}

void StudentListWidget::loadStudents() {
    if (!apiService_) {
        std::cerr << "[StudentList] API service not available" << std::endl;
        return;
    }

    try {
        std::cerr << "[StudentList] Loading students..." << std::endl;

        auto response = apiService_->getApiClient()->get("student");

        if (!response.success) {
            std::cerr << "[StudentList] Failed to load students: " << response.errorMessage << std::endl;
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);
        allStudents_.clear();

        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            for (const auto& studentData : jsonResponse["data"]) {
                Models::Student student;

                if (studentData.contains("id")) {
                    if (studentData["id"].is_string()) {
                        student.setId(std::stoi(studentData["id"].get<std::string>()));
                    } else {
                        student.setId(studentData["id"].get<int>());
                    }
                }

                if (studentData.contains("attributes")) {
                    auto& attrs = studentData["attributes"];

                    if (attrs.contains("email") && !attrs["email"].is_null()) {
                        student.setEmail(attrs["email"].get<std::string>());
                    }
                    if (attrs.contains("first_name") && !attrs["first_name"].is_null()) {
                        student.setFirstName(attrs["first_name"].get<std::string>());
                    }
                    if (attrs.contains("last_name") && !attrs["last_name"].is_null()) {
                        student.setLastName(attrs["last_name"].get<std::string>());
                    }
                    if (attrs.contains("created_at") && !attrs["created_at"].is_null()) {
                        student.setCreatedAt(attrs["created_at"].get<std::string>());
                    }
                }

                allStudents_.push_back(student);
            }
        }

        std::cerr << "[StudentList] Loaded " << allStudents_.size() << " students" << std::endl;
        updateTable(allStudents_);

    } catch (const std::exception& e) {
        std::cerr << "[StudentList] Exception loading students: " << e.what() << std::endl;
    }
}

void StudentListWidget::applyFilters() {
    std::string searchTerm = searchInput_->text().toUTF8();
    int programIndex = programFilter_->currentIndex();
    int statusIndex = statusFilter_->currentIndex();

    std::vector<Models::Student> filtered;

    for (const auto& student : allStudents_) {
        bool matches = true;

        // Search filter (name or email)
        if (!searchTerm.empty()) {
            std::string fullName = student.getFullName();
            std::string email = student.getEmail();

            // Case-insensitive search
            std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::tolower);
            std::transform(email.begin(), email.end(), email.begin(), ::tolower);
            std::string searchLower = searchTerm;
            std::transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);

            if (fullName.find(searchLower) == std::string::npos &&
                email.find(searchLower) == std::string::npos) {
                matches = false;
            }
        }

        // Program filter (index 0 is "All Programs")
        if (programIndex > 0 && matches) {
            // In a real implementation, check student's curriculum
            // For now, we'll skip this filter
        }

        // Status filter (index 0 is "All Status")
        if (statusIndex > 0 && matches) {
            // In a real implementation, check student's status
            // For now, we'll skip this filter
        }

        if (matches) {
            filtered.push_back(student);
        }
    }

    updateTable(filtered);
}

void StudentListWidget::clearFilters() {
    searchInput_->setText("");
    programFilter_->setCurrentIndex(0);
    statusFilter_->setCurrentIndex(0);
    updateTable(allStudents_);
}

void StudentListWidget::updateTable(const std::vector<Models::Student>& students) {
    // Clear existing rows (except header)
    while (studentTable_->rowCount() > 1) {
        studentTable_->removeRow(1);
    }

    // Update result count
    resultCount_->setText("Showing " + std::to_string(students.size()) + " student(s)");

    // Add student rows
    int row = 1;
    for (const auto& student : students) {
        studentTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>(std::to_string(student.getId())));

        studentTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>(student.getFullName()));

        studentTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(student.getEmail()));

        // Program (placeholder for now)
        studentTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>("Computer Science - BS"));

        // Status badge
        auto statusBadge = studentTable_->elementAt(row, 4)->addWidget(
            std::make_unique<Wt::WText>("Active"));
        statusBadge->addStyleClass("badge badge-success");

        // Enrolled date
        studentTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WText>(formatDate(student.getCreatedAt())));

        // Actions
        auto actionsContainer = studentTable_->elementAt(row, 6)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(
            std::make_unique<Wt::WPushButton>("View"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");

        int studentId = student.getId();
        viewBtn->clicked().connect([this, studentId]() {
            onStudentRowClicked(studentId);
        });

        // Style row
        for (int col = 0; col < 7; col++) {
            studentTable_->elementAt(row, col)->addStyleClass("admin-table-cell");
        }

        row++;
    }
}

void StudentListWidget::onStudentRowClicked(int studentId) {
    std::cerr << "[StudentList] Student selected: " << studentId << std::endl;
    studentSelected_.emit(studentId);
}

std::string StudentListWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    // Try to extract just the date part (YYYY-MM-DD)
    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

std::string StudentListWidget::getStatusBadgeClass(const std::string& status) {
    if (status == "active") return "badge badge-success";
    if (status == "pending") return "badge badge-warning";
    if (status == "completed") return "badge badge-info";
    if (status == "revoked") return "badge badge-danger";
    return "badge badge-secondary";
}

} // namespace Admin
} // namespace StudentIntake
