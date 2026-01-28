#include "RoleNavigationWidget.h"

namespace StudentIntake {
namespace Widgets {

RoleNavigationWidget::RoleNavigationWidget()
    : activeRole_(Models::UserRole::Student)
{
    setStyleClass("role-navigation");
    setupUI();
}

void RoleNavigationWidget::setupUI() {
    navContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    navContainer_->setStyleClass("role-nav-container");

    // Current role indicator
    auto currentRoleContainer = navContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    currentRoleContainer->setStyleClass("current-role");

    auto label = currentRoleContainer->addWidget(std::make_unique<Wt::WText>("Current view: "));
    label->setStyleClass("role-label");

    currentRoleText_ = currentRoleContainer->addWidget(std::make_unique<Wt::WText>());
    currentRoleText_->setStyleClass("role-name");

    // Separator
    auto separator = navContainer_->addWidget(std::make_unique<Wt::WText>(" | "));
    separator->setStyleClass("role-separator");

    // Switch to label
    auto switchLabel = navContainer_->addWidget(std::make_unique<Wt::WText>("Switch to: "));
    switchLabel->setStyleClass("switch-label");

    // Buttons container
    buttonsContainer_ = navContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonsContainer_->setStyleClass("role-buttons");
}

void RoleNavigationWidget::setUser(const Models::User& user) {
    currentUser_ = user;
    userRoles_ = user.getRoles();

    // Set active role to user's primary role if not already set
    if (userRoles_.empty()) {
        activeRole_ = Models::UserRole::Student;
    } else {
        activeRole_ = user.getPrimaryRole();
    }

    updateRoleButtons();
    updateVisibility();
}

void RoleNavigationWidget::setActiveRole(Models::UserRole role) {
    activeRole_ = role;
    updateRoleButtons();
}

void RoleNavigationWidget::updateVisibility() {
    if (shouldBeVisible()) {
        show();
    } else {
        hide();
    }
}

void RoleNavigationWidget::updateRoleButtons() {
    // Update current role text
    currentRoleText_->setText(Models::User::roleToDisplayString(activeRole_));

    // Clear existing buttons
    buttonsContainer_->clear();
    roleButtons_.clear();

    // Create buttons for each role (except active one)
    for (const auto& role : userRoles_) {
        if (role != activeRole_) {
            createRoleButton(role);
        }
    }
}

void RoleNavigationWidget::createRoleButton(Models::UserRole role) {
    std::string buttonText;
    std::string styleClass = "btn btn-sm role-switch-btn ";

    switch (role) {
        case Models::UserRole::Admin:
            buttonText = "Administration";
            styleClass += "btn-admin";
            break;
        case Models::UserRole::Manager:
            buttonText = "Manager Portal";
            styleClass += "btn-manager";
            break;
        case Models::UserRole::Staff:
            buttonText = "Staff Portal";
            styleClass += "btn-staff";
            break;
        case Models::UserRole::Examiner:
            buttonText = "Examiner Portal";
            styleClass += "btn-examiner";
            break;
        case Models::UserRole::Instructor:
            buttonText = "Instructor Portal";
            styleClass += "btn-instructor";
            break;
        case Models::UserRole::Student:
            buttonText = "Student Portal";
            styleClass += "btn-student";
            break;
    }

    auto button = buttonsContainer_->addWidget(std::make_unique<Wt::WPushButton>(buttonText));
    button->setStyleClass(styleClass);

    button->clicked().connect([this, role] {
        roleSelected_.emit(role);
    });

    roleButtons_[role] = button;
}

} // namespace Widgets
} // namespace StudentIntake
