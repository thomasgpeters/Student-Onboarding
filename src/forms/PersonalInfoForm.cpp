#include "PersonalInfoForm.h"
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include <Wt/WDate.h>
#include <ctime>

namespace StudentIntake {
namespace Forms {

PersonalInfoForm::PersonalInfoForm()
    : BaseForm("personal_info", "Personal Information")
    , firstNameInput_(nullptr)
    , middleNameInput_(nullptr)
    , lastNameInput_(nullptr)
    , preferredNameInput_(nullptr)
    , dateOfBirthInput_(nullptr)
    , genderSelect_(nullptr)
    , pronounsInput_(nullptr)
    , emailInput_(nullptr)
    , phoneInput_(nullptr)
    , altPhoneInput_(nullptr)
    , addressLine1Input_(nullptr)
    , addressLine2Input_(nullptr)
    , cityInput_(nullptr)
    , stateSelect_(nullptr)
    , zipCodeInput_(nullptr)
    , countrySelect_(nullptr)
    , sameAsHomeCheckbox_(nullptr)
    , mailingAddressContainer_(nullptr)
    , mailingAddressLine1Input_(nullptr)
    , mailingAddressLine2Input_(nullptr)
    , mailingCityInput_(nullptr)
    , mailingStateSelect_(nullptr)
    , mailingZipCodeInput_(nullptr)
    , mailingCountrySelect_(nullptr)
    , ssnInput_(nullptr)
    , citizenshipSelect_(nullptr)
    , internationalCheckbox_(nullptr) {
}

PersonalInfoForm::~PersonalInfoForm() {
}

void PersonalInfoForm::createFormFields() {
    // Personal Details Section
    auto personalSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    personalSection->addStyleClass("form-section");
    personalSection->addWidget(std::make_unique<Wt::WText>("<h4>Personal Details</h4>"));

    // Name row
    auto nameRow = personalSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameRow->addStyleClass("form-row");

    auto firstNameGroup = nameRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    firstNameGroup->addStyleClass("form-group col-md-4");
    firstNameGroup->addWidget(std::make_unique<Wt::WLabel>("First Name *"));
    firstNameInput_ = firstNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    firstNameInput_->setPlaceholderText("First name");
    firstNameInput_->addStyleClass("form-control");

    auto middleNameGroup = nameRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    middleNameGroup->addStyleClass("form-group col-md-4");
    middleNameGroup->addWidget(std::make_unique<Wt::WLabel>("Middle Name"));
    middleNameInput_ = middleNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    middleNameInput_->setPlaceholderText("Middle name");
    middleNameInput_->addStyleClass("form-control");

    auto lastNameGroup = nameRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    lastNameGroup->addStyleClass("form-group col-md-4");
    lastNameGroup->addWidget(std::make_unique<Wt::WLabel>("Last Name *"));
    lastNameInput_ = lastNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    lastNameInput_->setPlaceholderText("Last name");
    lastNameInput_->addStyleClass("form-control");

    // Preferred name and DOB row
    auto row2 = personalSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    row2->addStyleClass("form-row");

    auto preferredGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    preferredGroup->addStyleClass("form-group col-md-6");
    preferredGroup->addWidget(std::make_unique<Wt::WLabel>("Preferred Name"));
    preferredNameInput_ = preferredGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    preferredNameInput_->setPlaceholderText("Preferred name (if different)");
    preferredNameInput_->addStyleClass("form-control");

    auto dobGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    dobGroup->addStyleClass("form-group col-md-6");
    dobGroup->addWidget(std::make_unique<Wt::WLabel>("Date of Birth *"));
    dateOfBirthInput_ = dobGroup->addWidget(std::make_unique<Wt::WDateEdit>());
    dateOfBirthInput_->addStyleClass("form-control");

    // Gender row
    auto genderRow = personalSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    genderRow->addStyleClass("form-row");

    auto genderGroup = genderRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    genderGroup->addStyleClass("form-group col-md-6");
    genderGroup->addWidget(std::make_unique<Wt::WLabel>("Gender"));
    genderSelect_ = genderGroup->addWidget(std::make_unique<Wt::WComboBox>());
    genderSelect_->addStyleClass("form-control");
    genderSelect_->addItem("Select...");
    genderSelect_->addItem("Male");
    genderSelect_->addItem("Female");
    genderSelect_->addItem("Non-binary");
    genderSelect_->addItem("Prefer not to say");
    genderSelect_->addItem("Other");

    auto pronounsGroup = genderRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    pronounsGroup->addStyleClass("form-group col-md-6");
    pronounsGroup->addWidget(std::make_unique<Wt::WLabel>("Preferred Pronouns"));
    pronounsInput_ = pronounsGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    pronounsInput_->setPlaceholderText("e.g., he/him, she/her, they/them");
    pronounsInput_->addStyleClass("form-control");

    // Contact Information Section
    auto contactSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contactSection->addStyleClass("form-section");
    contactSection->addWidget(std::make_unique<Wt::WText>("<h4>Contact Information</h4>"));

    auto contactRow = contactSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    contactRow->addStyleClass("form-row");

    auto emailGroup = contactRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group col-md-4");
    emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email Address *"));
    emailInput_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    emailInput_->setPlaceholderText("email@example.com");
    emailInput_->addStyleClass("form-control");

    auto phoneGroup = contactRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneGroup->addStyleClass("form-group col-md-4");
    phoneGroup->addWidget(std::make_unique<Wt::WLabel>("Phone Number *"));
    phoneInput_ = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    phoneInput_->setPlaceholderText("(555) 555-5555");
    phoneInput_->addStyleClass("form-control");

    auto altPhoneGroup = contactRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    altPhoneGroup->addStyleClass("form-group col-md-4");
    altPhoneGroup->addWidget(std::make_unique<Wt::WLabel>("Alternate Phone"));
    altPhoneInput_ = altPhoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    altPhoneInput_->setPlaceholderText("(555) 555-5555");
    altPhoneInput_->addStyleClass("form-control");

    // Home Address Section
    auto addressSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    addressSection->addStyleClass("form-section");
    addressSection->addWidget(std::make_unique<Wt::WText>("<h4>Home Address</h4>"));

    auto addr1Group = addressSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    addr1Group->addStyleClass("form-group");
    addr1Group->addWidget(std::make_unique<Wt::WLabel>("Address Line 1 *"));
    addressLine1Input_ = addr1Group->addWidget(std::make_unique<Wt::WLineEdit>());
    addressLine1Input_->setPlaceholderText("Street address");
    addressLine1Input_->addStyleClass("form-control");

    auto addr2Group = addressSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    addr2Group->addStyleClass("form-group");
    addr2Group->addWidget(std::make_unique<Wt::WLabel>("Address Line 2"));
    addressLine2Input_ = addr2Group->addWidget(std::make_unique<Wt::WLineEdit>());
    addressLine2Input_->setPlaceholderText("Apartment, suite, unit, etc.");
    addressLine2Input_->addStyleClass("form-control");

    auto cityStateRow = addressSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityStateRow->addStyleClass("form-row");

    auto cityGroup = cityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityGroup->addStyleClass("form-group col-md-5");
    cityGroup->addWidget(std::make_unique<Wt::WLabel>("City *"));
    cityInput_ = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    cityInput_->addStyleClass("form-control");

    auto stateGroup = cityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    stateGroup->addStyleClass("form-group col-md-4");
    stateGroup->addWidget(std::make_unique<Wt::WLabel>("State *"));
    stateSelect_ = stateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    stateSelect_->addStyleClass("form-control");
    stateSelect_->addItem("Select...");
    for (const auto& state : getUSStates()) {
        stateSelect_->addItem(state);
    }

    auto zipGroup = cityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    zipGroup->addStyleClass("form-group col-md-3");
    zipGroup->addWidget(std::make_unique<Wt::WLabel>("ZIP Code *"));
    zipCodeInput_ = zipGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    zipCodeInput_->setPlaceholderText("12345");
    zipCodeInput_->addStyleClass("form-control");

    auto countryGroup = addressSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    countryGroup->addStyleClass("form-group");
    countryGroup->addWidget(std::make_unique<Wt::WLabel>("Country *"));
    countrySelect_ = countryGroup->addWidget(std::make_unique<Wt::WComboBox>());
    countrySelect_->addStyleClass("form-control");
    for (const auto& country : getCountries()) {
        countrySelect_->addItem(country);
    }

    // Mailing Address Section
    auto mailingSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    mailingSection->addStyleClass("form-section");
    mailingSection->addWidget(std::make_unique<Wt::WText>("<h4>Mailing Address</h4>"));

    sameAsHomeCheckbox_ = mailingSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " Same as home address"));
    sameAsHomeCheckbox_->addStyleClass("form-check");
    sameAsHomeCheckbox_->setChecked(true);
    sameAsHomeCheckbox_->changed().connect(this, &PersonalInfoForm::toggleMailingAddress);

    mailingAddressContainer_ = mailingSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    mailingAddressContainer_->hide();

    // Mailing address fields
    auto mAddr1Group = mailingAddressContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    mAddr1Group->addStyleClass("form-group");
    mAddr1Group->addWidget(std::make_unique<Wt::WLabel>("Address Line 1 *"));
    mailingAddressLine1Input_ = mAddr1Group->addWidget(std::make_unique<Wt::WLineEdit>());
    mailingAddressLine1Input_->addStyleClass("form-control");

    auto mAddr2Group = mailingAddressContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    mAddr2Group->addStyleClass("form-group");
    mAddr2Group->addWidget(std::make_unique<Wt::WLabel>("Address Line 2"));
    mailingAddressLine2Input_ = mAddr2Group->addWidget(std::make_unique<Wt::WLineEdit>());
    mailingAddressLine2Input_->addStyleClass("form-control");

    auto mCityStateRow = mailingAddressContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    mCityStateRow->addStyleClass("form-row");

    auto mCityGroup = mCityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    mCityGroup->addStyleClass("form-group col-md-5");
    mCityGroup->addWidget(std::make_unique<Wt::WLabel>("City *"));
    mailingCityInput_ = mCityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    mailingCityInput_->addStyleClass("form-control");

    auto mStateGroup = mCityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    mStateGroup->addStyleClass("form-group col-md-4");
    mStateGroup->addWidget(std::make_unique<Wt::WLabel>("State *"));
    mailingStateSelect_ = mStateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    mailingStateSelect_->addStyleClass("form-control");
    mailingStateSelect_->addItem("Select...");
    for (const auto& state : getUSStates()) {
        mailingStateSelect_->addItem(state);
    }

    auto mZipGroup = mCityStateRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    mZipGroup->addStyleClass("form-group col-md-3");
    mZipGroup->addWidget(std::make_unique<Wt::WLabel>("ZIP Code *"));
    mailingZipCodeInput_ = mZipGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    mailingZipCodeInput_->addStyleClass("form-control");

    auto mCountryGroup = mailingAddressContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    mCountryGroup->addStyleClass("form-group");
    mCountryGroup->addWidget(std::make_unique<Wt::WLabel>("Country *"));
    mailingCountrySelect_ = mCountryGroup->addWidget(std::make_unique<Wt::WComboBox>());
    mailingCountrySelect_->addStyleClass("form-control");
    for (const auto& country : getCountries()) {
        mailingCountrySelect_->addItem(country);
    }

    // Citizenship Section
    auto citizenshipSection = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    citizenshipSection->addStyleClass("form-section");
    citizenshipSection->addWidget(std::make_unique<Wt::WText>("<h4>Citizenship Information</h4>"));

    auto citizenRow = citizenshipSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    citizenRow->addStyleClass("form-row");

    auto ssnGroup = citizenRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    ssnGroup->addStyleClass("form-group col-md-6");
    ssnGroup->addWidget(std::make_unique<Wt::WLabel>("Social Security Number"));
    ssnInput_ = ssnGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    ssnInput_->setPlaceholderText("XXX-XX-XXXX");
    ssnInput_->addStyleClass("form-control");

    auto citizenGroup = citizenRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    citizenGroup->addStyleClass("form-group col-md-6");
    citizenGroup->addWidget(std::make_unique<Wt::WLabel>("Citizenship Status *"));
    citizenshipSelect_ = citizenGroup->addWidget(std::make_unique<Wt::WComboBox>());
    citizenshipSelect_->addStyleClass("form-control");
    citizenshipSelect_->addItem("Select...");
    citizenshipSelect_->addItem("U.S. Citizen");
    citizenshipSelect_->addItem("Permanent Resident");
    citizenshipSelect_->addItem("International Student");
    citizenshipSelect_->addItem("DACA Recipient");
    citizenshipSelect_->addItem("Refugee/Asylee");
    citizenshipSelect_->addItem("Other");

    internationalCheckbox_ = citizenshipSection->addWidget(std::make_unique<Wt::WCheckBox>(
        " I am an international student requiring a visa"));
    internationalCheckbox_->addStyleClass("form-check");

    // Pre-populate fields if available from session
    if (session_) {
        const auto& student = session_->getStudent();

        // Personal Details
        emailInput_->setText(student.getEmail());
        firstNameInput_->setText(student.getFirstName());
        middleNameInput_->setText(student.getMiddleName());
        lastNameInput_->setText(student.getLastName());
        preferredNameInput_->setText(student.getPreferredName());
        pronounsInput_->setText(student.getPreferredPronouns());

        // Date of Birth
        auto dob = student.getDateOfBirth();
        if (dob != std::chrono::system_clock::time_point{}) {
            auto time = std::chrono::system_clock::to_time_t(dob);
            std::tm* tm = std::gmtime(&time);
            if (tm) {
                Wt::WDate date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
                if (date.isValid()) {
                    dateOfBirthInput_->setDate(date);
                }
            }
        }

        // Gender - find matching index
        std::string gender = student.getGender();
        if (!gender.empty()) {
            for (int i = 0; i < genderSelect_->count(); ++i) {
                if (genderSelect_->itemText(i).toUTF8() == gender) {
                    genderSelect_->setCurrentIndex(i);
                    break;
                }
            }
        }

        // Contact Information
        phoneInput_->setText(student.getPhoneNumber());
        altPhoneInput_->setText(student.getAlternatePhone());

        // Address
        addressLine1Input_->setText(student.getAddressLine1());
        addressLine2Input_->setText(student.getAddressLine2());
        cityInput_->setText(student.getCity());
        zipCodeInput_->setText(student.getZipCode());

        // State - find matching index
        std::string state = student.getState();
        if (!state.empty()) {
            for (int i = 0; i < stateSelect_->count(); ++i) {
                if (stateSelect_->itemText(i).toUTF8() == state) {
                    stateSelect_->setCurrentIndex(i);
                    break;
                }
            }
        }

        // Citizenship Information
        ssnInput_->setText(student.getSsn());

        // Citizenship Status - find matching index
        std::string citizenshipStatus = student.getCitizenshipStatus();
        if (!citizenshipStatus.empty()) {
            for (int i = 0; i < citizenshipSelect_->count(); ++i) {
                if (citizenshipSelect_->itemText(i).toUTF8() == citizenshipStatus) {
                    citizenshipSelect_->setCurrentIndex(i);
                    break;
                }
            }
        }

        // International checkbox
        internationalCheckbox_->setChecked(student.isInternational());
    }
}

void PersonalInfoForm::toggleMailingAddress() {
    if (sameAsHomeCheckbox_->isChecked()) {
        mailingAddressContainer_->hide();
    } else {
        mailingAddressContainer_->show();
    }
}

bool PersonalInfoForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    validateRequired(firstNameInput_->text().toUTF8(), "First name");
    validateRequired(lastNameInput_->text().toUTF8(), "Last name");

    std::string email = emailInput_->text().toUTF8();
    if (validateRequired(email, "Email")) {
        validateEmail(email);
    }

    std::string phone = phoneInput_->text().toUTF8();
    if (validateRequired(phone, "Phone number")) {
        validatePhone(phone);
    }

    if (!dateOfBirthInput_->date().isValid()) {
        validationErrors_.push_back("Please enter a valid date of birth");
        isValid_ = false;
    }

    validateRequired(addressLine1Input_->text().toUTF8(), "Address");
    validateRequired(cityInput_->text().toUTF8(), "City");

    if (stateSelect_->currentIndex() == 0) {
        validationErrors_.push_back("Please select a state");
        isValid_ = false;
    }

    std::string zip = zipCodeInput_->text().toUTF8();
    if (validateRequired(zip, "ZIP code")) {
        validateZipCode(zip);
    }

    if (citizenshipSelect_->currentIndex() == 0) {
        validationErrors_.push_back("Please select citizenship status");
        isValid_ = false;
    }

    // Validate SSN if provided
    std::string ssn = ssnInput_->text().toUTF8();
    if (!ssn.empty()) {
        validateSSN(ssn);
    }

    // Validate mailing address if different
    if (!sameAsHomeCheckbox_->isChecked()) {
        validateRequired(mailingAddressLine1Input_->text().toUTF8(), "Mailing address");
        validateRequired(mailingCityInput_->text().toUTF8(), "Mailing city");
        if (mailingStateSelect_->currentIndex() == 0) {
            validationErrors_.push_back("Please select mailing state");
            isValid_ = false;
        }
        std::string mailingZip = mailingZipCodeInput_->text().toUTF8();
        if (validateRequired(mailingZip, "Mailing ZIP code")) {
            validateZipCode(mailingZip);
        }
    }

    return isValid_;
}

void PersonalInfoForm::collectFormData(Models::FormData& data) const {
    data.setField("firstName", firstNameInput_->text().toUTF8());
    data.setField("middleName", middleNameInput_->text().toUTF8());
    data.setField("lastName", lastNameInput_->text().toUTF8());
    data.setField("preferredName", preferredNameInput_->text().toUTF8());
    data.setField("dateOfBirth", dateOfBirthInput_->date().toString("yyyy-MM-dd").toUTF8());
    data.setField("gender", genderSelect_->currentText().toUTF8());
    data.setField("pronouns", pronounsInput_->text().toUTF8());

    data.setField("email", emailInput_->text().toUTF8());
    data.setField("phone", phoneInput_->text().toUTF8());
    data.setField("altPhone", altPhoneInput_->text().toUTF8());

    data.setField("addressLine1", addressLine1Input_->text().toUTF8());
    data.setField("addressLine2", addressLine2Input_->text().toUTF8());
    data.setField("city", cityInput_->text().toUTF8());
    data.setField("state", stateSelect_->currentText().toUTF8());
    data.setField("zipCode", zipCodeInput_->text().toUTF8());
    data.setField("country", countrySelect_->currentText().toUTF8());

    data.setField("sameMailingAddress", sameAsHomeCheckbox_->isChecked());

    if (!sameAsHomeCheckbox_->isChecked()) {
        data.setField("mailingAddressLine1", mailingAddressLine1Input_->text().toUTF8());
        data.setField("mailingAddressLine2", mailingAddressLine2Input_->text().toUTF8());
        data.setField("mailingCity", mailingCityInput_->text().toUTF8());
        data.setField("mailingState", mailingStateSelect_->currentText().toUTF8());
        data.setField("mailingZipCode", mailingZipCodeInput_->text().toUTF8());
        data.setField("mailingCountry", mailingCountrySelect_->currentText().toUTF8());
    }

    data.setField("ssn", ssnInput_->text().toUTF8());
    data.setField("citizenshipStatus", citizenshipSelect_->currentText().toUTF8());
    data.setField("isInternational", internationalCheckbox_->isChecked());
}

void PersonalInfoForm::populateFormFields(const Models::FormData& data) {
    if (data.hasField("firstName"))
        firstNameInput_->setText(data.getField("firstName").stringValue);
    if (data.hasField("middleName"))
        middleNameInput_->setText(data.getField("middleName").stringValue);
    if (data.hasField("lastName"))
        lastNameInput_->setText(data.getField("lastName").stringValue);
    if (data.hasField("preferredName"))
        preferredNameInput_->setText(data.getField("preferredName").stringValue);
    if (data.hasField("pronouns"))
        pronounsInput_->setText(data.getField("pronouns").stringValue);

    if (data.hasField("email"))
        emailInput_->setText(data.getField("email").stringValue);
    if (data.hasField("phone"))
        phoneInput_->setText(data.getField("phone").stringValue);
    if (data.hasField("altPhone"))
        altPhoneInput_->setText(data.getField("altPhone").stringValue);

    if (data.hasField("addressLine1"))
        addressLine1Input_->setText(data.getField("addressLine1").stringValue);
    if (data.hasField("addressLine2"))
        addressLine2Input_->setText(data.getField("addressLine2").stringValue);
    if (data.hasField("city"))
        cityInput_->setText(data.getField("city").stringValue);
    if (data.hasField("zipCode"))
        zipCodeInput_->setText(data.getField("zipCode").stringValue);
    if (data.hasField("ssn"))
        ssnInput_->setText(data.getField("ssn").stringValue);

    if (data.hasField("sameMailingAddress")) {
        sameAsHomeCheckbox_->setChecked(data.getField("sameMailingAddress").boolValue);
        toggleMailingAddress();
    }

    if (data.hasField("isInternational")) {
        internationalCheckbox_->setChecked(data.getField("isInternational").boolValue);
    }
}

void PersonalInfoForm::updateStudentFromForm() {
    if (!session_) return;

    Models::Student& student = session_->getStudent();

    // Update all student fields from form inputs
    student.setFirstName(firstNameInput_->text().toUTF8());
    student.setMiddleName(middleNameInput_->text().toUTF8());
    student.setLastName(lastNameInput_->text().toUTF8());
    student.setPreferredName(preferredNameInput_->text().toUTF8());
    student.setEmail(emailInput_->text().toUTF8());
    student.setPhoneNumber(phoneInput_->text().toUTF8());
    student.setAlternatePhone(altPhoneInput_->text().toUTF8());
    student.setAddressLine1(addressLine1Input_->text().toUTF8());
    student.setAddressLine2(addressLine2Input_->text().toUTF8());
    student.setCity(cityInput_->text().toUTF8());
    student.setZipCode(zipCodeInput_->text().toUTF8());
    student.setSsn(ssnInput_->text().toUTF8());
    student.setPreferredPronouns(pronounsInput_->text().toUTF8());
    student.setInternational(internationalCheckbox_->isChecked());

    // Handle gender (skip "Select...")
    if (genderSelect_->currentIndex() > 0) {
        student.setGender(genderSelect_->currentText().toUTF8());
    }

    // Handle state (skip "Select...")
    if (stateSelect_->currentIndex() > 0) {
        student.setState(stateSelect_->currentText().toUTF8());
    }

    // Handle citizenship status (skip "Select...")
    if (citizenshipSelect_->currentIndex() > 0) {
        student.setCitizenshipStatus(citizenshipSelect_->currentText().toUTF8());
    }

    // Handle date of birth
    if (dateOfBirthInput_->date().isValid()) {
        Wt::WDate date = dateOfBirthInput_->date();
        std::tm tm = {};
        tm.tm_year = date.year() - 1900;
        tm.tm_mon = date.month() - 1;
        tm.tm_mday = date.day();
        student.setDateOfBirth(std::chrono::system_clock::from_time_t(std::mktime(&tm)));
    }
}

void PersonalInfoForm::handleSubmit() {
    // First let base class validate
    clearErrors();
    if (!validate()) {
        showErrors(validationErrors_);
        return;
    }

    // Update the Student record with form data
    updateStudentFromForm();

    // Update student profile on server (save all personal info fields)
    if (apiService_ && session_) {
        apiService_->updateStudentProfile(session_->getStudent());
    }

    // Now proceed with form submission (skip validation since we already did it)
    isSubmitting_ = true;
    nextButton_->setEnabled(false);
    nextButton_->setText("Submitting...");

    Models::FormData data = getFormData();
    data.setStatus("submitted");

    // Save to session
    if (session_) {
        session_->setFormData(formId_, data);
    }

    // Submit form data to API
    if (apiService_ && session_) {
        Api::SubmissionResult result = apiService_->submitForm(
            session_->getStudent().getId(), formId_, data);

        if (result.success) {
            onSubmitSuccess(result);
        } else {
            onSubmitError(result);
        }
    } else {
        // No API service, just emit success
        onSubmitSuccess(Api::SubmissionResult{true, "", "Form data saved", {}, {}});
    }
}

std::vector<std::string> PersonalInfoForm::getUSStates() const {
    return {
        "Alabama", "Alaska", "Arizona", "Arkansas", "California", "Colorado",
        "Connecticut", "Delaware", "Florida", "Georgia", "Hawaii", "Idaho",
        "Illinois", "Indiana", "Iowa", "Kansas", "Kentucky", "Louisiana",
        "Maine", "Maryland", "Massachusetts", "Michigan", "Minnesota",
        "Mississippi", "Missouri", "Montana", "Nebraska", "Nevada",
        "New Hampshire", "New Jersey", "New Mexico", "New York",
        "North Carolina", "North Dakota", "Ohio", "Oklahoma", "Oregon",
        "Pennsylvania", "Rhode Island", "South Carolina", "South Dakota",
        "Tennessee", "Texas", "Utah", "Vermont", "Virginia", "Washington",
        "West Virginia", "Wisconsin", "Wyoming", "District of Columbia"
    };
}

std::vector<std::string> PersonalInfoForm::getCountries() const {
    return {
        "United States", "Canada", "Mexico", "United Kingdom", "Germany",
        "France", "Japan", "China", "India", "Brazil", "Australia",
        "South Korea", "Italy", "Spain", "Netherlands", "Switzerland",
        "Sweden", "Norway", "Denmark", "Finland", "Other"
    };
}

} // namespace Forms
} // namespace StudentIntake
