#include "ActivityListWidget.h"
#include <Wt/WBreak.h>
#include <chrono>
#include <ctime>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

ActivityListWidget::ActivityListWidget(DisplayMode mode)
    : WContainerWidget()
    , activityService_(nullptr)
    , displayMode_(mode)
    , limit_(mode == DisplayMode::Compact ? 5 : 50)
    , totalCount_(0)
    , todayCount_(0)
    , authCount_(0)
    , formsCount_(0)
    , adminCount_(0)
    , headerTitle_(nullptr)
    , headerSubtitle_(nullptr)
    , statsContainer_(nullptr)
    , totalCountText_(nullptr)
    , todayCountText_(nullptr)
    , authCountText_(nullptr)
    , formsCountText_(nullptr)
    , adminCountText_(nullptr)
    , headerContainer_(nullptr)
    , filterContainer_(nullptr)
    , listContainer_(nullptr)
    , footerContainer_(nullptr)
    , titleText_(nullptr)
    , countText_(nullptr)
    , searchInput_(nullptr)
    , categoryFilter_(nullptr)
    , actorTypeFilter_(nullptr)
    , refreshButton_(nullptr)
    , clearButton_(nullptr)
    , viewAllButton_(nullptr)
    , emptyMessage_(nullptr)
    , loadingIndicator_(nullptr)
    , resultCount_(nullptr) {
    setupUI();
}

ActivityListWidget::~ActivityListWidget() {
}

void ActivityListWidget::setActivityService(std::shared_ptr<ActivityLogServiceType> service) {
    activityService_ = service;
}

void ActivityListWidget::setupUI() {
    if (displayMode_ == DisplayMode::Compact) {
        addStyleClass("activity-list-widget activity-list-compact");
        setupCompactUI();
    } else {
        addStyleClass("admin-activity-log");
        setupFullUI();
    }
}

void ActivityListWidget::setupCompactUI() {
    // Compact mode for dashboard embedding - minimal UI
    setupHeader();
    setupActivityList();
}

void ActivityListWidget::setupFullUI() {
    // Full mode - matches Curriculum/Forms management style

    // Header section with title and subtitle
    auto headerSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection->addStyleClass("admin-section-header");

    auto headerRow = headerSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerRow->addStyleClass("admin-header-row");

    auto headerLeft = headerRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerLeft->addStyleClass("admin-header-left");

    headerTitle_ = headerLeft->addWidget(std::make_unique<Wt::WText>("Activity Log"));
    headerTitle_->addStyleClass("admin-section-title");

    headerSubtitle_ = headerLeft->addWidget(std::make_unique<Wt::WText>(
        "Monitor user activity and system events across the platform"));
    headerSubtitle_->addStyleClass("admin-section-subtitle");

    // Refresh button in header right
    auto headerRight = headerRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerRight->addStyleClass("admin-header-right");

    refreshButton_ = headerRight->addWidget(std::make_unique<Wt::WPushButton>("↻ Refresh"));
    refreshButton_->addStyleClass("btn btn-outline-primary");
    refreshButton_->clicked().connect([this]() {
        reload();
    });

    // Statistics placards
    setupStats();

    // Filters section
    setupFilters();

    // Result count
    resultCount_ = addWidget(std::make_unique<Wt::WText>(""));
    resultCount_->addStyleClass("admin-result-count");

    // Activity list container
    setupActivityList();

    // Footer for pagination (future)
    setupFooter();
}

void ActivityListWidget::setupHeader() {
    // Compact mode header only
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("activity-list-header");

    auto titleRow = headerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleRow->addStyleClass("activity-list-title-row");
    titleRow->setAttributeValue("style", "display:flex;justify-content:space-between;align-items:center;");

    titleText_ = titleRow->addWidget(std::make_unique<Wt::WText>("Recent Activity"));
    titleText_->addStyleClass("activity-list-title");
    titleText_->setAttributeValue("style", "font-weight:700;font-size:1rem;color:#1f2937;");

    // Refresh button - floats right
    refreshButton_ = titleRow->addWidget(std::make_unique<Wt::WPushButton>());
    refreshButton_->addStyleClass("activity-refresh-btn");
    refreshButton_->setTextFormat(Wt::TextFormat::XHTML);
    refreshButton_->setText("&#x21bb;");  // Refresh icon
    refreshButton_->setToolTip("Refresh activity list");
    refreshButton_->setAttributeValue("style", "margin-left:auto;padding:4px 8px;border:1px solid #e5e7eb;border-radius:4px;background:transparent;cursor:pointer;");
    refreshButton_->clicked().connect([this]() {
        reload();
    });
}

void ActivityListWidget::setupStats() {
    // Statistics placards - similar to Curriculum/Forms management
    statsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer_->addStyleClass("admin-submission-stats");

    // Total Activities card
    auto totalCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalCard->addStyleClass("admin-stat-mini-card active");
    auto totalIcon = totalCard->addWidget(std::make_unique<Wt::WText>("📊"));
    totalIcon->addStyleClass("admin-stat-mini-icon");
    totalCountText_ = totalCard->addWidget(std::make_unique<Wt::WText>("0"));
    totalCountText_->addStyleClass("admin-stat-mini-number");
    auto totalLabel = totalCard->addWidget(std::make_unique<Wt::WText>("Total"));
    totalLabel->addStyleClass("admin-stat-mini-label");

    // Today's Activities card
    auto todayCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    todayCard->addStyleClass("admin-stat-mini-card pending");
    auto todayIcon = todayCard->addWidget(std::make_unique<Wt::WText>("📅"));
    todayIcon->addStyleClass("admin-stat-mini-icon");
    todayCountText_ = todayCard->addWidget(std::make_unique<Wt::WText>("0"));
    todayCountText_->addStyleClass("admin-stat-mini-number");
    auto todayLabel = todayCard->addWidget(std::make_unique<Wt::WText>("Today"));
    todayLabel->addStyleClass("admin-stat-mini-label");

    // Authentication card
    auto authCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    authCard->addStyleClass("admin-stat-mini-card approved");
    auto authIcon = authCard->addWidget(std::make_unique<Wt::WText>("🔐"));
    authIcon->addStyleClass("admin-stat-mini-icon");
    authCountText_ = authCard->addWidget(std::make_unique<Wt::WText>("0"));
    authCountText_->addStyleClass("admin-stat-mini-number");
    auto authLabel = authCard->addWidget(std::make_unique<Wt::WText>("Auth"));
    authLabel->addStyleClass("admin-stat-mini-label");

    // Forms card
    auto formsCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsCard->addStyleClass("admin-stat-mini-card online");
    auto formsIcon = formsCard->addWidget(std::make_unique<Wt::WText>("📝"));
    formsIcon->addStyleClass("admin-stat-mini-icon");
    formsCountText_ = formsCard->addWidget(std::make_unique<Wt::WText>("0"));
    formsCountText_->addStyleClass("admin-stat-mini-number");
    auto formsLabel = formsCard->addWidget(std::make_unique<Wt::WText>("Forms"));
    formsLabel->addStyleClass("admin-stat-mini-label");

    // Admin Actions card
    auto adminCard = statsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    adminCard->addStyleClass("admin-stat-mini-card rejected");
    auto adminIcon = adminCard->addWidget(std::make_unique<Wt::WText>("⚙️"));
    adminIcon->addStyleClass("admin-stat-mini-icon");
    adminCountText_ = adminCard->addWidget(std::make_unique<Wt::WText>("0"));
    adminCountText_->addStyleClass("admin-stat-mini-number");
    auto adminLabel = adminCard->addWidget(std::make_unique<Wt::WText>("Admin"));
    adminLabel->addStyleClass("admin-stat-mini-label");
}

void ActivityListWidget::setupFilters() {
    // Only shown in full mode
    filterContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer_->addStyleClass("admin-filter-container");

    // Search input
    auto searchGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    searchGroup->addStyleClass("admin-filter-group admin-filter-search");

    auto searchLabel = searchGroup->addWidget(std::make_unique<Wt::WText>("Search"));
    searchLabel->addStyleClass("admin-filter-label");

    searchInput_ = searchGroup->addWidget(std::make_unique<Wt::WLineEdit>());
    searchInput_->setPlaceholderText("Search activities...");
    searchInput_->addStyleClass("admin-filter-input");
    searchInput_->textInput().connect([this]() {
        applyFilter();
    });

    // Category filter
    auto categoryGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    categoryGroup->addStyleClass("admin-filter-group");

    auto categoryLabel = categoryGroup->addWidget(std::make_unique<Wt::WText>("Category"));
    categoryLabel->addStyleClass("admin-filter-label");

    categoryFilter_ = categoryGroup->addWidget(std::make_unique<Wt::WComboBox>());
    categoryFilter_->addStyleClass("admin-filter-select");
    categoryFilter_->addItem("All Categories");
    categoryFilter_->addItem("Authentication");
    categoryFilter_->addItem("Forms");
    categoryFilter_->addItem("Profile");
    categoryFilter_->addItem("Admin");
    categoryFilter_->addItem("System");
    categoryFilter_->changed().connect([this]() {
        applyFilter();
    });

    // Actor type filter
    auto actorGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    actorGroup->addStyleClass("admin-filter-group");

    auto actorLabel = actorGroup->addWidget(std::make_unique<Wt::WText>("Actor"));
    actorLabel->addStyleClass("admin-filter-label");

    actorTypeFilter_ = actorGroup->addWidget(std::make_unique<Wt::WComboBox>());
    actorTypeFilter_->addStyleClass("admin-filter-select");
    actorTypeFilter_->addItem("All Actors");
    actorTypeFilter_->addItem("Student");
    actorTypeFilter_->addItem("Instructor");
    actorTypeFilter_->addItem("Admin");
    actorTypeFilter_->addItem("System");
    actorTypeFilter_->changed().connect([this]() {
        applyFilter();
    });

    // Clear button
    auto buttonGroup = filterContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonGroup->addStyleClass("admin-filter-buttons admin-filter-buttons-right");

    clearButton_ = buttonGroup->addWidget(std::make_unique<Wt::WPushButton>("Clear"));
    clearButton_->addStyleClass("btn btn-secondary");
    clearButton_->clicked().connect([this]() {
        resetFilters();
    });
}

void ActivityListWidget::setupActivityList() {
    listContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    listContainer_->addStyleClass("activity-list-container");

    if (displayMode_ == DisplayMode::Full) {
        listContainer_->addStyleClass("admin-table-container");
    }

    // Loading indicator
    loadingIndicator_ = listContainer_->addWidget(std::make_unique<Wt::WText>("Loading activities..."));
    loadingIndicator_->addStyleClass("activity-loading");
    loadingIndicator_->hide();

    // Empty message
    emptyMessage_ = listContainer_->addWidget(std::make_unique<Wt::WText>("No activities found matching your criteria."));
    emptyMessage_->addStyleClass("admin-no-data-message");
    emptyMessage_->hide();
}

void ActivityListWidget::setupFooter() {
    // Footer only needed in full mode for pagination (future)
    if (displayMode_ == DisplayMode::Full) {
        footerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
        footerContainer_->addStyleClass("activity-list-footer");
        footerContainer_->setAttributeValue("style", "padding:10px 12px;border-top:1px solid #e5e7eb;text-align:center;color:#6b7280;font-size:13px;");

        auto footerText = footerContainer_->addWidget(std::make_unique<Wt::WText>(
            "Showing most recent activities. Use filters to narrow results."));
    }
}

void ActivityListWidget::refresh() {
    loadActivities();
}

void ActivityListWidget::reload() {
    // Clear existing items
    activities_.clear();

    // Remove all activity items (keep loading indicator and empty message)
    auto children = listContainer_->children();
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        Wt::WWidget* child = *it;
        if (child != loadingIndicator_ && child != emptyMessage_) {
            listContainer_->removeWidget(child);
        }
    }

    loadActivities();
}

void ActivityListWidget::loadActivities() {
    if (!activityService_) {
        LOG_ERROR("ActivityListWidget", "Activity service not configured");
        emptyMessage_->setText("Activity service not available.");
        emptyMessage_->show();
        return;
    }

    // Show loading indicator
    loadingIndicator_->show();
    emptyMessage_->hide();

    try {
        // Build filter based on current selections
        ActivityFilter filter;
        filter.limit = limit_;

        if (displayMode_ == DisplayMode::Full) {
            if (categoryFilter_ && categoryFilter_->currentIndex() > 0) {
                std::string categories[] = {"", "authentication", "forms", "profile", "admin", "system"};
                filter.actionCategory = categories[categoryFilter_->currentIndex()];
            }

            if (actorTypeFilter_ && actorTypeFilter_->currentIndex() > 0) {
                std::string actors[] = {"", "student", "instructor", "admin", "system"};
                filter.actorType = actors[actorTypeFilter_->currentIndex()];
            }
        }

        // Fetch activities (synchronous for now)
        activities_ = activityService_->getActivities(filter);

        // Hide loading indicator
        loadingIndicator_->hide();

        // Remove previous activity items (keep loading and empty message)
        auto children = listContainer_->children();
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            Wt::WWidget* child = *it;
            if (child != loadingIndicator_ && child != emptyMessage_) {
                listContainer_->removeWidget(child);
            }
        }

        if (activities_.empty()) {
            emptyMessage_->show();
        } else {
            emptyMessage_->hide();

            // Render each activity item with alternating backgrounds
            int index = 0;
            for (const auto& activity : activities_) {
                auto item = createActivityItem(activity, index);
                listContainer_->addWidget(std::unique_ptr<Wt::WContainerWidget>(item));
                index++;
            }
        }

        // Update result count in full mode
        if (displayMode_ == DisplayMode::Full && resultCount_) {
            resultCount_->setText("Showing " + std::to_string(activities_.size()) + " activities");
        }

        // Update statistics
        updateStats();

        LOG_DEBUG("ActivityListWidget", "Loaded " << activities_.size() << " activities");

    } catch (const std::exception& e) {
        LOG_ERROR("ActivityListWidget", "Error loading activities: " << e.what());
        loadingIndicator_->hide();
        emptyMessage_->setText("Error loading activities. Please try again.");
        emptyMessage_->show();
    }
}

void ActivityListWidget::updateStats() {
    if (displayMode_ != DisplayMode::Full) return;

    // Reset counts
    totalCount_ = 0;
    todayCount_ = 0;
    authCount_ = 0;
    formsCount_ = 0;
    adminCount_ = 0;

    // Get today's date for comparison
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    char todayStr[11];
    std::strftime(todayStr, sizeof(todayStr), "%Y-%m-%d", &tm);
    std::string today(todayStr);

    // Count from loaded activities
    totalCount_ = static_cast<int>(activities_.size());

    for (const auto& activity : activities_) {
        // Check if today
        std::string createdAt = activity.getFormattedTime();
        if (createdAt.length() >= 10 && createdAt.substr(0, 10) == today) {
            todayCount_++;
        }

        // Count by category (using type alias from header)
        auto category = activity.getActionCategory();
        using Cat = ::StudentIntake::Models::ActivityCategory;
        if (category == Cat::Authentication) {
            authCount_++;
        } else if (category == Cat::Forms) {
            formsCount_++;
        } else if (category == Cat::Admin) {
            adminCount_++;
        }
    }

    // Update UI
    if (totalCountText_) totalCountText_->setText(std::to_string(totalCount_));
    if (todayCountText_) todayCountText_->setText(std::to_string(todayCount_));
    if (authCountText_) authCountText_->setText(std::to_string(authCount_));
    if (formsCountText_) formsCountText_->setText(std::to_string(formsCount_));
    if (adminCountText_) adminCountText_->setText(std::to_string(adminCount_));
}

Wt::WContainerWidget* ActivityListWidget::createActivityItem(const ActivityLogModel& activity, int index) {
    auto item = new Wt::WContainerWidget();
    item->addStyleClass("activity-item");
    item->addStyleClass(getSeverityClass(activity.getSeverity()));

    // Alternating row backgrounds - very subtle
    std::string bgColor = (index % 2 == 0) ? "background:#ffffff;" : "background:#fafafa;";
    item->setAttributeValue("style", "display:flex;flex-direction:row;align-items:flex-start;gap:10px;padding:10px 12px;border-bottom:1px solid #e5e7eb;cursor:pointer;" + bgColor);

    // Make item clickable
    const int activityId = activity.getId();
    item->clicked().connect([this, activityId]() {
        onActivityClicked(activityId);
    });

    // Icon container
    auto iconContainer = item->addWidget(std::make_unique<Wt::WContainerWidget>());
    iconContainer->addStyleClass("activity-item-icon");
    iconContainer->addStyleClass(activity.getIconClass());
    iconContainer->setAttributeValue("style", "flex:0 0 28px;width:28px;height:28px;border-radius:6px;display:flex;align-items:center;justify-content:center;background:#f3f4f6;font-size:14px;");

    auto iconText = iconContainer->addWidget(std::make_unique<Wt::WText>(activity.getIcon()));
    iconText->setTextFormat(Wt::TextFormat::XHTML);

    // Content container
    auto contentContainer = item->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentContainer->addStyleClass("activity-item-content");
    contentContainer->setAttributeValue("style", "flex:1;min-width:0;overflow:hidden;");

    // Description
    auto descText = contentContainer->addWidget(std::make_unique<Wt::WText>(activity.getDescription()));
    descText->addStyleClass("activity-item-description");
    descText->setAttributeValue("style", "display:block;font-size:13px;color:#1f2937;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;margin-bottom:2px;");

    // Meta row (actor badge + timestamp)
    auto metaRow = contentContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    metaRow->addStyleClass("activity-item-meta");
    metaRow->setAttributeValue("style", "display:flex;flex-direction:row;align-items:center;gap:8px;");

    // Actor type badge
    auto actorBadge = metaRow->addWidget(std::make_unique<Wt::WText>(activity.getActorTypeString()));
    actorBadge->addStyleClass("activity-actor-badge");
    actorBadge->addStyleClass(getActorTypeClass(activity.getActorType()));
    actorBadge->setAttributeValue("style", "display:inline-block;font-size:10px;font-weight:600;text-transform:uppercase;padding:2px 6px;border-radius:3px;background:#dbeafe;color:#1d4ed8;");

    // Timestamp
    auto timestamp = metaRow->addWidget(std::make_unique<Wt::WText>(activity.getRelativeTime()));
    timestamp->addStyleClass("activity-item-time");
    timestamp->setToolTip(activity.getFormattedTime());
    timestamp->setAttributeValue("style", "font-size:11px;color:#6b7280;");

    return item;
}

void ActivityListWidget::applyFilter() {
    loadActivities();
}

void ActivityListWidget::resetFilters() {
    if (searchInput_) searchInput_->setText("");
    if (categoryFilter_) categoryFilter_->setCurrentIndex(0);
    if (actorTypeFilter_) actorTypeFilter_->setCurrentIndex(0);
    loadActivities();
}

std::string ActivityListWidget::getSeverityClass(ActivitySeverity severity) const {
    switch (severity) {
        case ActivitySeverity::Success:
            return "activity-severity-success";
        case ActivitySeverity::Warning:
            return "activity-severity-warning";
        case ActivitySeverity::Error:
            return "activity-severity-error";
        case ActivitySeverity::Info:
        default:
            return "activity-severity-info";
    }
}

std::string ActivityListWidget::getActorTypeClass(ActorType actorType) const {
    switch (actorType) {
        case ActorType::Student:
            return "actor-type-student";
        case ActorType::Instructor:
            return "actor-type-instructor";
        case ActorType::Admin:
            return "actor-type-admin";
        case ActorType::System:
        default:
            return "actor-type-system";
    }
}

void ActivityListWidget::onActivityClicked(int activityId) {
    LOG_DEBUG("ActivityListWidget", "Activity clicked: " << activityId);
    activityClicked_.emit(activityId);
}

} // namespace Admin
} // namespace StudentIntake
