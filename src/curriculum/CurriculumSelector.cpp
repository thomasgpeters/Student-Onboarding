#include "CurriculumSelector.h"
#include <Wt/WBreak.h>
#include <Wt/WLabel.h>
#include <Wt/WRadioButton.h>
#include <Wt/WButtonGroup.h>

namespace StudentIntake {
namespace Curriculum {

CurriculumSelector::CurriculumSelector()
    : WContainerWidget()
    , searchInput_(nullptr)
    , departmentSelect_(nullptr)
    , degreeTypeSelect_(nullptr)
    , curriculumListContainer_(nullptr)
    , detailsContainer_(nullptr)
    , curriculumNameText_(nullptr)
    , curriculumDescText_(nullptr)
    , curriculumInfoText_(nullptr)
    , continueButton_(nullptr)
    , backButton_(nullptr) {
    setupUI();
}

CurriculumSelector::~CurriculumSelector() {
}

void CurriculumSelector::setupUI() {
    addStyleClass("curriculum-selector");

    // Title
    auto title = addWidget(std::make_unique<Wt::WText>("<h2>Select Your Program</h2>"));
    title->addStyleClass("section-title");

    // Description
    auto desc = addWidget(std::make_unique<Wt::WText>(
        "<p>Choose your intended program of study. This will determine which forms you need to complete.</p>"));
    desc->addStyleClass("section-description");

    // Filters container
    auto filtersContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    filtersContainer->addStyleClass("filters-container");

    // Search input
    auto searchGroup = filtersContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("form-group search-group");
    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WLabel>("Search Programs"));
    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Enter keywords...");
    searchInput_->addStyleClass("form-control");
    searchLabel->setBuddy(searchInput_);
    searchInput_->keyWentUp().connect(this, &CurriculumSelector::handleSearchChange);

    // Filter row
    auto filterRow = filtersContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    filterRow->addStyleClass("filter-row");

    // Department filter
    auto deptGroup = filterRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    deptGroup->addStyleClass("form-group filter-group");
    auto deptLabel = deptGroup->addWidget(std::make_unique<Wt::WLabel>("Department"));
    departmentSelect_ = deptGroup->addWidget(std::make_unique<Wt::WComboBox>());
    departmentSelect_->addStyleClass("form-control");
    deptLabel->setBuddy(departmentSelect_);
    departmentSelect_->changed().connect(this, &CurriculumSelector::handleDepartmentChange);

    // Degree type filter
    auto degreeGroup = filterRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    degreeGroup->addStyleClass("form-group filter-group");
    auto degreeLabel = degreeGroup->addWidget(std::make_unique<Wt::WLabel>("Degree Type"));
    degreeTypeSelect_ = degreeGroup->addWidget(std::make_unique<Wt::WComboBox>());
    degreeTypeSelect_->addStyleClass("form-control");
    degreeLabel->setBuddy(degreeTypeSelect_);
    degreeTypeSelect_->changed().connect(this, &CurriculumSelector::handleDegreeTypeChange);

    // Main content area
    auto contentArea = addWidget(std::make_unique<Wt::WContainerWidget>());
    contentArea->addStyleClass("content-area");

    // Curriculum list
    auto listSection = contentArea->addWidget(std::make_unique<Wt::WContainerWidget>());
    listSection->addStyleClass("list-section");
    listSection->addWidget(std::make_unique<Wt::WText>("<h4>Available Programs</h4>"));
    curriculumListContainer_ = listSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumListContainer_->addStyleClass("curriculum-list");

    // Details panel
    detailsContainer_ = contentArea->addWidget(std::make_unique<Wt::WContainerWidget>());
    detailsContainer_->addStyleClass("details-section");
    detailsContainer_->hide();

    auto detailsTitle = detailsContainer_->addWidget(std::make_unique<Wt::WText>("<h4>Program Details</h4>"));

    curriculumNameText_ = detailsContainer_->addWidget(std::make_unique<Wt::WText>());
    curriculumNameText_->addStyleClass("curriculum-name");

    curriculumDescText_ = detailsContainer_->addWidget(std::make_unique<Wt::WText>());
    curriculumDescText_->addStyleClass("curriculum-description");

    curriculumInfoText_ = detailsContainer_->addWidget(std::make_unique<Wt::WText>());
    curriculumInfoText_->addStyleClass("curriculum-info");

    // Button container
    auto buttonContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("button-container");

    backButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Back"));
    backButton_->addStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this]() {
        backRequested_.emit();
    });

    continueButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Continue"));
    continueButton_->addStyleClass("btn btn-primary");
    continueButton_->setEnabled(false);
    continueButton_->clicked().connect(this, &CurriculumSelector::handleContinue);
}

void CurriculumSelector::setCurriculumManager(std::shared_ptr<CurriculumManager> manager) {
    curriculumManager_ = manager;
    if (!curriculumManager_->isLoaded()) {
        curriculumManager_->loadCurriculums();
    }
    populateDepartments();
    populateDegreeTypes();
    updateCurriculumList();
}

void CurriculumSelector::populateDepartments() {
    if (!curriculumManager_) return;

    departmentSelect_->clear();
    departmentSelect_->addItem("All Departments");

    auto departments = curriculumManager_->getAllDepartments();
    for (const auto& dept : departments) {
        departmentSelect_->addItem(dept.name);
    }
}

void CurriculumSelector::populateDegreeTypes() {
    degreeTypeSelect_->clear();
    degreeTypeSelect_->addItem("All Degree Types");
    degreeTypeSelect_->addItem("Bachelor's");
    degreeTypeSelect_->addItem("Master's");
    degreeTypeSelect_->addItem("Doctoral");
    degreeTypeSelect_->addItem("Certificate");
    degreeTypeSelect_->addItem("Associate");
}

void CurriculumSelector::updateCurriculumList() {
    if (!curriculumManager_) return;

    curriculumListContainer_->clear();

    std::vector<Models::Curriculum> curriculums;
    std::string searchText = searchInput_->text().toUTF8();

    if (!searchText.empty()) {
        curriculums = curriculumManager_->searchCurriculums(searchText);
    } else {
        curriculums = curriculumManager_->getActiveCurriculums();
    }

    // Apply department filter
    int deptIndex = departmentSelect_->currentIndex();
    if (deptIndex > 0) {
        auto departments = curriculumManager_->getAllDepartments();
        if (deptIndex - 1 < static_cast<int>(departments.size())) {
            std::string deptId = departments[deptIndex - 1].id;
            std::vector<Models::Curriculum> filtered;
            for (const auto& c : curriculums) {
                if (c.getDepartment() == deptId) {
                    filtered.push_back(c);
                }
            }
            curriculums = filtered;
        }
    }

    // Apply degree type filter
    int degreeIndex = degreeTypeSelect_->currentIndex();
    if (degreeIndex > 0) {
        std::vector<std::string> degreeTypes = {"bachelor", "master", "doctoral", "certificate", "associate"};
        if (degreeIndex - 1 < static_cast<int>(degreeTypes.size())) {
            std::string degreeType = degreeTypes[degreeIndex - 1];
            std::vector<Models::Curriculum> filtered;
            for (const auto& c : curriculums) {
                if (c.getDegreeType() == degreeType) {
                    filtered.push_back(c);
                }
            }
            curriculums = filtered;
        }
    }

    if (curriculums.empty()) {
        auto noResults = curriculumListContainer_->addWidget(
            std::make_unique<Wt::WText>("No programs found matching your criteria."));
        noResults->addStyleClass("no-results");
        return;
    }

    // Create radio button group for curriculum selection
    auto buttonGroup = std::make_shared<Wt::WButtonGroup>();

    for (const auto& curriculum : curriculums) {
        auto itemContainer = curriculumListContainer_->addWidget(
            std::make_unique<Wt::WContainerWidget>());
        itemContainer->addStyleClass("curriculum-item");

        auto radio = itemContainer->addWidget(
            std::make_unique<Wt::WRadioButton>(curriculum.getName()));
        radio->addStyleClass("curriculum-radio");
        buttonGroup->addButton(radio);

        // Store curriculum ID as object name for lookup
        radio->setObjectName(curriculum.getId());

        // Get department name
        std::string deptName = "";
        auto dept = curriculumManager_->getDepartment(curriculum.getDepartment());
        if (!dept.id.empty()) {
            deptName = dept.name;
        }

        auto infoText = itemContainer->addWidget(std::make_unique<Wt::WText>(
            "<span class='curriculum-meta'>" + deptName +
            " | " + std::to_string(curriculum.getCreditHours()) + " credits</span>"));
        infoText->addStyleClass("curriculum-item-info");

        radio->checked().connect([this, curriculum]() {
            handleCurriculumSelect();
            showCurriculumDetails(curriculum);
            selectedCurriculum_ = curriculum;
            continueButton_->setEnabled(true);
        });
    }
}

void CurriculumSelector::handleDepartmentChange() {
    updateCurriculumList();
}

void CurriculumSelector::handleDegreeTypeChange() {
    updateCurriculumList();
}

void CurriculumSelector::handleSearchChange() {
    updateCurriculumList();
}

void CurriculumSelector::handleCurriculumSelect() {
    detailsContainer_->show();
}

void CurriculumSelector::showCurriculumDetails(const Models::Curriculum& curriculum) {
    curriculumNameText_->setText("<h3>" + curriculum.getName() + "</h3>");
    curriculumDescText_->setText("<p>" + curriculum.getDescription() + "</p>");

    std::string deptName = "";
    if (curriculumManager_) {
        auto dept = curriculumManager_->getDepartment(curriculum.getDepartment());
        if (!dept.id.empty()) {
            deptName = dept.name;
        }
    }

    std::string degreeLabel;
    std::string degreeType = curriculum.getDegreeType();
    if (degreeType == "bachelor") degreeLabel = "Bachelor's Degree";
    else if (degreeType == "master") degreeLabel = "Master's Degree";
    else if (degreeType == "doctoral") degreeLabel = "Doctoral Degree";
    else if (degreeType == "certificate") degreeLabel = "Certificate Program";
    else if (degreeType == "associate") degreeLabel = "Associate Degree";
    else degreeLabel = degreeType;

    std::string info = "<ul class='details-list'>";
    info += "<li><strong>Department:</strong> " + deptName + "</li>";
    info += "<li><strong>Degree Type:</strong> " + degreeLabel + "</li>";
    info += "<li><strong>Credit Hours:</strong> " + std::to_string(curriculum.getCreditHours()) + "</li>";
    info += "<li><strong>Duration:</strong> " + std::to_string(curriculum.getDurationSemesters()) + " semesters</li>";
    info += "<li><strong>Required Forms:</strong> " + std::to_string(curriculum.getRequiredForms().size()) + " forms</li>";
    info += "</ul>";

    curriculumInfoText_->setText(info);
}

void CurriculumSelector::handleContinue() {
    if (!selectedCurriculum_.getId().empty()) {
        if (session_) {
            session_->setCurrentCurriculum(selectedCurriculum_);
            session_->getStudent().setCurriculumId(selectedCurriculum_.getId());
        }
        curriculumSelected_.emit(selectedCurriculum_);
    }
}

void CurriculumSelector::refresh() {
    if (curriculumManager_) {
        curriculumManager_->refresh();
        populateDepartments();
        populateDegreeTypes();
        updateCurriculumList();
    }
}

void CurriculumSelector::selectCurriculum(const std::string& curriculumId) {
    if (curriculumManager_) {
        selectedCurriculum_ = curriculumManager_->getCurriculum(curriculumId);
        if (!selectedCurriculum_.getId().empty()) {
            showCurriculumDetails(selectedCurriculum_);
            detailsContainer_->show();
            continueButton_->setEnabled(true);
        }
    }
}

} // namespace Curriculum
} // namespace StudentIntake
