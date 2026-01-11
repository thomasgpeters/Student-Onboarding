#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTemplate.h>
#include <Wt/WSignal.h>
#include <memory>
#include "AuthManager.h"
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Login form widget
 */
class LoginWidget : public Wt::WContainerWidget {
public:
    LoginWidget();
    ~LoginWidget();

    // Dependencies
    void setAuthManager(std::shared_ptr<AuthManager> authManager) { authManager_ = authManager; }
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }

    // Signals
    Wt::Signal<>& loginSuccess() { return loginSuccess_; }
    Wt::Signal<>& registerRequested() { return registerRequested_; }
    Wt::Signal<>& forgotPasswordRequested() { return forgotPasswordRequested_; }

    // Actions
    void reset();
    void focus();

private:
    void setupUI();
    void handleLogin();
    void showError(const std::string& message);
    void clearError();

    std::shared_ptr<AuthManager> authManager_;
    std::shared_ptr<Session::StudentSession> session_;

    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* passwordInput_;
    Wt::WPushButton* loginButton_;
    Wt::WText* errorText_;
    Wt::WContainerWidget* errorContainer_;

    Wt::Signal<> loginSuccess_;
    Wt::Signal<> registerRequested_;
    Wt::Signal<> forgotPasswordRequested_;
};

} // namespace Auth
} // namespace StudentIntake

#endif // LOGIN_WIDGET_H
