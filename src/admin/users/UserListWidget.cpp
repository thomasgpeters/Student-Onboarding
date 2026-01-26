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
    , titleText_(nullptr)
    , subtitleText_(nullptr)
    , statsContainer_(nullptr)
    , totalCard_(nullptr)
    , adminCard_(nullptr)
    , instructorCard_(nullptr)
    , studentCard_(nullptr)
    , totalUsersText_(nullptr)
    , adminCountText_(nullptr)
    , instructorCountText_(nullptr)
    , studentCountText_(nullptr)
    , activeRoleFilter_("")
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

void UserListWidget::setCurrentUserRoles(const std::vector<StudentIntake::Models::UserRole>& roles) {
    currentUserRoles_ = roles;
    isCurrentUserAdmin_ = false;
    for (const auto& role : roles) {
        if (role == StudentIntake::Models::UserRole::Admin) {
            isCurrentUserAdmin_ = true;
            break;
        }
    }

    // Update UI based on permissions
    // Instructors can only manage students, so hide admin/instructor options
    if (!isCurrentUserAdmin_) {
        // Update title for instructor view
        if (titleText_) {
            titleText_->setText("Student Management");
        }
        if (subtitleText_) {
            subtitleText_->setText("View and manage your students");
        }

        // Hide admin and instructor stat cards
        if (adminCard_) {
            adminCard_->hide();
        }
        if (instructorCard_) {
            instructorCard_->hide();
        }
        // Hide total card for instructors - they only see students
        if (totalCard_) {
            totalCard_->hide();
        }

        // Update role filter to only show Student option
        if (roleFilter_) {
            roleFilter_->clear();
            roleFilter_->addItem("All Roles");
            roleFilter_->addItem("Student");
        }
    } else {
        // Admin view - ensure title is set correctly
        if (titleText_) {
            titleText_->setText("User Management");
        }
        if (subtitleText_) {
            subtitleText_->setText("View and manage all users");
        }
    }
}

void UserListWidget::refresh() {
    loadUsers();
}

void UserListWidget::setupUI() {
    addStyleClass("admin-student-list");

    // Header with title and Add User button (matching StudentListWidget style)
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("admin-section-header admin-section-header-with-action");

    auto titleContainer = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleContainer->addStyleClass("admin-header-title-container");

    titleText_ = titleContainer->addWidget(std::make_unique<Wt::WText>("User Management"));
    titleText_->addStyleClass("admin-section-title");

    subtitleText_ = titleContainer->addWidget(std::make_unique<Wt::WText>(
        "View and manage all users"));
    subtitleText_->addStyleClass("admin-section-subtitle");

    // Add User button
    addUserBtn_ = header->addWidget(std::make_unique<Wt::WPushButton>("+ Add User"));
    addUserBtn_->addStyleClass("btn btn-primary");
    addUserBtn_->clicked().connect([this] { addUserClicked_.emit(); });

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

void UserListWidget::setupStats() {
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Total users card (clickable to show all)
    totalCard_ = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalCard_->addStyleClass("admin-stat-mini-card active");
    auto totalIcon = totalCard_->addWidget(std::make_unique<Wt::WText>("👥"));
    totalIcon->addStyleClass("admin-stat-mini-icon");
    totalUsersText_ = totalCard_->addWidget(std::make_unique<Wt::WText>("0"));
    totalUsersText_->addStyleClass("admin-stat-mini-number");
    auto totalLabel = totalCard_->addWidget(std::make_unique<Wt::WText>("Total"));
    totalLabel->addStyleClass("admin-stat-mini-label");
    totalCard_->clicked().connect([this] { filterByRole(""); });

    // Admin count card (clickable to filter admins)
    adminCard_ = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    adminCard_->addStyleClass("admin-stat-mini-card rejected");
    auto adminIcon = adminCard_->addWidget(std::make_unique<Wt::WText>("🛡️"));
    adminIcon->addStyleClass("admin-stat-mini-icon");
    adminCountText_ = adminCard_->addWidget(std::make_unique<Wt::WText>("0"));
    adminCountText_->addStyleClass("admin-stat-mini-number");
    auto adminLabel = adminCard_->addWidget(std::make_unique<Wt::WText>("Admins"));
    adminLabel->addStyleClass("admin-stat-mini-label");
    adminCard_->clicked().connect([this] { filterByRole("Admin"); });

    // Instructor count card (clickable to filter instructors)
    instructorCard_ = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    instructorCard_->addStyleClass("admin-stat-mini-card pending");
    auto instructorIcon = instructorCard_->addWidget(std::make_unique<Wt::WText>("📚"));
    instructorIcon->addStyleClass("admin-stat-mini-icon");
    instructorCountText_ = instructorCard_->addWidget(std::make_unique<Wt::WText>("0"));
    instructorCountText_->addStyleClass("admin-stat-mini-number");
    auto instructorLabel = instructorCard_->addWidget(std::make_unique<Wt::WText>("Instructors"));
    instructorLabel->addStyleClass("admin-stat-mini-label");
    instructorCard_->clicked().connect([this] { filterByRole("Instructor"); });

    // Student count card (clickable to filter students)
    studentCard_ = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentCard_->addStyleClass("admin-stat-mini-card approved");
    auto studentIcon = studentCard_->addWidget(std::make_unique<Wt::WText>("🎓"));
    studentIcon->addStyleClass("admin-stat-mini-icon");
    studentCountText_ = studentCard_->addWidget(std::make_unique<Wt::WText>("0"));
    studentCountText_->addStyleClass("admin-stat-mini-number");
    auto studentLabel = studentCard_->addWidget(std::make_unique<Wt::WText>("Students"));
    studentLabel->addStyleClass("admin-stat-mini-label");
    studentCard_->clicked().connect([this] { filterByRole("Student"); });
}

void UserListWidget::setupFilters() {
    auto filterContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer->addStyleClass("admin-filter-container");

    // Search input group
    auto searchGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group admin-filter-search");

    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WText>("Search"));
    searchLabel->addStyleClass("admin-filter-label");

    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search by name or email...");
    searchInput_->addStyleClass("admin-filter-input");
    searchInput_->textInput().connect([this] { applyFilters(); });

    // Role filter group
    auto roleGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    roleGroup->addStyleClass("admin-filter-group");

    auto roleLabel = roleGroup->addWidget(std::make_unique<Wt::WText>("Role"));
    roleLabel->addStyleClass("admin-filter-label");

    roleFilter_ = roleGroup->addWidget(std::make_unique<Wt::WComboBox>());
    roleFilter_->addStyleClass("admin-filter-select");
    roleFilter_->addItem("All Roles");
    roleFilter_->addItem("Admin");
    roleFilter_->addItem("Instructor");
    roleFilter_->addItem("Student");
    roleFilter_->changed().connect([this] {
        activeRoleFilter_ = "";  // Clear placard filter when dropdown changes
        applyFilters();
    });

    // Status filter group
    auto statusGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusGroup->addStyleClass("admin-filter-group");

    auto statusLabel = statusGroup->addWidget(std::make_unique<Wt::WText>("Status"));
    statusLabel->addStyleClass("admin-filter-label");

    statusFilter_ = statusGroup->addWidget(std::make_unique<Wt::WComboBox>());
    statusFilter_->addStyleClass("admin-filter-select");
    statusFilter_->addItem("All Status");
    statusFilter_->addItem("Active");
    statusFilter_->addItem("Inactive");
    statusFilter_->changed().connect([this] { applyFilters(); });

    // Clear button
    auto buttonGroup = filterContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons admin-filter-buttons-right");

    clearButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
    clearButton_->addStyleClass("btn btn-secondary");
    clearButton_->clicked().connect([this] { clearFilters(); });
}

void UserListWidget::setupTable() {
    userTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    userTable_->addStyleClass("admin-data-table");
    userTable_->setHeaderCount(1);

    // Table headers - icon column first (matching StudentListWidget)
    auto iconHeader = userTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>(""));
    iconHeader->addStyleClass("admin-table-icon-header");
    userTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Name"));
    userTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Email"));
    userTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Roles"));
    userTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Status"));
    userTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < 6; i++) {
        userTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }
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
                if (!user.hasRole(StudentIntake::Models::UserRole::Student)) {
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
        if (user.hasRole(StudentIntake::Models::UserRole::Admin)) admins++;
        if (user.hasRole(StudentIntake::Models::UserRole::Instructor)) instructors++;
        if (user.hasRole(StudentIntake::Models::UserRole::Student)) students++;
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

        // Role filter (from dropdown or placard click)
        if (roleIndex > 0 || !activeRoleFilter_.empty()) {
            bool hasRole = false;

            // If placard was clicked, use that role filter
            if (!activeRoleFilter_.empty()) {
                if (activeRoleFilter_ == "Admin") {
                    hasRole = user.hasRole(StudentIntake::Models::UserRole::Admin);
                } else if (activeRoleFilter_ == "Instructor") {
                    hasRole = user.hasRole(StudentIntake::Models::UserRole::Instructor);
                } else if (activeRoleFilter_ == "Student") {
                    hasRole = user.hasRole(StudentIntake::Models::UserRole::Student);
                }
            } else if (roleIndex > 0) {
                // Use dropdown selection
                if (isCurrentUserAdmin_) {
                    // Admin sees all roles
                    switch (roleIndex) {
                        case 1: hasRole = user.hasRole(StudentIntake::Models::UserRole::Admin); break;
                        case 2: hasRole = user.hasRole(StudentIntake::Models::UserRole::Instructor); break;
                        case 3: hasRole = user.hasRole(StudentIntake::Models::UserRole::Student); break;
                    }
                } else {
                    // Instructor only sees students
                    if (roleIndex == 1) {
                        hasRole = user.hasRole(StudentIntake::Models::UserRole::Student);
                    }
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
    resultCount_->setText("Showing " + std::to_string(filtered.size()) + " user(s)");
}

void UserListWidget::clearFilters() {
    searchInput_->setText("");
    roleFilter_->setCurrentIndex(0);
    statusFilter_->setCurrentIndex(0);
    activeRoleFilter_ = "";
    applyFilters();
}

void UserListWidget::filterByRole(const std::string& role) {
    activeRoleFilter_ = role;

    // Update role dropdown to match
    if (role.empty()) {
        roleFilter_->setCurrentIndex(0);  // All Roles
    } else if (role == "Admin") {
        if (isCurrentUserAdmin_) {
            roleFilter_->setCurrentIndex(1);
        }
    } else if (role == "Instructor") {
        if (isCurrentUserAdmin_) {
            roleFilter_->setCurrentIndex(2);
        }
    } else if (role == "Student") {
        if (isCurrentUserAdmin_) {
            roleFilter_->setCurrentIndex(3);
        } else {
            roleFilter_->setCurrentIndex(1);  // For instructors, Student is at index 1
        }
    }

    applyFilters();
}

void UserListWidget::updateTable(const std::vector<StudentIntake::Models::User>& users) {
    // Clear existing rows (except header)
    while (userTable_->rowCount() > 1) {
        userTable_->removeRow(1);
    }

    // Add user rows
    int row = 1;
    for (const auto& user : users) {
        // User icon (matching StudentListWidget style)
        auto iconCell = userTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>("👤"));
        iconCell->addStyleClass("admin-row-icon user-icon");

        // Name
        std::string displayName = user.getFullName();
        if (displayName.empty()) displayName = user.getEmail();
        userTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(displayName));

        // Email
        userTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(user.getEmail()));

        // Roles
        auto rolesText = userTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(getRoleBadges(user)));
        rolesText->setTextFormat(Wt::TextFormat::XHTML);

        // Status
        auto statusBadge = userTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(
            user.isActive() ? "Active" : "Inactive"));
        statusBadge->addStyleClass(getStatusBadgeClass(user.isActive()));

        // Actions
        auto actionsContainer = userTable_->elementAt(row, 5)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto editBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("View"));
        editBtn->addStyleClass("btn btn-sm btn-primary");
        int userId = user.getId();
        editBtn->clicked().connect([this, userId] {
            userSelected_.emit(userId);
        });

        // Style all cells in this row
        for (int col = 0; col < 6; col++) {
            userTable_->elementAt(row, col)->addStyleClass("admin-table-cell");
        }

        row++;
    }
}

std::string UserListWidget::getRoleBadges(const StudentIntake::Models::User& user) const {
    std::string badges;

    if (user.hasRole(StudentIntake::Models::UserRole::Admin)) {
        badges += "<span class='badge badge-danger' style='margin-right: 4px;'>Admin</span>";
    }
    if (user.hasRole(StudentIntake::Models::UserRole::Instructor)) {
        badges += "<span class='badge badge-info' style='margin-right: 4px;'>Instructor</span>";
    }
    if (user.hasRole(StudentIntake::Models::UserRole::Student)) {
        badges += "<span class='badge badge-success' style='margin-right: 4px;'>Student</span>";
    }

    if (badges.empty()) {
        badges = "<span class='badge badge-secondary'>No Role</span>";
    }

    return badges;
}

std::string UserListWidget::getStatusBadgeClass(bool isActive) const {
    return isActive ? "badge badge-success" : "badge badge-secondary";
}

void UserListWidget::onUserRowClicked(int userId) {
    userSelected_.emit(userId);
}

} // namespace Admin
} // namespace StudentIntake
