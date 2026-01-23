#ifndef INSTITUTION_SETTINGS_WIDGET_H
#define INSTITUTION_SETTINGS_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WPushButton.h>
#include <Wt/WText.h>
#include <Wt/WTable.h>
#include <Wt/WDialog.h>
#include <memory>
#include <vector>
#include "api/FormSubmissionService.h"
#include "models/InstitutionSettings.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Department data structure for UI
 */
struct DepartmentData {
    int id = 0;
    std::string code;
    std::string name;
    std::string dean;
    std::string contactEmail;
};

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
    void createLookupDataSection();
    void saveSettings();
    void showMessage(const std::string& message, bool isError = false);

    // Department management
    void loadDepartments();
    void refreshDepartmentTable();
    void showAddDepartmentDialog();
    void showEditDepartmentDialog(int departmentId);
    void saveDepartment(bool isNew, int departmentId = 0);
    void deleteDepartment(int departmentId);
    void confirmDeleteDepartment(int departmentId, const std::string& deptName);

    std::shared_ptr<Api::FormSubmissionService> apiService_;
    StudentIntake::Models::InstitutionSettings settings_;

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

    // Department management
    std::vector<DepartmentData> departments_;
    Wt::WContainerWidget* departmentTableContainer_;
    Wt::WTable* departmentTable_;

    // Department dialog fields (shared for add/edit)
    Wt::WDialog* departmentDialog_;
    Wt::WLineEdit* dialogDeptCodeEdit_;
    Wt::WLineEdit* dialogDeptNameEdit_;
    Wt::WLineEdit* dialogDeptDeanEdit_;
    Wt::WLineEdit* dialogDeptEmailEdit_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // INSTITUTION_SETTINGS_WIDGET_H
