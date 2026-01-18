#include "FormTypesListWidget.h"
#include <Wt/WPushButton.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

FormTypesListWidget::FormTypesListWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , statsContainer_(nullptr)
    , totalFormsText_(nullptr)
    , requiredFormsText_(nullptr)
    , optionalFormsText_(nullptr)
    , activeFormsText_(nullptr)
    , tableContainer_(nullptr)
    , formTypesTable_(nullptr)
    , noDataMessage_(nullptr) {
    setupUI();
}

FormTypesListWidget::~FormTypesListWidget() {
}

void FormTypesListWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void FormTypesListWidget::setupUI() {
    addStyleClass("admin-form-submissions");  // Use same styling as FormSubmissionsWidget

    // Header section
    auto headerContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer->addStyleClass("admin-section-header");

    headerTitle_ = headerContainer->addWidget(std::make_unique<Wt::WText>("Form Definitions"));
    headerTitle_->addStyleClass("admin-section-title");
    headerTitle_->setTextFormat(Wt::TextFormat::Plain);

    headerSubtitle_ = headerContainer->addWidget(std::make_unique<Wt::WText>(
        "View and manage form type definitions in the system"));
    headerSubtitle_->addStyleClass("admin-section-subtitle");

    // Stats cards - using consistent admin-stat-mini-card pattern
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Total forms card
    auto totalCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalCard->addStyleClass("admin-stat-mini-card today");
    auto totalIcon = totalCard->addWidget(std::make_unique<Wt::WText>("📋"));
    totalIcon->addStyleClass("admin-stat-mini-icon");
    totalFormsText_ = totalCard->addWidget(std::make_unique<Wt::WText>("0"));
    totalFormsText_->addStyleClass("admin-stat-mini-number");
    auto totalLabel = totalCard->addWidget(std::make_unique<Wt::WText>("Total Forms"));
    totalLabel->addStyleClass("admin-stat-mini-label");

    // Required forms card
    auto requiredCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    requiredCard->addStyleClass("admin-stat-mini-card pending");
    auto requiredIcon = requiredCard->addWidget(std::make_unique<Wt::WText>("*"));
    requiredIcon->addStyleClass("admin-stat-mini-icon");
    requiredFormsText_ = requiredCard->addWidget(std::make_unique<Wt::WText>("0"));
    requiredFormsText_->addStyleClass("admin-stat-mini-number");
    auto requiredLabel = requiredCard->addWidget(std::make_unique<Wt::WText>("Required"));
    requiredLabel->addStyleClass("admin-stat-mini-label");

    // Optional forms card
    auto optionalCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    optionalCard->addStyleClass("admin-stat-mini-card approved");
    auto optionalIcon = optionalCard->addWidget(std::make_unique<Wt::WText>("○"));
    optionalIcon->addStyleClass("admin-stat-mini-icon");
    optionalFormsText_ = optionalCard->addWidget(std::make_unique<Wt::WText>("0"));
    optionalFormsText_->addStyleClass("admin-stat-mini-number");
    auto optionalLabel = optionalCard->addWidget(std::make_unique<Wt::WText>("Optional"));
    optionalLabel->addStyleClass("admin-stat-mini-label");

    // Active forms card
    auto activeCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    activeCard->addStyleClass("admin-stat-mini-card active");
    auto activeIcon = activeCard->addWidget(std::make_unique<Wt::WText>("✓"));
    activeIcon->addStyleClass("admin-stat-mini-icon");
    activeFormsText_ = activeCard->addWidget(std::make_unique<Wt::WText>("0"));
    activeFormsText_->addStyleClass("admin-stat-mini-number");
    auto activeLabel = activeCard->addWidget(std::make_unique<Wt::WText>("Active"));
    activeLabel->addStyleClass("admin-stat-mini-label");

    // Table container
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("admin-table-container");

    formTypesTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    formTypesTable_->addStyleClass("admin-table");

    // No data message
    noDataMessage_ = addWidget(std::make_unique<Wt::WText>(
        "No form types found. Form definitions may need to be configured."));
    noDataMessage_->addStyleClass("text-muted admin-no-data");
    noDataMessage_->hide();
}

void FormTypesListWidget::loadData() {
    loadFormTypes();
}

void FormTypesListWidget::clearData() {
    formTypes_.clear();
    formTypesTable_->clear();
}

void FormTypesListWidget::loadFormTypes() {
    formTypes_.clear();

    if (!apiService_) {
        std::cerr << "[FormTypesList] API service not available" << std::endl;
        updateTable();
        return;
    }

    try {
        // Load form types from API
        auto response = apiService_->getApiClient()->get("/FormType");

        if (!response.success) {
            std::cerr << "[FormTypesList] Failed to load form types: " << response.errorMessage << std::endl;
            updateTable();
            return;
        }

        auto json = nlohmann::json::parse(response.body);

        // Handle both array and {data: [...]} formats
        nlohmann::json formTypesArray;
        if (json.is_array()) {
            formTypesArray = json;
        } else if (json.contains("data")) {
            formTypesArray = json["data"];
        } else {
            std::cerr << "[FormTypesList] Unexpected response format" << std::endl;
            updateTable();
            return;
        }

        for (const auto& ft : formTypesArray) {
            // Handle both direct and {attributes: {...}} formats
            nlohmann::json attrs = ft.contains("attributes") ? ft["attributes"] : ft;

            FormTypeRecord record;

            // Get ID
            if (ft.contains("id")) {
                if (ft["id"].is_string()) {
                    record.id = std::stoi(ft["id"].get<std::string>());
                } else {
                    record.id = ft["id"].get<int>();
                }
            } else {
                record.id = attrs.value("id", 0);
            }

            record.code = attrs.value("code", "");
            record.name = attrs.value("name", "");
            record.description = attrs.value("description", "");
            record.category = attrs.value("category", "");
            record.displayOrder = attrs.value("display_order", 0);
            record.isRequired = attrs.value("is_required", false);
            record.isActive = attrs.value("is_active", true);
            record.createdAt = attrs.value("created_at", "");
            record.updatedAt = attrs.value("updated_at", "");

            formTypes_.push_back(record);
        }

        // Sort by display order
        std::sort(formTypes_.begin(), formTypes_.end(),
            [](const FormTypeRecord& a, const FormTypeRecord& b) {
                return a.displayOrder < b.displayOrder;
            });

        std::cerr << "[FormTypesList] Loaded " << formTypes_.size() << " form types" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[FormTypesList] Exception loading form types: " << e.what() << std::endl;
    }

    updateTable();
}

void FormTypesListWidget::updateTable() {
    formTypesTable_->clear();

    // Update stats
    int totalCount = formTypes_.size();
    int requiredCount = 0;
    int optionalCount = 0;
    int activeCount = 0;

    for (const auto& ft : formTypes_) {
        if (ft.isRequired) requiredCount++;
        else optionalCount++;
        if (ft.isActive) activeCount++;
    }

    totalFormsText_->setText(std::to_string(totalCount));
    requiredFormsText_->setText(std::to_string(requiredCount));
    optionalFormsText_->setText(std::to_string(optionalCount));
    activeFormsText_->setText(std::to_string(activeCount));

    if (formTypes_.empty()) {
        formTypesTable_->hide();
        noDataMessage_->show();
        return;
    }

    formTypesTable_->show();
    noDataMessage_->hide();

    // Header row
    formTypesTable_->setHeaderCount(1);
    int col = 0;
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Order"));
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Form Name"));
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Category"));
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Required"));
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"));
    formTypesTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"));

    for (int i = 0; i < col; i++) {
        formTypesTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    // Data rows
    int row = 1;
    for (const auto& ft : formTypes_) {
        col = 0;

        // Display order
        formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(std::to_string(ft.displayOrder)));
        formTypesTable_->elementAt(row, col++)->addStyleClass("admin-table-cell text-center");

        // Form name with code
        auto nameContainer = formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        auto nameText = nameContainer->addWidget(std::make_unique<Wt::WText>(ft.name));
        nameText->addStyleClass("admin-primary-text");
        auto codeText = nameContainer->addWidget(std::make_unique<Wt::WText>(ft.code));
        codeText->addStyleClass("admin-secondary-text");
        formTypesTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Category
        auto categoryBadge = formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(getCategoryDisplayName(ft.category)));
        categoryBadge->addStyleClass("badge badge-secondary");
        formTypesTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Required
        auto requiredBadge = formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(ft.isRequired ? "Yes" : "No"));
        requiredBadge->addStyleClass(ft.isRequired ? "badge badge-primary" : "badge badge-light");
        formTypesTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Status
        auto statusBadge = formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(ft.isActive ? "Active" : "Inactive"));
        statusBadge->addStyleClass(ft.isActive ? "badge badge-success" : "badge badge-danger");
        formTypesTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Actions
        auto actionsContainer = formTypesTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("View Details"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");
        int formTypeId = ft.id;
        viewBtn->clicked().connect([this, formTypeId]() {
            formTypeSelected_.emit(formTypeId);
        });

        formTypesTable_->elementAt(row, col)->addStyleClass("admin-table-cell");
        row++;
    }
}

std::string FormTypesListWidget::formatDate(const std::string& dateStr) {
    if (dateStr.empty()) return "-";

    if (dateStr.length() >= 10) {
        return dateStr.substr(0, 10);
    }
    return dateStr;
}

std::string FormTypesListWidget::getCategoryDisplayName(const std::string& category) {
    if (category == "core") return "Core";
    if (category == "health") return "Health";
    if (category == "academic") return "Academic";
    if (category == "financial") return "Financial";
    if (category == "documents") return "Documents";
    if (category == "legal") return "Legal";
    return category.empty() ? "General" : category;
}

} // namespace Admin
} // namespace StudentIntake
