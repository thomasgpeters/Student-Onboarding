#include "FormTypeDetailWidget.h"
#include <Wt/WBreak.h>
#include "utils/Logger.h"
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormTypeDetailWidget::FormTypeDetailWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , currentFormTypeId_(0)
    , displayOrder_(0)
    , isRequired_(false)
    , isActive_(true)
    , topBar_(nullptr)
    , backBtn_(nullptr)
    , metadataContainer_(nullptr)
    , formNameText_(nullptr)
    , formCodeText_(nullptr)
    , formDescriptionText_(nullptr)
    , infoGrid_(nullptr)
    , categoryText_(nullptr)
    , orderText_(nullptr)
    , requiredText_(nullptr)
    , statusText_(nullptr)
    , createdText_(nullptr)
    , updatedText_(nullptr)
    , fieldsContainer_(nullptr)
    , fieldsTitle_(nullptr)
    , fieldsCount_(nullptr)
    , fieldsTable_(nullptr)
    , noFieldsText_(nullptr) {
    setupUI();
}

FormTypeDetailWidget::~FormTypeDetailWidget() {
}

void FormTypeDetailWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void FormTypeDetailWidget::setupUI() {
    addStyleClass("admin-form-type-detail");

    // Back button at top
    topBar_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    topBar_->addStyleClass("admin-detail-topbar");

    backBtn_ = topBar_->addWidget(std::make_unique<Wt::WPushButton>("< Back to Form Definitions"));
    backBtn_->addStyleClass("btn btn-link");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    // Metadata header section
    metadataContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    metadataContainer_->addStyleClass("admin-detail-header");

    auto nameContainer = metadataContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameContainer->addStyleClass("admin-detail-name-container");

    formNameText_ = nameContainer->addWidget(std::make_unique<Wt::WText>(""));
    formNameText_->addStyleClass("admin-detail-name");

    formCodeText_ = metadataContainer_->addWidget(std::make_unique<Wt::WText>(""));
    formCodeText_->addStyleClass("admin-detail-code");

    formDescriptionText_ = metadataContainer_->addWidget(std::make_unique<Wt::WText>(""));
    formDescriptionText_->addStyleClass("admin-detail-description");

    // Info cards grid
    infoGrid_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    infoGrid_->addStyleClass("admin-detail-info-grid");

    // Category card
    auto categoryCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    categoryCard->addStyleClass("admin-info-card");
    auto categoryLabel = categoryCard->addWidget(std::make_unique<Wt::WText>("Category"));
    categoryLabel->addStyleClass("admin-info-label");
    categoryText_ = categoryCard->addWidget(std::make_unique<Wt::WText>("-"));
    categoryText_->addStyleClass("admin-info-value");

    // Display order card
    auto orderCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    orderCard->addStyleClass("admin-info-card");
    auto orderLabel = orderCard->addWidget(std::make_unique<Wt::WText>("Display Order"));
    orderLabel->addStyleClass("admin-info-label");
    orderText_ = orderCard->addWidget(std::make_unique<Wt::WText>("-"));
    orderText_->addStyleClass("admin-info-value");

    // Required card
    auto requiredCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    requiredCard->addStyleClass("admin-info-card");
    auto requiredLabel = requiredCard->addWidget(std::make_unique<Wt::WText>("Required"));
    requiredLabel->addStyleClass("admin-info-label");
    requiredText_ = requiredCard->addWidget(std::make_unique<Wt::WText>("-"));
    requiredText_->addStyleClass("admin-info-value");

    // Status card
    auto statusCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusCard->addStyleClass("admin-info-card");
    auto statusLabel = statusCard->addWidget(std::make_unique<Wt::WText>("Status"));
    statusLabel->addStyleClass("admin-info-label");
    statusText_ = statusCard->addWidget(std::make_unique<Wt::WText>("-"));
    statusText_->addStyleClass("admin-info-value");

    // Created date card
    auto createdCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    createdCard->addStyleClass("admin-info-card");
    auto createdLabel = createdCard->addWidget(std::make_unique<Wt::WText>("Created"));
    createdLabel->addStyleClass("admin-info-label");
    createdText_ = createdCard->addWidget(std::make_unique<Wt::WText>("-"));
    createdText_->addStyleClass("admin-info-value");

    // Updated date card
    auto updatedCard = infoGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    updatedCard->addStyleClass("admin-info-card");
    auto updatedLabel = updatedCard->addWidget(std::make_unique<Wt::WText>("Last Updated"));
    updatedLabel->addStyleClass("admin-info-label");
    updatedText_ = updatedCard->addWidget(std::make_unique<Wt::WText>("-"));
    updatedText_->addStyleClass("admin-info-value");

    // Fields section
    fieldsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsContainer_->addStyleClass("admin-submissions-section");

    // Fields header
    auto fieldsHeader = fieldsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    fieldsHeader->addStyleClass("admin-submissions-header");

    fieldsTitle_ = fieldsHeader->addWidget(std::make_unique<Wt::WText>("Form Fields"));
    fieldsTitle_->addStyleClass("admin-section-title");

    fieldsCount_ = fieldsHeader->addWidget(std::make_unique<Wt::WText>(""));
    fieldsCount_->addStyleClass("admin-field-count");

    // Fields table
    fieldsTable_ = fieldsContainer_->addWidget(std::make_unique<Wt::WTable>());
    fieldsTable_->addStyleClass("admin-table");

    // No fields message
    noFieldsText_ = fieldsContainer_->addWidget(std::make_unique<Wt::WText>(
        "No field definitions available for this form type."));
    noFieldsText_->addStyleClass("text-muted admin-no-data");
    noFieldsText_->hide();
}

void FormTypeDetailWidget::loadFormType(int formTypeId) {
    currentFormTypeId_ = formTypeId;
    loadFormTypeData();
}

void FormTypeDetailWidget::loadFormTypeData() {
    if (!apiService_ || currentFormTypeId_ <= 0) {
        LOG_WARN("FormTypeDetail", "Invalid state for loading");
        return;
    }

    try {
        LOG_DEBUG("FormTypeDetail", "Loading form type: " << currentFormTypeId_);

        std::string endpoint = "/FormType/" + std::to_string(currentFormTypeId_);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (!response.success) {
            LOG_ERROR("FormTypeDetail", "Failed to load form type: " << response.errorMessage);
            return;
        }

        auto json = nlohmann::json::parse(response.body);

        // Handle both direct and {data: {...}} formats
        nlohmann::json ftData = json.contains("data") ? json["data"] : json;
        nlohmann::json attrs = ftData.contains("attributes") ? ftData["attributes"] : ftData;

        formCode_ = attrs.value("code", "");
        formName_ = attrs.value("name", "");
        formDescription_ = attrs.value("description", "");
        formCategory_ = attrs.value("category", "");
        displayOrder_ = attrs.value("display_order", 0);
        isRequired_ = attrs.value("is_required", false);
        isActive_ = attrs.value("is_active", true);
        createdAt_ = attrs.value("created_at", "");
        updatedAt_ = attrs.value("updated_at", "");

        LOG_INFO("FormTypeDetail", "Loaded form type: " << formName_ << " (" << formCode_ << ")");

        loadFieldDefinitions();
        updateDisplay();

    } catch (const std::exception& e) {
        LOG_ERROR("FormTypeDetail", "Exception loading form type: " << e.what());
    }
}

void FormTypeDetailWidget::loadFieldDefinitions() {
    fieldDefinitions_ = getFieldsForFormType(formCode_);
    updateFieldsTable();
}

std::vector<FormFieldDefinition> FormTypeDetailWidget::getFieldsForFormType(const std::string& formCode) {
    std::vector<FormFieldDefinition> fields;

    if (formCode == "personal_info") {
        fields = {
            {"first_name", "First Name", "string", true, true},
            {"last_name", "Last Name", "string", true, true},
            {"middle_name", "Middle Name", "string", true, false},
            {"preferred_name", "Preferred Name", "string", true, false},
            {"preferred_pronouns", "Preferred Pronouns", "string", true, false},
            {"date_of_birth", "Date of Birth", "date", true, true},
            {"gender", "Gender", "string", true, false},
            {"ssn", "Social Security Number", "string", false, false},
            {"citizenship_status", "Citizenship Status", "string", true, true},
            {"citizenship_country", "Country of Citizenship", "string", true, false},
            {"phone_number", "Phone Number", "string", true, true},
            {"alternate_phone", "Alternate Phone", "string", true, false},
            {"email", "Email Address", "string", true, true},
            {"street1", "Street Address Line 1", "string", true, true},
            {"street2", "Street Address Line 2", "string", true, false},
            {"city", "City", "string", true, true},
            {"state", "State/Province", "string", true, true},
            {"postal_code", "Postal Code", "string", true, true},
            {"country", "Country", "string", true, true}
        };
    }
    else if (formCode == "emergency_contact") {
        fields = {
            {"first_name", "Contact First Name", "string", true, true},
            {"last_name", "Contact Last Name", "string", true, true},
            {"relationship", "Relationship", "string", true, true},
            {"phone", "Phone Number", "string", true, true},
            {"alternate_phone", "Alternate Phone", "string", true, false},
            {"email", "Email Address", "string", true, false},
            {"street1", "Street Address Line 1", "string", true, false},
            {"street2", "Street Address Line 2", "string", true, false},
            {"city", "City", "string", true, false},
            {"state", "State/Province", "string", true, false},
            {"postal_code", "Postal Code", "string", true, false},
            {"country", "Country", "string", true, false},
            {"is_primary", "Primary Contact", "bool", true, false},
            {"priority", "Priority Order", "int", true, false}
        };
    }
    else if (formCode == "medical_info") {
        fields = {
            {"blood_type", "Blood Type", "string", true, false},
            {"allergies", "Allergies", "string", true, false},
            {"medications", "Current Medications", "string", true, false},
            {"chronic_conditions", "Chronic Conditions", "string", true, false},
            {"disabilities", "Disabilities", "string", true, false},
            {"accommodations_needed", "Accommodations Needed", "string", true, false},
            {"immunization_status", "Immunization Status", "string", true, false},
            {"last_physical_date", "Last Physical Exam Date", "date", true, false},
            {"has_health_insurance", "Has Health Insurance", "bool", true, false},
            {"insurance_provider", "Insurance Provider", "string", true, false},
            {"insurance_policy_number", "Policy Number", "string", true, false},
            {"insurance_group_number", "Group Number", "string", true, false},
            {"primary_physician_name", "Primary Physician Name", "string", true, false},
            {"primary_physician_phone", "Physician Phone", "string", true, false},
            {"emergency_medical_info", "Emergency Medical Information", "string", true, false},
            {"mental_health_history", "Mental Health History", "string", false, false},
            {"dietary_restrictions", "Dietary Restrictions", "string", true, false},
            {"requires_ada_accommodations", "Requires ADA Accommodations", "bool", true, false},
            {"ada_accommodation_details", "ADA Accommodation Details", "string", true, false},
            {"consent_to_treat", "Consent to Treat", "bool", true, true},
            {"hipaa_acknowledgment", "HIPAA Acknowledgment", "bool", true, true}
        };
    }
    else if (formCode == "academic_history") {
        fields = {
            {"institution_name", "Institution Name", "string", true, true},
            {"institution_type", "Institution Type", "string", true, true},
            {"institution_city", "City", "string", true, false},
            {"institution_state", "State/Province", "string", true, false},
            {"institution_country", "Country", "string", true, false},
            {"degree_type", "Degree/Certificate Type", "string", true, true},
            {"major", "Major/Field of Study", "string", true, true},
            {"minor", "Minor", "string", true, false},
            {"gpa", "GPA", "double", true, true},
            {"gpa_scale", "GPA Scale", "double", true, false},
            {"start_date", "Start Date", "date", true, true},
            {"end_date", "End Date", "date", true, true},
            {"graduation_date", "Graduation Date", "date", true, false},
            {"is_graduated", "Graduated", "bool", true, true},
            {"honors", "Honors/Awards", "string", true, false},
            {"transcript_received", "Transcript Received", "bool", true, false}
        };
    }
    else if (formCode == "financial_aid") {
        fields = {
            {"has_filed_fafsa", "Has Filed FAFSA", "bool", true, true},
            {"fafsa_year", "FAFSA Year", "int", true, false},
            {"efc", "Expected Family Contribution", "double", true, false},
            {"annual_income", "Annual Household Income", "double", false, false},
            {"household_size", "Household Size", "int", true, false},
            {"dependents_in_college", "Dependents in College", "int", true, false},
            {"is_independent", "Independent Student", "bool", true, true},
            {"has_federal_loans", "Has Federal Loans", "bool", true, false},
            {"federal_loan_amount", "Federal Loan Amount", "double", true, false},
            {"has_private_loans", "Has Private Loans", "bool", true, false},
            {"private_loan_amount", "Private Loan Amount", "double", true, false},
            {"scholarship_amount", "Scholarship Amount", "double", true, true},
            {"scholarship_name", "Scholarship Name", "string", true, false},
            {"work_study_interest", "Work Study Interest", "bool", true, false},
            {"veteran_benefits", "Veteran Benefits", "bool", true, true},
            {"va_chapter", "VA Chapter", "string", true, false},
            {"employer_tuition_assistance", "Employer Tuition Assistance", "bool", true, false}
        };
    }
    else if (formCode == "documents") {
        fields = {
            {"document_type", "Document Type", "string", true, true},
            {"document_name", "Document Name", "string", true, true},
            {"file_path", "File Path", "string", false, false},
            {"file_size", "File Size", "int", false, false},
            {"mime_type", "MIME Type", "string", false, false},
            {"upload_date", "Upload Date", "date", true, true},
            {"verified", "Verified", "bool", true, true},
            {"verified_by", "Verified By", "string", true, false},
            {"verified_date", "Verification Date", "date", true, false},
            {"notes", "Notes", "string", true, false}
        };
    }
    else if (formCode == "consent") {
        fields = {
            {"ferpa_consent", "FERPA Consent", "bool", true, true},
            {"ferpa_date", "FERPA Consent Date", "date", true, true},
            {"photo_release", "Photo Release", "bool", true, true},
            {"photo_release_date", "Photo Release Date", "date", true, false},
            {"directory_info_consent", "Directory Information Consent", "bool", true, true},
            {"marketing_consent", "Marketing Communications Consent", "bool", true, false},
            {"terms_accepted", "Terms and Conditions Accepted", "bool", true, true},
            {"terms_accepted_date", "Terms Accepted Date", "date", true, true},
            {"privacy_policy_accepted", "Privacy Policy Accepted", "bool", true, true},
            {"privacy_accepted_date", "Privacy Accepted Date", "date", true, false},
            {"honor_code_accepted", "Honor Code Accepted", "bool", true, true},
            {"honor_code_date", "Honor Code Date", "date", true, false},
            {"electronic_signature", "Electronic Signature", "string", true, true},
            {"signature_date", "Signature Date", "date", true, true},
            {"ip_address", "IP Address", "string", false, false}
        };
    }

    return fields;
}

void FormTypeDetailWidget::updateDisplay() {
    formNameText_->setText(formName_);
    formCodeText_->setText("Code: " + formCode_);
    formDescriptionText_->setText(formDescription_.empty() ? "No description available." : formDescription_);

    // Category with capitalization
    std::string categoryDisplay = formCategory_;
    if (!categoryDisplay.empty()) {
        categoryDisplay[0] = std::toupper(categoryDisplay[0]);
    } else {
        categoryDisplay = "General";
    }
    categoryText_->setText(categoryDisplay);

    orderText_->setText(std::to_string(displayOrder_));
    requiredText_->setText(isRequired_ ? "Yes" : "No");
    statusText_->setText(isActive_ ? "Active" : "Inactive");
    createdText_->setText(formatDate(createdAt_));
    updatedText_->setText(formatDate(updatedAt_));
}

void FormTypeDetailWidget::updateFieldsTable() {
    fieldsTable_->clear();

    // Update fields count
    fieldsCount_->setText("(" + std::to_string(fieldDefinitions_.size()) + " fields)");

    if (fieldDefinitions_.empty()) {
        fieldsTable_->hide();
        noFieldsText_->show();
        return;
    }

    fieldsTable_->show();
    noFieldsText_->hide();

    // Header row
    fieldsTable_->setHeaderCount(1);
    int col = 0;
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("#"));
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Column Name"));
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Label"));
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Data Type"));
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Required"));
    fieldsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Printable"));

    for (int i = 0; i < col; i++) {
        fieldsTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    // Data rows
    int row = 1;
    for (size_t i = 0; i < fieldDefinitions_.size(); i++) {
        const auto& field = fieldDefinitions_[i];
        col = 0;

        // Row number
        fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(std::to_string(i + 1)));
        fieldsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell text-center");

        // Column name (monospace)
        auto colNameText = fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(field.fieldName));
        colNameText->addStyleClass("admin-code-text");
        fieldsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Label
        fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(field.label));
        fieldsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Data type
        auto dataTypeBadge = fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(getDataTypeDisplayName(field.dataType)));
        dataTypeBadge->addStyleClass("badge badge-secondary");
        fieldsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Required checkbox (read-only)
        auto reqCheckbox = fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WCheckBox>());
        reqCheckbox->setChecked(field.isRequired);
        reqCheckbox->setEnabled(false);
        fieldsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell text-center");

        // Printable checkbox (read-only)
        auto printCheckbox = fieldsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WCheckBox>());
        printCheckbox->setChecked(field.isPrintable);
        printCheckbox->setEnabled(false);
        fieldsTable_->elementAt(row, col)->addStyleClass("admin-table-cell text-center");

        row++;
    }
}

void FormTypeDetailWidget::clear() {
    currentFormTypeId_ = 0;
    formCode_.clear();
    formName_.clear();
    formDescription_.clear();
    formCategory_.clear();
    displayOrder_ = 0;
    isRequired_ = false;
    isActive_ = true;
    createdAt_.clear();
    updatedAt_.clear();
    fieldDefinitions_.clear();

    formNameText_->setText("");
    formCodeText_->setText("");
    formDescriptionText_->setText("");
    categoryText_->setText("-");
    orderText_->setText("-");
    requiredText_->setText("-");
    statusText_->setText("-");
    createdText_->setText("-");
    updatedText_->setText("-");
    fieldsCount_->setText("");
    fieldsTable_->clear();
}

std::string FormTypeDetailWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

std::string FormTypeDetailWidget::getDataTypeDisplayName(const std::string& dataType) {
    if (dataType == "string") return "Text";
    if (dataType == "int") return "Integer";
    if (dataType == "double") return "Decimal";
    if (dataType == "bool") return "Boolean";
    if (dataType == "date") return "Date";
    if (dataType == "datetime") return "DateTime";
    if (dataType == "array") return "Array";
    return dataType;
}

} // namespace Admin
} // namespace StudentIntake
