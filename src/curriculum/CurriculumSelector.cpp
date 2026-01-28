#include "CurriculumSelector.h"
#include "utils/Logger.h"
#include <Wt/WApplication.h>
#include <Wt/WBreak.h>
#include <Wt/WLabel.h>
#include <Wt/WCheckBox.h>
#include <Wt/Utils.h>
#include <algorithm>

namespace StudentIntake {
namespace Curriculum {

CurriculumSelector::CurriculumSelector()
    : WContainerWidget()
    , baseSelectionContainer_(nullptr)
    , searchInput_(nullptr)
    , departmentSelect_(nullptr)
    , degreeTypeSelect_(nullptr)
    , curriculumCardsContainer_(nullptr)
    , endorsementSelectionContainer_(nullptr)
    , endorsementCardsContainer_(nullptr)
    , selectedBaseText_(nullptr) {
    setupUI();
}

CurriculumSelector::~CurriculumSelector() {
}

void CurriculumSelector::setupUI() {
    addStyleClass("curriculum-selector");

    // ============ BASE SELECTION CONTAINER ============
    baseSelectionContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    baseSelectionContainer_->addStyleClass("base-selection-container");

    // Title
    auto title = baseSelectionContainer_->addWidget(std::make_unique<Wt::WText>("<h2>Select Your Program</h2>"));
    title->addStyleClass("section-title");
    title->setTextFormat(Wt::TextFormat::XHTML);

    // Description
    auto desc = baseSelectionContainer_->addWidget(std::make_unique<Wt::WText>(
        "<p>Choose your intended program of study. This will determine which forms you need to complete.</p>"));
    desc->addStyleClass("section-description");
    desc->setTextFormat(Wt::TextFormat::XHTML);

    // Filters container - styled like a card
    auto filtersContainer = baseSelectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    filtersContainer->addStyleClass("filters-container card");

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

    // Programs grid container
    auto programsSection = baseSelectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    programsSection->addStyleClass("programs-section");

    auto programsTitle = programsSection->addWidget(std::make_unique<Wt::WText>("<h4>Available Programs</h4>"));
    programsTitle->addStyleClass("programs-title");
    programsTitle->setTextFormat(Wt::TextFormat::XHTML);

    curriculumCardsContainer_ = programsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumCardsContainer_->addStyleClass("curriculum-cards-grid");
    curriculumCardsContainer_->setAttributeValue("style",
        "display: grid; grid-template-columns: repeat(3, 1fr); gap: 1.5rem; width: 100%;");

    // ============ ENDORSEMENT SELECTION CONTAINER ============
    endorsementSelectionContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    endorsementSelectionContainer_->addStyleClass("endorsement-selection-container");
    endorsementSelectionContainer_->hide();  // Initially hidden

    // Endorsement title
    auto endorseTitle = endorsementSelectionContainer_->addWidget(
        std::make_unique<Wt::WText>("<h2>Select Additional Endorsements</h2>"));
    endorseTitle->addStyleClass("section-title");
    endorseTitle->setTextFormat(Wt::TextFormat::XHTML);

    // Selected base program info
    auto baseInfoContainer = endorsementSelectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    baseInfoContainer->addStyleClass("selected-base-info card");
    baseInfoContainer->setAttributeValue("style",
        "background: #eff6ff; border: 1px solid #bfdbfe; border-radius: 8px; padding: 1rem; margin-bottom: 1.5rem;");

    selectedBaseText_ = baseInfoContainer->addWidget(std::make_unique<Wt::WText>(""));
    selectedBaseText_->setTextFormat(Wt::TextFormat::XHTML);

    // Description for endorsements
    auto endorseDesc = endorsementSelectionContainer_->addWidget(std::make_unique<Wt::WText>(
        "<p>Select any additional endorsements you wish to add to your program. "
        "Endorsements are optional but may be required for specific job roles.</p>"));
    endorseDesc->addStyleClass("section-description");
    endorseDesc->setTextFormat(Wt::TextFormat::XHTML);

    // Endorsement cards container
    auto endorseSection = endorsementSelectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    endorseSection->addStyleClass("endorsements-section");

    auto endorseListTitle = endorseSection->addWidget(
        std::make_unique<Wt::WText>("<h4>Available Endorsements</h4>"));
    endorseListTitle->addStyleClass("programs-title");
    endorseListTitle->setTextFormat(Wt::TextFormat::XHTML);

    endorsementCardsContainer_ = endorseSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    endorsementCardsContainer_->addStyleClass("endorsement-cards-grid");
    endorsementCardsContainer_->setAttributeValue("style",
        "display: grid; grid-template-columns: repeat(2, 1fr); gap: 1rem; width: 100%;");

    // Button row for endorsement selection
    auto buttonRow = endorsementSelectionContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonRow->addStyleClass("endorsement-buttons");
    buttonRow->setAttributeValue("style",
        "display: flex; justify-content: space-between; margin-top: 2rem; padding-top: 1.5rem; border-top: 1px solid #e2e8f0;");

    auto backBtn = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Back to Programs"));
    backBtn->addStyleClass("btn btn-secondary");
    backBtn->setAttributeValue("style",
        "padding: 0.75rem 1.5rem; background-color: #64748b; color: white; border: none; "
        "border-radius: 8px; font-size: 0.95rem; font-weight: 500; cursor: pointer;");
    backBtn->clicked().connect(this, &CurriculumSelector::handleBackToBaseSelection);

    auto continueBtn = buttonRow->addWidget(std::make_unique<Wt::WPushButton>("Continue"));
    continueBtn->addStyleClass("btn btn-primary");
    continueBtn->setAttributeValue("style",
        "padding: 0.75rem 2rem; background-color: #2563eb; color: white; border: none; "
        "border-radius: 8px; font-size: 0.95rem; font-weight: 500; cursor: pointer;");
    continueBtn->clicked().connect(this, &CurriculumSelector::handleContinueWithEndorsements);
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

bool CurriculumSelector::isVocationalMode() const {
    if (!curriculumManager_) return false;

    // Check if any curriculum has is_endorsement flag set (vocational mode indicator)
    auto curriculums = curriculumManager_->getActiveCurriculums();
    for (const auto& c : curriculums) {
        if (c.isEndorsement()) {
            return true;
        }
        // Also check for CDL class as indicator of vocational mode
        if (!c.getCdlClass().empty()) {
            return true;
        }
    }
    return false;
}

void CurriculumSelector::updateCurriculumList() {
    if (!curriculumManager_) return;

    curriculumCardsContainer_->clear();

    std::vector<Models::Curriculum> curriculums;
    std::string searchText = searchInput_->text().toUTF8();

    if (!searchText.empty()) {
        curriculums = curriculumManager_->searchCurriculums(searchText);
    } else {
        curriculums = curriculumManager_->getActiveCurriculums();
    }

    // In vocational mode, filter out endorsements (show only base programs)
    bool vocationalMode = isVocationalMode();
    if (vocationalMode) {
        std::vector<Models::Curriculum> basePrograms;
        for (const auto& c : curriculums) {
            if (!c.isEndorsement()) {
                basePrograms.push_back(c);
            }
        }
        curriculums = basePrograms;
        LOG_DEBUG("CurriculumSelector", "Vocational mode: showing " << curriculums.size() << " base programs");
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
        auto noResults = curriculumCardsContainer_->addWidget(
            std::make_unique<Wt::WText>("No programs found matching your criteria."));
        noResults->addStyleClass("no-results");
        return;
    }

    // Create cards for each curriculum
    for (const auto& curriculum : curriculums) {
        auto card = curriculumCardsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        card->addStyleClass("program-card");
        card->setAttributeValue("style",
            "background: #ffffff; border-radius: 12px; box-shadow: 0 1px 3px rgba(0,0,0,0.1); "
            "display: flex; flex-direction: column; border: 1px solid #e2e8f0;");

        // Card header with program name
        auto cardHeader = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        cardHeader->addStyleClass("program-card-header");
        cardHeader->setAttributeValue("style", "padding: 1.25rem 1.25rem 0.75rem; border-bottom: 1px solid #e2e8f0;");

        auto nameText = cardHeader->addWidget(std::make_unique<Wt::WText>(curriculum.getName()));
        nameText->addStyleClass("program-card-title");
        nameText->setAttributeValue("style", "font-size: 1.1rem; font-weight: 600; color: #2563eb; display: block; margin-bottom: 0.25rem;");

        // Get department name
        std::string deptName = "";
        auto dept = curriculumManager_->getDepartment(curriculum.getDepartment());
        if (!dept.id.empty()) {
            deptName = dept.name;
        }

        // Degree type label
        std::string degreeLabel;
        std::string degreeType = curriculum.getDegreeType();
        if (degreeType == "bachelor") degreeLabel = "Bachelor's";
        else if (degreeType == "master") degreeLabel = "Master's";
        else if (degreeType == "doctoral") degreeLabel = "Doctoral";
        else if (degreeType == "certificate") degreeLabel = "Certificate";
        else if (degreeType == "associate") degreeLabel = "Associate";
        else degreeLabel = degreeType;

        auto deptText = cardHeader->addWidget(std::make_unique<Wt::WText>(deptName));
        deptText->addStyleClass("program-card-dept");
        deptText->setAttributeValue("style", "font-size: 0.875rem; color: #64748b;");

        // Card body with description
        auto cardBody = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        cardBody->addStyleClass("program-card-body");
        cardBody->setAttributeValue("style", "padding: 1rem 1.25rem; flex: 1;");

        std::string description = curriculum.getDescription();
        if (description.length() > 150) {
            description = description.substr(0, 147) + "...";
        }
        auto descText = cardBody->addWidget(std::make_unique<Wt::WText>(description));
        descText->addStyleClass("program-card-desc");
        descText->setAttributeValue("style", "font-size: 0.875rem; color: #64748b; line-height: 1.5;");

        // Card meta info
        auto cardMeta = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        cardMeta->addStyleClass("program-card-meta");
        cardMeta->setAttributeValue("style", "padding: 0.75rem 1.25rem; background: #f8fafc; border-top: 1px solid #e2e8f0;");

        // Use formatted duration for vocational programs
        std::string durationStr = curriculum.getFormattedDuration();
        auto metaText = cardMeta->addWidget(std::make_unique<Wt::WText>(
            "<span class='meta-item'>" + degreeLabel + "</span>"
            "<span class='meta-item'>" + std::to_string(curriculum.getCreditHours()) + " hours</span>"
            "<span class='meta-item'>" + durationStr + "</span>"));
        metaText->setTextFormat(Wt::TextFormat::XHTML);

        // Card footer with buttons
        auto cardFooter = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        cardFooter->addStyleClass("program-card-footer");
        cardFooter->setAttributeValue("style",
            "padding: 1rem 1.25rem; display: flex; justify-content: space-between; align-items: center; border-top: 1px solid #e2e8f0;");

        // Capture curriculum ID for use in button click handlers
        // Using ID instead of full object avoids potential issues with
        // Wt's signal/slot system when capturing complex objects by value
        std::string curriculumId = curriculum.getId();

        // Info button
        auto infoBtn = cardFooter->addWidget(std::make_unique<Wt::WPushButton>());
        infoBtn->setText("i");
        infoBtn->addStyleClass("btn btn-info-icon");
        infoBtn->setAttributeValue("style",
            "width: 36px; height: 36px; border-radius: 50%; background: #f1f5f9; color: #64748b; "
            "border: 1px solid #e2e8f0; font-size: 1rem; cursor: pointer; display: flex; align-items: center; justify-content: center;");
        infoBtn->setToolTip("View Syllabus");
        infoBtn->clicked().connect([this, curriculumId]() {
            if (curriculumManager_) {
                auto curr = curriculumManager_->getCurriculum(curriculumId);
                if (!curr.getId().empty()) {
                    showSyllabusDialog(curr);
                }
            }
        });

        // Select button
        auto selectBtn = cardFooter->addWidget(std::make_unique<Wt::WPushButton>("Select"));
        selectBtn->addStyleClass("btn btn-primary");
        selectBtn->setAttributeValue("style",
            "padding: 0.5rem 1.25rem; background-color: #2563eb; color: white; border: none; "
            "border-radius: 8px; font-size: 0.9rem; font-weight: 500; cursor: pointer;");
        selectBtn->clicked().connect([this, curriculumId]() {
            if (curriculumManager_) {
                auto curr = curriculumManager_->getCurriculum(curriculumId);
                if (!curr.getId().empty()) {
                    handleSelectProgram(curr);
                }
            }
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

void CurriculumSelector::handleSelectProgram(const Models::Curriculum& curriculum) {
    LOG_INFO("CurriculumSelector", "handleSelectProgram called for: " << curriculum.getName()
             << " (ID: " << curriculum.getId() << ")");

    // Check if we're in vocational mode and this is a base program
    bool vocational = isVocationalMode();
    bool isEndorsement = curriculum.isEndorsement();
    bool hasCdlClass = !curriculum.getCdlClass().empty();

    LOG_DEBUG("CurriculumSelector", "vocationalMode=" << vocational
              << ", isEndorsement=" << isEndorsement
              << ", hasCdlClass=" << hasCdlClass);

    if (vocational && !isEndorsement && hasCdlClass) {
        // Show endorsement selection for CDL programs
        LOG_INFO("CurriculumSelector", "Selected base program: " << curriculum.getName() << " (CDL Class " << curriculum.getCdlClass() << ")");
        showEndorsementSelection(curriculum);
    } else {
        // For accredited mode or non-CDL programs, proceed directly
        LOG_INFO("CurriculumSelector", "Emitting curriculumSelected signal for: " << curriculum.getName());
        if (session_) {
            session_->setCurrentCurriculum(curriculum);
            session_->getStudent().setCurriculumId(curriculum.getId());
            session_->getStudent().clearEndorsements();
        }
        curriculumSelected_.emit(curriculum);
        selectionComplete_.emit(curriculum, std::vector<Models::Curriculum>());
        LOG_INFO("CurriculumSelector", "Selection complete for: " << curriculum.getName());
    }
}

void CurriculumSelector::showEndorsementSelection(const Models::Curriculum& baseProgram) {
    selectedBaseProgram_ = baseProgram;
    selectedEndorsements_.clear();

    // Update the base program info display (HTML-encode dynamic values to prevent XSS and XHTML parsing issues)
    std::string encodedName = Wt::Utils::htmlEncode(baseProgram.getName());
    std::string encodedCdlClass = Wt::Utils::htmlEncode(baseProgram.getCdlClass());
    std::string encodedDuration = Wt::Utils::htmlEncode(baseProgram.getFormattedDuration());

    std::string baseInfo = "<strong>Selected Program:</strong> " + encodedName;
    if (!baseProgram.getCdlClass().empty()) {
        baseInfo += " <span style='color: #2563eb;'>(Class " + encodedCdlClass + ")</span>";
    }
    baseInfo += "<br/><span style='font-size: 0.875rem; color: #64748b;'>Duration: " +
                encodedDuration + "</span>";
    selectedBaseText_->setText(baseInfo);
    selectedBaseText_->setTextFormat(Wt::TextFormat::XHTML);

    // Find available endorsements for this CDL class
    availableEndorsements_.clear();
    if (curriculumManager_) {
        auto allCurriculums = curriculumManager_->getActiveCurriculums();
        for (const auto& c : allCurriculums) {
            // Match endorsements by CDL class
            if (c.isEndorsement() && c.getCdlClass() == baseProgram.getCdlClass()) {
                availableEndorsements_.push_back(c);
            }
        }
    }

    LOG_INFO("CurriculumSelector", "Found " << availableEndorsements_.size() <<
             " endorsements for CDL Class " << baseProgram.getCdlClass());

    // Update the endorsement list UI
    updateEndorsementList();

    // Switch views
    baseSelectionContainer_->hide();
    endorsementSelectionContainer_->show();
}

void CurriculumSelector::updateEndorsementList() {
    endorsementCardsContainer_->clear();

    if (availableEndorsements_.empty()) {
        auto noEndorse = endorsementCardsContainer_->addWidget(
            std::make_unique<Wt::WText>("No additional endorsements available for this program."));
        noEndorse->addStyleClass("no-results");
        noEndorse->setAttributeValue("style", "color: #64748b; padding: 1rem;");
        return;
    }

    for (const auto& endorsement : availableEndorsements_) {
        auto card = endorsementCardsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        card->addStyleClass("endorsement-card");
        card->setAttributeValue("style",
            "background: #ffffff; border-radius: 8px; border: 1px solid #e2e8f0; "
            "padding: 1rem; display: flex; align-items: flex-start; gap: 0.75rem;");

        // Checkbox
        auto checkbox = card->addWidget(std::make_unique<Wt::WCheckBox>());
        checkbox->setAttributeValue("style", "margin-top: 2px;");

        // Check if already selected
        bool isSelected = std::find_if(selectedEndorsements_.begin(), selectedEndorsements_.end(),
            [&endorsement](const Models::Curriculum& e) { return e.getId() == endorsement.getId(); })
            != selectedEndorsements_.end();
        checkbox->setChecked(isSelected);

        // Content container
        auto content = card->addWidget(std::make_unique<Wt::WContainerWidget>());
        content->setAttributeValue("style", "flex: 1;");

        auto nameText = content->addWidget(std::make_unique<Wt::WText>(endorsement.getName()));
        nameText->setAttributeValue("style", "font-weight: 600; color: #1e293b; display: block; margin-bottom: 0.25rem;");

        std::string desc = endorsement.getDescription();
        if (desc.length() > 100) {
            desc = desc.substr(0, 97) + "...";
        }
        auto descText = content->addWidget(std::make_unique<Wt::WText>(desc));
        descText->setAttributeValue("style", "font-size: 0.8rem; color: #64748b; display: block; margin-bottom: 0.5rem;");

        auto metaText = content->addWidget(std::make_unique<Wt::WText>(
            "Duration: " + endorsement.getFormattedDuration()));
        metaText->setAttributeValue("style", "font-size: 0.75rem; color: #94a3b8;");

        // Connect checkbox
        checkbox->changed().connect([this, endorsement, checkbox]() {
            handleEndorsementToggle(endorsement, checkbox->isChecked());
        });
    }
}

void CurriculumSelector::handleEndorsementToggle(const Models::Curriculum& endorsement, bool selected) {
    if (selected) {
        // Add to selected list
        auto it = std::find_if(selectedEndorsements_.begin(), selectedEndorsements_.end(),
            [&endorsement](const Models::Curriculum& e) { return e.getId() == endorsement.getId(); });
        if (it == selectedEndorsements_.end()) {
            selectedEndorsements_.push_back(endorsement);
            LOG_DEBUG("CurriculumSelector", "Added endorsement: " << endorsement.getName());
        }
    } else {
        // Remove from selected list
        auto it = std::remove_if(selectedEndorsements_.begin(), selectedEndorsements_.end(),
            [&endorsement](const Models::Curriculum& e) { return e.getId() == endorsement.getId(); });
        selectedEndorsements_.erase(it, selectedEndorsements_.end());
        LOG_DEBUG("CurriculumSelector", "Removed endorsement: " << endorsement.getName());
    }

    LOG_INFO("CurriculumSelector", "Selected endorsements count: " << selectedEndorsements_.size());
}

void CurriculumSelector::handleContinueWithEndorsements() {
    LOG_INFO("CurriculumSelector", "Continuing with base program: " << selectedBaseProgram_.getName() <<
             " and " << selectedEndorsements_.size() << " endorsements");

    // Update session with selections
    if (session_) {
        session_->setCurrentCurriculum(selectedBaseProgram_);
        session_->getStudent().setCurriculumId(selectedBaseProgram_.getId());

        // Clear and add endorsement IDs
        session_->getStudent().clearEndorsements();
        for (const auto& e : selectedEndorsements_) {
            session_->getStudent().addEndorsementId(e.getId());
        }
    }

    // Emit signals
    curriculumSelected_.emit(selectedBaseProgram_);
    selectionComplete_.emit(selectedBaseProgram_, selectedEndorsements_);
}

void CurriculumSelector::handleBackToBaseSelection() {
    // Switch back to base selection view
    endorsementSelectionContainer_->hide();
    baseSelectionContainer_->show();

    // Clear selections
    selectedBaseProgram_ = Models::Curriculum();
    selectedEndorsements_.clear();
}

void CurriculumSelector::showSyllabusDialog(const Models::Curriculum& curriculum) {
    auto dialog = addChild(std::make_unique<Wt::WDialog>("Program Syllabus"));
    dialog->setModal(true);
    dialog->setClosable(true);
    dialog->setResizable(false);
    dialog->rejectWhenEscapePressed();
    dialog->addStyleClass("syllabus-dialog");
    dialog->setAttributeValue("style",
        "width: 550px; border-radius: 12px; border: 1px solid #e2e8f0;");

    // Style the modal backdrop/overlay
    Wt::WApplication::instance()->styleSheet().addRule(
        ".Wt-dialogcover",
        "background-color: rgba(0, 0, 0, 0.5) !important;");

    // Style the title bar
    dialog->titleBar()->setAttributeValue("style",
        "background: #2563eb; color: white; padding: 1rem 1.25rem; font-size: 1.1rem; font-weight: 600;");

    auto content = dialog->contents();
    content->addStyleClass("syllabus-content");
    content->setAttributeValue("style",
        "padding: 1.5rem; background: #ffffff;");

    // Program name
    auto nameText = content->addWidget(std::make_unique<Wt::WText>("<h3>" + curriculum.getName() + "</h3>"));
    nameText->setTextFormat(Wt::TextFormat::XHTML);
    nameText->setAttributeValue("style",
        "color: #2563eb; margin: 0 0 1rem 0; font-size: 1.25rem; font-weight: 600;");

    // Get department name
    std::string deptName = "";
    if (curriculumManager_) {
        auto dept = curriculumManager_->getDepartment(curriculum.getDepartment());
        if (!dept.id.empty()) {
            deptName = dept.name;
        }
    }

    // Degree type label
    std::string degreeLabel;
    std::string degreeType = curriculum.getDegreeType();
    if (degreeType == "bachelor") degreeLabel = "Bachelor's Degree";
    else if (degreeType == "master") degreeLabel = "Master's Degree";
    else if (degreeType == "doctoral") degreeLabel = "Doctoral Degree";
    else if (degreeType == "certificate") degreeLabel = "Certificate Program";
    else if (degreeType == "associate") degreeLabel = "Associate Degree";
    else degreeLabel = degreeType;

    // Program details
    std::string durationStr = curriculum.getFormattedDuration();
    auto detailsHtml = "<div style='background: #f8fafc; padding: 1rem; border-radius: 8px; margin-bottom: 1rem;'>"
        "<p style='margin: 0.5rem 0; color: #1e293b;'><strong>Department:</strong> " + deptName + "</p>"
        "<p style='margin: 0.5rem 0; color: #1e293b;'><strong>Degree Type:</strong> " + degreeLabel + "</p>"
        "<p style='margin: 0.5rem 0; color: #1e293b;'><strong>Credit Hours:</strong> " + std::to_string(curriculum.getCreditHours()) + "</p>"
        "<p style='margin: 0.5rem 0; color: #1e293b;'><strong>Duration:</strong> " + durationStr + "</p>"
        "</div>";

    auto detailsText = content->addWidget(std::make_unique<Wt::WText>(detailsHtml));
    detailsText->setTextFormat(Wt::TextFormat::XHTML);

    // Description
    auto descHeader = content->addWidget(std::make_unique<Wt::WText>("<h4>Program Description</h4>"));
    descHeader->setTextFormat(Wt::TextFormat::XHTML);
    descHeader->setAttributeValue("style",
        "color: #1e293b; margin: 1.5rem 0 0.5rem 0; font-size: 1rem; font-weight: 600;");
    auto descText = content->addWidget(std::make_unique<Wt::WText>("<p>" + curriculum.getDescription() + "</p>"));
    descText->setTextFormat(Wt::TextFormat::XHTML);
    descText->setAttributeValue("style", "color: #64748b; line-height: 1.6; margin: 0;");

    // Required forms info
    auto formsCount = curriculum.getRequiredForms().size();
    auto formsHeader = content->addWidget(std::make_unique<Wt::WText>("<h4>Onboarding Requirements</h4>"));
    formsHeader->setTextFormat(Wt::TextFormat::XHTML);
    formsHeader->setAttributeValue("style",
        "color: #1e293b; margin: 1.5rem 0 0.5rem 0; font-size: 1rem; font-weight: 600;");
    auto formsText = content->addWidget(std::make_unique<Wt::WText>(
        "<p>This program requires completion of <strong>" + std::to_string(formsCount) +
        "</strong> onboarding forms.</p>"));
    formsText->setTextFormat(Wt::TextFormat::XHTML);
    formsText->setAttributeValue("style", "color: #64748b; margin: 0;");

    // Close button
    auto footer = dialog->footer();
    footer->setAttributeValue("style",
        "padding: 1rem 1.5rem; border-top: 1px solid #e2e8f0; background: #f8fafc; text-align: right;");
    auto closeBtn = footer->addWidget(std::make_unique<Wt::WPushButton>("Close"));
    closeBtn->addStyleClass("btn btn-secondary");
    closeBtn->setAttributeValue("style",
        "padding: 0.5rem 1.5rem; background-color: #64748b; color: white; border: none; "
        "border-radius: 8px; font-size: 0.9rem; font-weight: 500; cursor: pointer;");
    closeBtn->clicked().connect([dialog]() {
        dialog->accept();
    });

    dialog->show();
}

void CurriculumSelector::refresh() {
    if (curriculumManager_) {
        curriculumManager_->refresh();
        populateDepartments();
        populateDegreeTypes();
        updateCurriculumList();
    }

    // Reset to base selection view
    handleBackToBaseSelection();
}

void CurriculumSelector::selectCurriculum(const std::string& curriculumId) {
    if (curriculumManager_) {
        auto curriculum = curriculumManager_->getCurriculum(curriculumId);
        if (!curriculum.getId().empty()) {
            handleSelectProgram(curriculum);
        }
    }
}

} // namespace Curriculum
} // namespace StudentIntake
