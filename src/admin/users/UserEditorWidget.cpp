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
    auto rolesSection = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    rolesSection->addStyleClass("roles-section");
    rolesSection->addWidget(std::make_unique<Wt::WText>("Roles"))->addStyleClass("section-title");

    auto rolesContainer = rolesSection->addWidget(std::make_unique<Wt::WContainerWidget>());
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

    // In edit mode, password is optional (only set if changing)
    passwordSection_->hide();

    // Load user data
    std::string endpoint = "/AppUser/" + std::to_string(userId);
    auto response = apiClient_->get(endpoint);

    if (response.success) {
        auto json = response.getJson();
        if (json.contains("data")) {
            auto user = Models::User::fromJson(json["data"]);

            // Get roles
            if (authService_) {
                auto roles = authService_->getUserRoles(userId);
                user.setRoles(roles);
            }

            populateForm(user);
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
    titleText_->setText("Create New User");
    passwordSection_->show();
    clearForm();
    clearMessages();
}

void UserEditorWidget::populateForm(const Models::User& user) {
    emailInput_->setText(user.getEmail());
    firstNameInput_->setText(user.getFirstName());
    lastNameInput_->setText(user.getLastName());
    phoneInput_->setText(user.getPhoneNumber());

    adminRoleCheck_->setChecked(user.hasRole(Models::UserRole::Admin));
    instructorRoleCheck_->setChecked(user.hasRole(Models::UserRole::Instructor));
    studentRoleCheck_->setChecked(user.hasRole(Models::UserRole::Student));

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

} // namespace Admin
} // namespace StudentIntake
