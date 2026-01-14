#ifndef PERSONAL_INFO_FORM_H
#define PERSONAL_INFO_FORM_H

#include "BaseForm.h"
#include "models/StudentAddress.h"
#include <Wt/WLineEdit.h>
#include <Wt/WComboBox.h>
#include <Wt/WDateEdit.h>
#include <Wt/WTextArea.h>
#include <Wt/WCheckBox.h>

namespace StudentIntake {
namespace Forms {

/**
 * @brief Form for collecting personal information
 */
class PersonalInfoForm : public BaseForm {
public:
    PersonalInfoForm();
    ~PersonalInfoForm() override;

    bool validate() override;

protected:
    void createFormFields() override;
    void collectFormData(Models::FormData& data) const override;
    void populateFormFields(const Models::FormData& data) override;
    void handleSubmit() override;

private:
    // Personal details
    Wt::WLineEdit* firstNameInput_;
    Wt::WLineEdit* middleNameInput_;
    Wt::WLineEdit* lastNameInput_;
    Wt::WLineEdit* preferredNameInput_;
    Wt::WDateEdit* dateOfBirthInput_;
    Wt::WComboBox* genderSelect_;
    Wt::WLineEdit* pronounsInput_;

    // Contact information
    Wt::WLineEdit* emailInput_;
    Wt::WLineEdit* phoneInput_;
    Wt::WLineEdit* altPhoneInput_;

    // Address
    Wt::WLineEdit* addressLine1Input_;
    Wt::WLineEdit* addressLine2Input_;
    Wt::WLineEdit* cityInput_;
    Wt::WComboBox* stateSelect_;
    Wt::WLineEdit* zipCodeInput_;
    Wt::WComboBox* countrySelect_;

    // Mailing address
    Wt::WCheckBox* sameAsHomeCheckbox_;
    Wt::WContainerWidget* mailingAddressContainer_;
    Wt::WLineEdit* mailingAddressLine1Input_;
    Wt::WLineEdit* mailingAddressLine2Input_;
    Wt::WLineEdit* mailingCityInput_;
    Wt::WComboBox* mailingStateSelect_;
    Wt::WLineEdit* mailingZipCodeInput_;
    Wt::WComboBox* mailingCountrySelect_;

    // Identification
    Wt::WLineEdit* ssnInput_;
    Wt::WComboBox* citizenshipSelect_;
    Wt::WCheckBox* internationalCheckbox_;

    void setupAddressFields(Wt::WContainerWidget* container, const std::string& prefix);
    void toggleMailingAddress();
    void updateStudentFromForm();
    void loadAddressesFromApi();
    void saveAddressesToApi();
    Models::StudentAddress buildAddressFromForm(const std::string& addressType) const;
    Models::StudentAddress buildMailingAddressFromForm() const;
    void populateAddressFields(const Models::StudentAddress& address);
    void populateMailingAddressFields(const Models::StudentAddress& address);
    std::vector<std::string> getUSStates() const;
    std::vector<std::string> getCountries() const;

    // Cached address IDs for updates
    std::string homeAddressId_;
    std::string mailingAddressId_;
};

} // namespace Forms
} // namespace StudentIntake

#endif // PERSONAL_INFO_FORM_H
