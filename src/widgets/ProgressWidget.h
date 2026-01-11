#ifndef PROGRESS_WIDGET_H
#define PROGRESS_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WProgressBar.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "session/StudentSession.h"
#include "forms/FormFactory.h"

namespace StudentIntake {
namespace Widgets {

/**
 * @brief Widget showing form completion progress
 */
class ProgressWidget : public Wt::WContainerWidget {
public:
    ProgressWidget();
    ~ProgressWidget();

    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }
    void setFormFactory(std::shared_ptr<Forms::FormFactory> factory) { formFactory_ = factory; }

    // Update the progress display
    void refresh();

    // Set the list of required form IDs
    void setRequiredForms(const std::vector<std::string>& formIds);

    // Highlight a specific step
    void setCurrentStep(const std::string& formId);

    // Signal when a step is clicked
    Wt::Signal<std::string>& stepClicked() { return stepClicked_; }

private:
    void setupUI();
    void updateSteps();

    struct StepInfo {
        std::string formId;
        Wt::WContainerWidget* container;
        Wt::WText* numberText;
        Wt::WText* titleText;
        Wt::WText* statusIcon;
        bool completed;
        bool current;
    };

    std::shared_ptr<Session::StudentSession> session_;
    std::shared_ptr<Forms::FormFactory> formFactory_;
    std::vector<std::string> requiredFormIds_;
    std::vector<StepInfo> steps_;

    Wt::WContainerWidget* headerContainer_;
    Wt::WProgressBar* progressBar_;
    Wt::WText* progressText_;
    Wt::WContainerWidget* stepsContainer_;
    std::string currentFormId_;

    Wt::Signal<std::string> stepClicked_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // PROGRESS_WIDGET_H
