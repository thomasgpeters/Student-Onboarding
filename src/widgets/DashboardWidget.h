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
    Wt::Signal<>& changeProgramClicked() { return changeProgramClicked_; }
    Wt::Signal<>& enterClassroomClicked() { return enterClassroomClicked_; }

    // Check if the student's program has an online classroom
    void setHasOnlineCourse(bool has) { hasOnlineCourse_ = has; }
    bool hasOnlineCourse() const { return hasOnlineCourse_; }

private:
    void setupUI();
    void updateDisplay();
    void updateCompletedFormsDisplay();
    void updateRecommendedFormsDisplay();
    void updateClassroomSection();

    std::shared_ptr<Session::StudentSession> session_;

    // Layout containers
    Wt::WContainerWidget* mainLayout_;
    Wt::WContainerWidget* leftColumn_;
    Wt::WContainerWidget* rightPanel_;

    // Left column widgets
    Wt::WText* statusText_;
    Wt::WText* curriculumText_;
    Wt::WContainerWidget* progressSection_;
    Wt::WText* progressText_;
    Wt::WPushButton* continueButton_;
    Wt::WContainerWidget* completionSection_;

    // Right panel widgets (visible after completion)
    Wt::WContainerWidget* completedFormsSection_;
    Wt::WContainerWidget* completedFormsList_;
    Wt::WContainerWidget* recommendedFormsSection_;
    Wt::WContainerWidget* recommendedFormsList_;

    // Classroom section (visible if program has online course and onboarding complete)
    Wt::WContainerWidget* classroomSection_;
    Wt::WPushButton* enterClassroomButton_;
    bool hasOnlineCourse_;

    Wt::Signal<> continueClicked_;
    Wt::Signal<> startOverClicked_;
    Wt::Signal<std::string> viewFormClicked_;
    Wt::Signal<> additionalFormsClicked_;
    Wt::Signal<> changeProgramClicked_;
    Wt::Signal<> enterClassroomClicked_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // DASHBOARD_WIDGET_H
