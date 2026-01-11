#ifndef REGISTER_WIDGET_H
#define REGISTER_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WCheckBox.h>
#include <Wt/WSignal.h>
#include <memory>
#include "AuthManager.h"
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Registration form widget
 */
class RegisterWidget : public Wt::WContainerWidget {
public:
    RegisterWidget();
    ~RegisterWidget();

    // Dependencies
    void setAuthManager(std::shared_ptr<AuthManager> authManager) { authManager_ = authManager; }
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }

    // Signals
    Wt::Signal<>& registrationSuccess() { return registrationSuccess_; }
    Wt::Signal<>& loginRequested() { return loginRequested_; }

    // Actions
    void reset();
    void focus();

private:
    void setupUI();
    void handleRegister();
    void validateForm();
    void showError(const std::string& message);
    void showErrors(const std::vector<std::string>& errors);
    void clearError();

    std::shared_ptr<AuthManager> authManager_;
    std::shared_ptr<Session::StudentSession> session_;

    Wt::WLineEdit* firstNameInput_;
    Wt::WLineEdit* lastNameInput_;
    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* passwordInput_;
    Wt::WLineEdit* confirmPasswordInput_;
    Wt::WCheckBox* termsCheckbox_;
    Wt::WPushButton* registerButton_;
    Wt::WText* errorText_;
    Wt::WContainerWidget* errorContainer_;

    // Password strength indicators
    Wt::WContainerWidget* passwordStrengthContainer_;
    Wt::WText* passwordStrengthText_;

    Wt::Signal<> registrationSuccess_;
    Wt::Signal<> loginRequested_;
};

} // namespace Auth
} // namespace StudentIntake

#endif // REGISTER_WIDGET_H
