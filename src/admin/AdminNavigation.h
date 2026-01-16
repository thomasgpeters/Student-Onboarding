#ifndef ADMIN_NAVIGATION_H
#define ADMIN_NAVIGATION_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include "admin/models/AdminSession.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Top navigation bar for the admin portal
 */
class AdminNavigation : public Wt::WContainerWidget {
public:
    AdminNavigation();
    ~AdminNavigation();

    void setSession(std::shared_ptr<Models::AdminSession> session) { session_ = session; }

    // Update display based on session state
    void refresh();

    // Signals
    Wt::Signal<>& logoutClicked() { return logoutClicked_; }
    Wt::Signal<>& homeClicked() { return homeClicked_; }

private:
    void setupUI();
    void updateUserDisplay();

    std::shared_ptr<Models::AdminSession> session_;

    Wt::WContainerWidget* brandContainer_;
    Wt::WContainerWidget* userContainer_;
    Wt::WText* userNameText_;
    Wt::WText* roleText_;
    Wt::WPushButton* logoutButton_;

    Wt::Signal<> logoutClicked_;
    Wt::Signal<> homeClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_NAVIGATION_H
