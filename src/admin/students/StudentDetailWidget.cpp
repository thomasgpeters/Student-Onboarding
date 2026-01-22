#include "StudentDetailWidget.h"
#include <Wt/WBreak.h>
#include <map>
#include "utils/Logger.h"
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

StudentDetailWidget::StudentDetailWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , isRevoked_(false)
    , currentStudentId_(0)
    , headerContainer_(nullptr)
    , studentName_(nullptr)
    , studentEmail_(nullptr)
    , statusBadge_(nullptr)
    , infoContainer_(nullptr)
    , programText_(nullptr)
    , enrolledText_(nullptr)
    , phoneText_(nullptr)
    , addressText_(nullptr)
    , intakeStatusLabel_(nullptr)
    , intakeStatusText_(nullptr)
    , intakeStatus_("in_progress")
    , actionsContainer_(nullptr)
    , revokeBtn_(nullptr)
    , restoreBtn_(nullptr)
    , backBtn_(nullptr)
    , submissionsContainer_(nullptr)
    , submissionsHeader_(nullptr)
    , submissionsTitle_(nullptr)
    , previewPdfBtn_(nullptr)
    , printAllBtn_(nullptr)
    , submissionsTable_(nullptr)
    , noSubmissionsText_(nullptr)
    , academicHistorySectionContainer_(nullptr)
    , hasPreviousEducationCheckbox_(nullptr)
    , academicHistoryContainer_(nullptr)
    , academicHistoryHeader_(nullptr)
    , academicHistoryTitle_(nullptr)
    , academicHistoryTable_(nullptr)
    , noAcademicHistoryText_(nullptr) {
    setupUI();
}

StudentDetailWidget::~StudentDetailWidget() {
}

void StudentDetailWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void StudentDetailWidget::setupUI() {
    addStyleClass("admin-student-detail");

    // Back button at top
    auto topBar = addWidget(std::make_unique<Wt::WContainerWidget>());
    topBar->addStyleClass("admin-detail-topbar");

    backBtn_ = topBar->addWidget(std::make_unique<Wt::WPushButton>("< Back to Students"));
    backBtn_->addStyleClass("btn btn-link");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    // Header section with student name and status
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("admin-detail-header");

    auto nameContainer = headerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameContainer->addStyleClass("admin-detail-name-container");

    studentName_ = nameContainer->addWidget(std::make_unique<Wt::WText>(""));
    studentName_->addStyleClass("admin-detail-name");

    statusBadge_ = nameContainer->addWidget(std::make_unique<Wt::WText>(""));
    statusBadge_->addStyleClass("badge badge-success");

    studentEmail_ = headerContainer_->addWidget(std::make_unique<Wt::WText>(""));
    studentEmail_->addStyleClass("admin-detail-email");

    // Info cards container
    infoContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    infoContainer_->addStyleClass("admin-detail-info-grid");

    // Program card
    auto programCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    programCard->addStyleClass("admin-info-card");
    auto programLabel = programCard->addWidget(std::make_unique<Wt::WText>("Program"));
    programLabel->addStyleClass("admin-info-label");
    programText_ = programCard->addWidget(std::make_unique<Wt::WText>("-"));
    programText_->addStyleClass("admin-info-value");

    // Enrolled card
    auto enrolledCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    enrolledCard->addStyleClass("admin-info-card");
    auto enrolledLabel = enrolledCard->addWidget(std::make_unique<Wt::WText>("📅 Enrolled Date"));
    enrolledLabel->addStyleClass("admin-info-label");
    enrolledText_ = enrolledCard->addWidget(std::make_unique<Wt::WText>("-"));
    enrolledText_->addStyleClass("admin-info-value");

    // Phone card
    auto phoneCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneCard->addStyleClass("admin-info-card");
    auto phoneLabel = phoneCard->addWidget(std::make_unique<Wt::WText>("📞 Phone"));
    phoneLabel->addStyleClass("admin-info-label");
    phoneText_ = phoneCard->addWidget(std::make_unique<Wt::WText>("-"));
    phoneText_->addStyleClass("admin-info-value");

    // Address card
    auto addressCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    addressCard->addStyleClass("admin-info-card");
    auto addressLabel = addressCard->addWidget(std::make_unique<Wt::WText>("📍 Address"));
    addressLabel->addStyleClass("admin-info-label");
    addressText_ = addressCard->addWidget(std::make_unique<Wt::WText>("-"));
    addressText_->addStyleClass("admin-info-value");

    // Intake Status card
    auto intakeStatusCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    intakeStatusCard->addStyleClass("admin-info-card");
    intakeStatusLabel_ = intakeStatusCard->addWidget(std::make_unique<Wt::WText>("Intake Status"));
    intakeStatusLabel_->addStyleClass("admin-info-label");
    intakeStatusText_ = intakeStatusCard->addWidget(std::make_unique<Wt::WText>("-"));
    intakeStatusText_->addStyleClass("admin-info-value");

    // Actions section (Revoke/Reinstate access)
    actionsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsContainer_->addStyleClass("admin-detail-actions");

    auto actionsTitle = actionsContainer_->addWidget(std::make_unique<Wt::WText>("Account Actions"));
    actionsTitle->addStyleClass("admin-actions-title");

    auto buttonsContainer = actionsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonsContainer->addStyleClass("admin-action-buttons");

    revokeBtn_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Revoke Access"));
    revokeBtn_->addStyleClass("btn btn-danger");
    revokeBtn_->clicked().connect([this]() {
        onRevokeAccess();
    });

    restoreBtn_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Reinstate Access"));
    restoreBtn_->addStyleClass("btn btn-success");
    restoreBtn_->clicked().connect([this]() {
        onRestoreAccess();
    });
    restoreBtn_->hide();

    // Form Submissions section
    submissionsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    submissionsContainer_->addStyleClass("admin-submissions-section");

    // Submissions header with title and PDF buttons
    submissionsHeader_ = submissionsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    submissionsHeader_->addStyleClass("admin-submissions-header");

    submissionsTitle_ = submissionsHeader_->addWidget(std::make_unique<Wt::WText>("Form Submissions"));
    submissionsTitle_->addStyleClass("admin-section-title");

    auto pdfButtonsContainer = submissionsHeader_->addWidget(std::make_unique<Wt::WContainerWidget>());
    pdfButtonsContainer->addStyleClass("admin-pdf-buttons");

    printAllBtn_ = pdfButtonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Print All Forms"));
    printAllBtn_->addStyleClass("btn btn-info");
    printAllBtn_->clicked().connect([this]() {
        if (currentStudentId_ > 0) {
            printAllFormsClicked_.emit(currentStudentId_);
        }
    });

    // Submissions table
    submissionsTable_ = submissionsContainer_->addWidget(std::make_unique<Wt::WTable>());
    submissionsTable_->addStyleClass("admin-table");

    // No submissions message
    noSubmissionsText_ = submissionsContainer_->addWidget(std::make_unique<Wt::WText>(
        "No form submissions found for this student."));
    noSubmissionsText_->addStyleClass("text-muted admin-no-data");
    noSubmissionsText_->hide();

    // Academic History section - outer container with checkbox
    academicHistorySectionContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    academicHistorySectionContainer_->addStyleClass("admin-submissions-section admin-academic-section");

    // Checkbox to toggle academic history visibility
    auto checkboxContainer = academicHistorySectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    checkboxContainer->addStyleClass("admin-academic-checkbox-container");

    hasPreviousEducationCheckbox_ = checkboxContainer->addWidget(std::make_unique<Wt::WCheckBox>("Has Previous Education"));
    hasPreviousEducationCheckbox_->addStyleClass("admin-academic-checkbox");
    hasPreviousEducationCheckbox_->changed().connect([this]() {
        if (hasPreviousEducationCheckbox_->isChecked()) {
            academicHistoryContainer_->show();
            loadAcademicHistory();
        } else {
            academicHistoryContainer_->hide();
        }
    });

    // Inner container for academic history content (hidden by default)
    academicHistoryContainer_ = academicHistorySectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    academicHistoryContainer_->addStyleClass("admin-academic-content");
    academicHistoryContainer_->hide();  // Hidden by default

    // Academic History header with title (read-only for admins - no add button)
    academicHistoryHeader_ = academicHistoryContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    academicHistoryHeader_->addStyleClass("admin-submissions-header");

    academicHistoryTitle_ = academicHistoryHeader_->addWidget(std::make_unique<Wt::WText>("🎓 Academic History"));
    academicHistoryTitle_->addStyleClass("admin-section-title");

    // Note: Add button removed - academic history is read-only for admins
    // Students complete their academic history through the intake forms

    // Academic History table
    academicHistoryTable_ = academicHistoryContainer_->addWidget(std::make_unique<Wt::WTable>());
    academicHistoryTable_->addStyleClass("admin-data-table");

    // No academic history message
    noAcademicHistoryText_ = academicHistoryContainer_->addWidget(std::make_unique<Wt::WText>(
        "No academic history records found for this student."));
    noAcademicHistoryText_->addStyleClass("text-muted admin-no-data");
    noAcademicHistoryText_->hide();
}

void StudentDetailWidget::loadStudent(int studentId) {
    if (!apiService_) {
        LOG_WARN("StudentDetail", "API service not available");
        return;
    }

    currentStudentId_ = studentId;

    try {
        LOG_DEBUG("StudentDetail", "Loading student: " << studentId);

        std::string endpoint = "/Student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (!response.success) {
            LOG_ERROR("StudentDetail", "Failed to load student: " << response.errorMessage);
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);

        if (jsonResponse.contains("data")) {
            auto& studentData = jsonResponse["data"];

            if (studentData.contains("id")) {
                if (studentData["id"].is_string()) {
                    currentStudent_.setId(studentData["id"].get<std::string>());
                } else {
                    currentStudent_.setId(std::to_string(studentData["id"].get<int>()));
                }
            }

            if (studentData.contains("attributes")) {
                auto& attrs = studentData["attributes"];

                if (attrs.contains("email") && !attrs["email"].is_null()) {
                    currentStudent_.setEmail(attrs["email"].get<std::string>());
                }
                if (attrs.contains("first_name") && !attrs["first_name"].is_null()) {
                    currentStudent_.setFirstName(attrs["first_name"].get<std::string>());
                }
                if (attrs.contains("last_name") && !attrs["last_name"].is_null()) {
                    currentStudent_.setLastName(attrs["last_name"].get<std::string>());
                }
                // Load phone_number (database field name)
                if (attrs.contains("phone_number") && !attrs["phone_number"].is_null()) {
                    currentStudent_.setPhoneNumber(attrs["phone_number"].get<std::string>());
                }
                if (attrs.contains("created_at") && !attrs["created_at"].is_null()) {
                    currentStudent_.setCreatedAt(attrs["created_at"].get<std::string>());
                }
                // Load intake_status
                if (attrs.contains("intake_status") && !attrs["intake_status"].is_null()) {
                    intakeStatus_ = attrs["intake_status"].get<std::string>();
                } else {
                    intakeStatus_ = "in_progress";
                }
                if (attrs.contains("is_login_revoked") && !attrs["is_login_revoked"].is_null()) {
                    isRevoked_ = attrs["is_login_revoked"].get<bool>();
                } else {
                    isRevoked_ = false;
                }
            }
        }

        updateDisplay();
        loadStudentAddress();
        loadFormSubmissions();
        // Academic history is loaded when checkbox is checked
        // Reset checkbox state - it will be unchecked by default
        hasPreviousEducationCheckbox_->setChecked(false);
        academicHistoryContainer_->hide();
        LOG_DEBUG("StudentDetail", "Loaded student: " << currentStudent_.getFullName());

    } catch (const std::exception& e) {
        LOG_ERROR("StudentDetail", "Exception loading student: " << e.what());
    }
}

void StudentDetailWidget::loadStudentAddress() {
    if (!apiService_ || currentStudentId_ <= 0) {
        addressText_->setText("Not provided");
        return;
    }

    try {
        // Load primary address from StudentAddress table
        std::string endpoint = "/StudentAddress?filter[student_id]=" + std::to_string(currentStudentId_);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json addresses = json.is_array() ? json :
                (json.contains("data") ? json["data"] : nlohmann::json::array());

            // Find primary address or use first one
            for (const auto& addr : addresses) {
                nlohmann::json attrs = addr.contains("attributes") ? addr["attributes"] : addr;

                std::string street1, street2, city, state, postalCode;

                if (attrs.contains("street1") && !attrs["street1"].is_null())
                    street1 = attrs["street1"].get<std::string>();
                if (attrs.contains("street2") && !attrs["street2"].is_null())
                    street2 = attrs["street2"].get<std::string>();
                if (attrs.contains("city") && !attrs["city"].is_null())
                    city = attrs["city"].get<std::string>();
                if (attrs.contains("state") && !attrs["state"].is_null())
                    state = attrs["state"].get<std::string>();
                if (attrs.contains("postal_code") && !attrs["postal_code"].is_null())
                    postalCode = attrs["postal_code"].get<std::string>();

                // Build address string
                std::string fullAddress;
                if (!street1.empty()) {
                    fullAddress = street1;
                    if (!street2.empty()) fullAddress += ", " + street2;
                    if (!city.empty()) fullAddress += ", " + city;
                    if (!state.empty()) fullAddress += ", " + state;
                    if (!postalCode.empty()) fullAddress += " " + postalCode;

                    addressText_->setText(fullAddress);
                    return; // Use first valid address found
                }
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("StudentDetail", "Error loading address: " << e.what());
    }

    addressText_->setText("Not provided");
}

void StudentDetailWidget::loadFormSubmissions() {
    formSubmissions_.clear();

    if (!apiService_ || currentStudentId_ <= 0) {
        updateFormSubmissionsTable();
        return;
    }

    try {
        // Form type mapping
        std::map<int, std::pair<std::string, std::string>> formTypeMap = {
            {1, {"personal_info", "Personal Information"}},
            {2, {"emergency_contact", "Emergency Contacts"}},
            {3, {"medical_info", "Medical Information"}},
            {4, {"academic_history", "Academic History"}},
            {5, {"financial_aid", "Financial Aid"}},
            {6, {"documents", "Document Upload"}},
            {7, {"consent", "Terms and Consent"}}
        };

        auto response = apiService_->getApiClient()->get(
            "/FormSubmission?filter[student_id]=" + std::to_string(currentStudentId_));

        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json submissions = json.is_array() ? json :
                (json.contains("data") ? json["data"] : nlohmann::json::array());

            for (const auto& sub : submissions) {
                nlohmann::json attrs = sub.contains("attributes") ? sub["attributes"] : sub;

                StudentFormRecord record;
                record.id = sub.contains("id") ?
                    (sub["id"].is_string() ? std::stoi(sub["id"].get<std::string>()) : sub["id"].get<int>()) : 0;
                record.formTypeId = attrs.value("form_type_id", 0);

                auto formIt = formTypeMap.find(record.formTypeId);
                record.formType = formIt != formTypeMap.end() ? formIt->second.first : "unknown";
                record.formName = formIt != formTypeMap.end() ? formIt->second.second : "Unknown Form";

                record.status = attrs.value("status", "pending");
                record.submittedAt = attrs.value("submitted_at", "");
                record.reviewedAt = attrs.value("reviewed_at", "");
                record.reviewedBy = attrs.value("reviewed_by", "");

                formSubmissions_.push_back(record);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("StudentDetail", "Error loading submissions: " << e.what());
    }

    updateFormSubmissionsTable();
}

void StudentDetailWidget::updateFormSubmissionsTable() {
    submissionsTable_->clear();

    if (formSubmissions_.empty()) {
        submissionsTable_->hide();
        noSubmissionsText_->show();
        printAllBtn_->hide();
        return;
    }

    submissionsTable_->show();
    noSubmissionsText_->hide();
    printAllBtn_->show();

    // Header row
    submissionsTable_->setHeaderCount(1);
    int col = 0;
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>(""));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Form"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Submitted"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"));
    submissionsTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < col; i++) {
        submissionsTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    int row = 1;
    for (const auto& sub : formSubmissions_) {
        col = 0;

        // Form icon
        auto iconCell = submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>("📋"));
        iconCell->addStyleClass("admin-row-icon form-icon");
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Form name
        submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(sub.formName));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Submitted date
        submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(formatDate(sub.submittedAt)));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Status
        std::string statusDisplay = sub.status;
        if (statusDisplay == "needs_revision") statusDisplay = "Needs Revision";
        else if (!statusDisplay.empty()) {
            statusDisplay[0] = std::toupper(statusDisplay[0]);
        }
        auto statusBadge = submissionsTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(statusDisplay));
        statusBadge->addStyleClass("badge " + getStatusBadgeClass(sub.status));
        submissionsTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Actions
        auto actionsContainer = submissionsTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        // Preview PDF button
        auto previewBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Preview"));
        previewBtn->addStyleClass("btn btn-sm btn-primary");
        int submissionId = sub.id;
        previewBtn->clicked().connect([this, submissionId]() {
            previewFormClicked_.emit(submissionId);
        });

        // Approve/Reject buttons for pending or needs_revision status
        if (sub.status == "pending" || sub.status == "needs_revision") {
            auto approveBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Approve"));
            approveBtn->addStyleClass("btn btn-sm btn-success");
            approveBtn->clicked().connect([this, submissionId]() {
                approveSubmission(submissionId);
            });

            auto rejectBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Reject"));
            rejectBtn->addStyleClass("btn btn-sm btn-danger");
            rejectBtn->clicked().connect([this, submissionId]() {
                rejectSubmission(submissionId);
            });
        }

        submissionsTable_->elementAt(row, col)->addStyleClass("admin-table-cell");
        row++;
    }
}

void StudentDetailWidget::loadAcademicHistory() {
    academicHistory_.clear();

    if (!apiService_ || currentStudentId_ <= 0) {
        updateAcademicHistoryTable();
        return;
    }

    try {
        std::string endpoint = "/AcademicHistory?filter[student_id]=" + std::to_string(currentStudentId_);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (response.success) {
            auto json = nlohmann::json::parse(response.body);
            nlohmann::json records = json.is_array() ? json :
                (json.contains("data") ? json["data"] : nlohmann::json::array());

            for (const auto& record : records) {
                nlohmann::json attrs = record.contains("attributes") ? record["attributes"] : record;

                AcademicHistoryRecord historyRecord;
                historyRecord.id = record.contains("id") ?
                    (record["id"].is_string() ? std::stoi(record["id"].get<std::string>()) : record["id"].get<int>()) : 0;

                historyRecord.institutionName = attrs.value("institution_name", "");
                historyRecord.institutionType = attrs.value("institution_type", "");
                historyRecord.institutionCity = attrs.value("institution_city", "");
                historyRecord.institutionState = attrs.value("institution_state", "");
                historyRecord.degreeEarned = attrs.value("degree_earned", "");
                historyRecord.major = attrs.value("major", "");
                historyRecord.gpa = attrs.value("gpa", 0.0);
                historyRecord.startDate = attrs.value("start_date", "");
                historyRecord.endDate = attrs.value("end_date", "");
                historyRecord.graduationDate = attrs.value("graduation_date", "");
                historyRecord.isCurrentlyAttending = attrs.value("is_currently_attending", false);

                academicHistory_.push_back(historyRecord);
            }
        }
        LOG_DEBUG("StudentDetail", "Loaded " << academicHistory_.size() << " academic history records");
    } catch (const std::exception& e) {
        LOG_ERROR("StudentDetail", "Error loading academic history: " << e.what());
    }

    updateAcademicHistoryTable();
}

void StudentDetailWidget::updateAcademicHistoryTable() {
    academicHistoryTable_->clear();

    if (academicHistory_.empty()) {
        academicHistoryTable_->hide();
        noAcademicHistoryText_->show();
        return;
    }

    academicHistoryTable_->show();
    noAcademicHistoryText_->hide();

    // Header row (read-only view - no Actions column)
    academicHistoryTable_->setHeaderCount(1);
    int col = 0;
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>(""));
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Institution"));
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Type"));
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Degree/Major"));
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("GPA"));
    academicHistoryTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Dates"));

    for (int i = 0; i < col; i++) {
        academicHistoryTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    int row = 1;
    for (const auto& record : academicHistory_) {
        col = 0;

        // Institution icon
        auto iconCell = academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>("🏫"));
        iconCell->addStyleClass("admin-row-icon");
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Institution name and location
        auto instContainer = academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto instName = instContainer->addWidget(std::make_unique<Wt::WText>(record.institutionName));
        instName->addStyleClass("admin-primary-text");
        std::string location = record.institutionCity;
        if (!record.institutionState.empty()) {
            if (!location.empty()) location += ", ";
            location += record.institutionState;
        }
        if (!location.empty()) {
            auto locText = instContainer->addWidget(std::make_unique<Wt::WText>(location));
            locText->addStyleClass("admin-secondary-text");
        }
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Institution type
        std::string typeDisplay = record.institutionType;
        if (!typeDisplay.empty()) {
            typeDisplay[0] = std::toupper(typeDisplay[0]);
        }
        academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(typeDisplay));
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Degree and Major
        auto degreeContainer = academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        if (!record.degreeEarned.empty()) {
            auto degreeText = degreeContainer->addWidget(std::make_unique<Wt::WText>(record.degreeEarned));
            degreeText->addStyleClass("admin-primary-text");
        }
        if (!record.major.empty()) {
            auto majorText = degreeContainer->addWidget(std::make_unique<Wt::WText>(record.major));
            majorText->addStyleClass("admin-secondary-text");
        }
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // GPA
        std::string gpaStr = record.gpa > 0 ? std::to_string(record.gpa).substr(0, 4) : "-";
        academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(gpaStr));
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Dates
        std::string dateStr;
        if (!record.startDate.empty()) {
            dateStr = formatDate(record.startDate);
            if (record.isCurrentlyAttending) {
                dateStr += " - Present";
            } else if (!record.endDate.empty()) {
                dateStr += " - " + formatDate(record.endDate);
            }
        } else if (!record.graduationDate.empty()) {
            dateStr = "Graduated: " + formatDate(record.graduationDate);
        } else {
            dateStr = "-";
        }
        academicHistoryTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(dateStr));
        academicHistoryTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        row++;
    }
}

void StudentDetailWidget::approveSubmission(int submissionId) {
    LOG_DEBUG("StudentDetail", "Approving submission: " << submissionId);

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "FormSubmission";
            payload["data"]["id"] = std::to_string(submissionId);
            payload["data"]["attributes"]["status"] = "approved";

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(submissionId), payload);

            if (response.isSuccess()) {
                LOG_INFO("StudentDetail", "Submission approved successfully");
                // Update local state
                for (auto& sub : formSubmissions_) {
                    if (sub.id == submissionId) {
                        sub.status = "approved";
                        break;
                    }
                }
                updateFormSubmissionsTable();
            } else {
                LOG_ERROR("StudentDetail", "Failed to approve submission: " << response.errorMessage);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("StudentDetail", "Error approving submission: " << e.what());
        }
    }
}

void StudentDetailWidget::rejectSubmission(int submissionId) {
    LOG_DEBUG("StudentDetail", "Rejecting submission: " << submissionId);

    if (apiService_) {
        try {
            nlohmann::json payload;
            payload["data"]["type"] = "FormSubmission";
            payload["data"]["id"] = std::to_string(submissionId);
            payload["data"]["attributes"]["status"] = "rejected";

            auto response = apiService_->getApiClient()->patch(
                "/FormSubmission/" + std::to_string(submissionId), payload);

            if (response.isSuccess()) {
                LOG_INFO("StudentDetail", "Submission rejected successfully");
                // Update local state
                for (auto& sub : formSubmissions_) {
                    if (sub.id == submissionId) {
                        sub.status = "rejected";
                        break;
                    }
                }
                updateFormSubmissionsTable();
            } else {
                LOG_ERROR("StudentDetail", "Failed to reject submission: " << response.errorMessage);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("StudentDetail", "Error rejecting submission: " << e.what());
        }
    }
}

void StudentDetailWidget::updateDisplay() {
    studentName_->setText(currentStudent_.getFullName());
    studentEmail_->setText(currentStudent_.getEmail());

    // Update status badge
    if (isRevoked_) {
        statusBadge_->setText("Revoked");
        statusBadge_->removeStyleClass("badge-success");
        statusBadge_->removeStyleClass("badge-warning");
        statusBadge_->addStyleClass("badge-danger");
        revokeBtn_->hide();
        restoreBtn_->show();
    } else {
        statusBadge_->setText("Active");
        statusBadge_->removeStyleClass("badge-danger");
        statusBadge_->removeStyleClass("badge-warning");
        statusBadge_->addStyleClass("badge-success");
        revokeBtn_->show();
        restoreBtn_->hide();
    }

    // Update info cards
    programText_->setText("Computer Science - BS"); // Placeholder
    enrolledText_->setText(formatDate(currentStudent_.getCreatedAt()));

    std::string phone = currentStudent_.getPhoneNumber();
    phoneText_->setText(phone.empty() ? "Not provided" : phone);

    // Format intake status for display with appropriate icon
    std::string statusDisplay = intakeStatus_;
    std::string statusIcon = "📋"; // Default icon
    if (statusDisplay == "in_progress") {
        statusDisplay = "In Progress";
        statusIcon = "⏳";
    } else if (statusDisplay == "completed") {
        statusDisplay = "Completed";
        statusIcon = "✅";
    } else if (statusDisplay == "pending_review") {
        statusDisplay = "Pending Review";
        statusIcon = "📝";
    } else if (statusDisplay == "approved") {
        statusDisplay = "Approved";
        statusIcon = "✅";
    } else if (!statusDisplay.empty()) {
        statusDisplay[0] = std::toupper(statusDisplay[0]);
    }
    intakeStatusLabel_->setText(statusIcon + " Intake Status");
    intakeStatusText_->setText(statusDisplay);
}

void StudentDetailWidget::onRevokeAccess() {
    LOG_INFO("StudentDetail", "Revoking access for student: " << currentStudent_.getId());
    revokeAccessClicked_.emit(std::stoi(currentStudent_.getId()));

    isRevoked_ = true;
    updateDisplay();
}

void StudentDetailWidget::onRestoreAccess() {
    LOG_INFO("StudentDetail", "Restoring access for student: " << currentStudent_.getId());
    restoreAccessClicked_.emit(std::stoi(currentStudent_.getId()));

    isRevoked_ = false;
    updateDisplay();
}

void StudentDetailWidget::clear() {
    currentStudent_ = ::StudentIntake::Models::Student();
    isRevoked_ = false;
    currentStudentId_ = 0;
    formSubmissions_.clear();
    academicHistory_.clear();
    intakeStatus_ = "in_progress";

    studentName_->setText("");
    studentEmail_->setText("");
    statusBadge_->setText("");
    programText_->setText("-");
    enrolledText_->setText("-");
    phoneText_->setText("-");
    addressText_->setText("-");
    intakeStatusText_->setText("-");

    submissionsTable_->clear();
    noSubmissionsText_->hide();

    // Reset academic history section
    hasPreviousEducationCheckbox_->setChecked(false);
    academicHistoryContainer_->hide();
    academicHistoryTable_->clear();
    noAcademicHistoryText_->hide();
}

std::string StudentDetailWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

std::string StudentDetailWidget::getStatusBadgeClass(const std::string& status) {
    if (status == "approved") return "badge-success";
    if (status == "pending") return "badge-warning";
    if (status == "rejected") return "badge-danger";
    if (status == "needs_revision") return "badge-info";
    return "badge-secondary";
}

} // namespace Admin
} // namespace StudentIntake
