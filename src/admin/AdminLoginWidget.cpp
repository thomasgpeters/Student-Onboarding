#include "AdminLoginWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WLabel.h>
#include <Wt/WImage.h>
#include <iostream>

namespace StudentIntake {
namespace Admin {

// Static instance counter for debugging duplicate widget issues
static int s_instanceCount = 0;

AdminLoginWidget::AdminLoginWidget()
    : WContainerWidget()
    , authManager_(nullptr)
    , session_(nullptr)
    , emailInput_(nullptr)
    , passwordInput_(nullptr)
    , loginButton_(nullptr)
    , errorText_(nullptr)
    , errorContainer_(nullptr)
    , instanceNumber_(++s_instanceCount) {
    std::cerr << "[AdminLoginWidget] Constructor called - INSTANCE #" << instanceNumber_
              << " - this=" << this << " - TOTAL INSTANCES: " << s_instanceCount << std::endl;
    setupUI();
    std::cerr << "[AdminLoginWidget] Constructor complete - INSTANCE #" << instanceNumber_
              << " - id=" << id() << std::endl;
}

AdminLoginWidget::~AdminLoginWidget() {
}

void AdminLoginWidget::setupUI() {
    // Add unique identifier to prevent any duplicate rendering issues
    setId("admin-login-widget-main");
    addStyleClass("admin-login-widget");

    // DEBUG: Add visible instance number to identify duplicates
    auto debugBadge = addWidget(std::make_unique<Wt::WText>(
        "<div style='position:absolute;top:5px;right:5px;background:red;color:white;"
        "padding:2px 8px;border-radius:4px;font-size:12px;font-weight:bold;'>"
        "Instance #" + std::to_string(instanceNumber_) + "</div>"));
    debugBadge->setTextFormat(Wt::TextFormat::UnsafeXHTML);

    // Logo and title container
    auto headerContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->addStyleClass("admin-login-header");

    // University logo placeholder
    auto logoContainer = headerContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    logoContainer->addStyleClass("admin-logo-container");

    auto logoText = logoContainer->addWidget(std::make_unique<Wt::WText>("🎓"));
    logoText->addStyleClass("admin-logo-icon");

    // Title
    auto title = headerContainer->addWidget(std::make_unique<Wt::WText>("<h2>Admin Portal</h2>"));
    title->setTextFormat(Wt::TextFormat::XHTML);
    title->addStyleClass("admin-login-title");

    auto subtitle = headerContainer->addWidget(std::make_unique<Wt::WText>(
        "<p>Student Onboarding Administration</p>"));
    subtitle->setTextFormat(Wt::TextFormat::XHTML);
    subtitle->addStyleClass("admin-login-subtitle");

    // Error container (hidden by default)
    errorContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    errorContainer_->addStyleClass("alert alert-danger");
    errorContainer_->hide();

    errorText_ = errorContainer_->addWidget(std::make_unique<Wt::WText>());

    // Login form
    auto formContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer->addStyleClass("admin-login-form");

    // Email field
    auto emailGroup = formContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");

    auto emailLabel = emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email"));
    emailInput_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput_->setPlaceholderText("admin@university.edu");
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
    loginButton_ = formContainer->addWidget(std::make_unique<Wt::WPushButton>("Log In"));
    loginButton_->addStyleClass("btn btn-primary btn-block btn-lg");
    loginButton_->clicked().connect(this, &AdminLoginWidget::handleLogin);

    // Enter key triggers login
    passwordInput_->enterPressed().connect(this, &AdminLoginWidget::handleLogin);

    // Forgot password link
    auto linksContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    linksContainer->addStyleClass("admin-login-links");

    auto forgotLink = linksContainer->addWidget(std::make_unique<Wt::WText>("Forgot Password?"));
    forgotLink->addStyleClass("link-secondary");
    forgotLink->clicked().connect([this]() {
        forgotPasswordRequested_.emit();
    });

    // Footer note
    auto footerNote = addWidget(std::make_unique<Wt::WText>(
        "<p class='admin-login-note'>This portal is for authorized university staff only.</p>"));
    footerNote->setTextFormat(Wt::TextFormat::XHTML);
}

void AdminLoginWidget::handleLogin() {
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

    if (!authManager_) {
        showError("Authentication service unavailable");
        return;
    }

    // Attempt login
    auto result = authManager_->login(email, password);

    if (result.success) {
        // Update session
        if (session_) {
            session_->setAdminUser(result.adminUser);
            session_->setAuthenticated(true);
            session_->setToken(result.token);
        }

        // Clear form
        reset();

        // Emit success signal
        loginSuccess_.emit();
    } else {
        showError(result.message);
        passwordInput_->setText("");
        passwordInput_->setFocus();
    }
}

void AdminLoginWidget::showError(const std::string& message) {
    errorText_->setText(message);
    errorContainer_->show();
}

void AdminLoginWidget::clearError() {
    errorText_->setText("");
    errorContainer_->hide();
}

void AdminLoginWidget::reset() {
    emailInput_->setText("");
    passwordInput_->setText("");
    clearError();
}

void AdminLoginWidget::focus() {
    emailInput_->setFocus();
}

} // namespace Admin
} // namespace StudentIntake
