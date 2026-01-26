#include "AdminDashboard.h"
#include <Wt/WBreak.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include "utils/Logger.h"
#include "admin/activity/ActivityListWidget.h"

namespace StudentIntake {
namespace Admin {

AdminDashboard::AdminDashboard()
    : WContainerWidget()
    , session_(nullptr)
    , apiService_(nullptr)
    , activityService_(nullptr)
    , welcomeText_(nullptr)
    , studentCountText_(nullptr)
    , todaysStudentsText_(nullptr)
    , completedOnboardingText_(nullptr)
    , programCountText_(nullptr)
    , activityContainer_(nullptr)
    , quickActionsContainer_(nullptr)
    , activityListWidget_(nullptr)
    , totalStudents_(0)
    , todaysStudents_(0)
    , completedOnboarding_(0)
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

    // Total Students card
    auto studentsCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentsCard->addStyleClass("admin-stat-card");
    studentsCard->clicked().connect([this]() { viewStudentsClicked_.emit(); });

    auto studentsIcon = studentsCard->addWidget(std::make_unique<Wt::WText>("👥"));
    studentsIcon->addStyleClass("admin-stat-icon");

    studentCountText_ = studentsCard->addWidget(std::make_unique<Wt::WText>("0"));
    studentCountText_->addStyleClass("admin-stat-number");

    auto studentsLabel = studentsCard->addWidget(std::make_unique<Wt::WText>("Total Students"));
    studentsLabel->addStyleClass("admin-stat-label");

    // Today's Registrations card
    auto todaysCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    todaysCard->addStyleClass("admin-stat-card");
    todaysCard->clicked().connect([this]() { viewTodaysStudentsClicked_.emit(); });

    auto todaysIcon = todaysCard->addWidget(std::make_unique<Wt::WText>("📅"));
    todaysIcon->addStyleClass("admin-stat-icon");

    todaysStudentsText_ = todaysCard->addWidget(std::make_unique<Wt::WText>("0"));
    todaysStudentsText_->addStyleClass("admin-stat-number");

    auto todaysLabel = todaysCard->addWidget(std::make_unique<Wt::WText>("Today's Registrations"));
    todaysLabel->addStyleClass("admin-stat-label");

    // Completed Onboarding card
    auto completedCard = statsContainer->addWidget(std::make_unique<Wt::WContainerWidget>());
    completedCard->addStyleClass("admin-stat-card");
    completedCard->clicked().connect([this]() { viewStudentsClicked_.emit(); });

    auto completedIcon = completedCard->addWidget(std::make_unique<Wt::WText>("✅"));
    completedIcon->addStyleClass("admin-stat-icon");

    completedOnboardingText_ = completedCard->addWidget(std::make_unique<Wt::WText>("0"));
    completedOnboardingText_->addStyleClass("admin-stat-number");

    auto completedLabel = completedCard->addWidget(std::make_unique<Wt::WText>("Completed Onboarding"));
    completedLabel->addStyleClass("admin-stat-label");

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

    // Recent Activity section - using ActivityListWidget
    auto activitySection = mainContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    activitySection->addStyleClass("admin-activity-section card");

    // Create the ActivityListWidget in compact mode for dashboard embedding
    activityListWidget_ = activitySection->addWidget(
        std::make_unique<ActivityListWidget>(ActivityListWidget::DisplayMode::Compact));

    // Connect signals
    activityListWidget_->activityClicked().connect([this](int activityId) {
        activityDetailClicked_.emit(activityId);
    });

    activityListWidget_->viewAllClicked().connect([this]() {
        viewActivityLogClicked_.emit();
    });

    // Quick Actions section
    auto actionsSection = mainContent->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsSection->addStyleClass("admin-actions-section card");

    auto actionsHeader = actionsSection->addWidget(std::make_unique<Wt::WText>("<h4>Quick Actions</h4>"));
    actionsHeader->setTextFormat(Wt::TextFormat::XHTML);

    quickActionsContainer_ = actionsSection->addWidget(std::make_unique<Wt::WContainerWidget>());
    quickActionsContainer_->addStyleClass("admin-quick-actions");

    // View All Students button
    auto viewStudentsBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("View All Students"));
    viewStudentsBtn->addStyleClass("btn btn-primary");
    viewStudentsBtn->clicked().connect([this]() { viewStudentsClicked_.emit(); });

    // Today's Registrations button
    auto todaysBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Today's Registrations"));
    todaysBtn->addStyleClass("btn btn-outline-primary");
    todaysBtn->clicked().connect([this]() { viewTodaysStudentsClicked_.emit(); });

    // Manage Programs button
    auto programsBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Manage Programs"));
    programsBtn->addStyleClass("btn btn-outline-primary");
    programsBtn->clicked().connect([this]() { viewCurriculumClicked_.emit(); });

    // Institution Settings button
    auto settingsBtn = quickActionsContainer_->addWidget(std::make_unique<Wt::WPushButton>("Institution Settings"));
    settingsBtn->addStyleClass("btn btn-outline-secondary");
    settingsBtn->clicked().connect([this]() { viewSettingsClicked_.emit(); });
}

void AdminDashboard::refresh() {
    loadStatistics();
    updateDisplay();

    // Refresh activity list
    if (activityListWidget_ && activityService_) {
        activityListWidget_->setActivityService(activityService_);
        activityListWidget_->refresh();
    }
}

void AdminDashboard::loadStatistics() {
    // Reset to zero before loading
    totalStudents_ = 0;
    todaysStudents_ = 0;
    completedOnboarding_ = 0;
    activePrograms_ = 0;

    if (!apiService_) {
        LOG_ERROR("AdminDashboard", "API service not available");
        return;
    }

    try {
        // Fetch all students and calculate statistics
        auto studentsResponse = apiService_->getApiClient()->get("/Student");
        if (studentsResponse.success) {
            auto jsonResponse = nlohmann::json::parse(studentsResponse.body);
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                auto& students = jsonResponse["data"];
                totalStudents_ = static_cast<int>(students.size());

                // Get today's date in YYYY-MM-DD format
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                std::tm tm = *std::localtime(&time);
                char todayStr[11];
                std::strftime(todayStr, sizeof(todayStr), "%Y-%m-%d", &tm);
                std::string today(todayStr);

                for (const auto& student : students) {
                    const auto& attrs = student.contains("attributes") ? student["attributes"] : student;

                    // Count today's registrations
                    if (attrs.contains("created_at") && !attrs["created_at"].is_null()) {
                        std::string createdAt = attrs["created_at"].get<std::string>();
                        if (createdAt.substr(0, 10) == today) {
                            todaysStudents_++;
                        }
                    }

                    // Count completed onboarding
                    if (attrs.contains("intake_status") && !attrs["intake_status"].is_null()) {
                        std::string status = attrs["intake_status"].get<std::string>();
                        if (status == "completed" || status == "approved") {
                            completedOnboarding_++;
                        }
                    }
                }
            }
        }

        // Fetch active programs count
        auto curriculumResponse = apiService_->getApiClient()->get("/Curriculum?filter[is_active]=true");
        if (curriculumResponse.success) {
            auto jsonResponse = nlohmann::json::parse(curriculumResponse.body);
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
                activePrograms_ = static_cast<int>(jsonResponse["data"].size());
            }
        }

        LOG_DEBUG("AdminDashboard", "Loaded statistics - Total: " << totalStudents_
                  << ", Today: " << todaysStudents_
                  << ", Completed: " << completedOnboarding_
                  << ", Programs: " << activePrograms_);

    } catch (const std::exception& e) {
        LOG_ERROR("AdminDashboard", "Error loading statistics: " << e.what());
    }
}

void AdminDashboard::updateDisplay() {
    // Update welcome message
    if (session_ && session_->isAuthenticated()) {
        std::string name = session_->getAdminUser().getFullName();
        welcomeText_->setText("<h2>Welcome, " + name + "</h2>");
    }

    // Update statistics
    studentCountText_->setText(std::to_string(totalStudents_));
    todaysStudentsText_->setText(std::to_string(todaysStudents_));
    completedOnboardingText_->setText(std::to_string(completedOnboarding_));
    programCountText_->setText(std::to_string(activePrograms_));
}

} // namespace Admin
} // namespace StudentIntake
