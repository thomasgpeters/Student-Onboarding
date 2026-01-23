#include "InstitutionSettingsWidget.h"
#include <Wt/WApplication.h>
#include <Wt/WTemplate.h>
#include <Wt/WGroupBox.h>
#include <Wt/WLabel.h>
#include <Wt/WBreak.h>
#include <Wt/WTable.h>
#include <Wt/WDialog.h>
#include <Wt/WMessageBox.h>
#include <nlohmann/json.hpp>
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
    , saveButton_(nullptr)
    , departmentTableContainer_(nullptr)
    , departmentTable_(nullptr)
    , departmentDialog_(nullptr)
    , dialogDeptCodeEdit_(nullptr)
    , dialogDeptNameEdit_(nullptr)
    , dialogDeptDeanEdit_(nullptr)
    , dialogDeptEmailEdit_(nullptr) {
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
    createLookupDataSection();

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

    // Load lookup data
    loadDepartments();
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

void InstitutionSettingsWidget::createLookupDataSection() {
    auto section = addWidget(std::make_unique<Wt::WContainerWidget>());
    section->addStyleClass("settings-section lookup-data-section");

    auto sectionTitle = section->addWidget(std::make_unique<Wt::WText>("<h3>Lookup Data Management</h3>"));
    sectionTitle->setTextFormat(Wt::TextFormat::XHTML);

    auto sectionDesc = section->addWidget(std::make_unique<Wt::WText>(
        "<p class='text-muted'>Manage departments and other reference data used throughout the application.</p>"));
    sectionDesc->setTextFormat(Wt::TextFormat::XHTML);

    // Department subsection
    auto deptSubsection = section->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptSubsection->addStyleClass("lookup-subsection");

    auto deptHeader = deptSubsection->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptHeader->addStyleClass("lookup-header");

    auto deptTitle = deptHeader->addWidget(std::make_unique<Wt::WText>("<h4>Departments</h4>"));
    deptTitle->setTextFormat(Wt::TextFormat::XHTML);

    auto addDeptBtn = deptHeader->addWidget(std::make_unique<Wt::WPushButton>("+ Add Department"));
    addDeptBtn->addStyleClass("btn btn-primary btn-sm");
    addDeptBtn->clicked().connect(this, &InstitutionSettingsWidget::showAddDepartmentDialog);

    // Department table container
    departmentTableContainer_ = deptSubsection->addWidget(std::make_unique<Wt::WContainerWidget>());
    departmentTableContainer_->addStyleClass("admin-table-container");

    // Create the table
    departmentTable_ = departmentTableContainer_->addWidget(std::make_unique<Wt::WTable>());
    departmentTable_->addStyleClass("admin-data-table");
    departmentTable_->setHeaderCount(1);

    // Table headers
    departmentTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Code"));
    departmentTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Name"));
    departmentTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Director"));
    departmentTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Contact Email"));
    departmentTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));

    // Style header cells
    for (int col = 0; col < 5; ++col) {
        departmentTable_->elementAt(0, col)->addStyleClass("admin-table-header");
    }
}

void InstitutionSettingsWidget::loadDepartments() {
    if (!apiService_) return;

    departments_.clear();

    try {
        auto response = apiService_->getApiClient()->get("/Department");
        if (response.success) {
            auto jsonResponse = nlohmann::json::parse(response.body);

            nlohmann::json items;
            if (jsonResponse.is_array()) {
                items = jsonResponse;
            } else if (jsonResponse.contains("data")) {
                items = jsonResponse["data"];
            }

            for (const auto& item : items) {
                nlohmann::json attrs = item.contains("attributes") ? item["attributes"] : item;

                DepartmentData dept;
                // Get ID
                if (item.contains("id")) {
                    if (item["id"].is_number()) {
                        dept.id = item["id"].get<int>();
                    } else if (item["id"].is_string()) {
                        dept.id = std::stoi(item["id"].get<std::string>());
                    }
                }
                // Get code
                if (attrs.contains("code") && !attrs["code"].is_null()) {
                    dept.code = attrs["code"].get<std::string>();
                }
                // Get name
                if (attrs.contains("name") && !attrs["name"].is_null()) {
                    dept.name = attrs["name"].get<std::string>();
                }
                // Get dean
                if (attrs.contains("dean") && !attrs["dean"].is_null()) {
                    dept.dean = attrs["dean"].get<std::string>();
                }
                // Get contact_email
                if (attrs.contains("contact_email") && !attrs["contact_email"].is_null()) {
                    dept.contactEmail = attrs["contact_email"].get<std::string>();
                }

                if (dept.id > 0) {
                    departments_.push_back(dept);
                }
            }

            LOG_DEBUG("InstitutionSettingsWidget", "Loaded " << departments_.size() << " departments");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("InstitutionSettingsWidget", "Error loading departments: " << e.what());
    }

    refreshDepartmentTable();
}

void InstitutionSettingsWidget::refreshDepartmentTable() {
    if (!departmentTable_) return;

    // Clear existing rows (keep header)
    while (departmentTable_->rowCount() > 1) {
        departmentTable_->removeRow(departmentTable_->rowCount() - 1);
    }

    if (departments_.empty()) {
        int row = departmentTable_->rowCount();
        auto cell = departmentTable_->elementAt(row, 0);
        cell->setColumnSpan(5);
        cell->addStyleClass("admin-table-cell text-center text-muted");
        cell->addWidget(std::make_unique<Wt::WText>("No departments found. Click 'Add Department' to create one."));
        return;
    }

    // Add department rows
    for (const auto& dept : departments_) {
        int row = departmentTable_->rowCount();

        auto codeCell = departmentTable_->elementAt(row, 0);
        codeCell->addStyleClass("admin-table-cell");
        codeCell->addWidget(std::make_unique<Wt::WText>(dept.code));

        auto nameCell = departmentTable_->elementAt(row, 1);
        nameCell->addStyleClass("admin-table-cell");
        nameCell->addWidget(std::make_unique<Wt::WText>(dept.name));

        auto deanCell = departmentTable_->elementAt(row, 2);
        deanCell->addStyleClass("admin-table-cell");
        deanCell->addWidget(std::make_unique<Wt::WText>(dept.dean));

        auto emailCell = departmentTable_->elementAt(row, 3);
        emailCell->addStyleClass("admin-table-cell");
        emailCell->addWidget(std::make_unique<Wt::WText>(dept.contactEmail));

        // Actions cell
        auto actionsCell = departmentTable_->elementAt(row, 4);
        actionsCell->addStyleClass("admin-table-cell admin-table-actions");

        auto editBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
        editBtn->addStyleClass("btn btn-sm btn-outline-primary");
        int deptId = dept.id;
        editBtn->clicked().connect([this, deptId]() {
            showEditDepartmentDialog(deptId);
        });

        auto deleteBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
        deleteBtn->addStyleClass("btn btn-sm btn-outline-danger");
        std::string deptName = dept.name;
        deleteBtn->clicked().connect([this, deptId, deptName]() {
            confirmDeleteDepartment(deptId, deptName);
        });
    }
}

void InstitutionSettingsWidget::showAddDepartmentDialog() {
    departmentDialog_ = addChild(std::make_unique<Wt::WDialog>("Add Department"));
    departmentDialog_->setModal(true);
    departmentDialog_->setClosable(true);
    departmentDialog_->addStyleClass("admin-dialog");
    departmentDialog_->setWidth(Wt::WLength(450));

    auto content = departmentDialog_->contents();
    content->addStyleClass("admin-dialog-content");

    // Code field
    auto codeGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    codeGroup->addStyleClass("form-group");
    codeGroup->addWidget(std::make_unique<Wt::WText>("Code *"))->addStyleClass("form-label");
    dialogDeptCodeEdit_ = codeGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptCodeEdit_->addStyleClass("form-control");
    dialogDeptCodeEdit_->setPlaceholderText("e.g., CDL, AUTO, MED");

    // Name field
    auto nameGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group");
    nameGroup->addWidget(std::make_unique<Wt::WText>("Name *"))->addStyleClass("form-label");
    dialogDeptNameEdit_ = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptNameEdit_->addStyleClass("form-control");
    dialogDeptNameEdit_->setPlaceholderText("e.g., Professional Driving");

    // Director field
    auto directorGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    directorGroup->addStyleClass("form-group");
    directorGroup->addWidget(std::make_unique<Wt::WText>("Director"))->addStyleClass("form-label");
    dialogDeptDeanEdit_ = directorGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptDeanEdit_->addStyleClass("form-control");
    dialogDeptDeanEdit_->setPlaceholderText("e.g., John Smith");

    // Email field
    auto emailGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    emailGroup->addWidget(std::make_unique<Wt::WText>("Contact Email"))->addStyleClass("form-label");
    dialogDeptEmailEdit_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptEmailEdit_->addStyleClass("form-control");
    dialogDeptEmailEdit_->setPlaceholderText("e.g., department@school.edu");

    // Footer buttons
    auto footer = departmentDialog_->footer();
    footer->addStyleClass("admin-dialog-buttons");

    auto cancelBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn->addStyleClass("btn btn-secondary");
    cancelBtn->clicked().connect([this]() {
        departmentDialog_->reject();
    });

    auto saveBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveBtn->addStyleClass("btn btn-primary");
    saveBtn->clicked().connect([this]() {
        saveDepartment(true);
    });

    departmentDialog_->finished().connect([this](Wt::DialogCode code) {
        removeChild(departmentDialog_);
        departmentDialog_ = nullptr;
    });

    departmentDialog_->show();
}

void InstitutionSettingsWidget::showEditDepartmentDialog(int departmentId) {
    // Find the department
    DepartmentData* deptToEdit = nullptr;
    for (auto& dept : departments_) {
        if (dept.id == departmentId) {
            deptToEdit = &dept;
            break;
        }
    }

    if (!deptToEdit) {
        showMessage("Department not found", true);
        return;
    }

    departmentDialog_ = addChild(std::make_unique<Wt::WDialog>("Edit Department"));
    departmentDialog_->setModal(true);
    departmentDialog_->setClosable(true);
    departmentDialog_->addStyleClass("admin-dialog");
    departmentDialog_->setWidth(Wt::WLength(450));

    auto content = departmentDialog_->contents();
    content->addStyleClass("admin-dialog-content");

    // Code field
    auto codeGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    codeGroup->addStyleClass("form-group");
    codeGroup->addWidget(std::make_unique<Wt::WText>("Code *"))->addStyleClass("form-label");
    dialogDeptCodeEdit_ = codeGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptCodeEdit_->addStyleClass("form-control");
    dialogDeptCodeEdit_->setText(deptToEdit->code);

    // Name field
    auto nameGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group");
    nameGroup->addWidget(std::make_unique<Wt::WText>("Name *"))->addStyleClass("form-label");
    dialogDeptNameEdit_ = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptNameEdit_->addStyleClass("form-control");
    dialogDeptNameEdit_->setText(deptToEdit->name);

    // Director field
    auto directorGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    directorGroup->addStyleClass("form-group");
    directorGroup->addWidget(std::make_unique<Wt::WText>("Director"))->addStyleClass("form-label");
    dialogDeptDeanEdit_ = directorGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptDeanEdit_->addStyleClass("form-control");
    dialogDeptDeanEdit_->setText(deptToEdit->dean);

    // Email field
    auto emailGroup = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    emailGroup->addStyleClass("form-group");
    emailGroup->addWidget(std::make_unique<Wt::WText>("Contact Email"))->addStyleClass("form-label");
    dialogDeptEmailEdit_ = emailGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    dialogDeptEmailEdit_->addStyleClass("form-control");
    dialogDeptEmailEdit_->setText(deptToEdit->contactEmail);

    // Footer buttons
    auto footer = departmentDialog_->footer();
    footer->addStyleClass("admin-dialog-buttons");

    auto cancelBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn->addStyleClass("btn btn-secondary");
    cancelBtn->clicked().connect([this]() {
        departmentDialog_->reject();
    });

    auto saveBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveBtn->addStyleClass("btn btn-primary");
    int deptId = departmentId;
    saveBtn->clicked().connect([this, deptId]() {
        saveDepartment(false, deptId);
    });

    departmentDialog_->finished().connect([this](Wt::DialogCode code) {
        removeChild(departmentDialog_);
        departmentDialog_ = nullptr;
    });

    departmentDialog_->show();
}

void InstitutionSettingsWidget::saveDepartment(bool isNew, int departmentId) {
    if (!apiService_) {
        showMessage("Error: API service not available", true);
        return;
    }

    // Validate required fields
    std::string code = dialogDeptCodeEdit_->text().toUTF8();
    std::string name = dialogDeptNameEdit_->text().toUTF8();

    if (code.empty() || name.empty()) {
        showMessage("Code and Name are required fields", true);
        return;
    }

    // Build JSON payload
    nlohmann::json payload;
    payload["code"] = code;
    payload["name"] = name;
    payload["dean"] = dialogDeptDeanEdit_->text().toUTF8();
    payload["contact_email"] = dialogDeptEmailEdit_->text().toUTF8();

    try {
        Api::ApiResponse response;
        if (isNew) {
            response = apiService_->getApiClient()->post("/Department", payload);
        } else {
            response = apiService_->getApiClient()->put("/Department/" + std::to_string(departmentId), payload);
        }

        if (response.success) {
            departmentDialog_->accept();
            showMessage(isNew ? "Department created successfully!" : "Department updated successfully!", false);
            loadDepartments();
        } else {
            showMessage("Error saving department: " + response.body, true);
        }
    } catch (const std::exception& e) {
        showMessage(std::string("Error saving department: ") + e.what(), true);
    }
}

void InstitutionSettingsWidget::confirmDeleteDepartment(int departmentId, const std::string& deptName) {
    auto messageBox = addChild(std::make_unique<Wt::WMessageBox>(
        "Confirm Delete",
        "Are you sure you want to delete the department '" + deptName + "'?\n\n"
        "This may affect programs assigned to this department.",
        Wt::Icon::Warning,
        Wt::StandardButton::Yes | Wt::StandardButton::No
    ));

    messageBox->setModal(true);
    messageBox->buttonClicked().connect([this, messageBox, departmentId](Wt::StandardButton btn) {
        if (btn == Wt::StandardButton::Yes) {
            deleteDepartment(departmentId);
        }
        removeChild(messageBox);
    });

    messageBox->show();
}

void InstitutionSettingsWidget::deleteDepartment(int departmentId) {
    if (!apiService_) {
        showMessage("Error: API service not available", true);
        return;
    }

    try {
        auto response = apiService_->getApiClient()->del("/Department/" + std::to_string(departmentId));

        if (response.success) {
            showMessage("Department deleted successfully!", false);
            loadDepartments();
        } else {
            showMessage("Error deleting department: " + response.body, true);
        }
    } catch (const std::exception& e) {
        showMessage(std::string("Error deleting department: ") + e.what(), true);
    }
}

} // namespace Admin
} // namespace StudentIntake
