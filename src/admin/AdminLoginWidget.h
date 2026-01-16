#ifndef ADMIN_LOGIN_WIDGET_H
#define ADMIN_LOGIN_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <memory>
#include "AdminAuthManager.h"
#include "admin/models/AdminSession.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Admin login form widget
 */
class AdminLoginWidget : public Wt::WContainerWidget {
public:
    AdminLoginWidget();
    ~AdminLoginWidget();

    // Dependencies
    void setAuthManager(std::shared_ptr<AdminAuthManager> authManager) { authManager_ = authManager; }
    void setSession(std::shared_ptr<Models::AdminSession> session) { session_ = session; }

    // Signals
    Wt::Signal<>& loginSuccess() { return loginSuccess_; }
    Wt::Signal<>& forgotPasswordRequested() { return forgotPasswordRequested_; }

    // Actions
    void reset();
    void focus();

private:
    void setupUI();
    void handleLogin();
    void showError(const std::string& message);
    void clearError();

    std::shared_ptr<AdminAuthManager> authManager_;
    std::shared_ptr<Models::AdminSession> session_;

    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* passwordInput_;
    Wt::WPushButton* loginButton_;
    Wt::WText* errorText_;
    Wt::WContainerWidget* errorContainer_;

    Wt::Signal<> loginSuccess_;
    Wt::Signal<> forgotPasswordRequested_;

    // Debug: instance counter to track duplicate creation
    int instanceNumber_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_LOGIN_WIDGET_H
