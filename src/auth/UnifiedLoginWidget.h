#ifndef UNIFIED_LOGIN_WIDGET_H
#define UNIFIED_LOGIN_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTemplate.h>
#include <Wt/WSignal.h>
#include <memory>
#include "AuthService.h"
#include "models/User.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Unified login widget for all user types
 *
 * Handles authentication at "/" endpoint and provides role information
 * for routing to the appropriate section (student, instructor, admin)
 */
class UnifiedLoginWidget : public Wt::WContainerWidget {
public:
    UnifiedLoginWidget();
    ~UnifiedLoginWidget() = default;

    // Dependencies
    void setAuthService(std::shared_ptr<AuthService> authService) { authService_ = authService; }

    // Signals
    /**
     * @brief Emitted on successful login with the authenticated user
     * User contains roles for routing decisions
     */
    Wt::Signal<Models::User>& loginSuccess() { return loginSuccess_; }

    /**
     * @brief Emitted when user requests registration
     */
    Wt::Signal<>& registerRequested() { return registerRequested_; }

    /**
     * @brief Emitted when user requests password reset
     */
    Wt::Signal<>& forgotPasswordRequested() { return forgotPasswordRequested_; }

    // Actions
    void reset();
    void focus();

    // Get the authenticated user (after successful login)
    const Models::User& getAuthenticatedUser() const { return authenticatedUser_; }

    // Get session token
    std::string getSessionToken() const { return sessionToken_; }

private:
    void setupUI();
    void handleLogin();
    void showError(const std::string& message);
    void clearError();
    void setLoading(bool loading);

    std::shared_ptr<AuthService> authService_;
    Models::User authenticatedUser_;
    std::string sessionToken_;
    std::string refreshToken_;

    // UI Components
    Wt::WContainerWidget* formContainer_;
    Wt::WText* titleText_;
    Wt::WText* subtitleText_;
    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* passwordInput_;
    Wt::WPushButton* loginButton_;
    Wt::WPushButton* registerButton_;
    Wt::WPushButton* forgotPasswordButton_;
    Wt::WText* errorText_;
    Wt::WContainerWidget* errorContainer_;
    Wt::WContainerWidget* loadingIndicator_;

    // Signals
    Wt::Signal<Models::User> loginSuccess_;
    Wt::Signal<> registerRequested_;
    Wt::Signal<> forgotPasswordRequested_;
};

} // namespace Auth
} // namespace StudentIntake

#endif // UNIFIED_LOGIN_WIDGET_H
