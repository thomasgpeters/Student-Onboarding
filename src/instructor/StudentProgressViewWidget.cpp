#include "StudentProgressViewWidget.h"
#include <Wt/WBreak.h>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Instructor {

StudentProgressViewWidget::StudentProgressViewWidget()
    : instructorId_(0)
    , selectedStudentId_(0)
{
    setStyleClass("student-progress-view");
    setupUI();
}

void StudentProgressViewWidget::setupUI() {
    // List view
    listView_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    listView_->setStyleClass("list-view");
    createStudentListView();

    // Detail view
    detailView_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    detailView_->setStyleClass("detail-view");
    detailView_->hide();
    createStudentDetailView();
}

void StudentProgressViewWidget::createStudentListView() {
    // Header
    auto header = listView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("view-header");

    listBackButton_ = header->addWidget(std::make_unique<Wt::WPushButton>("Back"));
    listBackButton_->setStyleClass("btn btn-secondary");
    listBackButton_->clicked().connect([this] { backClicked_.emit(); });

    listTitle_ = header->addWidget(std::make_unique<Wt::WText>("Assigned Students"));
    listTitle_->setStyleClass("view-title");

    // Students table
    studentsTable_ = listView_->addWidget(std::make_unique<Wt::WTable>());
    studentsTable_->setStyleClass("students-table");
    studentsTable_->setHeaderCount(1);

    studentsTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Student Name"));
    studentsTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Course"));
    studentsTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Progress"));
    studentsTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Hours"));
    studentsTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Status"));
    studentsTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Actions"));

    noStudentsText_ = listView_->addWidget(std::make_unique<Wt::WText>("No students assigned"));
    noStudentsText_->setStyleClass("no-data-text");
    noStudentsText_->hide();
}

void StudentProgressViewWidget::createStudentDetailView() {
    // Header with back button
    auto header = detailView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    header->setStyleClass("detail-header");

    detailBackButton_ = header->addWidget(std::make_unique<Wt::WPushButton>("Back to Students"));
    detailBackButton_->setStyleClass("btn btn-secondary");
    detailBackButton_->clicked().connect([this] { showStudentList(); });

    auto nameContainer = header->addWidget(std::make_unique<Wt::WContainerWidget>());
    nameContainer->setStyleClass("student-info");

    studentName_ = nameContainer->addWidget(std::make_unique<Wt::WText>());
    studentName_->setStyleClass("student-name");

    studentEmail_ = nameContainer->addWidget(std::make_unique<Wt::WText>());
    studentEmail_->setStyleClass("student-email");

    // Progress overview
    progressOverview_ = detailView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    progressOverview_->setStyleClass("progress-overview");

    auto progressTitle = progressOverview_->addWidget(std::make_unique<Wt::WText>("Overall Progress"));
    progressTitle->setStyleClass("section-title");

    overallProgressBar_ = progressOverview_->addWidget(std::make_unique<Wt::WProgressBar>());
    overallProgressBar_->setRange(0, 100);
    overallProgressBar_->setStyleClass("overall-progress-bar");

    progressText_ = progressOverview_->addWidget(std::make_unique<Wt::WText>());
    progressText_->setStyleClass("progress-text");

    // Content sections in a grid
    auto contentGrid = detailView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentGrid->setStyleClass("detail-grid");

    // Module progress section
    moduleProgressSection_ = contentGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    moduleProgressSection_->setStyleClass("detail-section");

    auto moduleTitle = moduleProgressSection_->addWidget(std::make_unique<Wt::WText>("Module Progress"));
    moduleTitle->setStyleClass("section-title");

    moduleProgressTable_ = moduleProgressSection_->addWidget(std::make_unique<Wt::WTable>());
    moduleProgressTable_->setStyleClass("progress-table");
    moduleProgressTable_->setHeaderCount(1);
    moduleProgressTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Module"));
    moduleProgressTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Status"));
    moduleProgressTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Score"));

    // Skill progress section
    skillProgressSection_ = contentGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    skillProgressSection_->setStyleClass("detail-section");

    auto skillTitle = skillProgressSection_->addWidget(std::make_unique<Wt::WText>("Skills Progress"));
    skillTitle->setStyleClass("section-title");

    skillProgressTable_ = skillProgressSection_->addWidget(std::make_unique<Wt::WTable>());
    skillProgressTable_->setStyleClass("progress-table");
    skillProgressTable_->setHeaderCount(1);
    skillProgressTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Skill"));
    skillProgressTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Practice"));
    skillProgressTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Validated"));

    // Time tracking section
    timeTrackingSection_ = contentGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    timeTrackingSection_->setStyleClass("detail-section time-section");

    auto timeTitle = timeTrackingSection_->addWidget(std::make_unique<Wt::WText>("Time Tracking"));
    timeTitle->setStyleClass("section-title");

    auto timeGrid = timeTrackingSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    timeGrid->setStyleClass("time-grid");

    auto totalCard = timeGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalCard->setStyleClass("time-card");
    totalHoursText_ = totalCard->addWidget(std::make_unique<Wt::WText>("0"));
    totalHoursText_->setStyleClass("time-value");
    totalCard->addWidget(std::make_unique<Wt::WText>("Total Hours"))->setStyleClass("time-label");

    auto practiceCard = timeGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    practiceCard->setStyleClass("time-card");
    practiceHoursText_ = practiceCard->addWidget(std::make_unique<Wt::WText>("0"));
    practiceHoursText_->setStyleClass("time-value");
    practiceCard->addWidget(std::make_unique<Wt::WText>("Practice Hours"))->setStyleClass("time-label");

    auto classroomCard = timeGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    classroomCard->setStyleClass("time-card");
    classroomHoursText_ = classroomCard->addWidget(std::make_unique<Wt::WText>("0"));
    classroomHoursText_->setStyleClass("time-value");
    classroomCard->addWidget(std::make_unique<Wt::WText>("Classroom Hours"))->setStyleClass("time-label");

    // Recent feedback section
    feedbackSection_ = contentGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    feedbackSection_->setStyleClass("detail-section");

    auto feedbackTitle = feedbackSection_->addWidget(std::make_unique<Wt::WText>("Recent Feedback"));
    feedbackTitle->setStyleClass("section-title");

    recentFeedbackTable_ = feedbackSection_->addWidget(std::make_unique<Wt::WTable>());
    recentFeedbackTable_->setStyleClass("feedback-table");
    recentFeedbackTable_->setHeaderCount(1);
    recentFeedbackTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Date"));
    recentFeedbackTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Type"));
    recentFeedbackTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Subject"));

    // Actions section
    detailActions_ = detailView_->addWidget(std::make_unique<Wt::WContainerWidget>());
    detailActions_->setStyleClass("detail-actions");

    addFeedbackButton_ = detailActions_->addWidget(std::make_unique<Wt::WPushButton>("Add Feedback"));
    addFeedbackButton_->setStyleClass("btn btn-primary");
    addFeedbackButton_->clicked().connect([this] { addFeedbackClicked_.emit(selectedStudentId_); });

    scheduleButton_ = detailActions_->addWidget(std::make_unique<Wt::WPushButton>("Schedule Session"));
    scheduleButton_->setStyleClass("btn btn-primary");
    scheduleButton_->clicked().connect([this] { scheduleSessionClicked_.emit(selectedStudentId_); });

    validateButton_ = detailActions_->addWidget(std::make_unique<Wt::WPushButton>("Validate Skill"));
    validateButton_->setStyleClass("btn btn-success");
    validateButton_->clicked().connect([this] { validateSkillClicked_.emit(selectedStudentId_); });
}

void StudentProgressViewWidget::loadStudents() {
    showStudentList();
    updateStudentList();
}

void StudentProgressViewWidget::loadStudentDetail(int studentId) {
    selectedStudentId_ = studentId;

    if (instructorService_) {
        selectedStudent_ = instructorService_->getStudentProgressDetail(instructorId_, studentId);
    }

    listView_->hide();
    detailView_->show();
    updateStudentDetail();
}

void StudentProgressViewWidget::showStudentList() {
    detailView_->hide();
    listView_->show();
}

void StudentProgressViewWidget::updateStudentList() {
    // Clear existing rows
    while (studentsTable_->rowCount() > 1) {
        studentsTable_->removeRow(1);
    }

    if (!instructorService_) {
        noStudentsText_->show();
        studentsTable_->hide();
        return;
    }

    auto students = instructorService_->getAssignedStudentProgress(instructorId_);

    if (students.empty()) {
        noStudentsText_->show();
        studentsTable_->hide();
        return;
    }

    noStudentsText_->hide();
    studentsTable_->show();

    for (const auto& student : students) {
        int row = studentsTable_->rowCount();

        // Student name
        std::string name = student.studentName.empty() ?
            "Student #" + std::to_string(student.studentId) : student.studentName;
        studentsTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(name));

        // Course
        studentsTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(student.courseName));

        // Progress with progress bar
        auto progressCell = studentsTable_->elementAt(row, 2);
        auto progressBar = progressCell->addWidget(std::make_unique<Wt::WProgressBar>());
        progressBar->setRange(0, 100);
        progressBar->setValue(static_cast<int>(student.overallProgress));
        progressBar->setStyleClass("table-progress-bar");

        std::ostringstream progressStr;
        progressStr << std::fixed << std::setprecision(0) << student.overallProgress << "%";
        progressCell->addWidget(std::make_unique<Wt::WText>(progressStr.str()))->setStyleClass("progress-value");

        // Hours
        std::ostringstream hoursStr;
        hoursStr << std::fixed << std::setprecision(1) << student.hoursCompleted << "h";
        studentsTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(hoursStr.str()));

        // Status
        auto statusText = studentsTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(student.status));
        statusText->setStyleClass("status-badge status-" + student.status);

        // Actions
        auto actionsCell = studentsTable_->elementAt(row, 5);
        auto viewBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->setStyleClass("btn btn-sm btn-primary");
        int studentId = student.studentId;
        viewBtn->clicked().connect([this, studentId] { loadStudentDetail(studentId); });
    }
}

void StudentProgressViewWidget::updateStudentDetail() {
    if (!instructorService_) return;

    // Update header
    studentName_->setText(selectedStudent_.studentName.empty() ?
        "Student #" + std::to_string(selectedStudent_.studentId) : selectedStudent_.studentName);
    studentEmail_->setText(selectedStudent_.email);

    // Update overall progress
    overallProgressBar_->setValue(static_cast<int>(selectedStudent_.overallProgress));

    std::ostringstream progressStr;
    progressStr << std::fixed << std::setprecision(1) << selectedStudent_.overallProgress << "% Complete - "
                << selectedStudent_.modulesCompleted << " of " << selectedStudent_.totalModules << " modules";
    progressText_->setText(progressStr.str());

    // Update time tracking
    std::ostringstream totalHours, practiceHours, classroomHours;
    totalHours << std::fixed << std::setprecision(1) << selectedStudent_.hoursCompleted;
    totalHoursText_->setText(totalHours.str());

    // These would come from detailed data - for now estimate
    double practice = selectedStudent_.hoursCompleted * 0.6;
    double classroom = selectedStudent_.hoursCompleted * 0.4;

    practiceHours << std::fixed << std::setprecision(1) << practice;
    practiceHoursText_->setText(practiceHours.str());

    classroomHours << std::fixed << std::setprecision(1) << classroom;
    classroomHoursText_->setText(classroomHours.str());

    // Update module progress table
    while (moduleProgressTable_->rowCount() > 1) {
        moduleProgressTable_->removeRow(1);
    }

    // Get module progress if available
    // For now, show placeholder data

    // Update skill progress table
    while (skillProgressTable_->rowCount() > 1) {
        skillProgressTable_->removeRow(1);
    }

    auto skillProgress = instructorService_->getSkillProgress(selectedStudent_.studentId,
                                                               selectedStudent_.enrollmentId);
    for (const auto& skill : skillProgress) {
        int row = skillProgressTable_->rowCount();

        skillProgressTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>("Skill #" + std::to_string(skill.getSkillItemId())));

        std::ostringstream practiceStr;
        practiceStr << skill.getPracticeCount() << " (" << std::fixed << std::setprecision(0)
                   << skill.getSuccessRate() << "% success)";
        skillProgressTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(practiceStr.str()));

        auto validatedText = skillProgressTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(skill.isValidated() ? "Yes" : "No"));
        validatedText->setStyleClass(skill.isValidated() ? "validated-yes" : "validated-no");
    }

    // Update recent feedback table
    while (recentFeedbackTable_->rowCount() > 1) {
        recentFeedbackTable_->removeRow(1);
    }

    for (const auto& feedback : selectedStudent_.recentFeedback) {
        int row = recentFeedbackTable_->rowCount();
        if (row > 5) break; // Show max 5 recent feedback items

        recentFeedbackTable_->elementAt(row, 0)->addWidget(
            std::make_unique<Wt::WText>(feedback.getCreatedAt().substr(0, 10)));
        recentFeedbackTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>(feedback.getFeedbackTypeString()));
        recentFeedbackTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(feedback.getSubject()));
    }
}

void StudentProgressViewWidget::createProgressCard(Wt::WContainerWidget* parent, const std::string& title,
                                                     double progress, const std::string& details) {
    auto card = parent->addWidget(std::make_unique<Wt::WContainerWidget>());
    card->setStyleClass("progress-card");

    card->addWidget(std::make_unique<Wt::WText>(title))->setStyleClass("card-title");

    auto progressBar = card->addWidget(std::make_unique<Wt::WProgressBar>());
    progressBar->setRange(0, 100);
    progressBar->setValue(static_cast<int>(progress));

    card->addWidget(std::make_unique<Wt::WText>(details))->setStyleClass("card-details");
}

} // namespace Instructor
} // namespace StudentIntake
