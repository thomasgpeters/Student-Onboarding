#include "AdminSidebar.h"

namespace StudentIntake {
namespace Admin {

AdminSidebar::AdminSidebar()
    : WContainerWidget()
    , session_(nullptr)
    , activeSection_(AdminSection::Dashboard)
    , items_() {
    setupUI();
}

AdminSidebar::~AdminSidebar() {
}

void AdminSidebar::setupUI() {
    addStyleClass("admin-sidebar");

    // Define sidebar items
    struct ItemDef {
        AdminSection section;
        std::string label;
        std::string icon;
        bool requiresAdmin;
    };

    std::vector<ItemDef> itemDefs = {
        {AdminSection::Dashboard, "Dashboard", "📊", false},
        {AdminSection::Users, "Users", "👤", true},  // Admin only
        {AdminSection::Students, "Students", "👥", false},
        {AdminSection::Forms, "Forms", "📋", false},
        {AdminSection::Curriculum, "Curriculum", "📚", true},
        {AdminSection::ActivityLog, "Activity Log", "📜", true},  // Admin only
        {AdminSection::Settings, "Settings", "⚙️", true}  // Super admin only
    };

    for (const auto& def : itemDefs) {
        auto itemContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
        itemContainer->addStyleClass("admin-sidebar-item");

        // Icon
        auto icon = itemContainer->addWidget(std::make_unique<Wt::WText>(def.icon));
        icon->addStyleClass("admin-sidebar-icon");

        // Label
        auto label = itemContainer->addWidget(std::make_unique<Wt::WText>(def.label));
        label->addStyleClass("admin-sidebar-label");

        // Store item reference
        SidebarItem item;
        item.section = def.section;
        item.label = def.label;
        item.icon = def.icon;
        item.widget = itemContainer;
        item.requiresAdmin = def.requiresAdmin;
        items_.push_back(item);

        // Click handler
        itemContainer->clicked().connect([this, section = def.section]() {
            handleSectionClick(section);
        });
    }

    updateActiveState();
}

void AdminSidebar::setActiveSection(AdminSection section) {
    activeSection_ = section;
    updateActiveState();
}

void AdminSidebar::refresh() {
    // Update item visibility based on permissions
    if (!session_ || !session_->isAuthenticated()) {
        return;
    }

    auto& user = session_->getAdminUser();
    bool isSuperAdmin = user.canManageAdmins();  // SuperAdmin only
    bool isAdminOrHigher = (user.getRole() == Models::AdminRole::Administrator ||
                            user.getRole() == Models::AdminRole::SuperAdmin);
    bool isManagerOrHigher = isAdminOrHigher || (user.getRole() == Models::AdminRole::Manager);
    bool isStaffLevel = (user.getRole() == Models::AdminRole::Staff ||
                         user.getRole() == Models::AdminRole::Examiner ||
                         user.getRole() == Models::AdminRole::Instructor);

    for (auto& item : items_) {
        bool visible = true;

        switch (item.section) {
            case AdminSection::Dashboard:
                // Everyone can see dashboard
                visible = true;
                break;

            case AdminSection::Users:
                // Admins and super admins can manage all users
                visible = isAdminOrHigher;
                break;

            case AdminSection::Students:
                // Staff-level roles see Students (admins use Users instead)
                visible = isStaffLevel && !isAdminOrHigher;
                break;

            case AdminSection::Forms:
                // Admins and super admins can manage forms
                visible = isAdminOrHigher;
                break;

            case AdminSection::Curriculum:
                // Both admins and instructors can view curriculum
                visible = true;
                break;

            case AdminSection::Settings:
                // Only super admin
                visible = isSuperAdmin;
                break;

            case AdminSection::ActivityLog:
                // Admins and super admins can view activity log
                visible = isAdminOrHigher;
                break;
        }

        if (visible) {
            item.widget->show();
        } else {
            item.widget->hide();
        }
    }
}

void AdminSidebar::updateActiveState() {
    for (auto& item : items_) {
        if (item.section == activeSection_) {
            item.widget->addStyleClass("active");
        } else {
            item.widget->removeStyleClass("active");
        }
    }
}

void AdminSidebar::handleSectionClick(AdminSection section) {
    setActiveSection(section);
    sectionClicked_.emit(section);
}

} // namespace Admin
} // namespace StudentIntake
