#include "StudentFormViewer.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

StudentFormViewer::StudentFormViewer()
    : WContainerWidget()
    , apiService_(nullptr)
    , currentStudentId_(0)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , tableContainer_(nullptr)
    , formsTable_(nullptr)
    , noFormsMessage_(nullptr)
    , backBtn_(nullptr) {
    setupUI();
}

StudentFormViewer::~StudentFormViewer() {
}

void StudentFormViewer::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void StudentFormViewer::setupUI() {
    addStyleClass("admin-form-viewer");

    // Back button at top
    auto topBar = addWidget(std::make_unique<Wt::WContainerWidget>());
    topBar->addStyleClass("admin-detail-topbar");

    backBtn_ = topBar->addWidget(std::make_unique<Wt::WPushButton>("< Back to Student"));
    backBtn_->addStyleClass("btn btn-link");
    backBtn_->clicked().connect([this]() {
        backClicked_.emit();
    });

    // Header
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("admin-section-header");

    headerTitle_ = header->addWidget(std::make_unique<Wt::WText>("Submitted Forms"));
    headerTitle_->addStyleClass("admin-section-title");

    headerSubtitle_ = header->addWidget(std::make_unique<Wt::WText>(""));
    headerSubtitle_->addStyleClass("admin-section-subtitle");

    // Table container
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("admin-table-container");

    // No forms message (hidden by default)
    noFormsMessage_ = tableContainer_->addWidget(std::make_unique<Wt::WText>(
        "No forms have been submitted by this student yet."));
    noFormsMessage_->addStyleClass("admin-no-data-message");
    noFormsMessage_->hide();

    // Forms table
    formsTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    formsTable_->addStyleClass("admin-data-table");
    formsTable_->setHeaderCount(1);

    // Table headers
    formsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Form Name"));
    formsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Status"));
    formsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Submitted"));
    formsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Reviewed"));
    formsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < 5; i++) {
        formsTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }
}

void StudentFormViewer::loadStudentForms(int studentId, const std::string& studentName) {
    currentStudentId_ = studentId;
    currentStudentName_ = studentName;

    headerSubtitle_->setText("Forms submitted by " + studentName);

    forms_.clear();

    if (!apiService_) {
        std::cerr << "[FormViewer] API service not available" << std::endl;
        updateTable();
        return;
    }

    try {
        std::cerr << "[FormViewer] Loading forms for student: " << studentId << std::endl;

        // Query form submissions for this student
        std::string endpoint = "form_submission?filter[student_id]=" + std::to_string(studentId);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (!response.success) {
            std::cerr << "[FormViewer] Failed to load forms: " << response.errorMessage << std::endl;
            updateTable();
            return;
        }

        auto jsonResponse = nlohmann::json::parse(response.body);

        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            for (const auto& formData : jsonResponse["data"]) {
                FormSubmission form;

                if (formData.contains("id")) {
                    if (formData["id"].is_string()) {
                        form.id = std::stoi(formData["id"].get<std::string>());
                    } else {
                        form.id = formData["id"].get<int>();
                    }
                }

                if (formData.contains("attributes")) {
                    auto& attrs = formData["attributes"];

                    if (attrs.contains("form_name") && !attrs["form_name"].is_null()) {
                        form.formName = attrs["form_name"].get<std::string>();
                    } else {
                        form.formName = "Unknown Form";
                    }

                    if (attrs.contains("status") && !attrs["status"].is_null()) {
                        form.status = attrs["status"].get<std::string>();
                    } else {
                        form.status = "pending";
                    }

                    if (attrs.contains("submitted_at") && !attrs["submitted_at"].is_null()) {
                        form.submittedAt = attrs["submitted_at"].get<std::string>();
                    } else if (attrs.contains("created_at") && !attrs["created_at"].is_null()) {
                        form.submittedAt = attrs["created_at"].get<std::string>();
                    }

                    if (attrs.contains("reviewed_at") && !attrs["reviewed_at"].is_null()) {
                        form.reviewedAt = attrs["reviewed_at"].get<std::string>();
                    }

                    if (attrs.contains("reviewed_by") && !attrs["reviewed_by"].is_null()) {
                        form.reviewedBy = attrs["reviewed_by"].get<std::string>();
                    }
                }

                forms_.push_back(form);
            }
        }

        std::cerr << "[FormViewer] Loaded " << forms_.size() << " forms" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[FormViewer] Exception loading forms: " << e.what() << std::endl;
    }

    updateTable();
}

void StudentFormViewer::updateTable() {
    // Clear existing rows (except header)
    while (formsTable_->rowCount() > 1) {
        formsTable_->removeRow(1);
    }

    if (forms_.empty()) {
        formsTable_->hide();
        noFormsMessage_->show();
        return;
    }

    formsTable_->show();
    noFormsMessage_->hide();

    int row = 1;
    for (const auto& form : forms_) {
        // Form name
        formsTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>(form.formName));

        // Status badge
        auto statusBadge = formsTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>(form.status));
        statusBadge->addStyleClass(getStatusBadgeClass(form.status));

        // Submitted date
        formsTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(formatDate(form.submittedAt)));

        // Reviewed date
        std::string reviewedText = form.reviewedAt.empty() ? "-" : formatDate(form.reviewedAt);
        if (!form.reviewedBy.empty()) {
            reviewedText += " by " + form.reviewedBy;
        }
        formsTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(reviewedText));

        // Actions
        auto actionsContainer = formsTable_->elementAt(row, 4)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(
            std::make_unique<Wt::WPushButton>("View Details"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");

        int formId = form.id;
        viewBtn->clicked().connect([this, formId]() {
            viewFormDetailClicked_.emit(formId);
        });

        // Style cells
        for (int col = 0; col < 5; col++) {
            formsTable_->elementAt(row, col)->addStyleClass("admin-table-cell");
        }

        row++;
    }
}

void StudentFormViewer::clear() {
    currentStudentId_ = 0;
    currentStudentName_.clear();
    forms_.clear();
    headerSubtitle_->setText("");

    while (formsTable_->rowCount() > 1) {
        formsTable_->removeRow(1);
    }

    formsTable_->show();
    noFormsMessage_->hide();
}

std::string StudentFormViewer::getStatusBadgeClass(const std::string& status) {
    if (status == "approved" || status == "completed") return "badge badge-success";
    if (status == "pending" || status == "submitted") return "badge badge-warning";
    if (status == "rejected") return "badge badge-danger";
    if (status == "in_review") return "badge badge-info";
    return "badge badge-secondary";
}

std::string StudentFormViewer::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

} // namespace Admin
} // namespace StudentIntake
