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
    searchInput_->textInput().connect([this]() {
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
    programFilter_->changed().connect([this]() {
        applyFilters();
    });

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
    statusFilter_->changed().connect([this]() {
        applyFilters();
    });

    // Clear button only - filters apply automatically on change
    auto buttonGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons");

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

    // Table headers - icon column first
    auto iconHeader = studentTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>(""));
    iconHeader->addStyleClass("admin-table-icon-header");
    studentTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("ID"));
    studentTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Name"));
    studentTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Email"));
    studentTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Program"));
    studentTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Status"));
    studentTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Enrolled"));
    studentTable_->elementAt(0, 7)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < 8; i++) {
        studentTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }
}

void StudentListWidget::refresh() {
    loadCurriculum();
    loadStudents();
}

void StudentListWidget::loadCurriculum() {
    if (!apiService_) {
        return;
    }

    try {
        auto response = apiService_->getApiClient()->get("/Curriculum");

        if (!response.success) {
            std::cerr << "[StudentList] Failed to load curriculum: " << response.errorMessage << std::endl;
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);
        curriculumMap_.clear();

        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            for (const auto& currData : jsonResponse["data"]) {
                std::string id;
                std::string programName;

                if (currData.contains("id")) {
                    if (currData["id"].is_string()) {
                        id = currData["id"].get<std::string>();
                    } else {
                        id = std::to_string(currData["id"].get<int>());
                    }
                }

                if (currData.contains("attributes")) {
                    auto& attrs = currData["attributes"];
                    if (attrs.contains("program_name") && !attrs["program_name"].is_null()) {
                        programName = attrs["program_name"].get<std::string>();
                    }
                }

                if (!id.empty() && !programName.empty()) {
                    curriculumMap_[id] = programName;
                }
            }
        }

        std::cerr << "[StudentList] Loaded " << curriculumMap_.size() << " curriculum entries" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[StudentList] Exception loading curriculum: " << e.what() << std::endl;
    }
}

std::string StudentListWidget::getProgramName(const std::string& curriculumId) const {
    auto it = curriculumMap_.find(curriculumId);
    if (it != curriculumMap_.end()) {
        return it->second;
    }
    return "Unknown Program";
}

void StudentListWidget::loadStudents() {
    if (!apiService_) {
        std::cerr << "[StudentList] API service not available" << std::endl;
        return;
    }

    try {
        std::cerr << "[StudentList] Loading students..." << std::endl;

        auto response = apiService_->getApiClient()->get("/Student");

        if (!response.success) {
            std::cerr << "[StudentList] Failed to load students: " << response.errorMessage << std::endl;
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);
        allStudents_.clear();

        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            for (const auto& studentData : jsonResponse["data"]) {
                ::StudentIntake::Models::Student student;

                if (studentData.contains("id")) {
                    if (studentData["id"].is_string()) {
                        student.setId(studentData["id"].get<std::string>());
                    } else {
                        student.setId(std::to_string(studentData["id"].get<int>()));
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
                    // Load curriculum_id
                    if (attrs.contains("curriculum_id") && !attrs["curriculum_id"].is_null()) {
                        if (attrs["curriculum_id"].is_string()) {
                            student.setCurriculumId(attrs["curriculum_id"].get<std::string>());
                        } else {
                            student.setCurriculumId(std::to_string(attrs["curriculum_id"].get<int>()));
                        }
                    }
                    // Load status
                    if (attrs.contains("status") && !attrs["status"].is_null()) {
                        student.setStatus(attrs["status"].get<std::string>());
                    } else {
                        student.setStatus("active");  // Default to active if not specified
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

    std::vector<::StudentIntake::Models::Student> filtered;

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
            std::string selectedProgram = programFilter_->currentText().toUTF8();
            std::string studentProgram = getProgramName(student.getCurriculumId());
            if (studentProgram != selectedProgram) {
                matches = false;
            }
        }

        // Status filter (index 0 is "All Status")
        if (statusIndex > 0 && matches) {
            std::string selectedStatus = statusFilter_->currentText().toUTF8();
            std::string studentStatus = student.getStatus();

            // Case-insensitive comparison
            std::transform(selectedStatus.begin(), selectedStatus.end(), selectedStatus.begin(), ::tolower);
            std::transform(studentStatus.begin(), studentStatus.end(), studentStatus.begin(), ::tolower);

            if (studentStatus != selectedStatus) {
                matches = false;
            }
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

void StudentListWidget::updateTable(const std::vector<::StudentIntake::Models::Student>& students) {
    // Clear existing rows (except header)
    while (studentTable_->rowCount() > 1) {
        studentTable_->removeRow(1);
    }

    // Update result count
    resultCount_->setText("Showing " + std::to_string(students.size()) + " student(s)");

    // Add student rows
    int row = 1;
    for (const auto& student : students) {
        // Student icon
        auto iconCell = studentTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>("👤"));
        iconCell->addStyleClass("admin-row-icon student-icon");

        studentTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>(student.getId()));

        studentTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(student.getFullName()));

        studentTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(student.getEmail()));

        // Program - use actual curriculum data
        std::string programName = getProgramName(student.getCurriculumId());
        studentTable_->elementAt(row, 4)->addWidget(
            std::make_unique<Wt::WText>(programName));

        // Status badge - use actual student status
        std::string status = student.getStatus();
        if (status.empty()) {
            status = "Active";
        } else {
            // Capitalize first letter for display
            status[0] = std::toupper(status[0]);
        }
        auto statusBadge = studentTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WText>(status));
        statusBadge->addStyleClass(getStatusBadgeClass(student.getStatus()));

        // Enrolled date
        studentTable_->elementAt(row, 6)->addWidget(
            std::make_unique<Wt::WText>(formatDate(student.getCreatedAt())));

        // Actions
        auto actionsContainer = studentTable_->elementAt(row, 7)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(
            std::make_unique<Wt::WPushButton>("View"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");

        int studentId = std::stoi(student.getId());
        viewBtn->clicked().connect([this, studentId]() {
            onStudentRowClicked(studentId);
        });

        // Style row
        for (int col = 0; col < 8; col++) {
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
