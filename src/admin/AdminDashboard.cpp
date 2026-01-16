#include "AdminDashboard.h"
#include <Wt/WBreak.h>

namespace StudentIntake {
namespace Admin {

AdminDashboard::AdminDashboard()
    : WContainerWidget()
    , session_(nullptr)
    , apiService_(nullptr)
    , welcomeText_(nullptr)
    , studentCountText_(nullptr)
    , pendingFormsText_(nullptr)
    , programCountText_(nullptr)
    , activityContainer_(nullptr)
    , quickActionsContainer_(nullptr)
    , totalStudents_(0)
    , pendingForms_(0)
    , activePrograms_(0) {
    setupUI();
}

AdminDashboard::~AdminDashboard() {
}

void AdminDashboard::setupUI() {
    addStyleClass("admin-dashboard");

    // Welcome section
    auto welcomeSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    welcomeSection->addStyleClass("admin-welcome-section");

    welcomeText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>("<h2>Welcome, Administrator</h2>"));
    welcomeText_->setTextFormat(Wt::TextFormat::XHTML);

    auto subtitleText = welcomeSection->addWidget(std::make_unique<Wt::WText>(
        "<p>Manage students, forms, and curriculum from this dashboard.</p>"));
    subtitleText->setTextFormat(Wt::TextFormat::XHTML);
    subtitleText->addStyleClass("admin-welcome-subtitle");

    // Statistics cards
    auto statsContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer->addStyleClass("admin-stats-container");

    // Students card
    auto studentsCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentsCard->addStyleClass("admin-stat-card");
    studentsCard->clicked().connect([this]() { viewStudentsClicked_.emit(); });

    auto studentsIcon = studentsCard->addWidget(std::make_unique<Wt::WText>("👥"));
    studentsIcon->addStyleClass("admin-stat-icon");

    studentCountText_ = studentsCard->addWidget(std::make_unique<Wt::WText>("0"));
    studentCountText_->addStyleClass("admin-stat-number");

    auto studentsLabel = studentsCard->addWidget(std::make_unique<Wt::WText>("Students Enrolled"));
    studentsLabel->addStyleClass("admin-stat-label");

    // Pending Forms card
    auto formsCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    formsCard->addStyleClass("admin-stat-card");
    formsCard->clicked().connect([this]() { viewFormsClicked_.emit(); });

    auto formsIcon = formsCard->addWidget(std::make_unique<Wt::WText>("📋"));
    formsIcon->addStyleClass("admin-stat-icon");

    pendingFormsText_ = formsCard->addWidget(std::make_unique<Wt::WText>("0"));
    pendingFormsText_->addStyleClass("admin-stat-number");

    auto formsLabel = formsCard->addWidget(std::make_unique<Wt::WText>("Pending Forms"));
    formsLabel->addStyleClass("admin-stat-label");

    // Programs card
    auto programsCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    programsCard->addStyleClass("admin-stat-card");
    programsCard->clicked().connect([this]() { viewCurriculumClicked_.emit(); });

    auto programsIcon = programsCard->addWidget(std::make_unique<Wt::WText>("📚"));
    programsIcon->addStyleClass("admin-stat-icon");

    programCountText_ = programsCard->addWidget(std::make_unique<Wt::WText>("0"));
    programCountText_->addStyleClass("admin-stat-number");

    auto programsLabel = programsCard->addWidget(std::make_unique<Wt::WText>("Active Programs"));
    programsLabel->addStyleClass("admin-stat-label");

    // Main content area - two columns
    auto mainContent = addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContent->addStyleClass("admin-main-content");

    // Recent Activity section
    auto activitySection = mainContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    activitySection->addStyleClass("admin-activity-section card");

    auto activityHeader = activitySection->addWidget(std::make_unique<Wt::WText>("<h4>Recent Activity</h4>"));
    activityHeader->setTextFormat(Wt::TextFormat::XHTML);

    activityContainer_ = activitySection->addWidget(std::make_unique<Wt::WContainerWidget>());
    activityContainer_->addStyleClass("admin-activity-list");

    // Add placeholder activity items
    std::vector<std::string> activities = {
        "John Doe submitted Personal Information form",
        "Jane Smith completed onboarding",
        "CS-BS program syllabus was updated",
        "New student: Mike Johnson registered",
        "Emergency Contact form approved for Sarah Wilson"
    };

    for (const auto& activity : activities) {
        auto item = activityContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        item->addStyleClass("admin-activity-item");

        auto bullet = item->addWidget(std::make_unique<Wt::WText>("•"));
        bullet->addStyleClass("admin-activity-bullet");

        auto text = item->addWidget(std::make_unique<Wt::WText>(activity));
        text->addStyleClass("admin-activity-text");
    }

    // Quick Actions section
    auto actionsSection = mainContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsSection->addStyleClass("admin-actions-section card");

    auto actionsHeader = actionsSection->addWidget(std::make_unique<Wt::WText>("<h4>Quick Actions</h4>"));
    actionsHeader->setTextFormat(Wt::TextFormat::XHTML);

    quickActionsContainer_ = actionsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    quickActionsContainer_->addStyleClass("admin-quick-actions");

    // Review Forms button
    auto reviewBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Review Pending Forms"));
    reviewBtn->addStyleClass("btn btn-primary");
    reviewBtn->clicked().connect([this]() { viewFormsClicked_.emit(); });

    // Add Program button
    auto addProgramBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Add New Program"));
    addProgramBtn->addStyleClass("btn btn-outline-primary");
    addProgramBtn->clicked().connect([this]() { viewCurriculumClicked_.emit(); });

    // View All Students button
    auto viewStudentsBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("View All Students"));
    viewStudentsBtn->addStyleClass("btn btn-outline-secondary");
    viewStudentsBtn->clicked().connect([this]() { viewStudentsClicked_.emit(); });
}

void AdminDashboard::refresh() {
    loadStatistics();
    updateDisplay();
}

void AdminDashboard::loadStatistics() {
    // In production, these would be loaded from the API
    // For now, use placeholder values
    totalStudents_ = 124;
    pendingForms_ = 45;
    activePrograms_ = 8;

    // TODO: Implement actual API calls to get statistics
    // if (apiService_) {
    //     auto studentsResponse = apiService_->getApiClient()->get("Student?count=true");
    //     auto formsResponse = apiService_->getApiClient()->get("FormSubmission?filter[status]=pending&count=true");
    //     auto curriculumResponse = apiService_->getApiClient()->get("Curriculum?filter[is_active]=true&count=true");
    // }
}

void AdminDashboard::updateDisplay() {
    // Update welcome message
    if (session_ && session_->isAuthenticated()) {
        std::string name = session_->getAdminUser().getFullName();
        welcomeText_->setText("<h2>Welcome, " + name + "</h2>");
    }

    // Update statistics
    studentCountText_->setText(std::to_string(totalStudents_));
    pendingFormsText_->setText(std::to_string(pendingForms_));
    programCountText_->setText(std::to_string(activePrograms_));
}

} // namespace Admin
} // namespace StudentIntake
