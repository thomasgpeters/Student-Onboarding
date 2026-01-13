#ifndef DASHBOARD_WIDGET_H
#define DASHBOARD_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <memory>
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Widgets {

/**
 * @brief Dashboard widget showing completion status and next steps
 */
class DashboardWidget : public Wt::WContainerWidget {
public:
    DashboardWidget();
    ~DashboardWidget();

    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }

    void refresh();
    void showCompletionMessage();

    Wt::Signal<>& continueClicked() { return continueClicked_; }
    Wt::Signal<>& startOverClicked() { return startOverClicked_; }
    Wt::Signal<std::string>& viewFormClicked() { return viewFormClicked_; }
    Wt::Signal<>& additionalFormsClicked() { return additionalFormsClicked_; }

private:
    void setupUI();
    void updateDisplay();
    void updateCompletedFormsDisplay();

    std::shared_ptr<Session::StudentSession> session_;

    Wt::WText* welcomeText_;
    Wt::WText* statusText_;
    Wt::WText* curriculumText_;
    Wt::WContainerWidget* progressSection_;
    Wt::WText* progressText_;
    Wt::WPushButton* continueButton_;
    Wt::WContainerWidget* completionSection_;
    Wt::WContainerWidget* completedFormsSection_;
    Wt::WContainerWidget* completedFormsList_;
    Wt::WContainerWidget* additionalFormsSection_;

    Wt::Signal<> continueClicked_;
    Wt::Signal<> startOverClicked_;
    Wt::Signal<std::string> viewFormClicked_;
    Wt::Signal<> additionalFormsClicked_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // DASHBOARD_WIDGET_H
