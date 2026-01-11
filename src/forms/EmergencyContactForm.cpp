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
    intro->addWidget(std::make_unique<Wt::WText>(
        "<p>Please provide at least one emergency contact. We recommend adding two contacts.</p>"));

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
        if (contacts_.size() < 5) {
            addContactSection(static_cast<int>(contacts_.size()) + 1);
        }
        if (contacts_.size() >= 5) {
            addContactButton_->setEnabled(false);
        }
    });
}

void EmergencyContactForm::addContactSection(int contactNumber) {
    ContactFields fields;

    fields.container = contactsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fields.container->addStyleClass("contact-section form-section");

    // Header with remove button
    auto header = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("contact-header");
    header->addWidget(std::make_unique<Wt::WText>(
        "<h4>Emergency Contact " + std::to_string(contactNumber) + "</h4>"));

    if (contactNumber > 1) {
        auto removeBtn = header->addWidget(std::make_unique<Wt::WPushButton>("Remove"));
        removeBtn->addStyleClass("btn btn-link btn-sm text-danger");
        int index = contactNumber - 1;
        removeBtn->clicked().connect([this, index]() {
            removeContact(index);
        });
    }

    // Name and relationship row
    auto row1 = fields.container->addWidget(std::make_unique<Wt::WContainerWidget>());
    row1->addStyleClass("form-row");

    auto nameGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group col-md-6");
    nameGroup->addWidget(std::make_unique<Wt::WLabel>("Full Name *"));
    fields.nameInput = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    fields.nameInput->setPlaceholderText("Full name");
    fields.nameInput->addStyleClass("form-control");

    auto relationGroup = row1->addWidget(std::make_unique<Wt::WContainerWidget>());
    relationGroup->addStyleClass("form-group col-md-6");
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
        contactsContainer_->removeWidget(contacts_[index].container);
        contacts_.erase(contacts_.begin() + index);

        // Renumber remaining contacts
        for (size_t i = 0; i < contacts_.size(); ++i) {
            // Update header text would require more complex handling
        }

        if (contacts_.size() < 5) {
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
    validateRequired(primary.nameInput->text().toUTF8(), "Contact name");

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
        std::string name = contact.nameInput->text().toUTF8();

        // If any field is filled, require name and phone
        if (!name.empty() || contact.phoneInput->text().toUTF8().length() > 0) {
            if (name.empty()) {
                validationErrors_.push_back("Contact " + std::to_string(i + 1) + " name is required");
                isValid_ = false;
            }
            std::string contactPhone = contact.phoneInput->text().toUTF8();
            if (contactPhone.empty()) {
                validationErrors_.push_back("Contact " + std::to_string(i + 1) + " phone is required");
                isValid_ = false;
            }
        }
    }

    return isValid_;
}

void EmergencyContactForm::collectFormData(Models::FormData& data) const {
    data.setField("contactCount", static_cast<int>(contacts_.size()));

    for (size_t i = 0; i < contacts_.size(); ++i) {
        std::string prefix = "contact" + std::to_string(i + 1) + "_";
        const auto& contact = contacts_[i];

        data.setField(prefix + "name", contact.nameInput->text().toUTF8());
        data.setField(prefix + "relationship", contact.relationshipSelect->currentText().toUTF8());
        data.setField(prefix + "phone", contact.phoneInput->text().toUTF8());
        data.setField(prefix + "altPhone", contact.altPhoneInput->text().toUTF8());
        data.setField(prefix + "email", contact.emailInput->text().toUTF8());
        data.setField(prefix + "address", contact.addressInput->text().toUTF8());
        data.setField(prefix + "city", contact.cityInput->text().toUTF8());
        data.setField(prefix + "state", contact.stateSelect->currentText().toUTF8());
        data.setField(prefix + "zipCode", contact.zipCodeInput->text().toUTF8());
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

        if (data.hasField(prefix + "name"))
            contact.nameInput->setText(data.getField(prefix + "name").stringValue);
        if (data.hasField(prefix + "phone"))
            contact.phoneInput->setText(data.getField(prefix + "phone").stringValue);
        if (data.hasField(prefix + "altPhone"))
            contact.altPhoneInput->setText(data.getField(prefix + "altPhone").stringValue);
        if (data.hasField(prefix + "email"))
            contact.emailInput->setText(data.getField(prefix + "email").stringValue);
        if (data.hasField(prefix + "address"))
            contact.addressInput->setText(data.getField(prefix + "address").stringValue);
        if (data.hasField(prefix + "city"))
            contact.cityInput->setText(data.getField(prefix + "city").stringValue);
        if (data.hasField(prefix + "zipCode"))
            contact.zipCodeInput->setText(data.getField(prefix + "zipCode").stringValue);
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

} // namespace Forms
} // namespace StudentIntake
