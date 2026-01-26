#ifndef ADMIN_SIDEBAR_H
#define ADMIN_SIDEBAR_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WSignal.h>
#include <memory>
#include <string>
#include <vector>
#include "admin/models/AdminSession.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Admin portal sections
 */
enum class AdminSection {
    Dashboard,
    Users,
    Students,
    Forms,
    Curriculum,
    Settings,
    ActivityLog
};

/**
 * @brief Left sidebar navigation for admin sections
 */
class AdminSidebar : public Wt::WContainerWidget {
public:
    AdminSidebar();
    ~AdminSidebar();

    void setSession(std::shared_ptr<Models::AdminSession> session) { session_ = session; }

    // Set active section
    void setActiveSection(AdminSection section);
    AdminSection getActiveSection() const { return activeSection_; }

    // Update visibility based on permissions
    void refresh();

    // Signals
    Wt::Signal<AdminSection>& sectionClicked() { return sectionClicked_; }

private:
    void setupUI();
    void updateActiveState();
    void handleSectionClick(AdminSection section);

    struct SidebarItem {
        AdminSection section;
        std::string label;
        std::string icon;
        Wt::WContainerWidget* widget;
        bool requiresAdmin;  // Requires administrator role or higher
    };

    std::shared_ptr<Models::AdminSession> session_;
    AdminSection activeSection_;
    std::vector<SidebarItem> items_;

    Wt::Signal<AdminSection> sectionClicked_;
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_SIDEBAR_H
