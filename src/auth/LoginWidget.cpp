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
    , errorText_(nullptr)
    , errorContainer_(nullptr) {
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

    // Error container (hidden by default)
    errorContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->addStyleClass("alert alert-danger");
    errorContainer_->hide();
    errorText_ = errorContainer_->addWidget(std::make_unique<Wt::WText>());

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
            showError(errorMsg);
        }
    } else {
        showError("Authentication service unavailable");
    }

    // Re-enable login button
    loginButton_->setEnabled(true);
    loginButton_->setText("Log In");
}

void LoginWidget::showError(const std::string& message) {
    errorText_->setText(message);
    errorContainer_->show();
}

void LoginWidget::clearError() {
    errorText_->setText("");
    errorContainer_->hide();
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
