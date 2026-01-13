#ifndef NAVIGATION_WIDGET_H
#define NAVIGATION_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WImage.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Widgets {

/**
 * @brief Navigation bar widget for the application
 */
class NavigationWidget : public Wt::WContainerWidget {
public:
    NavigationWidget();
    ~NavigationWidget();

    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }

    // Update display based on session state
    void refresh();

    // Signals
    Wt::Signal<>& logoutClicked() { return logoutClicked_; }
    Wt::Signal<>& homeClicked() { return homeClicked_; }
    Wt::Signal<>& profileClicked() { return profileClicked_; }
    Wt::Signal<>& helpClicked() { return helpClicked_; }

private:
    void setupUI();
    void updateUserDisplay();

    std::shared_ptr<Session::StudentSession> session_;

    Wt::WContainerWidget* brandContainer_;
    Wt::WContainerWidget* userContainer_;
    Wt::WText* userNameText_;
    Wt::WPushButton* logoutButton_;
    Wt::WPushButton* profileButton_;
    Wt::WPushButton* helpButton_;

    Wt::Signal<> logoutClicked_;
    Wt::Signal<> homeClicked_;
    Wt::Signal<> profileClicked_;
    Wt::Signal<> helpClicked_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // NAVIGATION_WIDGET_H
