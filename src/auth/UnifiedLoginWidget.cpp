#include "UnifiedLoginWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WLabel.h>
#include <Wt/WApplication.h>

namespace StudentIntake {
namespace Auth {

UnifiedLoginWidget::UnifiedLoginWidget() {
    setStyleClass("unified-login-widget");
    setupUI();
}

void UnifiedLoginWidget::setupUI() {
    // Main container
    formContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer_->setStyleClass("login-form-container");

    // Header
    auto headerContainer = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->setStyleClass("login-header");

    titleText_ = headerContainer->addWidget(std::make_unique<Wt::WText>("Welcome"));
    titleText_->setStyleClass("login-title");

    subtitleText_ = headerContainer->addWidget(std::make_unique<Wt::WText>("Sign in to your account"));
    subtitleText_->setStyleClass("login-subtitle");

    // Error container (initially hidden)
    errorContainer_ = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->setStyleClass("login-error-container");
    errorContainer_->hide();

    errorText_ = errorContainer_->addWidget(std::make_unique<Wt::WText>());
    errorText_->setStyleClass("login-error-text");

    // Form fields
    auto fieldsContainer = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsContainer->setStyleClass("login-fields");

    // Email field
    auto emailGroup = fieldsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->setStyleClass("form-group");

    auto emailLabel = emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email Address"));
    emailLabel->setStyleClass("form-label");

    emailInput_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput_->setStyleClass("form-input");
    emailInput_->setPlaceholderText("Enter your email");
    emailInput_->setAttributeValue("type", "email");
    emailInput_->setAttributeValue("autocomplete", "email");
    emailLabel->setBuddy(emailInput_);

    // Password field
    auto passwordGroup = fieldsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    passwordGroup->setStyleClass("form-group");

    auto passwordLabel = passwordGroup->addWidget(std::make_unique<Wt::WLabel>("Password"));
    passwordLabel->setStyleClass("form-label");

    passwordInput_ = passwordGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    passwordInput_->setStyleClass("form-input");
    passwordInput_->setPlaceholderText("Enter your password");
    passwordInput_->setEchoMode(Wt::EchoMode::Password);
    passwordInput_->setAttributeValue("autocomplete", "current-password");
    passwordLabel->setBuddy(passwordInput_);

    // Login button
    auto buttonContainer = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->setStyleClass("login-buttons");

    loginButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Sign In"));
    loginButton_->setStyleClass("btn btn-primary btn-block login-btn");
    loginButton_->clicked().connect(this, &UnifiedLoginWidget::handleLogin);

    // Loading indicator (initially hidden)
    loadingIndicator_ = buttonContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    loadingIndicator_->setStyleClass("loading-indicator");
    loadingIndicator_->addWidget(std::make_unique<Wt::WText>("Signing in..."));
    loadingIndicator_->hide();

    // Forgot password link
    auto linksContainer = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    linksContainer->setStyleClass("login-links");

    forgotPasswordButton_ = linksContainer->addWidget(std::make_unique<Wt::WPushButton>("Forgot your password?"));
    forgotPasswordButton_->setStyleClass("btn btn-link");
    forgotPasswordButton_->clicked().connect([this] { forgotPasswordRequested_.emit(); });

    // Register section
    auto registerContainer = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    registerContainer->setStyleClass("register-section");

    auto registerText = registerContainer->addWidget(std::make_unique<Wt::WText>("Don't have an account? "));
    registerText->setStyleClass("register-text");

    registerButton_ = registerContainer->addWidget(std::make_unique<Wt::WPushButton>("Register"));
    registerButton_->setStyleClass("btn btn-link register-btn");
    registerButton_->clicked().connect([this] { registerRequested_.emit(); });

    // Enter key triggers login
    emailInput_->enterPressed().connect(this, &UnifiedLoginWidget::handleLogin);
    passwordInput_->enterPressed().connect(this, &UnifiedLoginWidget::handleLogin);
}

void UnifiedLoginWidget::handleLogin() {
    clearError();

    std::string email = emailInput_->text().toUTF8();
    std::string password = passwordInput_->text().toUTF8();

    // Basic validation
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

    if (!authService_) {
        showError("Authentication service not available");
        return;
    }

    // Show loading state
    setLoading(true);

    // Get client info for audit
    std::string ipAddress = "";
    std::string userAgent = "";
    if (Wt::WApplication::instance()) {
        auto& env = Wt::WApplication::instance()->environment();
        ipAddress = env.clientAddress();
        userAgent = env.userAgent();
    }

    // Attempt login
    auto result = authService_->login(email, password, ipAddress, userAgent);

    setLoading(false);

    if (result.success) {
        // Store authenticated user and tokens
        authenticatedUser_ = result.user;
        sessionToken_ = result.sessionToken;
        refreshToken_ = result.refreshToken;

        // Clear sensitive data
        passwordInput_->setText("");

        // Emit success signal with user for routing
        loginSuccess_.emit(authenticatedUser_);
    } else {
        showError(result.message.empty() ? "Invalid email or password" : result.message);
        passwordInput_->setFocus();
    }
}

void UnifiedLoginWidget::reset() {
    emailInput_->setText("");
    passwordInput_->setText("");
    clearError();
    authenticatedUser_ = Models::User();
    sessionToken_ = "";
    refreshToken_ = "";
}

void UnifiedLoginWidget::focus() {
    emailInput_->setFocus();
}

void UnifiedLoginWidget::showError(const std::string& message) {
    errorText_->setText(message);
    errorContainer_->show();
}

void UnifiedLoginWidget::clearError() {
    errorText_->setText("");
    errorContainer_->hide();
}

void UnifiedLoginWidget::setLoading(bool loading) {
    if (loading) {
        loginButton_->hide();
        loadingIndicator_->show();
        emailInput_->setDisabled(true);
        passwordInput_->setDisabled(true);
    } else {
        loginButton_->show();
        loadingIndicator_->hide();
        emailInput_->setDisabled(false);
        passwordInput_->setDisabled(false);
    }
}

} // namespace Auth
} // namespace StudentIntake
