#include "CurriculumListWidget.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

// Use Curriculum from the main Models namespace
using StudentIntake::Models::Curriculum;

CurriculumListWidget::CurriculumListWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , filterContainer_(nullptr)
    , searchInput_(nullptr)
    , departmentFilter_(nullptr)
    , degreeTypeFilter_(nullptr)
    , statusFilter_(nullptr)
    , searchBtn_(nullptr)
    , resetBtn_(nullptr)
    , addBtn_(nullptr)
    , resultCount_(nullptr)
    , tableContainer_(nullptr)
    , curriculumTable_(nullptr)
    , noDataMessage_(nullptr) {
    setupUI();
}

CurriculumListWidget::~CurriculumListWidget() {
}

void CurriculumListWidget::setApiService(std::shared_ptr<Api::FormSubmissionService> apiService) {
    apiService_ = apiService;
}

void CurriculumListWidget::setupUI() {
    addStyleClass("admin-curriculum-list");

    // Header section
    auto headerSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection->addStyleClass("admin-section-header");

    auto headerRow = headerSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerRow->addStyleClass("admin-header-row");

    auto headerLeft = headerRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerLeft->addStyleClass("admin-header-left");

    headerTitle_ = headerLeft->addWidget(std::make_unique<Wt::WText>("Curriculum Management"));
    headerTitle_->addStyleClass("admin-section-title");

    headerSubtitle_ = headerLeft->addWidget(std::make_unique<Wt::WText>("Manage academic programs and their requirements"));
    headerSubtitle_->addStyleClass("admin-section-subtitle");

    auto headerRight = headerRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerRight->addStyleClass("admin-header-right");

    addBtn_ = headerRight->addWidget(std::make_unique<Wt::WPushButton>("+ Add Program"));
    addBtn_->addStyleClass("btn btn-primary");
    addBtn_->clicked().connect([this]() {
        addCurriculumClicked_.emit();
    });

    // Filter section
    filterContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer_->addStyleClass("admin-filter-container");

    // Search input
    auto searchGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group admin-filter-search");

    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WText>("Search"));
    searchLabel->addStyleClass("admin-filter-label");

    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search by name or code...");
    searchInput_->addStyleClass("admin-filter-input");
    searchInput_->enterPressed().connect(this, &CurriculumListWidget::applyFilters);

    // Department filter
    auto deptGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptGroup->addStyleClass("admin-filter-group");

    auto deptLabel = deptGroup->addWidget(std::make_unique<Wt::WText>("Department"));
    deptLabel->addStyleClass("admin-filter-label");

    departmentFilter_ = deptGroup->addWidget(std::make_unique<Wt::WComboBox>());
    departmentFilter_->addStyleClass("admin-filter-select");
    departmentFilter_->addItem("All Departments");
    departmentFilter_->addItem("Computer Science");
    departmentFilter_->addItem("Engineering");
    departmentFilter_->addItem("Business");
    departmentFilter_->addItem("Arts & Sciences");
    departmentFilter_->addItem("Health Sciences");
    departmentFilter_->changed().connect(this, &CurriculumListWidget::applyFilters);

    // Degree type filter
    auto degreeGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    degreeGroup->addStyleClass("admin-filter-group");

    auto degreeLabel = degreeGroup->addWidget(std::make_unique<Wt::WText>("Degree Type"));
    degreeLabel->addStyleClass("admin-filter-label");

    degreeTypeFilter_ = degreeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    degreeTypeFilter_->addStyleClass("admin-filter-select");
    degreeTypeFilter_->addItem("All Types");
    degreeTypeFilter_->addItem("Associate");
    degreeTypeFilter_->addItem("Bachelor");
    degreeTypeFilter_->addItem("Master");
    degreeTypeFilter_->addItem("Doctoral");
    degreeTypeFilter_->addItem("Certificate");
    degreeTypeFilter_->changed().connect(this, &CurriculumListWidget::applyFilters);

    // Status filter
    auto statusGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusGroup->addStyleClass("admin-filter-group");

    auto statusLabel = statusGroup->addWidget(std::make_unique<Wt::WText>("Status"));
    statusLabel->addStyleClass("admin-filter-label");

    statusFilter_ = statusGroup->addWidget(std::make_unique<Wt::WComboBox>());
    statusFilter_->addStyleClass("admin-filter-select");
    statusFilter_->addItem("All Status");
    statusFilter_->addItem("Active");
    statusFilter_->addItem("Inactive");
    statusFilter_->changed().connect(this, &CurriculumListWidget::applyFilters);

    // Filter buttons
    auto buttonGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons");

    searchBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Search"));
    searchBtn_->addStyleClass("btn btn-primary");
    searchBtn_->clicked().connect(this, &CurriculumListWidget::applyFilters);

    resetBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Reset"));
    resetBtn_->addStyleClass("btn btn-secondary");
    resetBtn_->clicked().connect(this, &CurriculumListWidget::resetFilters);

    // Result count
    resultCount_ = addWidget(std::make_unique<Wt::WText>(""));
    resultCount_->addStyleClass("admin-result-count");

    // Table container
    tableContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    tableContainer_->addStyleClass("admin-table-container");

    curriculumTable_ = tableContainer_->addWidget(std::make_unique<Wt::WTable>());
    curriculumTable_->addStyleClass("admin-data-table");

    // No data message
    noDataMessage_ = addWidget(std::make_unique<Wt::WText>("No programs found matching your criteria."));
    noDataMessage_->addStyleClass("admin-no-data-message");
    noDataMessage_->hide();
}

void CurriculumListWidget::refresh() {
    loadCurriculums();
}

void CurriculumListWidget::clear() {
    curriculums_.clear();
    filteredCurriculums_.clear();
    if (curriculumTable_) {
        curriculumTable_->clear();
    }
}

void CurriculumListWidget::loadCurriculums() {
    curriculums_.clear();

    if (!apiService_) {
        std::cerr << "[CurriculumListWidget] API service not available" << std::endl;
        // Use default data for testing
        Curriculum cs;
        cs.setId("1");
        cs.setName("Computer Science");
        cs.setDescription("Bachelor of Science in Computer Science");
        cs.setDepartment("Computer Science");
        cs.setDegreeType("bachelor");
        cs.setCreditHours(120);
        cs.setDurationSemesters(8);
        cs.setActive(true);
        cs.setOnline(false);
        cs.setRequiredForms({"personal_info", "academic_history", "consent"});
        curriculums_.push_back(cs);

        Curriculum ba;
        ba.setId("2");
        ba.setName("Business Administration");
        ba.setDescription("Bachelor of Business Administration");
        ba.setDepartment("Business");
        ba.setDegreeType("bachelor");
        ba.setCreditHours(120);
        ba.setDurationSemesters(8);
        ba.setActive(true);
        ba.setOnline(true);
        ba.setRequiredForms({"personal_info", "financial_aid", "consent"});
        curriculums_.push_back(ba);

        Curriculum mba;
        mba.setId("3");
        mba.setName("MBA Program");
        mba.setDescription("Master of Business Administration");
        mba.setDepartment("Business");
        mba.setDegreeType("master");
        mba.setCreditHours(60);
        mba.setDurationSemesters(4);
        mba.setActive(true);
        mba.setOnline(true);
        mba.setRequiredForms({"personal_info", "academic_history", "financial_aid", "consent"});
        curriculums_.push_back(mba);

        Curriculum nursing;
        nursing.setId("4");
        nursing.setName("Nursing");
        nursing.setDescription("Bachelor of Science in Nursing");
        nursing.setDepartment("Health Sciences");
        nursing.setDegreeType("bachelor");
        nursing.setCreditHours(128);
        nursing.setDurationSemesters(8);
        nursing.setActive(true);
        nursing.setOnline(false);
        nursing.setRequiredForms({"personal_info", "medical_info", "academic_history", "consent"});
        curriculums_.push_back(nursing);

        Curriculum cert;
        cert.setId("5");
        cert.setName("Data Analytics Certificate");
        cert.setDescription("Professional Certificate in Data Analytics");
        cert.setDepartment("Computer Science");
        cert.setDegreeType("certificate");
        cert.setCreditHours(18);
        cert.setDurationSemesters(2);
        cert.setActive(true);
        cert.setOnline(true);
        cert.setRequiredForms({"personal_info", "consent"});
        curriculums_.push_back(cert);

        Curriculum inactive;
        inactive.setId("6");
        inactive.setName("Legacy Engineering Program");
        inactive.setDescription("Old Engineering curriculum - no longer accepting students");
        inactive.setDepartment("Engineering");
        inactive.setDegreeType("bachelor");
        inactive.setCreditHours(130);
        inactive.setDurationSemesters(8);
        inactive.setActive(false);
        inactive.setOnline(false);
        inactive.setRequiredForms({"personal_info", "academic_history", "consent"});
        curriculums_.push_back(inactive);

        applyFilters();
        return;
    }

    try {
        auto response = apiService_->getApiClient()->get("/Curriculum");
        if (response.success) {
            auto jsonResponse = nlohmann::json::parse(response.body);
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                for (const auto& item : jsonResponse["data"]) {
                    curriculums_.push_back(Curriculum::fromJson(item));
                }
            }
        }
        std::cerr << "[CurriculumListWidget] Loaded " << curriculums_.size() << " curriculums" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[CurriculumListWidget] Error loading curriculums: " << e.what() << std::endl;
    }

    applyFilters();
}

void CurriculumListWidget::applyFilters() {
    filteredCurriculums_.clear();

    std::string searchText = searchInput_->text().toUTF8();
    std::transform(searchText.begin(), searchText.end(), searchText.begin(), ::tolower);

    int deptIndex = departmentFilter_->currentIndex();
    std::string selectedDept = (deptIndex > 0) ? departmentFilter_->currentText().toUTF8() : "";

    int degreeIndex = degreeTypeFilter_->currentIndex();
    std::string selectedDegree = "";
    if (degreeIndex > 0) {
        std::string text = degreeTypeFilter_->currentText().toUTF8();
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        selectedDegree = text;
    }

    int statusIndex = statusFilter_->currentIndex();

    for (const auto& curriculum : curriculums_) {
        // Search filter
        if (!searchText.empty()) {
            std::string name = curriculum.getName();
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            std::string desc = curriculum.getDescription();
            std::transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
            std::string id = curriculum.getId();
            std::transform(id.begin(), id.end(), id.begin(), ::tolower);

            if (name.find(searchText) == std::string::npos &&
                desc.find(searchText) == std::string::npos &&
                id.find(searchText) == std::string::npos) {
                continue;
            }
        }

        // Department filter
        if (!selectedDept.empty() && curriculum.getDepartment() != selectedDept) {
            continue;
        }

        // Degree type filter
        if (!selectedDegree.empty() && curriculum.getDegreeType() != selectedDegree) {
            continue;
        }

        // Status filter
        if (statusIndex == 1 && !curriculum.isActive()) {
            continue; // Active only
        }
        if (statusIndex == 2 && curriculum.isActive()) {
            continue; // Inactive only
        }

        filteredCurriculums_.push_back(curriculum);
    }

    updateTable();
}

void CurriculumListWidget::resetFilters() {
    searchInput_->setText("");
    departmentFilter_->setCurrentIndex(0);
    degreeTypeFilter_->setCurrentIndex(0);
    statusFilter_->setCurrentIndex(0);
    applyFilters();
}

void CurriculumListWidget::updateTable() {
    curriculumTable_->clear();

    if (filteredCurriculums_.empty()) {
        resultCount_->setText("No results");
        tableContainer_->hide();
        noDataMessage_->show();
        return;
    }

    noDataMessage_->hide();
    tableContainer_->show();
    resultCount_->setText("Showing " + std::to_string(filteredCurriculums_.size()) + " of " +
                          std::to_string(curriculums_.size()) + " programs");

    // Header row
    curriculumTable_->setHeaderCount(1);
    int col = 0;
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Program Name"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Department"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Degree"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Credits"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Duration"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Forms"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"))->addStyleClass("admin-table-header");
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"))->addStyleClass("admin-table-header");

    int row = 1;
    for (const auto& curriculum : filteredCurriculums_) {
        col = 0;

        // Program Name
        auto nameContainer = curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        auto nameText = nameContainer->addWidget(std::make_unique<Wt::WText>(curriculum.getName()));
        nameText->addStyleClass("admin-curriculum-name");
        if (curriculum.isOnline()) {
            auto onlineBadge = nameContainer->addWidget(std::make_unique<Wt::WText>(" Online"));
            onlineBadge->addStyleClass("badge badge-info admin-online-badge");
        }
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Department
        curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(curriculum.getDepartment()));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Degree Type
        std::string degreeType = curriculum.getDegreeType();
        // Capitalize first letter
        if (!degreeType.empty()) {
            degreeType[0] = std::toupper(degreeType[0]);
        }
        auto degreeBadge = curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(degreeType));
        degreeBadge->addStyleClass("badge " + getDegreeTypeBadgeClass(curriculum.getDegreeType()));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Credits
        curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(std::to_string(curriculum.getCreditHours())));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Duration
        std::string duration = std::to_string(curriculum.getDurationSemesters()) + " semesters";
        curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(duration));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Required Forms count
        std::string formsCount = std::to_string(curriculum.getRequiredForms().size()) + " required";
        curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>(formsCount));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Status
        auto statusBadge = curriculumTable_->elementAt(row, col)->addWidget(
            std::make_unique<Wt::WText>(curriculum.isActive() ? "Active" : "Inactive"));
        statusBadge->addStyleClass("badge " + getStatusBadgeClass(curriculum.isActive()));
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

        // Actions
        auto actionsContainer = curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WContainerWidget>());
        actionsContainer->addStyleClass("admin-table-actions");

        auto viewBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->addStyleClass("btn btn-sm btn-primary");
        std::string curriculumId = curriculum.getId();
        viewBtn->clicked().connect([this, curriculumId]() {
            curriculumSelected_.emit(curriculumId);
        });

        auto editBtn = actionsContainer->addWidget(std::make_unique<Wt::WPushButton>("Edit"));
        editBtn->addStyleClass("btn btn-sm btn-secondary");
        editBtn->clicked().connect([this, curriculumId]() {
            curriculumSelected_.emit(curriculumId);
        });

        curriculumTable_->elementAt(row, col)->addStyleClass("admin-table-cell");

        row++;
    }
}

std::string CurriculumListWidget::getStatusBadgeClass(bool isActive) {
    return isActive ? "badge-success" : "badge-secondary";
}

std::string CurriculumListWidget::getDegreeTypeBadgeClass(const std::string& degreeType) {
    if (degreeType == "bachelor") return "badge-info";
    if (degreeType == "master") return "badge-warning";
    if (degreeType == "doctoral") return "badge-danger";
    if (degreeType == "certificate") return "badge-secondary";
    if (degreeType == "associate") return "badge-success";
    return "badge-secondary";
}

} // namespace Admin
} // namespace StudentIntake
