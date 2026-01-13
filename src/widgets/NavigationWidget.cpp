#include "NavigationWidget.h"

namespace StudentIntake {
namespace Widgets {

NavigationWidget::NavigationWidget()
    : WContainerWidget()
    , brandContainer_(nullptr)
    , userContainer_(nullptr)
    , userNameText_(nullptr)
    , logoutButton_(nullptr)
    , profileButton_(nullptr)
    , helpButton_(nullptr) {
    setupUI();
}

NavigationWidget::~NavigationWidget() {
}

void NavigationWidget::setupUI() {
    addStyleClass("navbar");

    // Brand/Logo section
    brandContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    brandContainer_->addStyleClass("navbar-brand");

    // Logo image
    auto logo = brandContainer_->addWidget(std::make_unique<Wt::WImage>(
        "https://media.licdn.com/dms/image/v2/D4E0BAQFNqqJ59i1lgQ/company-logo_200_200/company-logo_200_200/0/1733939002925/imagery_business_systems_llc_logo?e=2147483647&v=beta&t=s_hATe0kqIDc64S79VJYXNS4N_UwrcnUA1x7VCb3sFA"));
    logo->addStyleClass("brand-logo");
    logo->clicked().connect([this]() {
        homeClicked_.emit();
    });

    auto brandLink = brandContainer_->addWidget(std::make_unique<Wt::WText>("Student Onboarding"));
    brandLink->addStyleClass("brand-text");
    brandLink->clicked().connect([this]() {
        homeClicked_.emit();
    });

    // Spacer
    auto spacer = addWidget(std::make_unique<Wt::WContainerWidget>());
    spacer->addStyleClass("navbar-spacer");

    // User section
    userContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    userContainer_->addStyleClass("navbar-user");

    userNameText_ = userContainer_->addWidget(std::make_unique<Wt::WText>());
    userNameText_->addStyleClass("user-name");

    helpButton_ = userContainer_->addWidget(std::make_unique<Wt::WPushButton>("Help"));
    helpButton_->addStyleClass("btn btn-link nav-btn");
    helpButton_->clicked().connect([this]() {
        helpClicked_.emit();
    });

    profileButton_ = userContainer_->addWidget(std::make_unique<Wt::WPushButton>("Profile"));
    profileButton_->addStyleClass("btn btn-link nav-btn");
    profileButton_->clicked().connect([this]() {
        profileClicked_.emit();
    });

    logoutButton_ = userContainer_->addWidget(std::make_unique<Wt::WPushButton>("Logout"));
    logoutButton_->addStyleClass("btn btn-outline-secondary nav-btn");
    logoutButton_->clicked().connect([this]() {
        logoutClicked_.emit();
    });

    userContainer_->hide();
}

void NavigationWidget::refresh() {
    updateUserDisplay();
}

void NavigationWidget::updateUserDisplay() {
    if (session_ && session_->isLoggedIn()) {
        std::string displayName = session_->getStudent().getFullName();
        if (displayName.empty() || displayName == " ") {
            displayName = session_->getStudent().getEmail();
        }
        userNameText_->setText("Welcome, " + displayName);
        userContainer_->show();
    } else {
        userContainer_->hide();
    }
}

} // namespace Widgets
} // namespace StudentIntake
