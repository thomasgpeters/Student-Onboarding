#include "CurriculumEditorWidget.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

// Use Curriculum from the main Models namespace
using StudentIntake::Models::Curriculum;

CurriculumEditorWidget::CurriculumEditorWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , isNewCurriculum_(false)
    , headerSection_(nullptr)
    , headerTitle_(nullptr)
    , backBtn_(nullptr)
    , messageContainer_(nullptr)
    , errorMessage_(nullptr)
    , successMessage_(nullptr)
    , formContainer_(nullptr)
    , nameInput_(nullptr)
    , descriptionInput_(nullptr)
    , departmentSelect_(nullptr)
    , degreeTypeSelect_(nullptr)
    , creditHoursInput_(nullptr)
    , durationInput_(nullptr)
    , isActiveCheck_(nullptr)
    , isOnlineCheck_(nullptr)
    , formsSection_(nullptr)
    , formsCheckboxContainer_(nullptr)
    , actionsContainer_(nullptr)
    , saveBtn_(nullptr)
    , cancelBtn_(nullptr) {
    setupUI();
    loadAvailableForms();
}

CurriculumEditorWidget::~CurriculumEditorWidget() {
}

void CurriculumEditorWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void CurriculumEditorWidget::setupUI() {
    addStyleClass("admin-curriculum-editor");

    // Header section with back button
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->addStyleClass("admin-detail-topbar");

    backBtn_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("< Back to Programs"));
    backBtn_->addStyleClass("btn btn-link");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    // Title section
    auto titleSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    titleSection->addStyleClass("admin-editor-header");

    headerTitle_ = titleSection->addWidget(std::make_unique<Wt::WText>("<h2>Edit Program</h2>"));
    headerTitle_->setTextFormat(Wt::TextFormat::XHTML);

    // Message container
    messageContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    messageContainer_->addStyleClass("admin-messages");

    errorMessage_ = messageContainer_->addWidget(std::make_unique<Wt::WText>());
    errorMessage_->addStyleClass("alert alert-danger");
    errorMessage_->hide();

    successMessage_ = messageContainer_->addWidget(std::make_unique<Wt::WText>());
    successMessage_->addStyleClass("alert alert-success");
    successMessage_->hide();

    // Main form container
    formContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    formContainer_->addStyleClass("admin-editor-form");

    // ===== Basic Information Section =====
    auto basicSection = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    basicSection->addStyleClass("admin-form-section card");

    auto basicHeader = basicSection->addWidget(std::make_unique<Wt::WText>("<h4>Basic Information</h4>"));
    basicHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto basicFields = basicSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    basicFields->addStyleClass("admin-form-fields");

    // Program Name
    auto nameGroup = basicFields->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameGroup->addStyleClass("form-group");
    auto nameLabel = nameGroup->addWidget(std::make_unique<Wt::WText>("Program Name *"));
    nameLabel->addStyleClass("form-label");
    nameInput_ = nameGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    nameInput_->setPlaceholderText("e.g., Computer Science");
    nameInput_->addStyleClass("form-control");

    // Description
    auto descGroup = basicFields->addWidget(std::make_unique<Wt::WContainerWidget>());
    descGroup->addStyleClass("form-group");
    auto descLabel = descGroup->addWidget(std::make_unique<Wt::WText>("Description"));
    descLabel->addStyleClass("form-label");
    descriptionInput_ = descGroup->addWidget(std::make_unique<Wt::WTextArea>());
    descriptionInput_->setPlaceholderText("Program description...");
    descriptionInput_->addStyleClass("form-control");
    descriptionInput_->setRows(3);

    // Two-column row for department and degree type
    auto twoColRow = basicFields->addWidget(std::make_unique<Wt::WContainerWidget>());
    twoColRow->addStyleClass("admin-form-row");

    // Department
    auto deptGroup = twoColRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptGroup->addStyleClass("form-group");
    auto deptLabel = deptGroup->addWidget(std::make_unique<Wt::WText>("Department *"));
    deptLabel->addStyleClass("form-label");
    departmentSelect_ = deptGroup->addWidget(std::make_unique<Wt::WComboBox>());
    departmentSelect_->addStyleClass("form-control");
    departmentSelect_->addItem("Select Department");
    departmentSelect_->addItem("Computer Science");
    departmentSelect_->addItem("Engineering");
    departmentSelect_->addItem("Business");
    departmentSelect_->addItem("Arts & Sciences");
    departmentSelect_->addItem("Health Sciences");
    departmentSelect_->addItem("Education");
    departmentSelect_->addItem("Law");

    // Degree Type
    auto degreeGroup = twoColRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    degreeGroup->addStyleClass("form-group");
    auto degreeLabel = degreeGroup->addWidget(std::make_unique<Wt::WText>("Degree Type *"));
    degreeLabel->addStyleClass("form-label");
    degreeTypeSelect_ = degreeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    degreeTypeSelect_->addStyleClass("form-control");
    degreeTypeSelect_->addItem("Select Degree Type");
    degreeTypeSelect_->addItem("Associate");
    degreeTypeSelect_->addItem("Bachelor");
    degreeTypeSelect_->addItem("Master");
    degreeTypeSelect_->addItem("Doctoral");
    degreeTypeSelect_->addItem("Certificate");

    // ===== Program Details Section =====
    auto detailsSection = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    detailsSection->addStyleClass("admin-form-section card");

    auto detailsHeader = detailsSection->addWidget(std::make_unique<Wt::WText>("<h4>Program Details</h4>"));
    detailsHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto detailsFields = detailsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    detailsFields->addStyleClass("admin-form-fields");

    // Two-column row for credits and duration
    auto numericRow = detailsFields->addWidget(std::make_unique<Wt::WContainerWidget>());
    numericRow->addStyleClass("admin-form-row");

    // Credit Hours
    auto creditGroup = numericRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    creditGroup->addStyleClass("form-group");
    auto creditLabel = creditGroup->addWidget(std::make_unique<Wt::WText>("Credit Hours *"));
    creditLabel->addStyleClass("form-label");
    creditHoursInput_ = creditGroup->addWidget(std::make_unique<Wt::WSpinBox>());
    creditHoursInput_->setRange(1, 300);
    creditHoursInput_->setValue(120);
    creditHoursInput_->addStyleClass("form-control");

    // Duration
    auto durationGroup = numericRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    durationGroup->addStyleClass("form-group");
    auto durationLabel = durationGroup->addWidget(std::make_unique<Wt::WText>("Duration (Semesters) *"));
    durationLabel->addStyleClass("form-label");
    durationInput_ = durationGroup->addWidget(std::make_unique<Wt::WSpinBox>());
    durationInput_->setRange(1, 20);
    durationInput_->setValue(8);
    durationInput_->addStyleClass("form-control");

    // Checkboxes row
    auto checkRow = detailsFields->addWidget(std::make_unique<Wt::WContainerWidget>());
    checkRow->addStyleClass("admin-form-row admin-checkbox-row");

    // Is Active
    auto activeGroup = checkRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    activeGroup->addStyleClass("form-check");
    isActiveCheck_ = activeGroup->addWidget(std::make_unique<Wt::WCheckBox>(" Program is Active"));
    isActiveCheck_->setChecked(true);
    isActiveCheck_->addStyleClass("form-check-input");

    // Is Online
    auto onlineGroup = checkRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    onlineGroup->addStyleClass("form-check");
    isOnlineCheck_ = onlineGroup->addWidget(std::make_unique<Wt::WCheckBox>(" Available Online"));
    isOnlineCheck_->addStyleClass("form-check-input");

    // ===== Required Forms Section =====
    formsSection_ = formContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsSection_->addStyleClass("admin-form-section card");

    auto formsHeader = formsSection_->addWidget(std::make_unique<Wt::WText>("<h4>Required Forms</h4>"));
    formsHeader->setTextFormat(Wt::TextFormat::XHTML);

    auto formsDesc = formsSection_->addWidget(std::make_unique<Wt::WText>(
        "<p class=\"admin-form-description\">Select which forms students must complete for this program:</p>"));
    formsDesc->setTextFormat(Wt::TextFormat::XHTML);

    formsCheckboxContainer_ = formsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsCheckboxContainer_->addStyleClass("admin-forms-checkbox-list");

    // ===== Action Buttons =====
    actionsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsContainer_->addStyleClass("admin-editor-actions");

    saveBtn_ = actionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Save Program"));
    saveBtn_->addStyleClass("btn btn-primary");
    saveBtn_->clicked().connect(this, &CurriculumEditorWidget::saveCurriculum);

    cancelBtn_ = actionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    cancelBtn_->addStyleClass("btn btn-secondary");
    cancelBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });
}

void CurriculumEditorWidget::loadAvailableForms() {
    // Define available form types
    availableForms_.clear();

    availableForms_.push_back({"personal_info", "Personal Information", "Student demographics, contact info"});
    availableForms_.push_back({"emergency_contact", "Emergency Contact", "Emergency contact details"});
    availableForms_.push_back({"medical_info", "Medical Information", "Health and medical history"});
    availableForms_.push_back({"academic_history", "Academic History", "Previous education records"});
    availableForms_.push_back({"financial_aid", "Financial Aid", "Financial aid eligibility"});
    availableForms_.push_back({"document_upload", "Document Upload", "Supporting documents"});
    availableForms_.push_back({"consent", "Consent Form", "Terms and conditions"});

    // Clear existing checkboxes
    formsCheckboxContainer_->clear();
    formCheckboxes_.clear();

    // Create checkboxes for each form type
    for (const auto& form : availableForms_) {
        auto formItem = formsCheckboxContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        formItem->addStyleClass("admin-form-checkbox-item");

        auto checkbox = formItem->addWidget(std::make_unique<Wt::WCheckBox>());
        checkbox->addStyleClass("form-check-input");
        formCheckboxes_[form.id] = checkbox;

        auto labelContainer = formItem->addWidget(std::make_unique<Wt::WContainerWidget>());
        labelContainer->addStyleClass("admin-form-checkbox-label");

        auto nameText = labelContainer->addWidget(std::make_unique<Wt::WText>(form.name));
        nameText->addStyleClass("admin-form-name");

        auto descText = labelContainer->addWidget(std::make_unique<Wt::WText>(form.description));
        descText->addStyleClass("admin-form-desc");
    }
}

void CurriculumEditorWidget::loadCurriculum(const std::string& curriculumId) {
    clearMessages();
    isNewCurriculum_ = false;
    headerTitle_->setText("<h2>Edit Program</h2>");

    // Try to load from API
    if (apiService_) {
        try {
            auto response = apiService_->getApiClient()->get("/Curriculum/" + curriculumId);
            if (response.success) {
                auto jsonResponse = nlohmann::json::parse(response.body);
                if (jsonResponse.contains("data")) {
                    currentCurriculum_ = Curriculum::fromJson(jsonResponse["data"]);
                    populateForm(currentCurriculum_);
                    return;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "[CurriculumEditorWidget] Error loading curriculum: " << e.what() << std::endl;
        }
    }

    // Fallback: use mock data for testing
    currentCurriculum_.setId(curriculumId);
    currentCurriculum_.setName("Computer Science");
    currentCurriculum_.setDescription("Bachelor of Science in Computer Science with focus on software development and systems.");
    currentCurriculum_.setDepartment("Computer Science");
    currentCurriculum_.setDegreeType("bachelor");
    currentCurriculum_.setCreditHours(120);
    currentCurriculum_.setDurationSemesters(8);
    currentCurriculum_.setActive(true);
    currentCurriculum_.setOnline(false);
    currentCurriculum_.setRequiredForms({"personal_info", "academic_history", "consent"});

    populateForm(currentCurriculum_);
}

void CurriculumEditorWidget::createNew() {
    clearMessages();
    isNewCurriculum_ = true;
    headerTitle_->setText("<h2>Add New Program</h2>");

    currentCurriculum_ = Curriculum();
    currentCurriculum_.setActive(true);

    // Reset form fields
    nameInput_->setText("");
    descriptionInput_->setText("");
    departmentSelect_->setCurrentIndex(0);
    degreeTypeSelect_->setCurrentIndex(0);
    creditHoursInput_->setValue(120);
    durationInput_->setValue(8);
    isActiveCheck_->setChecked(true);
    isOnlineCheck_->setChecked(false);

    // Clear all form checkboxes
    for (auto& [id, checkbox] : formCheckboxes_) {
        checkbox->setChecked(false);
    }
    // Default: select personal_info and consent
    if (formCheckboxes_.count("personal_info")) {
        formCheckboxes_["personal_info"]->setChecked(true);
    }
    if (formCheckboxes_.count("consent")) {
        formCheckboxes_["consent"]->setChecked(true);
    }
}

void CurriculumEditorWidget::populateForm(const Curriculum& curriculum) {
    nameInput_->setText(curriculum.getName());
    descriptionInput_->setText(curriculum.getDescription());

    // Set department
    std::string dept = curriculum.getDepartment();
    for (int i = 0; i < departmentSelect_->count(); i++) {
        if (departmentSelect_->itemText(i).toUTF8() == dept) {
            departmentSelect_->setCurrentIndex(i);
            break;
        }
    }

    // Set degree type - capitalize first letter for matching
    std::string degreeType = curriculum.getDegreeType();
    if (!degreeType.empty()) {
        degreeType[0] = std::toupper(degreeType[0]);
    }
    for (int i = 0; i < degreeTypeSelect_->count(); i++) {
        if (degreeTypeSelect_->itemText(i).toUTF8() == degreeType) {
            degreeTypeSelect_->setCurrentIndex(i);
            break;
        }
    }

    creditHoursInput_->setValue(curriculum.getCreditHours());
    durationInput_->setValue(curriculum.getDurationSemesters());
    isActiveCheck_->setChecked(curriculum.isActive());
    isOnlineCheck_->setChecked(curriculum.isOnline());

    // Set required forms checkboxes
    auto requiredForms = curriculum.getRequiredForms();
    for (auto& [id, checkbox] : formCheckboxes_) {
        checkbox->setChecked(std::find(requiredForms.begin(), requiredForms.end(), id) != requiredForms.end());
    }
}

void CurriculumEditorWidget::clear() {
    nameInput_->setText("");
    descriptionInput_->setText("");
    departmentSelect_->setCurrentIndex(0);
    degreeTypeSelect_->setCurrentIndex(0);
    creditHoursInput_->setValue(120);
    durationInput_->setValue(8);
    isActiveCheck_->setChecked(true);
    isOnlineCheck_->setChecked(false);
    for (auto& [id, checkbox] : formCheckboxes_) {
        checkbox->setChecked(false);
    }
    clearMessages();
}

void CurriculumEditorWidget::saveCurriculum() {
    clearMessages();

    if (!validateForm()) {
        return;
    }

    // Gather form data
    currentCurriculum_.setName(nameInput_->text().toUTF8());
    currentCurriculum_.setDescription(descriptionInput_->text().toUTF8());

    if (departmentSelect_->currentIndex() > 0) {
        currentCurriculum_.setDepartment(departmentSelect_->currentText().toUTF8());
    }

    if (degreeTypeSelect_->currentIndex() > 0) {
        std::string degreeType = degreeTypeSelect_->currentText().toUTF8();
        std::transform(degreeType.begin(), degreeType.end(), degreeType.begin(), ::tolower);
        currentCurriculum_.setDegreeType(degreeType);
    }

    currentCurriculum_.setCreditHours(creditHoursInput_->value());
    currentCurriculum_.setDurationSemesters(durationInput_->value());
    currentCurriculum_.setActive(isActiveCheck_->isChecked());
    currentCurriculum_.setOnline(isOnlineCheck_->isChecked());

    // Gather required forms
    std::vector<std::string> requiredForms;
    for (const auto& [id, checkbox] : formCheckboxes_) {
        if (checkbox->isChecked()) {
            requiredForms.push_back(id);
        }
    }
    currentCurriculum_.setRequiredForms(requiredForms);

    // Save to API
    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "curriculum";
            payload["data"]["attributes"] = currentCurriculum_.toJson();

            Api::ApiResponse response;
            if (isNewCurriculum_) {
                response = apiService_->getApiClient()->post("/Curriculum", payload.dump());
            } else {
                payload["data"]["id"] = currentCurriculum_.getId();
                response = apiService_->getApiClient()->patch("/Curriculum/" + currentCurriculum_.getId(), payload.dump());
            }

            if (response.success) {
                showSuccess(isNewCurriculum_ ? "Program created successfully!" : "Program updated successfully!");
                saveSuccess_.emit();
                return;
            } else {
                showError("Failed to save program: " + response.errorMessage);
                return;
            }
        } catch (const std::exception& e) {
            std::cerr << "[CurriculumEditorWidget] Error saving curriculum: " << e.what() << std::endl;
            showError("Error saving program: " + std::string(e.what()));
            return;
        }
    }

    // No API - simulate success
    showSuccess(isNewCurriculum_ ? "Program created successfully!" : "Program updated successfully!");
    std::cerr << "[CurriculumEditorWidget] Program saved (simulated): " << currentCurriculum_.getName() << std::endl;
    saveSuccess_.emit();
}

bool CurriculumEditorWidget::validateForm() {
    if (nameInput_->text().empty()) {
        showError("Program name is required.");
        return false;
    }

    if (departmentSelect_->currentIndex() == 0) {
        showError("Please select a department.");
        return false;
    }

    if (degreeTypeSelect_->currentIndex() == 0) {
        showError("Please select a degree type.");
        return false;
    }

    if (creditHoursInput_->value() < 1) {
        showError("Credit hours must be at least 1.");
        return false;
    }

    if (durationInput_->value() < 1) {
        showError("Duration must be at least 1 semester.");
        return false;
    }

    // Check at least one form is selected
    bool hasForm = false;
    for (const auto& [id, checkbox] : formCheckboxes_) {
        if (checkbox->isChecked()) {
            hasForm = true;
            break;
        }
    }
    if (!hasForm) {
        showError("Please select at least one required form.");
        return false;
    }

    return true;
}

void CurriculumEditorWidget::showError(const std::string& message) {
    errorMessage_->setText(message);
    errorMessage_->show();
    successMessage_->hide();
}

void CurriculumEditorWidget::showSuccess(const std::string& message) {
    successMessage_->setText(message);
    successMessage_->show();
    errorMessage_->hide();
}

void CurriculumEditorWidget::clearMessages() {
    errorMessage_->hide();
    successMessage_->hide();
}

void CurriculumEditorWidget::updateRequiredFormsDisplay() {
    // Implemented through checkbox checked state
}

void CurriculumEditorWidget::toggleFormRequirement(const std::string& formId, bool required) {
    if (formCheckboxes_.count(formId)) {
        formCheckboxes_[formId]->setChecked(required);
    }
}

} // namespace Admin
} // namespace StudentIntake
