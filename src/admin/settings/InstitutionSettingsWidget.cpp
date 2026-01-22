#include "InstitutionSettingsWidget.h"
#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

InstitutionSettingsWidget::InstitutionSettingsWidget()
    : apiService_(nullptr)
    , institutionNameEdit_(nullptr)
    , taglineEdit_(nullptr)
    , logoUrlEdit_(nullptr)
    , contactEmailEdit_(nullptr)
    , contactPhoneEdit_(nullptr)
    , contactFaxEdit_(nullptr)
    , websiteUrlEdit_(nullptr)
    , addressStreet1Edit_(nullptr)
    , addressStreet2Edit_(nullptr)
    , addressCityEdit_(nullptr)
    , addressStateEdit_(nullptr)
    , addressPostalCodeEdit_(nullptr)
    , addressCountryEdit_(nullptr)
    , academicYearEdit_(nullptr)
    , accreditationEdit_(nullptr)
    , messageContainer_(nullptr)
    , saveButton_(nullptr) {
    addStyleClass("institution-settings-widget");
    createUI();
}

void InstitutionSettingsWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> service) {
    apiService_ = service;
}

void InstitutionSettingsWidget::createUI() {
    // Header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("settings-header");

    auto title = header->addWidget(std::make_unique<Wt::WText>("<h2>Institution Settings</h2>"));
    title->setTextFormat(Wt::TextFormat::XHTML);

    auto description = header->addWidget(std::make_unique<Wt::WText>(
        "<p class='text-muted'>Configure your institution's name, contact information, and branding for student forms and documents.</p>"));
    description->setTextFormat(Wt::TextFormat::XHTML);

    // Message container for success/error messages
    messageContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    messageContainer_->addStyleClass("settings-message");
    messageContainer_->hide();

    // Main form container
    auto formContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer->addStyleClass("settings-form");

    // Create sections
    createBrandingSection();
    createContactSection();
    createLocationSection();

    // Save button
    auto buttonContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("settings-actions");

    saveButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Save Settings"));
    saveButton_->addStyleClass("btn btn-primary btn-lg");
    saveButton_->clicked().connect(this, &InstitutionSettingsWidget::saveSettings);
}

void InstitutionSettingsWidget::createBrandingSection() {
    auto section = addWidget(std::make_unique<Wt::WContainerWidget>());
    section->addStyleClass("settings-section");

    auto sectionTitle = section->addWidget(std::make_unique<Wt::WText>("<h3>Branding</h3>"));
    sectionTitle->setTextFormat(Wt::TextFormat::XHTML);

    // Institution Name
    auto nameGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group");
    auto nameLabel = nameGroup->addWidget(std::make_unique<Wt::WLabel>("Institution Name *"));
    institutionNameEdit_ = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    institutionNameEdit_->addStyleClass("form-control");
    institutionNameEdit_->setPlaceholderText("e.g., Springfield Technical College");
    nameLabel->setBuddy(institutionNameEdit_);

    // Tagline
    auto taglineGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    taglineGroup->addStyleClass("form-group");
    auto taglineLabel = taglineGroup->addWidget(std::make_unique<Wt::WLabel>("Tagline / Subtitle"));
    taglineEdit_ = taglineGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    taglineEdit_->addStyleClass("form-control");
    taglineEdit_->setPlaceholderText("e.g., Official Student Records");
    taglineLabel->setBuddy(taglineEdit_);

    // Logo URL
    auto logoGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    logoGroup->addStyleClass("form-group");
    auto logoLabel = logoGroup->addWidget(std::make_unique<Wt::WLabel>("Logo URL"));
    logoUrlEdit_ = logoGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    logoUrlEdit_->addStyleClass("form-control");
    logoUrlEdit_->setPlaceholderText("https://example.com/logo.png");
    logoLabel->setBuddy(logoUrlEdit_);
    auto logoHelp = logoGroup->addWidget(std::make_unique<Wt::WText>(
        "<small class='form-text text-muted'>Enter the full URL to your institution's logo image</small>"));
    logoHelp->setTextFormat(Wt::TextFormat::XHTML);

    // Academic Year
    auto yearGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    yearGroup->addStyleClass("form-group");
    auto yearLabel = yearGroup->addWidget(std::make_unique<Wt::WLabel>("Current Academic Year"));
    academicYearEdit_ = yearGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    academicYearEdit_->addStyleClass("form-control");
    academicYearEdit_->setPlaceholderText("e.g., 2024-2025");
    yearLabel->setBuddy(academicYearEdit_);

    // Accreditation
    auto accredGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    accredGroup->addStyleClass("form-group");
    auto accredLabel = accredGroup->addWidget(std::make_unique<Wt::WLabel>("Accreditation Information"));
    accreditationEdit_ = accredGroup->addWidget(std::make_unique<Wt::WTextArea>());
    accreditationEdit_->addStyleClass("form-control");
    accreditationEdit_->setRows(3);
    accreditationEdit_->setPlaceholderText("Enter accreditation details...");
    accredLabel->setBuddy(accreditationEdit_);
}

void InstitutionSettingsWidget::createContactSection() {
    auto section = addWidget(std::make_unique<Wt::WContainerWidget>());
    section->addStyleClass("settings-section");

    auto sectionTitle = section->addWidget(std::make_unique<Wt::WText>("<h3>Contact Information</h3>"));
    sectionTitle->setTextFormat(Wt::TextFormat::XHTML);

    // Two-column layout
    auto row = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    row->addStyleClass("settings-row");

    // Left column
    auto leftCol = row->addWidget(std::make_unique<Wt::WContainerWidget>());
    leftCol->addStyleClass("settings-col");

    // Contact Email
    auto emailGroup = leftCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    auto emailLabel = emailGroup->addWidget(std::make_unique<Wt::WLabel>("Contact Email"));
    contactEmailEdit_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    contactEmailEdit_->addStyleClass("form-control");
    contactEmailEdit_->setPlaceholderText("admissions@example.edu");
    emailLabel->setBuddy(contactEmailEdit_);

    // Website
    auto websiteGroup = leftCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    websiteGroup->addStyleClass("form-group");
    auto websiteLabel = websiteGroup->addWidget(std::make_unique<Wt::WLabel>("Website URL"));
    websiteUrlEdit_ = websiteGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    websiteUrlEdit_->addStyleClass("form-control");
    websiteUrlEdit_->setPlaceholderText("https://www.example.edu");
    websiteLabel->setBuddy(websiteUrlEdit_);

    // Right column
    auto rightCol = row->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightCol->addStyleClass("settings-col");

    // Contact Phone
    auto phoneGroup = rightCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneGroup->addStyleClass("form-group");
    auto phoneLabel = phoneGroup->addWidget(std::make_unique<Wt::WLabel>("Phone Number"));
    contactPhoneEdit_ = phoneGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    contactPhoneEdit_->addStyleClass("form-control");
    contactPhoneEdit_->setPlaceholderText("(555) 123-4567");
    phoneLabel->setBuddy(contactPhoneEdit_);

    // Fax
    auto faxGroup = rightCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    faxGroup->addStyleClass("form-group");
    auto faxLabel = faxGroup->addWidget(std::make_unique<Wt::WLabel>("Fax Number"));
    contactFaxEdit_ = faxGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    contactFaxEdit_->addStyleClass("form-control");
    contactFaxEdit_->setPlaceholderText("(555) 123-4568");
    faxLabel->setBuddy(contactFaxEdit_);
}

void InstitutionSettingsWidget::createLocationSection() {
    auto section = addWidget(std::make_unique<Wt::WContainerWidget>());
    section->addStyleClass("settings-section");

    auto sectionTitle = section->addWidget(std::make_unique<Wt::WText>("<h3>Address / Location</h3>"));
    sectionTitle->setTextFormat(Wt::TextFormat::XHTML);

    // Street Address 1
    auto street1Group = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    street1Group->addStyleClass("form-group");
    auto street1Label = street1Group->addWidget(std::make_unique<Wt::WLabel>("Street Address"));
    addressStreet1Edit_ = street1Group->addWidget(std::make_unique<Wt::WLineEdit>());
    addressStreet1Edit_->addStyleClass("form-control");
    addressStreet1Edit_->setPlaceholderText("123 College Drive");
    street1Label->setBuddy(addressStreet1Edit_);

    // Street Address 2
    auto street2Group = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    street2Group->addStyleClass("form-group");
    auto street2Label = street2Group->addWidget(std::make_unique<Wt::WLabel>("Street Address 2"));
    addressStreet2Edit_ = street2Group->addWidget(std::make_unique<Wt::WLineEdit>());
    addressStreet2Edit_->addStyleClass("form-control");
    addressStreet2Edit_->setPlaceholderText("Suite 100, Building A");
    street2Label->setBuddy(addressStreet2Edit_);

    // City, State, Postal Code row
    auto cityRow = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityRow->addStyleClass("settings-row settings-row-3");

    // City
    auto cityCol = cityRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityCol->addStyleClass("settings-col");
    auto cityGroup = cityCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    cityGroup->addStyleClass("form-group");
    auto cityLabel = cityGroup->addWidget(std::make_unique<Wt::WLabel>("City"));
    addressCityEdit_ = cityGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    addressCityEdit_->addStyleClass("form-control");
    addressCityEdit_->setPlaceholderText("Springfield");
    cityLabel->setBuddy(addressCityEdit_);

    // State
    auto stateCol = cityRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    stateCol->addStyleClass("settings-col");
    auto stateGroup = stateCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    stateGroup->addStyleClass("form-group");
    auto stateLabel = stateGroup->addWidget(std::make_unique<Wt::WLabel>("State / Province"));
    addressStateEdit_ = stateGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    addressStateEdit_->addStyleClass("form-control");
    addressStateEdit_->setPlaceholderText("IL");
    stateLabel->setBuddy(addressStateEdit_);

    // Postal Code
    auto postalCol = cityRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    postalCol->addStyleClass("settings-col");
    auto postalGroup = postalCol->addWidget(std::make_unique<Wt::WContainerWidget>());
    postalGroup->addStyleClass("form-group");
    auto postalLabel = postalGroup->addWidget(std::make_unique<Wt::WLabel>("Postal Code"));
    addressPostalCodeEdit_ = postalGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    addressPostalCodeEdit_->addStyleClass("form-control");
    addressPostalCodeEdit_->setPlaceholderText("62701");
    postalLabel->setBuddy(addressPostalCodeEdit_);

    // Country
    auto countryGroup = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    countryGroup->addStyleClass("form-group");
    auto countryLabel = countryGroup->addWidget(std::make_unique<Wt::WLabel>("Country"));
    addressCountryEdit_ = countryGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    addressCountryEdit_->addStyleClass("form-control");
    addressCountryEdit_->setPlaceholderText("United States");
    countryLabel->setBuddy(addressCountryEdit_);
}

void InstitutionSettingsWidget::loadSettings() {
    if (!apiService_) {
        LOG_WARN("InstitutionSettingsWidget", "No API service available");
        return;
    }

    LOG_INFO("InstitutionSettingsWidget", "Loading institution settings...");

    settings_ = apiService_->getInstitutionSettings();

    // Populate form fields
    institutionNameEdit_->setText(settings_.getInstitutionName());
    taglineEdit_->setText(settings_.getTagline());
    logoUrlEdit_->setText(settings_.getLogoUrl());

    contactEmailEdit_->setText(settings_.getContactEmail());
    contactPhoneEdit_->setText(settings_.getContactPhone());
    contactFaxEdit_->setText(settings_.getContactFax());
    websiteUrlEdit_->setText(settings_.getWebsiteUrl());

    addressStreet1Edit_->setText(settings_.getAddressStreet1());
    addressStreet2Edit_->setText(settings_.getAddressStreet2());
    addressCityEdit_->setText(settings_.getAddressCity());
    addressStateEdit_->setText(settings_.getAddressState());
    addressPostalCodeEdit_->setText(settings_.getAddressPostalCode());
    addressCountryEdit_->setText(settings_.getAddressCountry());

    academicYearEdit_->setText(settings_.getAcademicYear());
    accreditationEdit_->setText(settings_.getAccreditationInfo());

    LOG_INFO("InstitutionSettingsWidget", "Settings loaded: " << settings_.getInstitutionName());
}

void InstitutionSettingsWidget::saveSettings() {
    if (!apiService_) {
        showMessage("Error: API service not available", true);
        return;
    }

    LOG_INFO("InstitutionSettingsWidget", "Saving institution settings...");

    // Update settings from form fields
    settings_.setInstitutionName(institutionNameEdit_->text().toUTF8());
    settings_.setTagline(taglineEdit_->text().toUTF8());
    settings_.setLogoUrl(logoUrlEdit_->text().toUTF8());

    settings_.setContactEmail(contactEmailEdit_->text().toUTF8());
    settings_.setContactPhone(contactPhoneEdit_->text().toUTF8());
    settings_.setContactFax(contactFaxEdit_->text().toUTF8());
    settings_.setWebsiteUrl(websiteUrlEdit_->text().toUTF8());

    settings_.setAddressStreet1(addressStreet1Edit_->text().toUTF8());
    settings_.setAddressStreet2(addressStreet2Edit_->text().toUTF8());
    settings_.setAddressCity(addressCityEdit_->text().toUTF8());
    settings_.setAddressState(addressStateEdit_->text().toUTF8());
    settings_.setAddressPostalCode(addressPostalCodeEdit_->text().toUTF8());
    settings_.setAddressCountry(addressCountryEdit_->text().toUTF8());

    settings_.setAcademicYear(academicYearEdit_->text().toUTF8());
    settings_.setAccreditationInfo(accreditationEdit_->text().toUTF8());

    // Save to API
    auto result = apiService_->updateInstitutionSettings(settings_);

    if (result.success) {
        showMessage("Settings saved successfully!", false);
    } else {
        showMessage("Error saving settings: " + result.message, true);
    }
}

void InstitutionSettingsWidget::showMessage(const std::string& message, bool isError) {
    messageContainer_->clear();

    std::string cssClass = isError ? "alert alert-danger" : "alert alert-success";
    messageContainer_->addStyleClass(cssClass);

    auto msgText = messageContainer_->addWidget(std::make_unique<Wt::WText>(message));
    messageContainer_->show();

    // Auto-hide success messages after 5 seconds
    if (!isError) {
        Wt::WApplication::instance()->doJavaScript(
            "setTimeout(function() { "
            "  var el = document.querySelector('.settings-message');"
            "  if (el) el.style.display = 'none';"
            "}, 5000);");
    }
}

} // namespace Admin
} // namespace StudentIntake
