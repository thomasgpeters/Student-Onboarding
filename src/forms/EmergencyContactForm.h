#ifndef EMERGENCY_CONTACT_FORM_H
#define EMERGENCY_CONTACT_FORM_H

#include "BaseForm.h"
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <vector>

namespace StudentIntake {
namespace Forms {

/**
 * @brief Form for collecting emergency contact information
 */
class EmergencyContactForm : public BaseForm {
public:
    EmergencyContactForm();
    ~EmergencyContactForm() override;

    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;

private:
    struct ContactFields {
        Wt::WContainerWidget* container;
        Wt::WLineEdit* nameInput;
        Wt::WComboBox* relationshipSelect;
        Wt::WLineEdit* phoneInput;
        Wt::WLineEdit* altPhoneInput;
        Wt::WLineEdit* emailInput;
        Wt::WLineEdit* addressInput;
        Wt::WLineEdit* cityInput;
        Wt::WComboBox* stateSelect;
        Wt::WLineEdit* zipCodeInput;
    };

    std::vector<ContactFields> contacts_;
    Wt::WContainerWidget* contactsContainer_;
    Wt::WPushButton* addContactButton_;

    void addContactSection(int contactNumber);
    void removeContact(int index);
    std::vector<std::string> getRelationships() const;
    std::vector<std::string> getUSStates() const;
};

} // namespace Forms
} // namespace StudentIntake

#endif // EMERGENCY_CONTACT_FORM_H
