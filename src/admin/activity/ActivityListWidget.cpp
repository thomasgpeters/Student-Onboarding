#include "ActivityListWidget.h"
#include <Wt/WBreak.h>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

ActivityListWidget::ActivityListWidget(DisplayMode mode)
    : WContainerWidget()
    , activityService_(nullptr)
    , displayMode_(mode)
    , limit_(mode == DisplayMode::Compact ? 5 : 20)
    , headerContainer_(nullptr)
    , filterContainer_(nullptr)
    , listContainer_(nullptr)
    , footerContainer_(nullptr)
    , titleText_(nullptr)
    , countText_(nullptr)
    , categoryFilter_(nullptr)
    , actorTypeFilter_(nullptr)
    , refreshButton_(nullptr)
    , viewAllButton_(nullptr)
    , emptyMessage_(nullptr)
    , loadingIndicator_(nullptr) {
    setupUI();
}

ActivityListWidget::~ActivityListWidget() {
}

void ActivityListWidget::setActivityService(std::shared_ptr<ActivityLogServiceType> service) {
    activityService_ = service;
}

void ActivityListWidget::setupUI() {
    addStyleClass("activity-list-widget");

    if (displayMode_ == DisplayMode::Compact) {
        addStyleClass("activity-list-compact");
        setupCompactUI();
    } else {
        addStyleClass("activity-list-full");
        setupFullUI();
    }
}

void ActivityListWidget::setupCompactUI() {
    // Compact mode for dashboard embedding
    // Minimal UI - just header and list

    setupHeader();
    setupActivityList();
    setupFooter();
}

void ActivityListWidget::setupFullUI() {
    // Full mode for dedicated activity page
    // Includes filters and more controls

    setupHeader();
    setupFilters();
    setupActivityList();
    setupFooter();
}

void ActivityListWidget::setupHeader() {
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("activity-list-header");

    auto titleRow = headerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    titleRow->addStyleClass("activity-list-title-row");
    titleRow->setAttributeValue("style", "display:flex;justify-content:space-between;align-items:center;");

    titleText_ = titleRow->addWidget(std::make_unique<Wt::WText>(
        displayMode_ == DisplayMode::Compact ? "Recent Activity" : "Activity Log"));
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

    // Count text (full mode only)
    if (displayMode_ == DisplayMode::Full) {
        countText_ = headerContainer_->addWidget(std::make_unique<Wt::WText>());
        countText_->addStyleClass("activity-list-count");
    }
}

void ActivityListWidget::setupFilters() {
    // Only shown in full mode
    filterContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterContainer_->addStyleClass("activity-list-filters");

    // Category filter
    auto categoryLabel = filterContainer_->addWidget(std::make_unique<Wt::WText>("Category:"));
    categoryLabel->addStyleClass("activity-filter-label");

    categoryFilter_ = filterContainer_->addWidget(std::make_unique<Wt::WComboBox>());
    categoryFilter_->addStyleClass("activity-filter-select");
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
    auto actorLabel = filterContainer_->addWidget(std::make_unique<Wt::WText>("Actor:"));
    actorLabel->addStyleClass("activity-filter-label");

    actorTypeFilter_ = filterContainer_->addWidget(std::make_unique<Wt::WComboBox>());
    actorTypeFilter_->addStyleClass("activity-filter-select");
    actorTypeFilter_->addItem("All Actors");
    actorTypeFilter_->addItem("Student");
    actorTypeFilter_->addItem("Instructor");
    actorTypeFilter_->addItem("Admin");
    actorTypeFilter_->addItem("System");
    actorTypeFilter_->changed().connect([this]() {
        applyFilter();
    });
}

void ActivityListWidget::setupActivityList() {
    listContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    listContainer_->addStyleClass("activity-list-container");

    // Loading indicator
    loadingIndicator_ = listContainer_->addWidget(std::make_unique<Wt::WText>("Loading activities..."));
    loadingIndicator_->addStyleClass("activity-loading");
    loadingIndicator_->hide();

    // Empty message
    emptyMessage_ = listContainer_->addWidget(std::make_unique<Wt::WText>("No recent activity to display."));
    emptyMessage_->addStyleClass("activity-empty-message");
    emptyMessage_->hide();
}

void ActivityListWidget::setupFooter() {
    footerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    footerContainer_->addStyleClass("activity-list-footer");
    footerContainer_->setAttributeValue("style", "padding:10px 12px;border-top:1px solid #e5e7eb;text-align:left;");

    // View All button (compact mode only)
    if (displayMode_ == DisplayMode::Compact) {
        viewAllButton_ = footerContainer_->addWidget(std::make_unique<Wt::WPushButton>("View All Activity"));
        viewAllButton_->addStyleClass("activity-view-all-btn");
        viewAllButton_->setAttributeValue("style", "display:inline-block;padding:6px 12px;font-size:13px;color:#2563eb;background:transparent;border:1px solid #e5e7eb;border-radius:4px;cursor:pointer;");
        viewAllButton_->clicked().connect([this]() {
            viewAllClicked_.emit();
        });
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

        if (displayMode_ == DisplayMode::Full && categoryFilter_) {
            int catIdx = categoryFilter_->currentIndex();
            if (catIdx > 0) {
                std::string categories[] = {"", "authentication", "forms", "profile", "admin", "system"};
                filter.actionCategory = categories[catIdx];
            }
        }

        if (displayMode_ == DisplayMode::Full && actorTypeFilter_) {
            int actorIdx = actorTypeFilter_->currentIndex();
            if (actorIdx > 0) {
                std::string actors[] = {"", "student", "instructor", "admin", "system"};
                filter.actorType = actors[actorIdx];
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

            // Render each activity item
            for (const auto& activity : activities_) {
                auto item = createActivityItem(activity);
                listContainer_->addWidget(std::unique_ptr<Wt::WContainerWidget>(item));
            }
        }

        // Update count text in full mode
        if (displayMode_ == DisplayMode::Full && countText_) {
            countText_->setText("Showing " + std::to_string(activities_.size()) + " activities");
        }

        LOG_DEBUG("ActivityListWidget", "Loaded " << activities_.size() << " activities");

    } catch (const std::exception& e) {
        LOG_ERROR("ActivityListWidget", "Error loading activities: " << e.what());
        loadingIndicator_->hide();
        emptyMessage_->setText("Error loading activities. Please try again.");
        emptyMessage_->show();
    }
}

Wt::WContainerWidget* ActivityListWidget::createActivityItem(const ActivityLogModel& activity) {
    auto item = new Wt::WContainerWidget();
    item->addStyleClass("activity-item");
    item->addStyleClass(getSeverityClass(activity.getSeverity()));
    item->setAttributeValue("style", "display:flex;flex-direction:row;align-items:flex-start;gap:10px;padding:10px 12px;border-bottom:1px solid #e5e7eb;cursor:pointer;");

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
