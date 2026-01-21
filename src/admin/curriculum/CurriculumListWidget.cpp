#include "CurriculumListWidget.h"
#include <Wt/WBreak.h>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Admin {

// Use Curriculum from the main Models namespace
using StudentIntake::Models::Curriculum;

CurriculumListWidget::CurriculumListWidget()
    : WContainerWidget()
    , apiService_(nullptr)
    , statsContainer_(nullptr)
    , activeCountText_(nullptr)
    , inactiveCountText_(nullptr)
    , onlineCountText_(nullptr)
    , onCampusCountText_(nullptr)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , filterContainer_(nullptr)
    , searchInput_(nullptr)
    , departmentFilter_(nullptr)
    , degreeTypeFilter_(nullptr)
    , statusFilter_(nullptr)
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

    // Statistics cards
    setupStats();

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
    searchInput_->textInput().connect(this, &CurriculumListWidget::applyFilters);

    // Department filter
    auto deptGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptGroup->addStyleClass("admin-filter-group");

    auto deptLabel = deptGroup->addWidget(std::make_unique<Wt::WText>("Department"));
    deptLabel->addStyleClass("admin-filter-label");

    departmentFilter_ = deptGroup->addWidget(std::make_unique<Wt::WComboBox>());
    departmentFilter_->addStyleClass("admin-filter-select");
    departmentFilter_->addItem("All Departments");  // Will be repopulated by loadData()
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

    // Clear button only - filters apply automatically on change
    auto buttonGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons admin-filter-buttons-right");

    resetBtn_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
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

void CurriculumListWidget::setupStats() {
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Active card
    auto activeCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    activeCard->addStyleClass("admin-stat-mini-card active");
    auto activeIcon = activeCard->addWidget(std::make_unique<Wt::WText>("✓"));
    activeIcon->addStyleClass("admin-stat-mini-icon");
    activeCountText_ = activeCard->addWidget(std::make_unique<Wt::WText>("0"));
    activeCountText_->addStyleClass("admin-stat-mini-number");
    auto activeLabel = activeCard->addWidget(std::make_unique<Wt::WText>("Active"));
    activeLabel->addStyleClass("admin-stat-mini-label");

    // Inactive card
    auto inactiveCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    inactiveCard->addStyleClass("admin-stat-mini-card inactive");
    auto inactiveIcon = inactiveCard->addWidget(std::make_unique<Wt::WText>("○"));
    inactiveIcon->addStyleClass("admin-stat-mini-icon");
    inactiveCountText_ = inactiveCard->addWidget(std::make_unique<Wt::WText>("0"));
    inactiveCountText_->addStyleClass("admin-stat-mini-number");
    auto inactiveLabel = inactiveCard->addWidget(std::make_unique<Wt::WText>("Inactive"));
    inactiveLabel->addStyleClass("admin-stat-mini-label");

    // Online card
    auto onlineCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    onlineCard->addStyleClass("admin-stat-mini-card online");
    auto onlineIcon = onlineCard->addWidget(std::make_unique<Wt::WText>("🌐"));
    onlineIcon->addStyleClass("admin-stat-mini-icon");
    onlineCountText_ = onlineCard->addWidget(std::make_unique<Wt::WText>("0"));
    onlineCountText_->addStyleClass("admin-stat-mini-number");
    auto onlineLabel = onlineCard->addWidget(std::make_unique<Wt::WText>("Online"));
    onlineLabel->addStyleClass("admin-stat-mini-label");

    // On-Campus card
    auto onCampusCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    onCampusCard->addStyleClass("admin-stat-mini-card approved");
    auto onCampusIcon = onCampusCard->addWidget(std::make_unique<Wt::WText>("🏛"));
    onCampusIcon->addStyleClass("admin-stat-mini-icon");
    onCampusCountText_ = onCampusCard->addWidget(std::make_unique<Wt::WText>("0"));
    onCampusCountText_->addStyleClass("admin-stat-mini-number");
    auto onCampusLabel = onCampusCard->addWidget(std::make_unique<Wt::WText>("On-Campus"));
    onCampusLabel->addStyleClass("admin-stat-mini-label");
}

void CurriculumListWidget::loadData() {
    loadDepartments();
    loadCurriculums();
}

void CurriculumListWidget::loadDepartments() {
    departmentMap_.clear();

    if (!apiService_) return;

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

                int deptId = 0;
                std::string deptName;

                // Get ID
                if (item.contains("id")) {
                    if (item["id"].is_number()) {
                        deptId = item["id"].get<int>();
                    } else if (item["id"].is_string()) {
                        deptId = std::stoi(item["id"].get<std::string>());
                    }
                }
                // Get name
                if (attrs.contains("name") && !attrs["name"].is_null()) {
                    deptName = attrs["name"].get<std::string>();
                }

                if (deptId > 0 && !deptName.empty()) {
                    departmentMap_[deptId] = deptName;
                }
            }

            std::cerr << "[CurriculumListWidget] Loaded " << departmentMap_.size() << " departments" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CurriculumListWidget] Error loading departments: " << e.what() << std::endl;
    }
}

void CurriculumListWidget::clearData() {
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
        // Use default data for testing - vocational/adult education programs
        Curriculum cdl;
        cdl.setId("1");
        cdl.setName("Commercial Driver's License (CDL)");
        cdl.setDescription("Professional truck driving certification program");
        cdl.setDepartment("Professional Driving School");
        cdl.setDegreeType("certificate");
        cdl.setCreditHours(160);
        cdl.setDurationSemesters(1);
        cdl.setActive(true);
        cdl.setOnline(false);
        cdl.setRequiredForms({"personal_info", "medical_info", "documents", "consent"});
        curriculums_.push_back(cdl);

        Curriculum itFund;
        itFund.setId("2");
        itFund.setName("IT Fundamentals");
        itFund.setDescription("Introduction to information technology and computer systems");
        itFund.setDepartment("Information Technology");
        itFund.setDegreeType("certificate");
        itFund.setCreditHours(120);
        itFund.setDurationSemesters(2);
        itFund.setActive(true);
        itFund.setOnline(true);
        itFund.setRequiredForms({"personal_info", "consent"});
        curriculums_.push_back(itFund);

        Curriculum electrical;
        electrical.setId("3");
        electrical.setName("Electrical Technician");
        electrical.setDescription("Hands-on electrical installation and maintenance training");
        electrical.setDepartment("Electrical Technology");
        electrical.setDegreeType("certificate");
        electrical.setCreditHours(200);
        electrical.setDurationSemesters(3);
        electrical.setActive(true);
        electrical.setOnline(false);
        electrical.setRequiredForms({"personal_info", "medical_info", "consent"});
        curriculums_.push_back(electrical);

        Curriculum nursing;
        nursing.setId("4");
        nursing.setName("Certified Nursing Assistant (CNA)");
        nursing.setDescription("Entry-level nursing assistant certification");
        nursing.setDepartment("Nursing");
        nursing.setDegreeType("certificate");
        nursing.setCreditHours(150);
        nursing.setDurationSemesters(2);
        nursing.setActive(true);
        nursing.setOnline(false);
        nursing.setRequiredForms({"personal_info", "medical_info", "academic_history", "consent"});
        curriculums_.push_back(nursing);

        Curriculum culinary;
        culinary.setId("5");
        culinary.setName("Culinary Arts");
        culinary.setDescription("Professional cooking and food preparation training");
        culinary.setDepartment("Food Services");
        culinary.setDegreeType("certificate");
        culinary.setCreditHours(180);
        culinary.setDurationSemesters(2);
        culinary.setActive(true);
        culinary.setOnline(false);
        culinary.setRequiredForms({"personal_info", "medical_info", "consent"});
        curriculums_.push_back(culinary);

        Curriculum automotive;
        automotive.setId("6");
        automotive.setName("Automotive Technology");
        automotive.setDescription("Vehicle maintenance and repair certification");
        automotive.setDepartment("Automotive");
        automotive.setDegreeType("certificate");
        automotive.setCreditHours(240);
        automotive.setDurationSemesters(3);
        automotive.setActive(true);
        automotive.setOnline(false);
        automotive.setRequiredForms({"personal_info", "consent"});
        curriculums_.push_back(automotive);

        applyFilters();
        return;
    }

    try {
        auto response = apiService_->getApiClient()->get("/Curriculum");
        if (response.success) {
            auto jsonResponse = nlohmann::json::parse(response.body);
            nlohmann::json items;
            if (jsonResponse.is_array()) {
                items = jsonResponse;
            } else if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                items = jsonResponse["data"];
            }
            for (const auto& item : items) {
                curriculums_.push_back(Curriculum::fromJson(item));
            }
        }
        std::cerr << "[CurriculumListWidget] Loaded " << curriculums_.size() << " curriculums" << std::endl;

        // Map department_id to department name using loaded departments
        for (auto& curriculum : curriculums_) {
            int deptId = curriculum.getDepartmentId();
            if (deptId > 0 && departmentMap_.count(deptId) > 0) {
                curriculum.setDepartment(departmentMap_[deptId]);
            }
        }

        // Load all form requirements from junction table and map to curriculums
        loadAllFormRequirements();

    } catch (const std::exception& e) {
        std::cerr << "[CurriculumListWidget] Error loading curriculums: " << e.what() << std::endl;
    }

    // Populate department filter with distinct departments from loaded curricula
    std::set<std::string> departments;
    for (const auto& curriculum : curriculums_) {
        if (!curriculum.getDepartment().empty()) {
            departments.insert(curriculum.getDepartment());
        }
    }
    departmentFilter_->clear();
    departmentFilter_->addItem("All Departments");
    for (const auto& dept : departments) {
        departmentFilter_->addItem(dept);
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
    updateStats();
}

void CurriculumListWidget::updateStats() {
    int activeCount = 0, inactiveCount = 0, onlineCount = 0, onCampusCount = 0;

    for (const auto& curriculum : curriculums_) {
        if (curriculum.isActive()) {
            activeCount++;
        } else {
            inactiveCount++;
        }

        if (curriculum.isOnline()) {
            onlineCount++;
        } else {
            onCampusCount++;
        }
    }

    activeCountText_->setText(std::to_string(activeCount));
    inactiveCountText_->setText(std::to_string(inactiveCount));
    onlineCountText_->setText(std::to_string(onlineCount));
    onCampusCountText_->setText(std::to_string(onCampusCount));
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

    // Header row - icon column first
    curriculumTable_->setHeaderCount(1);
    int col = 0;
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>(""));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Program Name"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Department"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Degree"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Credits"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Duration"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Forms"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Status"));
    curriculumTable_->elementAt(0, col++)->addWidget(std::make_unique<Wt::WText>("Actions"));

    // Apply header styling to cells
    for (int i = 0; i < col; i++) {
        curriculumTable_->elementAt(0, i)->addStyleClass("admin-table-header");
    }

    int row = 1;
    for (const auto& curriculum : filteredCurriculums_) {
        col = 0;

        // Curriculum icon
        auto iconCell = curriculumTable_->elementAt(row, col)->addWidget(std::make_unique<Wt::WText>("📚"));
        iconCell->addStyleClass("admin-row-icon curriculum-icon");
        curriculumTable_->elementAt(row, col++)->addStyleClass("admin-table-cell");

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

void CurriculumListWidget::loadAllFormRequirements() {
    if (!apiService_ || curriculums_.empty()) return;

    try {
        // Fetch all form requirements at once
        auto response = apiService_->getApiClient()->get("/CurriculumFormRequirement");
        if (!response.success) return;

        auto jsonResponse = nlohmann::json::parse(response.body);
        nlohmann::json items;
        if (jsonResponse.is_array()) {
            items = jsonResponse;
        } else if (jsonResponse.contains("data")) {
            items = jsonResponse["data"];
        }

        // Build a map of curriculum_id -> list of form_type_ids
        std::map<int, std::vector<int>> curriculumForms;
        for (const auto& item : items) {
            nlohmann::json attrs = item.contains("attributes") ? item["attributes"] : item;
            if (attrs.contains("curriculum_id") && attrs.contains("form_type_id")) {
                int currId = attrs["curriculum_id"].get<int>();
                int formTypeId = attrs["form_type_id"].get<int>();
                curriculumForms[currId].push_back(formTypeId);
            }
        }

        // Update each curriculum with its required forms
        for (auto& curriculum : curriculums_) {
            int currId = 0;
            try {
                currId = std::stoi(curriculum.getId());
            } catch (...) {
                continue;
            }

            if (curriculumForms.count(currId)) {
                std::vector<std::string> formIds;
                for (int typeId : curriculumForms[currId]) {
                    std::string formId = Curriculum::typeIdToFormId(typeId);
                    if (!formId.empty()) {
                        formIds.push_back(formId);
                    }
                }
                curriculum.setRequiredForms(formIds);
            }
        }

        std::cerr << "[CurriculumListWidget] Loaded form requirements for "
                  << curriculumForms.size() << " curriculums" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "[CurriculumListWidget] Error loading form requirements: " << e.what() << std::endl;
    }
}

} // namespace Admin
} // namespace StudentIntake
