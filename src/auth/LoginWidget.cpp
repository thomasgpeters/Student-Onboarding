#include "LoginWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WAnchor.h>
#include <Wt/WLabel.h>

namespace StudentIntake {
namespace Auth {

LoginWidget::LoginWidget()
    : WContainerWidget()
    , emailInput_(nullptr)
    , passwordInput_(nullptr)
    , loginButton_(nullptr)
    , errorIcon_(nullptr)
    , errorText_(nullptr)
    , errorContainer_(nullptr)
    , errorHelpContainer_(nullptr)
    , errorHelpText_(nullptr) {
    setupUI();
}

LoginWidget::~LoginWidget() {
}

void LoginWidget::setupUI() {
    addStyleClass("login-widget");

    // Title
    auto title = addWidget(std::make_unique<Wt::WText>("<h2>Student Login</h2>"));
    title->addStyleClass("login-title");
    title->setTextFormat(Wt::TextFormat::XHTML);

    // Description
    auto desc = addWidget(std::make_unique<Wt::WText>(
        "<p>Welcome to Student Onboarding. Please log in to continue.</p>"));
    desc->addStyleClass("login-description");
    desc->setTextFormat(Wt::TextFormat::XHTML);

    // Error container (hidden by default) - Enhanced with icon and help text
    errorContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->addStyleClass("alert alert-danger login-error-container");
    errorContainer_->hide();

    // Error content wrapper for icon and message
    auto errorContent = errorContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContent->addStyleClass("login-error-content");

    // Error icon
    errorIcon_ = errorContent->addWidget(std::make_unique<Wt::WText>());
    errorIcon_->addStyleClass("login-error-icon");
    errorIcon_->setTextFormat(Wt::TextFormat::XHTML);

    // Error message text
    errorText_ = errorContent->addWidget(std::make_unique<Wt::WText>());
    errorText_->addStyleClass("login-error-message");

    // Help container for additional guidance (e.g., register link)
    errorHelpContainer_ = errorContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    errorHelpContainer_->addStyleClass("login-error-help");
    errorHelpContainer_->hide();
    errorHelpText_ = errorHelpContainer_->addWidget(std::make_unique<Wt::WText>());
    errorHelpText_->setTextFormat(Wt::TextFormat::XHTML);

    // Form container
    auto formContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer->addStyleClass("login-form");

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
    passwordInput_->setPlaceholderText("Enter your password");
    passwordInput_->addStyleClass("form-control");
    passwordLabel->setBuddy(passwordInput_);

    // Login button
    auto buttonGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("form-group button-group");

    loginButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Log In"));
    loginButton_->addStyleClass("btn btn-primary btn-block");
    loginButton_->clicked().connect(this, &LoginWidget::handleLogin);

    // Enter key triggers login
    passwordInput_->enterPressed().connect(this, &LoginWidget::handleLogin);
    emailInput_->enterPressed().connect([this]() {
        passwordInput_->setFocus();
    });

    // Links container
    auto linksContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    linksContainer->addStyleClass("login-links");

    // Forgot password link
    auto forgotLink = linksContainer->addWidget(std::make_unique<Wt::WAnchor>());
    forgotLink->setText("Forgot your password?");
    forgotLink->addStyleClass("link-secondary");
    forgotLink->clicked().connect([this]() {
        forgotPasswordRequested_.emit();
    });

    linksContainer->addWidget(std::make_unique<Wt::WBreak>());

    // Register link
    auto registerContainer = linksContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    registerContainer->addStyleClass("register-prompt");
    registerContainer->addWidget(std::make_unique<Wt::WText>("Don't have an account? "));

    auto registerLink = registerContainer->addWidget(std::make_unique<Wt::WAnchor>());
    registerLink->setText("Create one here");
    registerLink->addStyleClass("link-primary");
    registerLink->clicked().connect([this]() {
        registerRequested_.emit();
    });
}

void LoginWidget::handleLogin() {
    clearError();

    std::string email = emailInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();

    if (email.empty()) {
        showError("Please enter your email address");
        emailInput_->setFocus();
        return;
    }

    if (password.empty()) {
        showError("Please enter your password");
        passwordInput_->setFocus();
        return;
    }

    // Disable login button during authentication
    loginButton_->setEnabled(false);
    loginButton_->setText("Logging in...");

    if (authManager_) {
        AuthResult result = authManager_->login(email, password);

        if (result.success) {
            // Update session
            if (session_) {
                session_->setLoggedIn(true);
                session_->setAuthToken(result.token);
                session_->setStudent(result.student);
            }

            loginSuccess_.emit();
        } else {
            std::string errorMsg = result.message;
            if (!result.errors.empty()) {
                errorMsg = result.errors[0];
            }

            // Check if this is a "user not found" type error
            bool isUserNotFound = (errorMsg.find("not found") != std::string::npos ||
                                   errorMsg.find("Invalid email or password") != std::string::npos ||
                                   errorMsg.find("No account") != std::string::npos);

            showError(errorMsg, isUserNotFound);
        }
    } else {
        showError("Authentication service unavailable");
    }

    // Re-enable login button
    loginButton_->setEnabled(true);
    loginButton_->setText("Log In");
}

void LoginWidget::showError(const std::string& message, bool isUserNotFound) {
    // Set appropriate icon based on error type
    if (isUserNotFound) {
        errorIcon_->setText("<i class=\"fa fa-user-times\"></i>");
        errorText_->setText("Account not found");

        // Show helpful message with register link
        errorHelpText_->setText(
            "<span class=\"help-message\">No account exists with this email address. "
            "Please check your email or <a href=\"#\" class=\"register-link\">create a new account</a>.</span>");
        errorHelpContainer_->show();

        // Highlight the email field
        highlightEmailField(true);
    } else {
        errorIcon_->setText("<i class=\"fa fa-exclamation-circle\"></i>");
        errorText_->setText(message);
        errorHelpContainer_->hide();
        highlightEmailField(false);
    }

    errorContainer_->show();

    // Connect register link click if user not found
    if (isUserNotFound && errorHelpText_) {
        errorHelpText_->clicked().connect([this]() {
            clearError();
            registerRequested_.emit();
        });
    }
}

void LoginWidget::clearError() {
    errorText_->setText("");
    errorIcon_->setText("");
    errorHelpContainer_->hide();
    errorContainer_->hide();
    highlightEmailField(false);
}

void LoginWidget::highlightEmailField(bool highlight) {
    if (emailInput_) {
        if (highlight) {
            emailInput_->addStyleClass("input-error");
        } else {
            emailInput_->removeStyleClass("input-error");
        }
    }
}

void LoginWidget::reset() {
    emailInput_->setText("");
    passwordInput_->setText("");
    clearError();
}

void LoginWidget::focus() {
    emailInput_->setFocus();
}

} // namespace Auth
} // namespace StudentIntake
