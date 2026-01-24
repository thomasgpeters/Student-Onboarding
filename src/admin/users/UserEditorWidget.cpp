#include "UserEditorWidget.h"
#include <Wt/WVBoxLayout.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include "utils/Logger.h"
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

UserEditorWidget::UserEditorWidget()
    : apiClient_(nullptr)
    , authService_(nullptr)
    , isCurrentUserAdmin_(false)
    , isEditMode_(false)
    , editingUserId_(0)
    , emailInput_(nullptr)
    , firstNameInput_(nullptr)
    , lastNameInput_(nullptr)
    , phoneInput_(nullptr)
    , passwordInput_(nullptr)
    , confirmPasswordInput_(nullptr)
    , adminRoleCheck_(nullptr)
    , instructorRoleCheck_(nullptr)
    , studentRoleCheck_(nullptr)
    , activeCheck_(nullptr)
    , loginEnabledCheck_(nullptr)
    , titleText_(nullptr)
    , messageText_(nullptr)
    , messageContainer_(nullptr)
    , saveBtn_(nullptr)
    , cancelBtn_(nullptr)
    , passwordSection_(nullptr)
    , rolesSection_(nullptr)
{
    setupUI();
}

UserEditorWidget::~UserEditorWidget() = default;

void UserEditorWidget::setApiClient(std::shared_ptr<Api::ApiClient> apiClient) {
    apiClient_ = apiClient;
}

void UserEditorWidget::setAuthService(std::shared_ptr<Auth::AuthService> authService) {
    authService_ = authService;
}

void UserEditorWidget::setCurrentUserRoles(const std::vector<StudentIntake::StudentIntake::Models::UserRole>& roles) {
    currentUserRoles_ = roles;
    isCurrentUserAdmin_ = false;
    for (const auto& role : roles) {
        if (role == StudentIntake::StudentIntake::Models::UserRole::Admin) {
            isCurrentUserAdmin_ = true;
            break;
        }
    }
    updateRoleVisibility();
}

void UserEditorWidget::updateRoleVisibility() {
    // Instructors can only create students, so hide Admin and Instructor options
    if (!isCurrentUserAdmin_) {
        if (adminRoleCheck_) {
            adminRoleCheck_->hide();
            adminRoleCheck_->setChecked(false);
        }
        if (instructorRoleCheck_) {
            instructorRoleCheck_->hide();
            instructorRoleCheck_->setChecked(false);
        }
        // Force student role for instructor-created users
        if (studentRoleCheck_) {
            studentRoleCheck_->setChecked(true);
            studentRoleCheck_->setEnabled(false);  // Can't uncheck
        }
    } else {
        // Admin can manage all roles
        if (adminRoleCheck_) {
            adminRoleCheck_->show();
        }
        if (instructorRoleCheck_) {
            instructorRoleCheck_->show();
        }
        if (studentRoleCheck_) {
            studentRoleCheck_->setEnabled(true);
        }
    }
}

void UserEditorWidget::setupUI() {
    addStyleClass("user-editor-widget");

    // Header
    auto headerContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->addStyleClass("widget-header");

    titleText_ = headerContainer->addWidget(std::make_unique<Wt::WText>("Create New User"));
    titleText_->addStyleClass("widget-title");

    // Message container
    messageContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    messageContainer_->addStyleClass("message-container");
    messageContainer_->hide();
    messageText_ = messageContainer_->addWidget(std::make_unique<Wt::WText>());

    // Form container
    auto formContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer->addStyleClass("form-container");

    // Email
    auto emailGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    auto emailLabel = emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email *"));
    emailInput_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput_->setPlaceholderText("user@example.com");
    emailInput_->addStyleClass("form-control");
    emailLabel->setBuddy(emailInput_);

    // First Name
    auto firstNameGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    firstNameGroup->addStyleClass("form-group");
    auto firstNameLabel = firstNameGroup->addWidget(std::make_unique<Wt::WLabel>("First Name *"));
    firstNameInput_ = firstNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    firstNameInput_->addStyleClass("form-control");
    firstNameLabel->setBuddy(firstNameInput_);

    // Last Name
    auto lastNameGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    lastNameGroup->addStyleClass("form-group");
    auto lastNameLabel = lastNameGroup->addWidget(std::make_unique<Wt::WLabel>("Last Name *"));
    lastNameInput_ = lastNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    lastNameInput_->addStyleClass("form-control");
    lastNameLabel->setBuddy(lastNameInput_);

    // Phone
    auto phoneGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneGroup->addStyleClass("form-group");
    auto phoneLabel = phoneGroup->addWidget(std::make_unique<Wt::WLabel>("Phone"));
    phoneInput_ = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    phoneInput_->addStyleClass("form-control");
    phoneLabel->setBuddy(phoneInput_);

    // Password section
    passwordSection_ = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordSection_->addStyleClass("password-section");

    // Password
    auto passwordGroup = passwordSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordGroup->addStyleClass("form-group");
    auto passwordLabel = passwordGroup->addWidget(std::make_unique<Wt::WLabel>("Password *"));
    passwordInput_ = passwordGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    passwordInput_->setEchoMode(Wt::EchoMode::Password);
    passwordInput_->addStyleClass("form-control");
    passwordLabel->setBuddy(passwordInput_);

    // Confirm Password
    auto confirmPasswordGroup = passwordSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    confirmPasswordGroup->addStyleClass("form-group");
    auto confirmPasswordLabel = confirmPasswordGroup->addWidget(std::make_unique<Wt::WLabel>("Confirm Password *"));
    confirmPasswordInput_ = confirmPasswordGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    confirmPasswordInput_->setEchoMode(Wt::EchoMode::Password);
    confirmPasswordInput_->addStyleClass("form-control");
    confirmPasswordLabel->setBuddy(confirmPasswordInput_);

    // Roles section
    rolesSection_ = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    rolesSection_->addStyleClass("roles-section");
    rolesSection_->addWidget(std::make_unique<Wt::WText>("Roles"))->addStyleClass("section-title");

    auto rolesContainer = rolesSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    rolesContainer->addStyleClass("roles-container");

    adminRoleCheck_ = rolesContainer->addWidget(std::make_unique<Wt::WCheckBox>("Administrator"));
    adminRoleCheck_->addStyleClass("form-check");

    instructorRoleCheck_ = rolesContainer->addWidget(std::make_unique<Wt::WCheckBox>("Instructor"));
    instructorRoleCheck_->addStyleClass("form-check");

    studentRoleCheck_ = rolesContainer->addWidget(std::make_unique<Wt::WCheckBox>("Student"));
    studentRoleCheck_->addStyleClass("form-check");

    // Status section
    auto statusSection = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusSection->addStyleClass("status-section");
    statusSection->addWidget(std::make_unique<Wt::WText>("Status"))->addStyleClass("section-title");

    auto statusContainer = statusSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusContainer->addStyleClass("status-container");

    activeCheck_ = statusContainer->addWidget(std::make_unique<Wt::WCheckBox>("Account Active"));
    activeCheck_->setChecked(true);
    activeCheck_->addStyleClass("form-check");

    loginEnabledCheck_ = statusContainer->addWidget(std::make_unique<Wt::WCheckBox>("Login Enabled"));
    loginEnabledCheck_->setChecked(true);
    loginEnabledCheck_->addStyleClass("form-check");

    // Buttons
    auto buttonContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("button-container");

    saveBtn_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveBtn_->addStyleClass("btn btn-primary");
    saveBtn_->clicked().connect(this, &UserEditorWidget::handleSave);

    cancelBtn_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn_->addStyleClass("btn btn-secondary");
    cancelBtn_->clicked().connect(this, &UserEditorWidget::handleCancel);
}

void UserEditorWidget::loadUser(int userId) {
    if (!apiClient_) {
        showError("API client not available");
        return;
    }

    isEditMode_ = true;
    editingUserId_ = userId;
    titleText_->setText("Edit User");
    existingUserRoles_.clear();

    // In edit mode, password is optional (only set if changing)
    passwordSection_->hide();

    // Load user data
    std::string endpoint = "/AppUser/" + std::to_string(userId);
    auto response = apiClient_->get(endpoint);

    if (response.success) {
        auto json = response.getJson();
        if (json.contains("data")) {
            auto user = StudentIntake::Models::User::fromJson(json["data"]);

            // Get roles
            if (authService_) {
                auto roles = authService_->getUserRoles(userId);
                user.setRoles(roles);
                existingUserRoles_ = roles;  // Store for comparison when saving
            }

            populateForm(user);
            updateRoleVisibility();
        } else {
            showError("Failed to load user data");
        }
    } else {
        showError("Failed to load user: " + response.errorMessage);
    }
}

void UserEditorWidget::newUser() {
    isEditMode_ = false;
    editingUserId_ = 0;
    existingUserRoles_.clear();
    titleText_->setText("Create New User");
    passwordSection_->show();
    clearForm();
    clearMessages();
    updateRoleVisibility();
}

void UserEditorWidget::populateForm(const StudentIntake::Models::User& user) {
    emailInput_->setText(user.getEmail());
    firstNameInput_->setText(user.getFirstName());
    lastNameInput_->setText(user.getLastName());
    phoneInput_->setText(user.getPhoneNumber());

    adminRoleCheck_->setChecked(user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Admin));
    instructorRoleCheck_->setChecked(user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Instructor));
    studentRoleCheck_->setChecked(user.hasRole(StudentIntake::StudentIntake::Models::UserRole::Student));

    activeCheck_->setChecked(user.isActive());
    loginEnabledCheck_->setChecked(user.isLoginEnabled());
}

void UserEditorWidget::clearForm() {
    emailInput_->setText("");
    firstNameInput_->setText("");
    lastNameInput_->setText("");
    phoneInput_->setText("");
    passwordInput_->setText("");
    confirmPasswordInput_->setText("");

    adminRoleCheck_->setChecked(false);
    instructorRoleCheck_->setChecked(false);
    studentRoleCheck_->setChecked(true);  // Default to student role

    activeCheck_->setChecked(true);
    loginEnabledCheck_->setChecked(true);
}

bool UserEditorWidget::validateForm() {
    std::string email = emailInput_->text().toUTF8();
    std::string firstName = firstNameInput_->text().toUTF8();
    std::string lastName = lastNameInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();
    std::string confirmPassword = confirmPasswordInput_->text().toUTF8();

    if (email.empty()) {
        showError("Email is required");
        return false;
    }

    if (firstName.empty()) {
        showError("First name is required");
        return false;
    }

    if (lastName.empty()) {
        showError("Last name is required");
        return false;
    }

    // Password validation only for new users
    if (!isEditMode_) {
        if (password.empty()) {
            showError("Password is required");
            return false;
        }

        if (password.length() < 8) {
            showError("Password must be at least 8 characters");
            return false;
        }

        if (password != confirmPassword) {
            showError("Passwords do not match");
            return false;
        }
    }

    // At least one role must be selected
    if (!adminRoleCheck_->isChecked() &&
        !instructorRoleCheck_->isChecked() &&
        !studentRoleCheck_->isChecked()) {
        showError("At least one role must be selected");
        return false;
    }

    return true;
}

void UserEditorWidget::handleSave() {
    clearMessages();

    if (!validateForm()) {
        return;
    }

    if (!apiClient_) {
        showError("API client not available");
        return;
    }

    // Build user data
    nlohmann::json userData;
    userData["email"] = emailInput_->text().toUTF8();
    userData["first_name"] = firstNameInput_->text().toUTF8();
    userData["last_name"] = lastNameInput_->text().toUTF8();
    userData["phone_number"] = phoneInput_->text().toUTF8();
    userData["is_active"] = activeCheck_->isChecked();
    userData["login_enabled"] = loginEnabledCheck_->isChecked();

    // For new users, include password hash (in production, this would be hashed server-side)
    if (!isEditMode_) {
        // Note: In a real application, password hashing should be done server-side
        userData["password_hash"] = passwordInput_->text().toUTF8();
    }

    // Build payload
    nlohmann::json payload;
    payload["data"]["type"] = "AppUser";
    payload["data"]["attributes"] = userData;

    Api::ApiResponse response;

    if (isEditMode_) {
        payload["data"]["id"] = std::to_string(editingUserId_);
        response = apiClient_->patch("/AppUser/" + std::to_string(editingUserId_), payload);
    } else {
        response = apiClient_->post("/AppUser", payload);
    }

    if (!response.success) {
        showError("Failed to save user: " + response.errorMessage);
        return;
    }

    // Get the user ID (for new users, it's in the response)
    int userId = editingUserId_;
    if (!isEditMode_) {
        auto json = response.getJson();
        if (json.contains("data") && json["data"].contains("id")) {
            if (json["data"]["id"].is_number()) {
                userId = json["data"]["id"].get<int>();
            } else if (json["data"]["id"].is_string()) {
                userId = std::stoi(json["data"]["id"].get<std::string>());
            }
        }
    }

    // Update roles
    if (userId > 0) {
        // First, delete existing roles
        std::string deleteEndpoint = "/UserRole?filter[user_id]=" + std::to_string(userId);
        auto existingRoles = apiClient_->get(deleteEndpoint);
        if (existingRoles.success) {
            auto rolesJson = existingRoles.getJson();
            if (rolesJson.contains("data") && rolesJson["data"].is_array()) {
                for (const auto& role : rolesJson["data"]) {
                    if (role.contains("id")) {
                        std::string roleId = role["id"].is_string() ?
                            role["id"].get<std::string>() :
                            std::to_string(role["id"].get<int>());
                        apiClient_->del("/UserRole/" + roleId);
                    }
                }
            }
        }

        // Add new roles
        auto addRole = [this, userId](const std::string& role) {
            nlohmann::json rolePayload;
            rolePayload["data"]["type"] = "UserRole";
            rolePayload["data"]["attributes"]["user_id"] = userId;
            rolePayload["data"]["attributes"]["role"] = role;
            rolePayload["data"]["attributes"]["is_active"] = true;
            apiClient_->post("/UserRole", rolePayload);
        };

        if (adminRoleCheck_->isChecked()) addRole("admin");
        if (instructorRoleCheck_->isChecked()) addRole("instructor");
        if (studentRoleCheck_->isChecked()) addRole("student");

        // Build the list of new roles
        std::vector<StudentIntake::StudentIntake::Models::UserRole> newRoles;
        if (adminRoleCheck_->isChecked()) newRoles.push_back(StudentIntake::StudentIntake::Models::UserRole::Admin);
        if (instructorRoleCheck_->isChecked()) newRoles.push_back(StudentIntake::StudentIntake::Models::UserRole::Instructor);
        if (studentRoleCheck_->isChecked()) newRoles.push_back(StudentIntake::StudentIntake::Models::UserRole::Student);

        // Create/delete role-specific records based on role changes
        if (!createRoleSpecificRecords(userId, newRoles, existingUserRoles_)) {
            showError("User saved but failed to create some role-specific records");
            return;
        }
    }

    showSuccess(isEditMode_ ? "User updated successfully" : "User created successfully");
    saved_.emit();
}

void UserEditorWidget::handleCancel() {
    clearMessages();
    cancelled_.emit();
}

void UserEditorWidget::showError(const std::string& message) {
    messageContainer_->removeStyleClass("alert-success");
    messageContainer_->addStyleClass("alert alert-danger");
    messageText_->setText(message);
    messageContainer_->show();
}

void UserEditorWidget::showSuccess(const std::string& message) {
    messageContainer_->removeStyleClass("alert-danger");
    messageContainer_->addStyleClass("alert alert-success");
    messageText_->setText(message);
    messageContainer_->show();
}

void UserEditorWidget::clearMessages() {
    messageContainer_->hide();
    messageText_->setText("");
}

bool UserEditorWidget::createRoleSpecificRecords(int userId,
                                                  const std::vector<StudentIntake::StudentIntake::Models::UserRole>& newRoles,
                                                  const std::vector<StudentIntake::StudentIntake::Models::UserRole>& existingRoles) {
    bool success = true;

    // Helper to check if a role exists in a vector
    auto hasRole = [](const std::vector<StudentIntake::StudentIntake::Models::UserRole>& roles, StudentIntake::StudentIntake::Models::UserRole role) {
        return std::find(roles.begin(), roles.end(), role) != roles.end();
    };

    // Check each role type
    bool hadAdmin = hasRole(existingRoles, StudentIntake::StudentIntake::Models::UserRole::Admin);
    bool hasAdmin = hasRole(newRoles, StudentIntake::StudentIntake::Models::UserRole::Admin);
    bool hadInstructor = hasRole(existingRoles, StudentIntake::StudentIntake::Models::UserRole::Instructor);
    bool hasInstructor = hasRole(newRoles, StudentIntake::StudentIntake::Models::UserRole::Instructor);
    bool hadStudent = hasRole(existingRoles, StudentIntake::StudentIntake::Models::UserRole::Student);
    bool hasStudent = hasRole(newRoles, StudentIntake::StudentIntake::Models::UserRole::Student);

    // Handle Admin role changes
    if (hasAdmin && !hadAdmin) {
        // Added Admin role - create AdminUser record
        if (!createAdminUser(userId)) {
            LOG_ERROR("UserEditorWidget", "Failed to create AdminUser for user " << userId);
            success = false;
        }
    } else if (!hasAdmin && hadAdmin) {
        // Removed Admin role - delete AdminUser record
        if (!deleteAdminUser(userId)) {
            LOG_WARN("UserEditorWidget", "Failed to delete AdminUser for user " << userId);
        }
    }

    // Handle Instructor role changes
    if (hasInstructor && !hadInstructor) {
        // Added Instructor role - create Instructor record
        if (!createInstructor(userId)) {
            LOG_ERROR("UserEditorWidget", "Failed to create Instructor for user " << userId);
            success = false;
        }
    } else if (!hasInstructor && hadInstructor) {
        // Removed Instructor role - delete Instructor record
        if (!deleteInstructor(userId)) {
            LOG_WARN("UserEditorWidget", "Failed to delete Instructor for user " << userId);
        }
    }

    // Handle Student role changes
    if (hasStudent && !hadStudent) {
        // Added Student role - create Student record
        if (!createStudent(userId)) {
            LOG_ERROR("UserEditorWidget", "Failed to create Student for user " << userId);
            success = false;
        }
    } else if (!hasStudent && hadStudent) {
        // Removed Student role - delete Student record
        if (!deleteStudent(userId)) {
            LOG_WARN("UserEditorWidget", "Failed to delete Student for user " << userId);
        }
    }

    return success;
}

bool UserEditorWidget::createAdminUser(int userId) {
    if (!apiClient_) return false;

    // First check if AdminUser already exists
    std::string checkEndpoint = "/AdminUser?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array() && !json["data"].empty()) {
            LOG_DEBUG("UserEditorWidget", "AdminUser already exists for user " << userId);
            return true;  // Already exists
        }
    }

    // Create AdminUser record
    nlohmann::json payload;
    payload["data"]["type"] = "AdminUser";
    payload["data"]["attributes"]["user_id"] = userId;
    payload["data"]["attributes"]["is_active"] = true;

    auto response = apiClient_->post("/AdminUser", payload);
    if (response.success) {
        LOG_INFO("UserEditorWidget", "Created AdminUser for user " << userId);
        return true;
    } else {
        LOG_ERROR("UserEditorWidget", "Failed to create AdminUser: " << response.errorMessage);
        return false;
    }
}

bool UserEditorWidget::createInstructor(int userId) {
    if (!apiClient_) return false;

    // First check if Instructor already exists
    std::string checkEndpoint = "/Instructor?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array() && !json["data"].empty()) {
            LOG_DEBUG("UserEditorWidget", "Instructor already exists for user " << userId);
            return true;  // Already exists
        }
    }

    // Get user details for instructor record
    std::string userEndpoint = "/AppUser/" + std::to_string(userId);
    auto userResponse = apiClient_->get(userEndpoint);
    std::string firstName, lastName, email;
    if (userResponse.success) {
        auto json = userResponse.getJson();
        if (json.contains("data") && json["data"].contains("attributes")) {
            auto& attrs = json["data"]["attributes"];
            firstName = attrs.value("first_name", "");
            lastName = attrs.value("last_name", "");
            email = attrs.value("email", "");
        }
    }

    // Create Instructor record
    nlohmann::json payload;
    payload["data"]["type"] = "Instructor";
    payload["data"]["attributes"]["user_id"] = userId;
    payload["data"]["attributes"]["first_name"] = firstName;
    payload["data"]["attributes"]["last_name"] = lastName;
    payload["data"]["attributes"]["email"] = email;
    payload["data"]["attributes"]["is_active"] = true;

    auto response = apiClient_->post("/Instructor", payload);
    if (response.success) {
        LOG_INFO("UserEditorWidget", "Created Instructor for user " << userId);
        return true;
    } else {
        LOG_ERROR("UserEditorWidget", "Failed to create Instructor: " << response.errorMessage);
        return false;
    }
}

bool UserEditorWidget::createStudent(int userId) {
    if (!apiClient_) return false;

    // First check if Student already exists
    std::string checkEndpoint = "/Student?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array() && !json["data"].empty()) {
            LOG_DEBUG("UserEditorWidget", "Student already exists for user " << userId);
            return true;  // Already exists
        }
    }

    // Get user details for student record
    std::string userEndpoint = "/AppUser/" + std::to_string(userId);
    auto userResponse = apiClient_->get(userEndpoint);
    std::string firstName, lastName, email;
    if (userResponse.success) {
        auto json = userResponse.getJson();
        if (json.contains("data") && json["data"].contains("attributes")) {
            auto& attrs = json["data"]["attributes"];
            firstName = attrs.value("first_name", "");
            lastName = attrs.value("last_name", "");
            email = attrs.value("email", "");
        }
    }

    // Create Student record
    nlohmann::json payload;
    payload["data"]["type"] = "Student";
    payload["data"]["attributes"]["user_id"] = userId;
    payload["data"]["attributes"]["first_name"] = firstName;
    payload["data"]["attributes"]["last_name"] = lastName;
    payload["data"]["attributes"]["email"] = email;
    payload["data"]["attributes"]["is_active"] = true;
    payload["data"]["attributes"]["enrollment_status"] = "enrolled";

    auto response = apiClient_->post("/Student", payload);
    if (response.success) {
        LOG_INFO("UserEditorWidget", "Created Student for user " << userId);
        return true;
    } else {
        LOG_ERROR("UserEditorWidget", "Failed to create Student: " << response.errorMessage);
        return false;
    }
}

bool UserEditorWidget::deleteAdminUser(int userId) {
    if (!apiClient_) return false;

    // Find the AdminUser record
    std::string checkEndpoint = "/AdminUser?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                if (item.contains("id")) {
                    std::string recordId = item["id"].is_string() ?
                        item["id"].get<std::string>() :
                        std::to_string(item["id"].get<int>());
                    auto delResponse = apiClient_->del("/AdminUser/" + recordId);
                    if (delResponse.success) {
                        LOG_INFO("UserEditorWidget", "Deleted AdminUser " << recordId << " for user " << userId);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool UserEditorWidget::deleteInstructor(int userId) {
    if (!apiClient_) return false;

    // Find the Instructor record
    std::string checkEndpoint = "/Instructor?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                if (item.contains("id")) {
                    std::string recordId = item["id"].is_string() ?
                        item["id"].get<std::string>() :
                        std::to_string(item["id"].get<int>());
                    auto delResponse = apiClient_->del("/Instructor/" + recordId);
                    if (delResponse.success) {
                        LOG_INFO("UserEditorWidget", "Deleted Instructor " << recordId << " for user " << userId);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool UserEditorWidget::deleteStudent(int userId) {
    if (!apiClient_) return false;

    // Find the Student record
    std::string checkEndpoint = "/Student?filter[user_id]=" + std::to_string(userId);
    auto checkResponse = apiClient_->get(checkEndpoint);
    if (checkResponse.success) {
        auto json = checkResponse.getJson();
        if (json.contains("data") && json["data"].is_array()) {
            for (const auto& item : json["data"]) {
                if (item.contains("id")) {
                    std::string recordId = item["id"].is_string() ?
                        item["id"].get<std::string>() :
                        std::to_string(item["id"].get<int>());
                    auto delResponse = apiClient_->del("/Student/" + recordId);
                    if (delResponse.success) {
                        LOG_INFO("UserEditorWidget", "Deleted Student " << recordId << " for user " << userId);
                    }
                }
            }
            return true;
        }
    }
    return false;
}

} // namespace Admin
} // namespace StudentIntake
