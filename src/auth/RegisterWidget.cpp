#include "RegisterWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WAnchor.h>
#include <Wt/WLabel.h>
#include <iostream>

namespace StudentIntake {
namespace Auth {

RegisterWidget::RegisterWidget()
    : WContainerWidget()
    , firstNameInput_(nullptr)
    , lastNameInput_(nullptr)
    , emailInput_(nullptr)
    , passwordInput_(nullptr)
    , confirmPasswordInput_(nullptr)
    , termsCheckbox_(nullptr)
    , registerButton_(nullptr)
    , errorText_(nullptr)
    , errorContainer_(nullptr)
    , passwordStrengthContainer_(nullptr)
    , passwordStrengthText_(nullptr) {
    setupUI();
}

RegisterWidget::~RegisterWidget() {
}

void RegisterWidget::setupUI() {
    addStyleClass("register-widget");

    // Title
    auto title = addWidget(std::make_unique<Wt::WText>("<h2>Create Account</h2>"));
    title->addStyleClass("register-title");

    // Description
    auto desc = addWidget(std::make_unique<Wt::WText>(
        "<p>Create a new account to begin your student intake process.</p>"));
    desc->addStyleClass("register-description");

    // Error container (hidden by default)
    errorContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->addStyleClass("alert alert-danger");
    errorContainer_->hide();
    errorText_ = errorContainer_->addWidget(std::make_unique<Wt::WText>());

    // Form container
    auto formContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer->addStyleClass("register-form");

    // Name row
    auto nameRow = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameRow->addStyleClass("form-row");

    // First name
    auto firstNameGroup = nameRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    firstNameGroup->addStyleClass("form-group half-width");
    auto firstNameLabel = firstNameGroup->addWidget(std::make_unique<Wt::WLabel>("First Name"));
    firstNameInput_ = firstNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    firstNameInput_->setPlaceholderText("Enter your first name");
    firstNameInput_->addStyleClass("form-control");
    firstNameLabel->setBuddy(firstNameInput_);

    // Last name
    auto lastNameGroup = nameRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    lastNameGroup->addStyleClass("form-group half-width");
    auto lastNameLabel = lastNameGroup->addWidget(std::make_unique<Wt::WLabel>("Last Name"));
    lastNameInput_ = lastNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    lastNameInput_->setPlaceholderText("Enter your last name");
    lastNameInput_->addStyleClass("form-control");
    lastNameLabel->setBuddy(lastNameInput_);

    // Email field
    auto emailGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    auto emailLabel = emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email Address"));
    emailInput_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput_->setPlaceholderText("Enter your email");
    emailInput_->addStyleClass("form-control");
    emailLabel->setBuddy(emailInput_);

    // Password field
    auto passwordGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordGroup->addStyleClass("form-group");
    auto passwordLabel = passwordGroup->addWidget(std::make_unique<Wt::WLabel>("Password"));
    passwordInput_ = passwordGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    passwordInput_->setEchoMode(Wt::EchoMode::Password);
    passwordInput_->setPlaceholderText("Create a password");
    passwordInput_->addStyleClass("form-control");
    passwordLabel->setBuddy(passwordInput_);

    // Password strength indicator
    passwordStrengthContainer_ = passwordGroup->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordStrengthContainer_->addStyleClass("password-strength");
    passwordStrengthText_ = passwordStrengthContainer_->addWidget(std::make_unique<Wt::WText>());

    // Password requirements
    auto requirementsContainer = passwordGroup->addWidget(std::make_unique<Wt::WContainerWidget>());
    requirementsContainer->addStyleClass("password-requirements");
    requirementsContainer->addWidget(std::make_unique<Wt::WText>(
        "<small>Password must be at least 8 characters with uppercase, lowercase, and a number.</small>"));

    // Password change handler for strength indicator
    passwordInput_->keyWentUp().connect([this]() {
        validateForm();
    });

    // Confirm password field
    auto confirmGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    confirmGroup->addStyleClass("form-group");
    auto confirmLabel = confirmGroup->addWidget(std::make_unique<Wt::WLabel>("Confirm Password"));
    confirmPasswordInput_ = confirmGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    confirmPasswordInput_->setEchoMode(Wt::EchoMode::Password);
    confirmPasswordInput_->setPlaceholderText("Confirm your password");
    confirmPasswordInput_->addStyleClass("form-control");
    confirmLabel->setBuddy(confirmPasswordInput_);

    // Terms checkbox
    auto termsGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    termsGroup->addStyleClass("form-group checkbox-group");
    termsCheckbox_ = termsGroup->addWidget(std::make_unique<Wt::WCheckBox>(
        " I agree to the Terms of Service and Privacy Policy"));
    termsCheckbox_->addStyleClass("form-check-input");

    // Register button
    auto buttonGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("form-group button-group");
    registerButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Create Account"));
    registerButton_->addStyleClass("btn btn-primary btn-block");
    registerButton_->clicked().connect(this, &RegisterWidget::handleRegister);

    // Links container
    auto linksContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    linksContainer->addStyleClass("register-links");

    // Login link
    auto loginContainer = linksContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    loginContainer->addStyleClass("login-prompt");
    loginContainer->addWidget(std::make_unique<Wt::WText>("Already have an account? "));

    auto loginLink = loginContainer->addWidget(std::make_unique<Wt::WAnchor>());
    loginLink->setText("Log in here");
    loginLink->addStyleClass("link-primary");
    loginLink->clicked().connect([this]() {
        loginRequested_.emit();
    });
}

void RegisterWidget::validateForm() {
    std::string password = passwordInput_->text().toUTF8();

    if (password.empty()) {
        passwordStrengthText_->setText("");
        return;
    }

    // Calculate password strength
    int strength = 0;
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        if (std::islower(c)) hasLower = true;
        if (std::isdigit(c)) hasDigit = true;
        if (!std::isalnum(c)) hasSpecial = true;
    }

    if (password.length() >= 8) strength++;
    if (password.length() >= 12) strength++;
    if (hasUpper) strength++;
    if (hasLower) strength++;
    if (hasDigit) strength++;
    if (hasSpecial) strength++;

    std::string strengthClass;
    std::string strengthLabel;

    if (strength <= 2) {
        strengthClass = "strength-weak";
        strengthLabel = "Weak";
    } else if (strength <= 4) {
        strengthClass = "strength-medium";
        strengthLabel = "Medium";
    } else {
        strengthClass = "strength-strong";
        strengthLabel = "Strong";
    }

    passwordStrengthContainer_->setStyleClass("password-strength " + strengthClass);
    passwordStrengthText_->setText("Password strength: " + strengthLabel);
}

void RegisterWidget::handleRegister() {
    std::cout << "[RegisterWidget] handleRegister called" << std::endl;
    std::cout.flush();

    clearError();

    std::string firstName = firstNameInput_->text().toUTF8();
    std::string lastName = lastNameInput_->text().toUTF8();
    std::string email = emailInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();
    std::string confirmPassword = confirmPasswordInput_->text().toUTF8();

    std::cout << "[RegisterWidget] Registration attempt for: " << email << std::endl;
    std::cout.flush();

    // Validate inputs
    if (firstName.empty()) {
        showError("Please enter your first name");
        firstNameInput_->setFocus();
        return;
    }

    if (lastName.empty()) {
        showError("Please enter your last name");
        lastNameInput_->setFocus();
        return;
    }

    if (email.empty()) {
        showError("Please enter your email address");
        emailInput_->setFocus();
        return;
    }

    if (password.empty()) {
        showError("Please enter a password");
        passwordInput_->setFocus();
        return;
    }

    if (password != confirmPassword) {
        showError("Passwords do not match");
        confirmPasswordInput_->setFocus();
        return;
    }

    if (!termsCheckbox_->isChecked()) {
        showError("Please agree to the Terms of Service and Privacy Policy");
        return;
    }

    std::cout << "[RegisterWidget] Validation passed, calling authManager" << std::endl;
    std::cout.flush();

    // Disable register button during registration
    registerButton_->setEnabled(false);
    registerButton_->setText("Creating account...");

    if (authManager_) {
        std::cout << "[RegisterWidget] AuthManager is set, calling registerStudent" << std::endl;
        std::cout.flush();

        AuthResult result = authManager_->registerStudent(email, password, firstName, lastName);

        std::cout << "[RegisterWidget] Registration result - success: " << result.success
                  << ", message: " << result.message << std::endl;
        std::cout.flush();

        if (result.success) {
            // Update session
            if (session_) {
                session_->setLoggedIn(true);
                session_->setAuthToken(result.token);
                session_->setStudent(result.student);
            }

            registrationSuccess_.emit();
        } else {
            if (!result.errors.empty()) {
                showErrors(result.errors);
            } else {
                showError(result.message);
            }
        }
    } else {
        std::cerr << "[RegisterWidget] ERROR: authManager_ is null!" << std::endl;
        std::cerr.flush();
        showError("Registration service unavailable");
    }

    // Re-enable register button
    registerButton_->setEnabled(true);
    registerButton_->setText("Create Account");
}

void RegisterWidget::showError(const std::string& message) {
    errorText_->setText(message);
    errorContainer_->show();
}

void RegisterWidget::showErrors(const std::vector<std::string>& errors) {
    std::string html = "<ul>";
    for (const auto& error : errors) {
        html += "<li>" + error + "</li>";
    }
    html += "</ul>";
    errorText_->setText(html);
    errorContainer_->show();
}

void RegisterWidget::clearError() {
    errorText_->setText("");
    errorContainer_->hide();
}

void RegisterWidget::reset() {
    firstNameInput_->setText("");
    lastNameInput_->setText("");
    emailInput_->setText("");
    passwordInput_->setText("");
    confirmPasswordInput_->setText("");
    termsCheckbox_->setChecked(false);
    passwordStrengthText_->setText("");
    clearError();
}

void RegisterWidget::focus() {
    firstNameInput_->setFocus();
}

} // namespace Auth
} // namespace StudentIntake
