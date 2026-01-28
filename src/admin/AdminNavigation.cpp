#include "AdminNavigation.h"
#include <Wt/WImage.h>

namespace StudentIntake {
namespace Admin {

AdminNavigation::AdminNavigation()
    : WContainerWidget()
    , session_(nullptr)
    , brandContainer_(nullptr)
    , userContainer_(nullptr)
    , userNameText_(nullptr)
    , roleText_(nullptr)
    , logoutButton_(nullptr) {
    setupUI();
}

AdminNavigation::~AdminNavigation() {
}

void AdminNavigation::setupUI() {
    addStyleClass("admin-navbar");

    // Brand section
    brandContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    brandContainer_->addStyleClass("admin-navbar-brand");
    brandContainer_->clicked().connect([this]() {
        homeClicked_.emit();
    });

    // Logo image (same as student portal)
    auto logo = brandContainer_->addWidget(std::make_unique<Wt::WImage>(
        "https://media.licdn.com/dms/image/v2/D4E0BAQFNqqJ59i1lgQ/company-logo_200_200/company-logo_200_200/0/1733939002925/imagery_business_systems_llc_logo?e=2147483647&v=beta&t=s_hATe0kqIDc64S79VJYXNS4N_UwrcnUA1x7VCb3sFA"));
    logo->addStyleClass("admin-brand-logo");

    // Brand text
    auto brandText = brandContainer_->addWidget(std::make_unique<Wt::WText>("Student Intake Administration"));
    brandText->addStyleClass("admin-brand-text");

    // Spacer
    auto spacer = addWidget(std::make_unique<Wt::WContainerWidget>());
    spacer->addStyleClass("admin-navbar-spacer");

    // User section
    userContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    userContainer_->addStyleClass("admin-navbar-user");
    userContainer_->hide();  // Hidden until authenticated

    // User info
    auto userInfo = userContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    userInfo->addStyleClass("admin-user-info");

    userNameText_ = userInfo->addWidget(std::make_unique<Wt::WText>());
    userNameText_->addStyleClass("admin-user-name");

    roleText_ = userInfo->addWidget(std::make_unique<Wt::WText>());
    roleText_->addStyleClass("admin-user-role");

    // Logout button
    logoutButton_ = userContainer_->addWidget(std::make_unique<Wt::WPushButton>("Logout"));
    logoutButton_->addStyleClass("btn btn-outline-light btn-sm");
    logoutButton_->clicked().connect([this]() {
        logoutClicked_.emit();
    });
}

void AdminNavigation::refresh() {
    updateUserDisplay();
}

void AdminNavigation::updateUserDisplay() {
    if (session_ && session_->isAuthenticated()) {
        userContainer_->show();

        auto& user = session_->getAdminUser();
        userNameText_->setText(user.getFullName());

        // Display role in a friendly format
        std::string roleDisplay;
        switch (user.getRole()) {
            case Models::AdminRole::SuperAdmin:
                roleDisplay = "Super Admin";
                break;
            case Models::AdminRole::Administrator:
                roleDisplay = "Administrator";
                break;
            case Models::AdminRole::Manager:
                roleDisplay = "Manager";
                break;
            case Models::AdminRole::Staff:
                roleDisplay = "Staff";
                break;
            case Models::AdminRole::Examiner:
                roleDisplay = "Examiner";
                break;
            case Models::AdminRole::Instructor:
                roleDisplay = "Instructor";
                break;
        }
        roleText_->setText(roleDisplay);
    } else {
        userContainer_->hide();
    }
}

} // namespace Admin
} // namespace StudentIntake
