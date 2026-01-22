#include "StudentListWidget.h"
#include <Wt/WTemplate.h>
#include <Wt/WBreak.h>
#include <algorithm>
#include "utils/Logger.h"
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Admin {

StudentListWidget::StudentListWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , statsContainer_(nullptr)
    , activeCountText_(nullptr)
    , pendingCountText_(nullptr)
    , completedCountText_(nullptr)
    , revokedCountText_(nullptr)
    , addStudentBtn_(nullptr)
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

    // Header with title and Add Student button
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("admin-section-header admin-section-header-with-action");

    auto titleContainer = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleContainer->addStyleClass("admin-header-title-container");

    auto title = titleContainer->addWidget(std::make_unique<Wt::WText>("Student Management"));
    title->addStyleClass("admin-section-title");

    auto subtitle = titleContainer->addWidget(std::make_unique<Wt::WText>(
        "View and manage all enrolled students"));
    subtitle->addStyleClass("admin-section-subtitle");

    // Add Student button
    addStudentBtn_ = header->addWidget(std::make_unique<Wt::WPushButton>("+ Add Student"));
    addStudentBtn_->addStyleClass("btn btn-primary");
    addStudentBtn_->clicked().connect([this]() {
        showAddStudentDialog();
    });

    // Statistics cards
    setupStats();

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

void StudentListWidget::setupStats() {
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Active card
    auto activeCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    activeCard->addStyleClass("admin-stat-mini-card active");
    auto activeIcon = activeCard->addWidget(std::make_unique<Wt::WText>("✓"));
    activeIcon->addStyleClass("admin-stat-mini-icon");
    activeCountText_ = activeCard->addWidget(std::make_unique<Wt::WText>("0"));
    activeCountText_->addStyleClass("admin-stat-mini-number");
    auto activeLabel = activeCard->addWidget(std::make_unique<Wt::WText>("Active"));
    activeLabel->addStyleClass("admin-stat-mini-label");

    // Pending card
    auto pendingCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    pendingCard->addStyleClass("admin-stat-mini-card pending");
    auto pendingIcon = pendingCard->addWidget(std::make_unique<Wt::WText>("⏳"));
    pendingIcon->addStyleClass("admin-stat-mini-icon");
    pendingCountText_ = pendingCard->addWidget(std::make_unique<Wt::WText>("0"));
    pendingCountText_->addStyleClass("admin-stat-mini-number");
    auto pendingLabel = pendingCard->addWidget(std::make_unique<Wt::WText>("Pending"));
    pendingLabel->addStyleClass("admin-stat-mini-label");

    // Completed card
    auto completedCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completedCard->addStyleClass("admin-stat-mini-card approved");
    auto completedIcon = completedCard->addWidget(std::make_unique<Wt::WText>("★"));
    completedIcon->addStyleClass("admin-stat-mini-icon");
    completedCountText_ = completedCard->addWidget(std::make_unique<Wt::WText>("0"));
    completedCountText_->addStyleClass("admin-stat-mini-number");
    auto completedLabel = completedCard->addWidget(std::make_unique<Wt::WText>("Completed"));
    completedLabel->addStyleClass("admin-stat-mini-label");

    // Revoked card
    auto revokedCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    revokedCard->addStyleClass("admin-stat-mini-card rejected");
    auto revokedIcon = revokedCard->addWidget(std::make_unique<Wt::WText>("✗"));
    revokedIcon->addStyleClass("admin-stat-mini-icon");
    revokedCountText_ = revokedCard->addWidget(std::make_unique<Wt::WText>("0"));
    revokedCountText_->addStyleClass("admin-stat-mini-number");
    auto revokedLabel = revokedCard->addWidget(std::make_unique<Wt::WText>("Revoked"));
    revokedLabel->addStyleClass("admin-stat-mini-label");
}

void StudentListWidget::setupFilters() {
    auto filterContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer->addStyleClass("admin-filter-container");

    // Search input
    auto searchGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group admin-filter-search");

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
    programFilter_->addItem("All Programs");  // Will be repopulated by loadCurriculum()
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
    buttonGroup->addStyleClass("admin-filter-buttons admin-filter-buttons-right");

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
            LOG_ERROR("StudentList", "Failed to load curriculum: " << response.errorMessage);
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);
        curriculumMap_.clear();

        // Collect unique program names for the filter dropdown
        std::vector<std::string> programNames;

        nlohmann::json items;
        if (jsonResponse.is_array()) {
            items = jsonResponse;
        } else if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            items = jsonResponse["data"];
        }

        for (const auto& currData : items) {
            std::string id;
            std::string programName;

            if (currData.contains("id")) {
                if (currData["id"].is_string()) {
                    id = currData["id"].get<std::string>();
                } else {
                    id = std::to_string(currData["id"].get<int>());
                }
            }

            // Get attributes - check both JSON:API format and flat format
            const auto& attrs = currData.contains("attributes") ? currData["attributes"] : currData;

            // Try different field names for program name
            if (attrs.contains("name") && !attrs["name"].is_null()) {
                programName = attrs["name"].get<std::string>();
            } else if (attrs.contains("program_name") && !attrs["program_name"].is_null()) {
                programName = attrs["program_name"].get<std::string>();
            }

            if (!id.empty() && !programName.empty()) {
                curriculumMap_[id] = programName;
                programNames.push_back(programName);
            }
        }

        LOG_DEBUG("StudentList", "Loaded " << curriculumMap_.size() << " curriculum entries");

        // Update the program filter dropdown
        programFilter_->clear();
        programFilter_->addItem("All Programs");
        for (const auto& name : programNames) {
            programFilter_->addItem(name);
        }

    } catch (const std::exception& e) {
        LOG_ERROR("StudentList", "Exception loading curriculum: " << e.what());
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
        LOG_WARN("StudentList", "API service not available");
        return;
    }

    try {
        LOG_DEBUG("StudentList", "Loading students...");

        auto response = apiService_->getApiClient()->get("/Student");

        if (!response.success) {
            LOG_ERROR("StudentList", "Failed to load students: " << response.errorMessage);
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

                    // Check if login is revoked - override status to "revoked" if true
                    if (attrs.contains("is_login_revoked") && !attrs["is_login_revoked"].is_null()) {
                        if (attrs["is_login_revoked"].get<bool>()) {
                            student.setStatus("revoked");
                        }
                    }
                }

                allStudents_.push_back(student);
            }
        }

        LOG_DEBUG("StudentList", "Loaded " << allStudents_.size() << " students");
        updateTable(allStudents_);
        updateStats();  // Update stat cards with correct counts

    } catch (const std::exception& e) {
        LOG_ERROR("StudentList", "Exception loading students: " << e.what());
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
    updateStats();
}

void StudentListWidget::updateStats() {
    int activeCount = 0, pendingCount = 0, completedCount = 0, revokedCount = 0;

    for (const auto& student : allStudents_) {
        std::string status = student.getStatus();
        // Convert to lowercase for comparison
        std::transform(status.begin(), status.end(), status.begin(), ::tolower);

        if (status == "active") activeCount++;
        else if (status == "pending") pendingCount++;
        else if (status == "completed") completedCount++;
        else if (status == "revoked") revokedCount++;
    }

    activeCountText_->setText(std::to_string(activeCount));
    pendingCountText_->setText(std::to_string(pendingCount));
    completedCountText_->setText(std::to_string(completedCount));
    revokedCountText_->setText(std::to_string(revokedCount));
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
    LOG_DEBUG("StudentList", "Student selected: " << studentId);
    studentSelected_.emit(studentId);
}

void StudentListWidget::showAddStudentDialog() {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Add New Student"));
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->addStyleClass("admin-dialog");
    dialog->setWidth(Wt::WLength(450));

    auto content = dialog->contents();
    content->addStyleClass("admin-dialog-content");

    // Info message
    auto infoMsg = content->addWidget(std::make_unique<Wt::WText>(
        "Create a student account with minimal information. The student will complete their intake forms after logging in."));
    infoMsg->addStyleClass("admin-dialog-info");

    // First Name
    auto firstNameGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    firstNameGroup->addStyleClass("form-group");
    firstNameGroup->addWidget(std::make_unique<Wt::WText>("First Name *"))->addStyleClass("form-label");
    auto firstNameInput = firstNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    firstNameInput->setPlaceholderText("Enter first name");
    firstNameInput->addStyleClass("form-control");

    // Last Name
    auto lastNameGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    lastNameGroup->addStyleClass("form-group");
    lastNameGroup->addWidget(std::make_unique<Wt::WText>("Last Name *"))->addStyleClass("form-label");
    auto lastNameInput = lastNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    lastNameInput->setPlaceholderText("Enter last name");
    lastNameInput->addStyleClass("form-control");

    // Email
    auto emailGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    emailGroup->addWidget(std::make_unique<Wt::WText>("Email *"))->addStyleClass("form-label");
    auto emailInput = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput->setPlaceholderText("student@email.com");
    emailInput->addStyleClass("form-control");

    // Password
    auto passwordGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordGroup->addStyleClass("form-group");
    passwordGroup->addWidget(std::make_unique<Wt::WText>("Temporary Password *"))->addStyleClass("form-label");
    auto passwordInput = passwordGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    passwordInput->setEchoMode(Wt::EchoMode::Password);
    passwordInput->setPlaceholderText("Enter temporary password");
    passwordInput->addStyleClass("form-control");
    auto passwordHint = passwordGroup->addWidget(std::make_unique<Wt::WText>("Student will change password on first login"));
    passwordHint->addStyleClass("form-text text-muted");

    // Program selection
    auto programGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    programGroup->addStyleClass("form-group");
    programGroup->addWidget(std::make_unique<Wt::WText>("Program *"))->addStyleClass("form-label");
    auto programSelect = programGroup->addWidget(std::make_unique<Wt::WComboBox>());
    programSelect->addStyleClass("form-control");
    programSelect->addItem("Select a program...");

    // Populate programs from curriculumMap_
    for (const auto& [id, name] : curriculumMap_) {
        programSelect->addItem(name);
    }

    // Error message container
    auto errorContainer = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer->addStyleClass("admin-dialog-error");
    errorContainer->hide();
    auto errorText = errorContainer->addWidget(std::make_unique<Wt::WText>(""));
    errorText->addStyleClass("text-danger");

    // Buttons
    auto buttonRow = dialog->footer();
    buttonRow->addStyleClass("admin-dialog-buttons");

    auto cancelBtn = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn->addStyleClass("btn btn-secondary");
    cancelBtn->clicked().connect([dialog]() {
        dialog->reject();
    });

    auto saveBtn = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Create Student"));
    saveBtn->addStyleClass("btn btn-primary");

    // Save handler
    saveBtn->clicked().connect([this, dialog, firstNameInput, lastNameInput, emailInput,
                                passwordInput, programSelect, errorContainer, errorText]() {
        // Validate required fields
        if (firstNameInput->text().empty() || lastNameInput->text().empty() ||
            emailInput->text().empty() || passwordInput->text().empty() ||
            programSelect->currentIndex() == 0) {
            errorText->setText("Please fill in all required fields");
            errorContainer->show();
            return;
        }

        // Validate email format
        std::string email = emailInput->text().toUTF8();
        if (email.find('@') == std::string::npos || email.find('.') == std::string::npos) {
            errorText->setText("Please enter a valid email address");
            errorContainer->show();
            return;
        }

        // Get curriculum ID from selection
        std::string selectedProgram = programSelect->currentText().toUTF8();
        std::string curriculumId = "";
        for (const auto& [id, name] : curriculumMap_) {
            if (name == selectedProgram) {
                curriculumId = id;
                break;
            }
        }

        if (apiService_) {
            try {
                // Create student object
                Models::Student newStudent;
                newStudent.setFirstName(firstNameInput->text().toUTF8());
                newStudent.setLastName(lastNameInput->text().toUTF8());
                newStudent.setEmail(email);
                newStudent.setCurriculumId(curriculumId);

                // Register student via API
                auto result = apiService_->registerStudent(newStudent, passwordInput->text().toUTF8());

                if (result.success) {
                    LOG_INFO("StudentList", "Student created successfully");
                    dialog->accept();
                    refresh();  // Refresh the student list
                } else {
                    std::string errorMsg = result.message.empty() ?
                        "Failed to create student. Email may already be in use." : result.message;
                    errorText->setText(errorMsg);
                    errorContainer->show();
                }
            } catch (const std::exception& e) {
                LOG_ERROR("StudentList", "Error creating student: " << e.what());
                errorText->setText("Error creating student: " + std::string(e.what()));
                errorContainer->show();
            }
        }
    });

    dialog->show();
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
