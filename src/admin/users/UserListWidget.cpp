#include "UserListWidget.h"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WTableRow.h>
#include <algorithm>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

UserListWidget::UserListWidget()
    : apiClient_(nullptr)
    , authService_(nullptr)
    , isCurrentUserAdmin_(false)
    , statsContainer_(nullptr)
    , totalUsersText_(nullptr)
    , adminCountText_(nullptr)
    , instructorCountText_(nullptr)
    , studentCountText_(nullptr)
    , addUserBtn_(nullptr)
    , searchInput_(nullptr)
    , roleFilter_(nullptr)
    , statusFilter_(nullptr)
    , clearButton_(nullptr)
    , tableContainer_(nullptr)
    , userTable_(nullptr)
    , resultCount_(nullptr)
{
    setupUI();
}

UserListWidget::~UserListWidget() = default;

void UserListWidget::setApiClient(std::shared_ptr<Api::ApiClient> apiClient) {
    apiClient_ = apiClient;
}

void UserListWidget::setAuthService(std::shared_ptr<Auth::AuthService> authService) {
    authService_ = authService;
}

void UserListWidget::setCurrentUserRoles(const std::vector<StudentIntake::StudentIntake::Models::UserRole>& roles) {
    currentUserRoles_ = roles;
    isCurrentUserAdmin_ = false;
    for (const auto& role : roles) {
        if (role == StudentIntake::StudentIntake::Models::UserRole::Admin) {
            isCurrentUserAdmin_ = true;
            break;
        }
    }

    // Update UI based on permissions
    // Instructors can only manage students, so hide admin/instructor options
    if (!isCurrentUserAdmin_) {
        // Hide admin and instructor stats
        if (adminCountText_ && adminCountText_->parent()) {
            static_cast<Wt::WWidget*>(adminCountText_->parent())->hide();
        }
        if (instructorCountText_ && instructorCountText_->parent()) {
            static_cast<Wt::WWidget*>(instructorCountText_->parent())->hide();
        }

        // Update role filter to only show Student option
        if (roleFilter_) {
            roleFilter_->clear();
            roleFilter_->addItem("All Roles");
            roleFilter_->addItem("Student");
        }
    }
}

void UserListWidget::refresh() {
    loadUsers();
}

void UserListWidget::setupUI() {
    addStyleClass("user-list-widget");

    // Header with title and add button
    auto headerContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->addStyleClass("widget-header");

    auto titleText = headerContainer->addWidget(std::make_unique<Wt::WText>("User Management"));
    titleText->addStyleClass("widget-title");

    addUserBtn_ = headerContainer->addWidget(std::make_unique<Wt::WPushButton>("Add User"));
    addUserBtn_->addStyleClass("btn btn-primary");
    addUserBtn_->clicked().connect([this] { addUserClicked_.emit(); });

    // Stats section
    setupStats();

    // Filters section
    setupFilters();

    // Table section
    setupTable();
}

void UserListWidget::setupStats() {
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("stats-container");

    // Total users
    auto totalCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalCard->addStyleClass("stat-card stat-total");
    totalUsersText_ = totalCard->addWidget(std::make_unique<Wt::WText>("0"));
    totalUsersText_->addStyleClass("stat-value");
    totalCard->addWidget(std::make_unique<Wt::WText>("Total Users"))->addStyleClass("stat-label");

    // Admin count
    auto adminCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    adminCard->addStyleClass("stat-card stat-admin");
    adminCountText_ = adminCard->addWidget(std::make_unique<Wt::WText>("0"));
    adminCountText_->addStyleClass("stat-value");
    adminCard->addWidget(std::make_unique<Wt::WText>("Administrators"))->addStyleClass("stat-label");

    // Instructor count
    auto instructorCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    instructorCard->addStyleClass("stat-card stat-instructor");
    instructorCountText_ = instructorCard->addWidget(std::make_unique<Wt::WText>("0"));
    instructorCountText_->addStyleClass("stat-value");
    instructorCard->addWidget(std::make_unique<Wt::WText>("Instructors"))->addStyleClass("stat-label");

    // Student count
    auto studentCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentCard->addStyleClass("stat-card stat-student");
    studentCountText_ = studentCard->addWidget(std::make_unique<Wt::WText>("0"));
    studentCountText_->addStyleClass("stat-value");
    studentCard->addWidget(std::make_unique<Wt::WText>("Students"))->addStyleClass("stat-label");
}

void UserListWidget::setupFilters() {
    auto filterContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer->addStyleClass("filter-container");

    // Search input
    searchInput_ = filterContainer->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search by name or email...");
    searchInput_->addStyleClass("form-control search-input");
    searchInput_->keyWentUp().connect([this] { applyFilters(); });

    // Role filter
    roleFilter_ = filterContainer->addWidget(std::make_unique<Wt::WComboBox>());
    roleFilter_->addStyleClass("form-select role-filter");
    roleFilter_->addItem("All Roles");
    roleFilter_->addItem("Admin");
    roleFilter_->addItem("Instructor");
    roleFilter_->addItem("Student");
    roleFilter_->changed().connect([this] { applyFilters(); });

    // Status filter
    statusFilter_ = filterContainer->addWidget(std::make_unique<Wt::WComboBox>());
    statusFilter_->addStyleClass("form-select status-filter");
    statusFilter_->addItem("All Status");
    statusFilter_->addItem("Active");
    statusFilter_->addItem("Inactive");
    statusFilter_->changed().connect([this] { applyFilters(); });

    // Clear button
    clearButton_ = filterContainer->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
    clearButton_->addStyleClass("btn btn-secondary");
    clearButton_->clicked().connect([this] { clearFilters(); });

    // Result count
    resultCount_ = filterContainer->addWidget(std::make_unique<Wt::WText>(""));
    resultCount_->addStyleClass("result-count");
}

void UserListWidget::setupTable() {
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("table-container");

    userTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    userTable_->addStyleClass("table table-striped table-hover");
    userTable_->setHeaderCount(1);

    // Header row
    userTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Name"));
    userTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Email"));
    userTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Roles"));
    userTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Status"));
    userTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));
}

void UserListWidget::loadUsers() {
    if (!apiClient_) {
        LOG_WARN("UserListWidget", "API client not set");
        return;
    }

    allUsers_.clear();

    // Try to load from AppUser table first
    auto response = apiClient_->get("/AppUser");
    auto json = response.getJson();

    if (response.success && json.contains("data") && json["data"].is_array()) {
        for (const auto& item : json["data"]) {
            auto user = StudentIntake::Models::User::fromJson(item);

            // Get roles for this user
            if (authService_) {
                auto roles = authService_->getUserRoles(user.getId());
                if (!roles.empty()) {
                    user.setRoles(roles);
                }
            }

            // Filter based on current user permissions
            // Instructors can only see students
            if (!isCurrentUserAdmin_) {
                // Only include users who have the Student role
                if (!user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student)) {
                    continue;
                }
            }

            allUsers_.push_back(user);
        }
    }

    updateStats();
    applyFilters();
}

void UserListWidget::updateStats() {
    int total = allUsers_.size();
    int admins = 0;
    int instructors = 0;
    int students = 0;

    for (const auto& user : allUsers_) {
        if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Admin)) admins++;
        if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Instructor)) instructors++;
        if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student)) students++;
    }

    totalUsersText_->setText(std::to_string(total));
    adminCountText_->setText(std::to_string(admins));
    instructorCountText_->setText(std::to_string(instructors));
    studentCountText_->setText(std::to_string(students));
}

void UserListWidget::applyFilters() {
    std::vector<StudentIntake::Models::User> filtered;

    std::string searchTerm = searchInput_->text().toUTF8();
    std::transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), ::tolower);

    int roleIndex = roleFilter_->currentIndex();
    int statusIndex = statusFilter_->currentIndex();

    for (const auto& user : allUsers_) {
        // Search filter
        if (!searchTerm.empty()) {
            std::string name = user.getFullName();
            std::string email = user.getEmail();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::transform(email.begin(), email.end(), email.begin(), ::tolower);

            if (name.find(searchTerm) == std::string::npos &&
                email.find(searchTerm) == std::string::npos) {
                continue;
            }
        }

        // Role filter
        if (roleIndex > 0) {
            bool hasRole = false;
            if (isCurrentUserAdmin_) {
                // Admin sees all roles
                switch (roleIndex) {
                    case 1: hasRole = user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Admin); break;
                    case 2: hasRole = user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Instructor); break;
                    case 3: hasRole = user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student); break;
                }
            } else {
                // Instructor only sees students
                if (roleIndex == 1) {
                    hasRole = user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student);
                }
            }
            if (!hasRole) continue;
        }

        // Status filter
        if (statusIndex > 0) {
            bool isActive = user.isActive();
            if (statusIndex == 1 && !isActive) continue;
            if (statusIndex == 2 && isActive) continue;
        }

        filtered.push_back(user);
    }

    updateTable(filtered);
    resultCount_->setText("Showing " + std::to_string(filtered.size()) + " of " +
                          std::to_string(allUsers_.size()) + " users");
}

void UserListWidget::clearFilters() {
    searchInput_->setText("");
    roleFilter_->setCurrentIndex(0);
    statusFilter_->setCurrentIndex(0);
    applyFilters();
}

void UserListWidget::updateTable(const std::vector<StudentIntake::Models::User>& users) {
    // Clear existing rows (except header)
    while (userTable_->rowCount() > 1) {
        userTable_->deleteRow(1);
    }

    // Add user rows
    for (const auto& user : users) {
        int row = userTable_->rowCount();
        auto tableRow = userTable_->insertRow(row);
        tableRow->addStyleClass("user-row");

        // Name
        std::string displayName = user.getFullName();
        if (displayName.empty()) displayName = user.getEmail();
        tableRow->elementAt(0)->addWidget(std::make_unique<Wt::WText>(displayName));

        // Email
        tableRow->elementAt(1)->addWidget(std::make_unique<Wt::WText>(user.getEmail()));

        // Roles
        auto rolesCell = tableRow->elementAt(2);
        rolesCell->addWidget(std::make_unique<Wt::WText>(getRoleBadges(user)));

        // Status
        auto statusCell = tableRow->elementAt(3);
        auto statusBadge = statusCell->addWidget(std::make_unique<Wt::WText>(
            user.isActive() ? "Active" : "Inactive"));
        statusBadge->addStyleClass(getStatusBadgeClass(user.isActive()));

        // Actions
        auto actionsCell = tableRow->elementAt(4);
        auto editBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
        editBtn->addStyleClass("btn btn-sm btn-outline-primary");
        int userId = user.getId();
        editBtn->clicked().connect([this, userId] {
            userSelected_.emit(userId);
        });
    }
}

std::string UserListWidget::getRoleBadges(const StudentIntake::Models::User& user) const {
    std::string badges;

    if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Admin)) {
        badges += "<span class='badge bg-danger me-1'>Admin</span>";
    }
    if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Instructor)) {
        badges += "<span class='badge bg-primary me-1'>Instructor</span>";
    }
    if (user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student)) {
        badges += "<span class='badge bg-success me-1'>Student</span>";
    }

    if (badges.empty()) {
        badges = "<span class='badge bg-secondary'>No Role</span>";
    }

    return badges;
}

std::string UserListWidget::getStatusBadgeClass(bool isActive) const {
    return isActive ? "badge bg-success" : "badge bg-secondary";
}

void UserListWidget::onUserRowClicked(int userId) {
    userSelected_.emit(userId);
}

} // namespace Admin
} // namespace StudentIntake
