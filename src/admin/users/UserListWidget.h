#ifndef USER_LIST_WIDGET_H
#define USER_LIST_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <vector>
#include <memory>
#include "models/User.h"
#include "api/ApiClient.h"
#include "auth/AuthService.h"

namespace StudentIntake {
namespace Admin {

class UserListWidget : public Wt::WContainerWidget {
public:
    UserListWidget();
    ~UserListWidget();

    void setApiClient(std::shared_ptr<Api::ApiClient> apiClient);
    void setAuthService(std::shared_ptr<Auth::AuthService> authService);
    void setCurrentUserRoles(const std::vector<StudentIntake::Models::UserRole>& roles);
    void refresh();

    // Signal emitted when a user is selected for viewing/editing
    Wt::Signal<int>& userSelected() { return userSelected_; }

    // Signal emitted when "Add User" is clicked
    Wt::Signal<>& addUserClicked() { return addUserClicked_; }

private:
    void setupUI();
    void setupStats();
    void setupFilters();
    void setupTable();
    void loadUsers();
    void applyFilters();
    void updateStats();
    void clearFilters();
    void updateTable(const std::vector<StudentIntake::Models::User>& users);
    void onUserRowClicked(int userId);
    std::string getRoleBadges(const StudentIntake::Models::User& user) const;
    std::string getStatusBadgeClass(bool isActive) const;

    std::shared_ptr<Api::ApiClient> apiClient_;
    std::shared_ptr<Auth::AuthService> authService_;
    std::vector<StudentIntake::Models::User> allUsers_;
    std::vector<StudentIntake::Models::UserRole> currentUserRoles_;
    bool isCurrentUserAdmin_;

    // UI Elements - Stats
    Wt::WContainerWidget* statsContainer_;
    Wt::WText* totalUsersText_;
    Wt::WText* adminCountText_;
    Wt::WText* instructorCountText_;
    Wt::WText* studentCountText_;

    // UI Elements - Actions
    Wt::WPushButton* addUserBtn_;

    // UI Elements - Filters
    Wt::WLineEdit* searchInput_;
    Wt::WComboBox* roleFilter_;
    Wt::WComboBox* statusFilter_;
    Wt::WPushButton* clearButton_;
    Wt::WContainerWidget* tableContainer_;
    Wt::WTable* userTable_;
    Wt::WText* resultCount_;

    // Signals
    Wt::Signal<int> userSelected_;
    Wt::Signal<> addUserClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // USER_LIST_WIDGET_H
