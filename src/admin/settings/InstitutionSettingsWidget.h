#ifndef INSTITUTION_SETTINGS_WIDGET_H
#define INSTITUTION_SETTINGS_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <memory>
#include "api/FormSubmissionService.h"
#include "models/InstitutionSettings.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Widget for managing institution/school settings
 */
class InstitutionSettingsWidget : public Wt::WContainerWidget {
public:
    InstitutionSettingsWidget();

    void setApiService(std::shared_ptr<Api::FormSubmissionService> service);
    void loadSettings();

private:
    void createUI();
    void createBrandingSection();
    void createContactSection();
    void createLocationSection();
    void saveSettings();
    void showMessage(const std::string& message, bool isError = false);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    Models::InstitutionSettings settings_;

    // Branding fields
    Wt::WLineEdit* institutionNameEdit_;
    Wt::WLineEdit* taglineEdit_;
    Wt::WLineEdit* logoUrlEdit_;

    // Contact fields
    Wt::WLineEdit* contactEmailEdit_;
    Wt::WLineEdit* contactPhoneEdit_;
    Wt::WLineEdit* contactFaxEdit_;
    Wt::WLineEdit* websiteUrlEdit_;

    // Location fields
    Wt::WLineEdit* addressStreet1Edit_;
    Wt::WLineEdit* addressStreet2Edit_;
    Wt::WLineEdit* addressCityEdit_;
    Wt::WLineEdit* addressStateEdit_;
    Wt::WLineEdit* addressPostalCodeEdit_;
    Wt::WLineEdit* addressCountryEdit_;

    // General settings
    Wt::WLineEdit* academicYearEdit_;
    Wt::WTextArea* accreditationEdit_;

    // UI elements
    Wt::WContainerWidget* messageContainer_;
    Wt::WPushButton* saveButton_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // INSTITUTION_SETTINGS_WIDGET_H
