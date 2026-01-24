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

    for (auto& item : items_) {
        bool visible = true;

        // Settings only visible to super admin
        if (item.section == AdminSection::Settings) {
            visible = user.canManageAdmins();
        }
        // Users requires admin role
        else if (item.section == AdminSection::Users) {
            visible = user.canManageAdmins();
        }
        // Curriculum requires admin role
        else if (item.section == AdminSection::Curriculum) {
            visible = user.canManageCurriculum();
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
