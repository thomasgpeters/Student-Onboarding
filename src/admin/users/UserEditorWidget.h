#ifndef USER_EDITOR_WIDGET_H
#define USER_EDITOR_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <memory>
#include "models/User.h"
#include "api/ApiClient.h"
#include "auth/AuthService.h"

namespace StudentIntake {
namespace Admin {

class UserEditorWidget : public Wt::WContainerWidget {
public:
    UserEditorWidget();
    ~UserEditorWidget();

    void setApiClient(std::shared_ptr<Api::ApiClient> apiClient);
    void setAuthService(std::shared_ptr<Auth::AuthService> authService);

    // Load existing user for editing
    void loadUser(int userId);

    // Reset for new user creation
    void newUser();

    // Signals
    Wt::Signal<>& saved() { return saved_; }
    Wt::Signal<>& cancelled() { return cancelled_; }

private:
    void setupUI();
    void handleSave();
    void handleCancel();
    void showError(const std::string& message);
    void showSuccess(const std::string& message);
    void clearMessages();
    bool validateForm();
    void populateForm(const Models::User& user);
    void clearForm();

    std::shared_ptr<Api::ApiClient> apiClient_;
    std::shared_ptr<Auth::AuthService> authService_;

    bool isEditMode_;
    int editingUserId_;

    // Form fields
    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* firstNameInput_;
    Wt::WLineEdit* lastNameInput_;
    Wt::WLineEdit* phoneInput_;
    Wt::WLineEdit* passwordInput_;
    Wt::WLineEdit* confirmPasswordInput_;

    // Role checkboxes
    Wt::WCheckBox* adminRoleCheck_;
    Wt::WCheckBox* instructorRoleCheck_;
    Wt::WCheckBox* studentRoleCheck_;

    // Status
    Wt::WCheckBox* activeCheck_;
    Wt::WCheckBox* loginEnabledCheck_;

    // UI elements
    Wt::WText* titleText_;
    Wt::WText* messageText_;
    Wt::WContainerWidget* messageContainer_;
    Wt::WPushButton* saveBtn_;
    Wt::WPushButton* cancelBtn_;
    Wt::WContainerWidget* passwordSection_;

    // Signals
    Wt::Signal<> saved_;
    Wt::Signal<> cancelled_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // USER_EDITOR_WIDGET_H
