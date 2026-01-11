#ifndef FORM_CONTAINER_H
#define FORM_CONTAINER_H

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WSignal.h>
#include <memory>
#include <vector>
#include "forms/BaseForm.h"
#include "forms/FormFactory.h"
#include "session/StudentSession.h"

namespace StudentIntake {
namespace Widgets {

/**
 * @brief Container that manages form navigation and display
 */
class FormContainer : public Wt::WContainerWidget {
public:
    FormContainer();
    ~FormContainer();

    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }
    void setFormFactory(std::shared_ptr<Forms::FormFactory> factory) { formFactory_ = factory; }

    // Load forms based on required form IDs
    void loadForms(const std::vector<std::string>& formIds);

    // Navigation
    void showForm(const std::string& formId);
    void showNextForm();
    void showPreviousForm();
    void showFirstIncompleteForm();

    // Get current form info
    std::string getCurrentFormId() const;
    int getCurrentFormIndex() const;
    int getTotalFormsCount() const;

    // Signals
    Wt::Signal<std::string>& formChanged() { return formChanged_; }
    Wt::Signal<>& allFormsCompleted() { return allFormsCompleted_; }
    Wt::Signal<std::string>& formSubmitted() { return formSubmitted_; }

private:
    void setupUI();
    void handleFormSubmitted(const std::string& formId);
    void handleFormPrevious(const std::string& formId);
    void updateFormNavigation();

    std::shared_ptr<Session::StudentSession> session_;
    std::shared_ptr<Forms::FormFactory> formFactory_;

    Wt::WStackedWidget* formStack_;
    std::vector<std::string> formIds_;
    std::map<std::string, Forms::BaseForm*> forms_;
    int currentIndex_;

    Wt::Signal<std::string> formChanged_;
    Wt::Signal<> allFormsCompleted_;
    Wt::Signal<std::string> formSubmitted_;
};

} // namespace Widgets
} // namespace StudentIntake

#endif // FORM_CONTAINER_H
