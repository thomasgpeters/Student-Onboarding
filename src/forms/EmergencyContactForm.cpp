#include "EmergencyContactForm.h"
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>

namespace StudentIntake {
namespace Forms {

EmergencyContactForm::EmergencyContactForm()
    : BaseForm("emergency_contact", "Emergency Contacts")
    , contactsContainer_(nullptr)
    , addContactButton_(nullptr) {
}

EmergencyContactForm::~EmergencyContactForm() {
}

void EmergencyContactForm::createFormFields() {
    auto intro = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    intro->addStyleClass("form-intro");
    auto introText = intro->addWidget(std::make_unique<Wt::WText>(
        "<p>Please provide at least one emergency contact. We recommend adding two contacts.</p>"));
    introText->setTextFormat(Wt::TextFormat::XHTML);

    contactsContainer_ = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contactsContainer_->addStyleClass("contacts-container");

    // Add first contact by default
    addContactSection(1);

    // Add contact button
    auto buttonContainer = formFieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("add-contact-container");

    addContactButton_ = buttonContainer->addWidget(
        std::make_unique<Wt::WPushButton>("+ Add Another Contact"));
    addContactButton_->addStyleClass("btn btn-outline-secondary");
    addContactButton_->clicked().connect([this]() {
        if (contacts_.size() < 10) {
            addContactSection(static_cast<int>(contacts_.size()) + 1);
        }
        if (contacts_.size() >= 10) {
            addContactButton_->setEnabled(false);
        }
    });

    // Load existing contacts from API
    loadContactsFromApi();
}

void EmergencyContactForm::addContactSection(int contactNumber) {
    ContactFields fields;

    fields.container = contactsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fields.container->addStyleClass("contact-section form-section");

    // Header with remove button
    auto header = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("contact-header");
    auto contactHeader = header->addWidget(std::make_unique<Wt::WText>(
        "<h4>Emergency Contact " + std::to_string(contactNumber) + "</h4>"));
    contactHeader->setTextFormat(Wt::TextFormat::XHTML);

    if (contactNumber > 1) {
        auto removeBtn = header->addWidget(std::make_unique<Wt::WPushButton>("Remove"));
        removeBtn->addStyleClass("btn btn-link btn-sm text-danger");
        int index = contactNumber - 1;
        removeBtn->clicked().connect([this, index]() {
            removeContact(index);
        });
    }

    // First Name, Last Name, and relationship row
    auto row1 = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    row1->addStyleClass("form-row");

    auto firstNameGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    firstNameGroup->addStyleClass("form-group col-md-4");
    firstNameGroup->addWidget(std::make_unique<Wt::WLabel>("First Name *"));
    fields.firstNameInput = firstNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.firstNameInput->setPlaceholderText("First name");
    fields.firstNameInput->addStyleClass("form-control");

    auto lastNameGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    lastNameGroup->addStyleClass("form-group col-md-4");
    lastNameGroup->addWidget(std::make_unique<Wt::WLabel>("Last Name *"));
    fields.lastNameInput = lastNameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.lastNameInput->setPlaceholderText("Last name");
    fields.lastNameInput->addStyleClass("form-control");

    auto relationGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    relationGroup->addStyleClass("form-group col-md-4");
    relationGroup->addWidget(std::make_unique<Wt::WLabel>("Relationship *"));
    fields.relationshipSelect = relationGroup->addWidget(std::make_unique<Wt::WComboBox>());
    fields.relationshipSelect->addStyleClass("form-control");
    fields.relationshipSelect->addItem("Select...");
    for (const auto& rel : getRelationships()) {
        fields.relationshipSelect->addItem(rel);
    }

    // Phone numbers row
    auto row2 = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    row2->addStyleClass("form-row");

    auto phoneGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneGroup->addStyleClass("form-group col-md-4");
    phoneGroup->addWidget(std::make_unique<Wt::WLabel>("Primary Phone *"));
    fields.phoneInput = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.phoneInput->setPlaceholderText("(555) 555-5555");
    fields.phoneInput->addStyleClass("form-control");

    auto altPhoneGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    altPhoneGroup->addStyleClass("form-group col-md-4");
    altPhoneGroup->addWidget(std::make_unique<Wt::WLabel>("Alternate Phone"));
    fields.altPhoneInput = altPhoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.altPhoneInput->setPlaceholderText("(555) 555-5555");
    fields.altPhoneInput->addStyleClass("form-control");

    auto emailGroup = row2->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group col-md-4");
    emailGroup->addWidget(std::make_unique<Wt::WLabel>("Email"));
    fields.emailInput = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.emailInput->setPlaceholderText("email@example.com");
    fields.emailInput->addStyleClass("form-control");

    // Address
    auto addrGroup = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    addrGroup->addStyleClass("form-group");
    addrGroup->addWidget(std::make_unique<Wt::WLabel>("Address"));
    fields.addressInput = addrGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.addressInput->setPlaceholderText("Street address");
    fields.addressInput->addStyleClass("form-control");

    // City, State, ZIP row
    auto row3 = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    row3->addStyleClass("form-row");

    auto cityGroup = row3->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityGroup->addStyleClass("form-group col-md-5");
    cityGroup->addWidget(std::make_unique<Wt::WLabel>("City"));
    fields.cityInput = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.cityInput->addStyleClass("form-control");

    auto stateGroup = row3->addWidget(std::make_unique<Wt::WContainerWidget>());
    stateGroup->addStyleClass("form-group col-md-4");
    stateGroup->addWidget(std::make_unique<Wt::WLabel>("State"));
    fields.stateSelect = stateGroup->addWidget(std::make_unique<Wt::WComboBox>());
    fields.stateSelect->addStyleClass("form-control");
    fields.stateSelect->addItem("Select...");
    for (const auto& state : getUSStates()) {
        fields.stateSelect->addItem(state);
    }

    auto zipGroup = row3->addWidget(std::make_unique<Wt::WContainerWidget>());
    zipGroup->addStyleClass("form-group col-md-3");
    zipGroup->addWidget(std::make_unique<Wt::WLabel>("ZIP Code"));
    fields.zipCodeInput = zipGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.zipCodeInput->setPlaceholderText("12345");
    fields.zipCodeInput->addStyleClass("form-control");

    contacts_.push_back(fields);
}

void EmergencyContactForm::removeContact(int index) {
    if (index > 0 && index < static_cast<int>(contacts_.size())) {
        // Delete from database if this contact has an ID
        if (index < static_cast<int>(contactIds_.size())) {
            if (!contactIds_[index].empty() && apiService_) {
                std::cout << "[EmergencyContactForm] Deleting contact ID: " << contactIds_[index] << std::endl;
                auto result = apiService_->deleteEmergencyContact(contactIds_[index]);
                if (!result.success) {
                    std::cout << "[EmergencyContactForm] Failed to delete contact: " << result.message << std::endl;
                }
            }
            // Keep contactIds_ in sync with contacts_
            contactIds_.erase(contactIds_.begin() + index);
        }

        contactsContainer_->removeWidget(contacts_[index].container);
        contacts_.erase(contacts_.begin() + index);

        // Renumber remaining contacts
        for (size_t i = 0; i < contacts_.size(); ++i) {
            // Update header text would require more complex handling
        }

        if (contacts_.size() < 10) {
            addContactButton_->setEnabled(true);
        }
    }
}

bool EmergencyContactForm::validate() {
    validationErrors_.clear();
    isValid_ = true;

    if (contacts_.empty()) {
        validationErrors_.push_back("At least one emergency contact is required");
        isValid_ = false;
        return isValid_;
    }

    // Validate first contact (required)
    const auto& primary = contacts_[0];
    validateRequired(primary.firstNameInput->text().toUTF8(), "Contact first name");
    validateRequired(primary.lastNameInput->text().toUTF8(), "Contact last name");

    if (primary.relationshipSelect->currentIndex() == 0) {
        validationErrors_.push_back("Please select relationship for primary contact");
        isValid_ = false;
    }

    std::string phone = primary.phoneInput->text().toUTF8();
    if (validateRequired(phone, "Contact phone")) {
        validatePhone(phone);
    }

    // Validate additional contacts (if any)
    for (size_t i = 1; i < contacts_.size(); ++i) {
        const auto& contact = contacts_[i];
        std::string firstName = contact.firstNameInput->text().toUTF8();
        std::string lastName = contact.lastNameInput->text().toUTF8();

        // If any field is filled, require first name, last name, and phone
        if (!firstName.empty() || !lastName.empty() || contact.phoneInput->text().toUTF8().length() > 0) {
            if (firstName.empty()) {
                validationErrors_.push_back("Contact " + std::to_string(i + 1) + " first name is required");
                isValid_ = false;
            }
            if (lastName.empty()) {
                validationErrors_.push_back("Contact " + std::to_string(i + 1) + " last name is required");
                isValid_ = false;
            }
            std::string contactPhone = contact.phoneInput->text().toUTF8();
            if (contactPhone.empty()) {
                validationErrors_.push_back("Contact " + std::to_string(i + 1) + " phone is required");
                isValid_ = false;
            }
        }
    }

    // Validate relationship limits (Spouse: max 1, Parent: max 2, Grandparent: max 4)
    if (!validateRelationshipLimits()) {
        isValid_ = false;
    }

    return isValid_;
}

void EmergencyContactForm::collectFormData(Models::FormData& data) const {
    data.setField("contactCount", static_cast<int>(contacts_.size()));

    for (size_t i = 0; i < contacts_.size(); ++i) {
        std::string prefix = "contact" + std::to_string(i + 1) + "_";
        const auto& contact = contacts_[i];

        data.setField(prefix + "FirstName", contact.firstNameInput->text().toUTF8());
        data.setField(prefix + "LastName", contact.lastNameInput->text().toUTF8());
        data.setField(prefix + "Relationship", contact.relationshipSelect->currentText().toUTF8());
        data.setField(prefix + "Phone", contact.phoneInput->text().toUTF8());
        data.setField(prefix + "AlternatePhone", contact.altPhoneInput->text().toUTF8());
        data.setField(prefix + "Email", contact.emailInput->text().toUTF8());
        data.setField(prefix + "AddressLine1", contact.addressInput->text().toUTF8());
        data.setField(prefix + "City", contact.cityInput->text().toUTF8());
        data.setField(prefix + "State", contact.stateSelect->currentText().toUTF8());
        data.setField(prefix + "ZipCode", contact.zipCodeInput->text().toUTF8());
    }
}

void EmergencyContactForm::populateFormFields(const Models::FormData& data) {
    if (!data.hasField("contactCount")) return;

    int count = data.getField("contactCount").intValue;

    // Add additional contact sections if needed
    while (static_cast<int>(contacts_.size()) < count) {
        addContactSection(static_cast<int>(contacts_.size()) + 1);
    }

    for (int i = 0; i < count && i < static_cast<int>(contacts_.size()); ++i) {
        std::string prefix = "contact" + std::to_string(i + 1) + "_";
        auto& contact = contacts_[i];

        if (data.hasField(prefix + "FirstName"))
            contact.firstNameInput->setText(data.getField(prefix + "FirstName").stringValue);
        if (data.hasField(prefix + "LastName"))
            contact.lastNameInput->setText(data.getField(prefix + "LastName").stringValue);
        if (data.hasField(prefix + "Phone"))
            contact.phoneInput->setText(data.getField(prefix + "Phone").stringValue);
        if (data.hasField(prefix + "AlternatePhone"))
            contact.altPhoneInput->setText(data.getField(prefix + "AlternatePhone").stringValue);
        if (data.hasField(prefix + "Email"))
            contact.emailInput->setText(data.getField(prefix + "Email").stringValue);
        if (data.hasField(prefix + "AddressLine1"))
            contact.addressInput->setText(data.getField(prefix + "AddressLine1").stringValue);
        if (data.hasField(prefix + "City"))
            contact.cityInput->setText(data.getField(prefix + "City").stringValue);
        if (data.hasField(prefix + "ZipCode"))
            contact.zipCodeInput->setText(data.getField(prefix + "ZipCode").stringValue);
    }
}

std::vector<std::string> EmergencyContactForm::getRelationships() const {
    return {
        "Parent", "Guardian", "Spouse", "Sibling", "Grandparent",
        "Aunt/Uncle", "Cousin", "Friend", "Employer", "Other"
    };
}

std::vector<std::string> EmergencyContactForm::getUSStates() const {
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

void EmergencyContactForm::loadContactsFromApi() {
    if (!apiService_ || !session_) return;

    std::string studentId = session_->getStudent().getId();
    if (studentId.empty()) return;

    // Load all emergency contacts for this student
    auto apiContacts = apiService_->getEmergencyContacts(studentId);

    if (apiContacts.empty()) return;

    // Clear contact IDs
    contactIds_.clear();

    // Add additional contact sections if needed
    while (contacts_.size() < apiContacts.size()) {
        addContactSection(static_cast<int>(contacts_.size()) + 1);
    }

    // Populate each contact
    for (size_t i = 0; i < apiContacts.size() && i < contacts_.size(); ++i) {
        contactIds_.push_back(apiContacts[i].getId());
        populateContactFields(static_cast<int>(i), apiContacts[i]);
    }

    // Ensure contactIds_ has same size as contacts_
    while (contactIds_.size() < contacts_.size()) {
        contactIds_.push_back("");
    }
}

void EmergencyContactForm::saveContactsToApi() {
    if (!apiService_ || !session_) return;

    std::string studentId = session_->getStudent().getId();
    if (studentId.empty()) return;

    // Ensure contactIds_ matches contacts_ size
    while (contactIds_.size() < contacts_.size()) {
        contactIds_.push_back("");
    }

    // Save each contact
    for (size_t i = 0; i < contacts_.size(); ++i) {
        Models::EmergencyContact contact = buildContactFromFields(static_cast<int>(i));
        contact.setStudentId(studentId);
        contact.setPrimary(i == 0);
        contact.setPriority(static_cast<int>(i) + 1);

        if (i < contactIds_.size() && !contactIds_[i].empty()) {
            contact.setId(contactIds_[i]);
        }

        if (!contact.isEmpty()) {
            Api::SubmissionResult result = apiService_->saveEmergencyContact(contact);
            if (result.success && contactIds_[i].empty()) {
                contactIds_[i] = result.submissionId;
            }
        }
    }
}

void EmergencyContactForm::populateContactFields(int index, const Models::EmergencyContact& contact) {
    if (index < 0 || index >= static_cast<int>(contacts_.size())) return;

    auto& fields = contacts_[index];

    fields.firstNameInput->setText(contact.getFirstName());
    fields.lastNameInput->setText(contact.getLastName());
    fields.phoneInput->setText(contact.getPhone());
    fields.altPhoneInput->setText(contact.getAlternatePhone());
    fields.emailInput->setText(contact.getEmail());
    fields.addressInput->setText(contact.getStreet1());
    fields.cityInput->setText(contact.getCity());
    fields.zipCodeInput->setText(contact.getPostalCode());

    // Set relationship if found
    std::string relationship = contact.getRelationship();
    if (!relationship.empty()) {
        for (int i = 0; i < fields.relationshipSelect->count(); ++i) {
            if (fields.relationshipSelect->itemText(i).toUTF8() == relationship) {
                fields.relationshipSelect->setCurrentIndex(i);
                break;
            }
        }
    }

    // Set state if found
    std::string state = contact.getState();
    if (!state.empty()) {
        for (int i = 0; i < fields.stateSelect->count(); ++i) {
            if (fields.stateSelect->itemText(i).toUTF8() == state) {
                fields.stateSelect->setCurrentIndex(i);
                break;
            }
        }
    }
}

Models::EmergencyContact EmergencyContactForm::buildContactFromFields(int index) const {
    Models::EmergencyContact contact;

    if (index < 0 || index >= static_cast<int>(contacts_.size())) return contact;

    const auto& fields = contacts_[index];

    contact.setFirstName(fields.firstNameInput->text().toUTF8());
    contact.setLastName(fields.lastNameInput->text().toUTF8());
    contact.setPhone(fields.phoneInput->text().toUTF8());
    contact.setAlternatePhone(fields.altPhoneInput->text().toUTF8());
    contact.setEmail(fields.emailInput->text().toUTF8());
    contact.setStreet1(fields.addressInput->text().toUTF8());
    contact.setCity(fields.cityInput->text().toUTF8());
    contact.setPostalCode(fields.zipCodeInput->text().toUTF8());

    // Get relationship if selected
    if (fields.relationshipSelect->currentIndex() > 0) {
        contact.setRelationship(fields.relationshipSelect->currentText().toUTF8());
    }

    // Get state if selected
    if (fields.stateSelect->currentIndex() > 0) {
        contact.setState(fields.stateSelect->currentText().toUTF8());
    }

    return contact;
}

void EmergencyContactForm::handleSubmit() {
    // First validate
    clearErrors();
    if (!validate()) {
        showErrors(validationErrors_);
        return;
    }

    // Save contacts to API
    saveContactsToApi();

    // Now proceed with form submission
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

std::map<std::string, int> EmergencyContactForm::countRelationshipsByType() const {
    std::map<std::string, int> counts;

    for (const auto& contact : contacts_) {
        if (contact.relationshipSelect->currentIndex() > 0) {
            std::string relationship = contact.relationshipSelect->currentText().toUTF8();
            counts[relationship]++;
        }
    }

    return counts;
}

bool EmergencyContactForm::validateRelationshipLimits() {
    bool valid = true;
    auto counts = countRelationshipsByType();

    // Check Spouse limit (0-1)
    if (counts.count("Spouse") && counts["Spouse"] > 1) {
        validationErrors_.push_back("Only one Spouse contact is allowed (found " +
                                    std::to_string(counts["Spouse"]) + ")");
        valid = false;
    }

    // Check Parent limit (0-2)
    if (counts.count("Parent") && counts["Parent"] > 2) {
        validationErrors_.push_back("Maximum of 2 Parent contacts allowed (found " +
                                    std::to_string(counts["Parent"]) + ")");
        valid = false;
    }

    // Check Grandparent limit (0-4)
    if (counts.count("Grandparent") && counts["Grandparent"] > 4) {
        validationErrors_.push_back("Maximum of 4 Grandparent contacts allowed (found " +
                                    std::to_string(counts["Grandparent"]) + ")");
        valid = false;
    }

    // No limits on other relationship types

    return valid;
}

} // namespace Forms
} // namespace StudentIntake
