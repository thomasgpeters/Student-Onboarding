#include "InstructorDashboardWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WVBoxLayout.h>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Instructor {

InstructorDashboardWidget::InstructorDashboardWidget() {
    setStyleClass("instructor-dashboard");
    setupUI();
}

void InstructorDashboardWidget::setInstructor(const Models::Instructor& instructor) {
    instructor_ = instructor;
    updateDashboard();
}

void InstructorDashboardWidget::refresh() {
    updateDashboard();
}

void InstructorDashboardWidget::setupUI() {
    // Header section
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->setStyleClass("dashboard-header");

    auto headerContent = headerSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContent->setStyleClass("header-content");

    welcomeText_ = headerContent->addWidget(std::make_unique<Wt::WText>("Welcome, Instructor"));
    welcomeText_->setStyleClass("welcome-text");

    instructorTypeText_ = headerContent->addWidget(std::make_unique<Wt::WText>());
    instructorTypeText_->setStyleClass("instructor-type");

    backButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Back to Login"));
    backButton_->setStyleClass("btn btn-secondary back-btn");
    backButton_->clicked().connect([this] { backClicked_.emit(); });

    // Stats section
    createStatsSection();

    // Content section with two columns
    contentSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    contentSection_->setStyleClass("dashboard-content");

    auto leftColumn = contentSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    leftColumn->setStyleClass("content-column left-column");

    auto rightColumn = contentSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightColumn->setStyleClass("content-column right-column");

    // Today's schedule in left column
    scheduleSection_ = leftColumn->addWidget(std::make_unique<Wt::WContainerWidget>());
    scheduleSection_->setStyleClass("dashboard-section schedule-section");
    createTodayScheduleSection();

    // Students in right column
    studentsSection_ = rightColumn->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentsSection_->setStyleClass("dashboard-section students-section");
    createStudentsSection();

    // Quick actions
    createQuickActionsSection();
}

void InstructorDashboardWidget::createStatsSection() {
    statsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsSection_->setStyleClass("stats-section");

    statsGrid_ = statsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    statsGrid_->setStyleClass("stats-grid");

    // Active students stat
    auto stat1 = statsGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    stat1->setStyleClass("stat-card");
    activeStudentsValue_ = stat1->addWidget(std::make_unique<Wt::WText>("0"));
    activeStudentsValue_->setStyleClass("stat-value");
    auto label1 = stat1->addWidget(std::make_unique<Wt::WText>("Active Students"));
    label1->setStyleClass("stat-label");

    // Sessions today stat
    auto stat2 = statsGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    stat2->setStyleClass("stat-card");
    sessionsTodayValue_ = stat2->addWidget(std::make_unique<Wt::WText>("0"));
    sessionsTodayValue_->setStyleClass("stat-value");
    auto label2 = stat2->addWidget(std::make_unique<Wt::WText>("Sessions Today"));
    label2->setStyleClass("stat-label");

    // Pending feedback stat
    auto stat3 = statsGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    stat3->setStyleClass("stat-card");
    pendingFeedbackValue_ = stat3->addWidget(std::make_unique<Wt::WText>("0"));
    pendingFeedbackValue_->setStyleClass("stat-value");
    auto label3 = stat3->addWidget(std::make_unique<Wt::WText>("Pending Follow-ups"));
    label3->setStyleClass("stat-label");

    // Average progress stat
    auto stat4 = statsGrid_->addWidget(std::make_unique<Wt::WContainerWidget>());
    stat4->setStyleClass("stat-card");
    avgProgressValue_ = stat4->addWidget(std::make_unique<Wt::WText>("0%"));
    avgProgressValue_->setStyleClass("stat-value");
    auto label4 = stat4->addWidget(std::make_unique<Wt::WText>("Avg. Student Progress"));
    label4->setStyleClass("stat-label");
}

void InstructorDashboardWidget::createTodayScheduleSection() {
    auto header = scheduleSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("section-header");

    scheduleTitle_ = header->addWidget(std::make_unique<Wt::WText>("Today's Schedule"));
    scheduleTitle_->setStyleClass("section-title");

    auto viewAllBtn = header->addWidget(std::make_unique<Wt::WPushButton>("View All"));
    viewAllBtn->setStyleClass("btn btn-link");
    viewAllBtn->clicked().connect([this] { viewScheduleClicked_.emit(); });

    scheduleTable_ = scheduleSection_->addWidget(std::make_unique<Wt::WTable>());
    scheduleTable_->setStyleClass("dashboard-table schedule-table");
    scheduleTable_->setHeaderCount(1);

    // Table headers
    scheduleTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Time"));
    scheduleTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Type"));
    scheduleTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Student"));
    scheduleTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Status"));
    scheduleTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Action"));

    noSessionsText_ = scheduleSection_->addWidget(std::make_unique<Wt::WText>("No sessions scheduled for today"));
    noSessionsText_->setStyleClass("no-data-text");
    noSessionsText_->hide();
}

void InstructorDashboardWidget::createStudentsSection() {
    auto header = studentsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("section-header");

    studentsTitle_ = header->addWidget(std::make_unique<Wt::WText>("Students Needing Attention"));
    studentsTitle_->setStyleClass("section-title");

    auto viewAllBtn = header->addWidget(std::make_unique<Wt::WPushButton>("View All"));
    viewAllBtn->setStyleClass("btn btn-link");
    viewAllBtn->clicked().connect([this] { viewStudentsClicked_.emit(); });

    studentsTable_ = studentsSection_->addWidget(std::make_unique<Wt::WTable>());
    studentsTable_->setStyleClass("dashboard-table students-table");
    studentsTable_->setHeaderCount(1);

    // Table headers
    studentsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Student"));
    studentsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Progress"));
    studentsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Status"));

    noStudentsText_ = studentsSection_->addWidget(std::make_unique<Wt::WText>("All students are on track"));
    noStudentsText_->setStyleClass("no-data-text success");
    noStudentsText_->hide();
}

void InstructorDashboardWidget::createQuickActionsSection() {
    actionsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsSection_->setStyleClass("quick-actions-section");

    auto title = actionsSection_->addWidget(std::make_unique<Wt::WText>("Quick Actions"));
    title->setStyleClass("section-title");

    auto buttonsContainer = actionsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonsContainer->setStyleClass("actions-grid");

    scheduleSessionButton_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Schedule Session"));
    scheduleSessionButton_->setStyleClass("btn btn-primary action-btn");
    scheduleSessionButton_->clicked().connect([this] { scheduleSessionClicked_.emit(); });

    addFeedbackButton_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Add Feedback"));
    addFeedbackButton_->setStyleClass("btn btn-primary action-btn");
    addFeedbackButton_->clicked().connect([this] { addFeedbackClicked_.emit(); });

    viewAllStudentsButton_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("View All Students"));
    viewAllStudentsButton_->setStyleClass("btn btn-secondary action-btn");
    viewAllStudentsButton_->clicked().connect([this] { viewStudentsClicked_.emit(); });

    viewScheduleButton_ = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("View Full Schedule"));
    viewScheduleButton_->setStyleClass("btn btn-secondary action-btn");
    viewScheduleButton_->clicked().connect([this] { viewScheduleClicked_.emit(); });

    auto validateSkillsBtn = buttonsContainer->addWidget(std::make_unique<Wt::WPushButton>("Validate Skills"));
    validateSkillsBtn->setStyleClass("btn btn-secondary action-btn");
    validateSkillsBtn->clicked().connect([this] { viewValidationsClicked_.emit(); });
}

void InstructorDashboardWidget::updateDashboard() {
    if (!instructorService_ || instructor_.getId().empty()) {
        return;
    }

    // Update welcome text
    std::string welcomeMsg = "Welcome, " + instructor_.getName();
    welcomeText_->setText(welcomeMsg);

    // Update instructor type
    std::string typeText;
    switch (instructor_.getInstructorType()) {
        case Models::InstructorType::Instructor:
            typeText = "CDL Instructor";
            break;
        case Models::InstructorType::Examiner:
            typeText = "CDL Examiner";
            break;
        case Models::InstructorType::Both:
            typeText = "CDL Instructor & Examiner";
            break;
    }
    instructorTypeText_->setText(typeText);

    // Update all sections
    updateStats();
    updateTodaySchedule();
    updateStudentsList();
}

void InstructorDashboardWidget::updateStats() {
    if (!instructorService_) return;

    int instructorId = std::stoi(instructor_.getId());
    auto stats = instructorService_->getDashboardStats(instructorId);

    activeStudentsValue_->setText(std::to_string(stats.activeStudents));
    sessionsTodayValue_->setText(std::to_string(stats.sessionsToday));
    pendingFeedbackValue_->setText(std::to_string(stats.pendingFeedback));

    std::ostringstream progressStr;
    progressStr << std::fixed << std::setprecision(0) << stats.averageStudentProgress << "%";
    avgProgressValue_->setText(progressStr.str());
}

void InstructorDashboardWidget::updateTodaySchedule() {
    if (!instructorService_) return;

    // Clear existing rows (except header)
    while (scheduleTable_->rowCount() > 1) {
        scheduleTable_->removeRow(1);
    }

    int instructorId = std::stoi(instructor_.getId());
    auto sessions = instructorService_->getTodaySessions(instructorId);

    if (sessions.empty()) {
        scheduleTable_->hide();
        noSessionsText_->show();
        return;
    }

    scheduleTable_->show();
    noSessionsText_->hide();

    for (const auto& session : sessions) {
        int row = scheduleTable_->rowCount();

        // Time
        std::string timeStr = session.getStartTime() + " - " + session.getEndTime();
        scheduleTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(timeStr));

        // Type
        scheduleTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(session.getDisplayName()));

        // Student (we'd need to fetch student name - for now show ID)
        std::string studentInfo = "Student #" + std::to_string(session.getStudentId());
        auto studentLink = scheduleTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WPushButton>(studentInfo));
        studentLink->setStyleClass("btn btn-link");
        int studentId = session.getStudentId();
        studentLink->clicked().connect([this, studentId] { studentSelected_.emit(studentId); });

        // Status
        auto statusText = scheduleTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(session.getStatusString()));
        std::string statusClass = "status-badge status-" + session.getStatusString();
        statusText->setStyleClass(statusClass);

        // Add action button to make session clickable
        std::string sessionId = session.getId();
        scheduleTable_->rowAt(row)->setStyleClass("clickable-row");
        auto viewBtn = scheduleTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->setStyleClass("btn btn-sm btn-outline-primary");
        viewBtn->clicked().connect([this, sessionId] {
            sessionSelected_.emit(sessionId);
        });
    }
}

void InstructorDashboardWidget::updateStudentsList() {
    if (!instructorService_) return;

    // Clear existing rows (except header)
    while (studentsTable_->rowCount() > 1) {
        studentsTable_->removeRow(1);
    }

    int instructorId = std::stoi(instructor_.getId());
    auto students = instructorService_->getStudentsNeedingAttention(instructorId);

    if (students.empty()) {
        studentsTable_->hide();
        noStudentsText_->show();
        return;
    }

    studentsTable_->show();
    noStudentsText_->hide();

    // Show max 5 students
    int count = 0;
    for (const auto& student : students) {
        if (count >= 5) break;

        int row = studentsTable_->rowCount();

        // Student name
        auto nameLink = studentsTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WPushButton>(student.studentName.empty() ?
                "Student #" + std::to_string(student.studentId) : student.studentName));
        nameLink->setStyleClass("btn btn-link");
        int studentId = student.studentId;
        nameLink->clicked().connect([this, studentId] { studentSelected_.emit(studentId); });

        // Progress
        std::ostringstream progressStr;
        progressStr << std::fixed << std::setprecision(0) << student.overallProgress << "%";
        auto progressText = studentsTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(progressStr.str()));

        // Color code progress
        if (student.overallProgress < 25) {
            progressText->setStyleClass("progress-low");
        } else if (student.overallProgress < 50) {
            progressText->setStyleClass("progress-medium");
        }

        // Status
        auto statusText = studentsTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(student.status));
        std::string statusClass = "status-badge";
        if (student.status == "inactive") {
            statusClass += " status-warning";
        }
        statusText->setStyleClass(statusClass);

        count++;
    }
}

} // namespace Instructor
} // namespace StudentIntake
