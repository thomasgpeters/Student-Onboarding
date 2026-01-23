#ifndef ROLE_NAVIGATION_WIDGET_H
#define ROLE_NAVIGATION_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <vector>
#include "models/User.h"

namespace StudentIntake {
namespace Widgets {

/**
 * @brief Navigation widget for users with multiple roles
 *
 * Displays links to switch between different sections based on user's roles.
 * Only visible for users with multiple roles (admins).
 */
class RoleNavigationWidget : public Wt::WContainerWidget {
public:
    RoleNavigationWidget();
    ~RoleNavigationWidget() = default;

    /**
     * @brief Set the current user and their roles
     */
    void setUser(const Models::User& user);

    /**
     * @brief Set the currently active role
     */
    void setActiveRole(Models::UserRole role);

    /**
     * @brief Get the currently active role
     */
    Models::UserRole getActiveRole() const { return activeRole_; }

    /**
     * @brief Signal emitted when user clicks to switch to a different role
     */
    Wt::Signal<Models::UserRole>& roleSelected() { return roleSelected_; }

    /**
     * @brief Check if widget should be visible (user has multiple roles)
     */
    bool shouldBeVisible() const { return userRoles_.size() > 1; }

    /**
     * @brief Update visibility based on roles
     */
    void updateVisibility();

private:
    void setupUI();
    void updateRoleButtons();
    void createRoleButton(Models::UserRole role);

    Models::User currentUser_;
    std::vector<Models::UserRole> userRoles_;
    Models::UserRole activeRole_;

    Wt::WContainerWidget* navContainer_;
    Wt::WText* currentRoleText_;
    Wt::WContainerWidget* buttonsContainer_;

    std::map<Models::UserRole, Wt::WPushButton*> roleButtons_;

    Wt::Signal<Models::UserRole> roleSelected_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // ROLE_NAVIGATION_WIDGET_H
