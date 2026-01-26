#ifndef ACTIVITY_LIST_WIDGET_H
#define ACTIVITY_LIST_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <vector>
#include <memory>
#include "../../models/ActivityLog.h"
#include "../../api/ActivityLogService.h"

namespace StudentIntake {
namespace Admin {

// Type aliases to reference types from parent namespace
// (needed because we're inside StudentIntake::Admin, not StudentIntake)
using ActivityLogModel = ::StudentIntake::Models::ActivityLog;
using ActivitySeverity = ::StudentIntake::Models::ActivitySeverity;
using ActorType = ::StudentIntake::Models::ActorType;
using ActivityLogServiceType = ::StudentIntake::Api::ActivityLogService;
using ActivityFilter = ::StudentIntake::Api::ActivityFilter;

/**
 * @brief Readonly widget displaying recent activity/audit trail entries
 *
 * This widget displays a list of recent activities from the activity_log table.
 * It is designed to be embedded in the admin dashboard or displayed as a full page.
 *
 * Features:
 * - Readonly display (no editing capability)
 * - Immutable data presentation
 * - Category filtering
 * - Refresh functionality
 * - Click-through to activity details
 * - Severity-based styling (info, success, warning, error)
 */
class ActivityListWidget : public Wt::WContainerWidget {
public:
    /**
     * @brief Display mode for the widget
     */
    enum class DisplayMode {
        Compact,    // For dashboard embedding (shows fewer items, minimal UI)
        Full        // For dedicated activity page (shows more items, filters, pagination)
    };

    explicit ActivityListWidget(DisplayMode mode = DisplayMode::Compact);
    ~ActivityListWidget();

    // =========================================================================
    // Configuration
    // =========================================================================

    /**
     * @brief Set the API service for fetching activities
     */
    void setActivityService(std::shared_ptr<ActivityLogServiceType> service);

    /**
     * @brief Set the maximum number of activities to display
     */
    void setLimit(int limit) { limit_ = limit; }

    /**
     * @brief Get the current display mode
     */
    DisplayMode getDisplayMode() const { return displayMode_; }

    // =========================================================================
    // Actions
    // =========================================================================

    /**
     * @brief Refresh the activity list from the server
     */
    void refresh();

    /**
     * @brief Clear and reload activities
     */
    void reload();

    // =========================================================================
    // Signals
    // =========================================================================

    /**
     * @brief Emitted when an activity item is clicked
     * @param activityId The ID of the clicked activity
     */
    Wt::Signal<int>& activityClicked() { return activityClicked_; }

    /**
     * @brief Emitted when "View All" is clicked (compact mode only)
     */
    Wt::Signal<>& viewAllClicked() { return viewAllClicked_; }

private:
    void setupUI();
    void setupCompactUI();
    void setupFullUI();
    void setupHeader();
    void setupFilters();
    void setupActivityList();
    void setupFooter();

    /**
     * @brief Load activities from the service
     */
    void loadActivities();

    /**
     * @brief Render a single activity item
     */
    Wt::WContainerWidget* createActivityItem(const ActivityLogModel& activity);

    /**
     * @brief Apply current filter and refresh display
     */
    void applyFilter();

    /**
     * @brief Get the CSS class for severity styling
     */
    std::string getSeverityClass(ActivitySeverity severity) const;

    /**
     * @brief Get the CSS class for actor type badge
     */
    std::string getActorTypeClass(ActorType actorType) const;

    /**
     * @brief Handle activity item click
     */
    void onActivityClicked(int activityId);

    // Service
    std::shared_ptr<ActivityLogServiceType> activityService_;

    // Configuration
    DisplayMode displayMode_;
    int limit_;

    // Data (immutable once loaded)
    std::vector<ActivityLogModel> activities_;

    // UI Elements
    Wt::WContainerWidget* headerContainer_;
    Wt::WContainerWidget* filterContainer_;
    Wt::WContainerWidget* listContainer_;
    Wt::WContainerWidget* footerContainer_;
    Wt::WText* titleText_;
    Wt::WText* countText_;
    Wt::WComboBox* categoryFilter_;
    Wt::WComboBox* actorTypeFilter_;
    Wt::WPushButton* refreshButton_;
    Wt::WPushButton* viewAllButton_;
    Wt::WText* emptyMessage_;
    Wt::WText* loadingIndicator_;

    // Signals
    Wt::Signal<int> activityClicked_;
    Wt::Signal<> viewAllClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ACTIVITY_LIST_WIDGET_H
