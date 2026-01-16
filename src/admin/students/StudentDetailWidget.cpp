#include "StudentDetailWidget.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

StudentDetailWidget::StudentDetailWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , isRevoked_(false)
    , headerContainer_(nullptr)
    , studentName_(nullptr)
    , studentEmail_(nullptr)
    , statusBadge_(nullptr)
    , infoContainer_(nullptr)
    , programText_(nullptr)
    , enrolledText_(nullptr)
    , phoneText_(nullptr)
    , addressText_(nullptr)
    , actionsContainer_(nullptr)
    , viewFormsBtn_(nullptr)
    , revokeBtn_(nullptr)
    , restoreBtn_(nullptr)
    , backBtn_(nullptr) {
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
    auto enrolledLabel = enrolledCard->addWidget(std::make_unique<Wt::WText>("Enrolled Date"));
    enrolledLabel->addStyleClass("admin-info-label");
    enrolledText_ = enrolledCard->addWidget(std::make_unique<Wt::WText>("-"));
    enrolledText_->addStyleClass("admin-info-value");

    // Phone card
    auto phoneCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    phoneCard->addStyleClass("admin-info-card");
    auto phoneLabel = phoneCard->addWidget(std::make_unique<Wt::WText>("Phone"));
    phoneLabel->addStyleClass("admin-info-label");
    phoneText_ = phoneCard->addWidget(std::make_unique<Wt::WText>("-"));
    phoneText_->addStyleClass("admin-info-value");

    // Address card
    auto addressCard = infoContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    addressCard->addStyleClass("admin-info-card");
    auto addressLabel = addressCard->addWidget(std::make_unique<Wt::WText>("Address"));
    addressLabel->addStyleClass("admin-info-label");
    addressText_ = addressCard->addWidget(std::make_unique<Wt::WText>("-"));
    addressText_->addStyleClass("admin-info-value");

    // Actions section
    actionsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsContainer_->addStyleClass("admin-detail-actions");

    auto actionsTitle = actionsContainer_->addWidget(std::make_unique<Wt::WText>("Actions"));
    actionsTitle->addStyleClass("admin-actions-title");

    auto buttonsContainer = actionsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonsContainer->addStyleClass("admin-action-buttons");

    viewFormsBtn_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("View Submitted Forms"));
    viewFormsBtn_->addStyleClass("btn btn-primary");
    viewFormsBtn_->clicked().connect([this]() {
        viewFormsClicked_.emit(std::stoi(currentStudent_.getId()));
    });

    revokeBtn_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Revoke Access"));
    revokeBtn_->addStyleClass("btn btn-danger");
    revokeBtn_->clicked().connect([this]() {
        onRevokeAccess();
    });

    restoreBtn_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Restore Access"));
    restoreBtn_->addStyleClass("btn btn-success");
    restoreBtn_->clicked().connect([this]() {
        onRestoreAccess();
    });
    restoreBtn_->hide(); // Hidden by default
}

void StudentDetailWidget::loadStudent(int studentId) {
    if (!apiService_) {
        std::cerr << "[StudentDetail] API service not available" << std::endl;
        return;
    }

    try {
        std::cerr << "[StudentDetail] Loading student: " << studentId << std::endl;

        std::string endpoint = "student/" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (!response.success) {
            std::cerr << "[StudentDetail] Failed to load student: " << response.errorMessage << std::endl;
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
                if (attrs.contains("phone") && !attrs["phone"].is_null()) {
                    currentStudent_.setPhoneNumber(attrs["phone"].get<std::string>());
                }
                if (attrs.contains("created_at") && !attrs["created_at"].is_null()) {
                    currentStudent_.setCreatedAt(attrs["created_at"].get<std::string>());
                }
                if (attrs.contains("is_login_revoked") && !attrs["is_login_revoked"].is_null()) {
                    isRevoked_ = attrs["is_login_revoked"].get<bool>();
                } else {
                    isRevoked_ = false;
                }
            }
        }

        updateDisplay();
        std::cerr << "[StudentDetail] Loaded student: " << currentStudent_.getFullName() << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[StudentDetail] Exception loading student: " << e.what() << std::endl;
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

    addressText_->setText("Not provided"); // Address needs to be loaded separately
}

void StudentDetailWidget::onRevokeAccess() {
    std::cerr << "[StudentDetail] Revoking access for student: " << currentStudent_.getId() << std::endl;
    revokeAccessClicked_.emit(std::stoi(currentStudent_.getId()));

    // Update local state
    isRevoked_ = true;
    updateDisplay();
}

void StudentDetailWidget::onRestoreAccess() {
    std::cerr << "[StudentDetail] Restoring access for student: " << currentStudent_.getId() << std::endl;
    restoreAccessClicked_.emit(std::stoi(currentStudent_.getId()));

    // Update local state
    isRevoked_ = false;
    updateDisplay();
}

void StudentDetailWidget::clear() {
    currentStudent_ = ::StudentIntake::Models::Student();
    isRevoked_ = false;
    studentName_->setText("");
    studentEmail_->setText("");
    statusBadge_->setText("");
    programText_->setText("-");
    enrolledText_->setText("-");
    phoneText_->setText("-");
    addressText_->setText("-");
}

std::string StudentDetailWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

} // namespace Admin
} // namespace StudentIntake
