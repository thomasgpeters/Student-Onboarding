#include "AssessmentReportWidget.h"
#include <Wt/WBreak.h>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Classroom {

AssessmentReportWidget::AssessmentReportWidget()
    : headerSection_(nullptr)
    , summarySection_(nullptr)
    , moduleBreakdownSection_(nullptr)
    , timeLogSection_(nullptr)
    , certificateSection_(nullptr)
    , actionsSection_(nullptr)
    , reportTitle_(nullptr)
    , reportNumber_(nullptr)
    , studentName_(nullptr)
    , courseName_(nullptr)
    , completionDate_(nullptr)
    , overallScore_(nullptr)
    , passFailStatus_(nullptr)
    , totalTime_(nullptr)
    , moduleTable_(nullptr)
    , certificateStatus_(nullptr)
    , certificateNumber_(nullptr)
    , issueCertificateButton_(nullptr)
    , printButton_(nullptr)
    , backButton_(nullptr) {
    setupUI();
}

void AssessmentReportWidget::setupUI() {
    addStyleClass("assessment-report-widget");

    // Header
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->addStyleClass("report-header");

    reportTitle_ = headerSection_->addWidget(std::make_unique<Wt::WText>("Course Completion Report"));
    reportTitle_->addStyleClass("report-title");

    reportNumber_ = headerSection_->addWidget(std::make_unique<Wt::WText>());
    reportNumber_->addStyleClass("report-number");

    // Summary section
    summarySection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    summarySection_->addStyleClass("report-summary");

    // Module breakdown section
    moduleBreakdownSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    moduleBreakdownSection_->addStyleClass("report-modules");

    // Time log section
    timeLogSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    timeLogSection_->addStyleClass("report-time");

    // Certificate section
    certificateSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    certificateSection_->addStyleClass("report-certificate");

    // Actions section
    actionsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsSection_->addStyleClass("report-actions");

    printButton_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Print Report"));
    printButton_->addStyleClass("btn btn-secondary");
    printButton_->clicked().connect([this] {
        printReport();
    });

    backButton_ = actionsSection_->addWidget(std::make_unique<Wt::WPushButton>("Back to Dashboard"));
    backButton_->addStyleClass("btn btn-primary");
    backButton_->clicked().connect([this] {
        backClicked_.emit();
    });
}

void AssessmentReportWidget::loadReport(const std::string& reportId) {
    if (!classroomService_) {
        return;
    }

    report_ = classroomService_->getReport(reportId);
    updateDisplay();
}

void AssessmentReportWidget::loadReportByEnrollment(const std::string& enrollmentId) {
    if (!classroomService_) {
        return;
    }

    report_ = classroomService_->getReportByEnrollment(enrollmentId);
    updateDisplay();
}

void AssessmentReportWidget::printReport() {
    // Trigger browser print dialog using JavaScript
    doJavaScript("window.print();");
}

void AssessmentReportWidget::updateDisplay() {
    // Update report number
    if (!report_.getReportNumber().empty()) {
        reportNumber_->setText("Report #: " + report_.getReportNumber());
    }

    createSummarySection();
    createModuleBreakdownSection();
    createTimeLogSection();
    createCertificateSection();
}

void AssessmentReportWidget::createSummarySection() {
    summarySection_->clear();

    auto summaryTitle = summarySection_->addWidget(std::make_unique<Wt::WText>("Summary"));
    summaryTitle->addStyleClass("section-title");

    auto summaryGrid = summarySection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    summaryGrid->addStyleClass("summary-grid");

    // Student info
    auto studentRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentRow->addStyleClass("summary-row");
    studentRow->addWidget(std::make_unique<Wt::WText>("Student:"))->addStyleClass("summary-label");
    studentName_ = studentRow->addWidget(std::make_unique<Wt::WText>(report_.getStudentName()));
    studentName_->addStyleClass("summary-value");

    // Course info
    auto courseRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    courseRow->addStyleClass("summary-row");
    courseRow->addWidget(std::make_unique<Wt::WText>("Course:"))->addStyleClass("summary-label");
    courseName_ = courseRow->addWidget(std::make_unique<Wt::WText>(report_.getCourseName()));
    courseName_->addStyleClass("summary-value");

    // Completion date
    auto dateRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    dateRow->addStyleClass("summary-row");
    dateRow->addWidget(std::make_unique<Wt::WText>("Completion Date:"))->addStyleClass("summary-label");
    completionDate_ = dateRow->addWidget(std::make_unique<Wt::WText>(report_.getCompletionDate()));
    completionDate_->addStyleClass("summary-value");

    // Overall score
    auto scoreRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    scoreRow->addStyleClass("summary-row");
    scoreRow->addWidget(std::make_unique<Wt::WText>("Overall Score:"))->addStyleClass("summary-label");
    std::ostringstream scoreStream;
    scoreStream << std::fixed << std::setprecision(1) << report_.getOverallScore() << "%";
    overallScore_ = scoreRow->addWidget(std::make_unique<Wt::WText>(scoreStream.str()));
    overallScore_->addStyleClass("summary-value score");

    // Final exam score
    auto examRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    examRow->addStyleClass("summary-row");
    examRow->addWidget(std::make_unique<Wt::WText>("Final Exam Score:"))->addStyleClass("summary-label");
    std::ostringstream examStream;
    examStream << std::fixed << std::setprecision(1) << report_.getFinalExamScore() << "%";
    examRow->addWidget(std::make_unique<Wt::WText>(examStream.str()))->addStyleClass("summary-value");

    // Average quiz score
    auto quizRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    quizRow->addStyleClass("summary-row");
    quizRow->addWidget(std::make_unique<Wt::WText>("Average Quiz Score:"))->addStyleClass("summary-label");
    std::ostringstream quizStream;
    quizStream << std::fixed << std::setprecision(1) << report_.getAverageQuizScore() << "%";
    quizRow->addWidget(std::make_unique<Wt::WText>(quizStream.str()))->addStyleClass("summary-value");

    // Pass/Fail status
    auto statusRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    statusRow->addStyleClass("summary-row status-row");
    statusRow->addWidget(std::make_unique<Wt::WText>("Status:"))->addStyleClass("summary-label");
    passFailStatus_ = statusRow->addWidget(std::make_unique<Wt::WText>(report_.hasPassed() ? "PASSED" : "FAILED"));
    passFailStatus_->addStyleClass(report_.hasPassed() ? "summary-value passed" : "summary-value failed");

    // Total time
    auto timeRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    timeRow->addStyleClass("summary-row");
    timeRow->addWidget(std::make_unique<Wt::WText>("Total Time:"))->addStyleClass("summary-label");
    totalTime_ = timeRow->addWidget(std::make_unique<Wt::WText>(report_.getFormattedTotalTime()));
    totalTime_->addStyleClass("summary-value");

    // Modules completed
    auto modulesRow = summaryGrid->addWidget(std::make_unique<Wt::WContainerWidget>());
    modulesRow->addStyleClass("summary-row");
    modulesRow->addWidget(std::make_unique<Wt::WText>("Modules Completed:"))->addStyleClass("summary-label");
    modulesRow->addWidget(std::make_unique<Wt::WText>(
        std::to_string(report_.getModulesCompleted()) + " / " + std::to_string(report_.getTotalModules())
    ))->addStyleClass("summary-value");
}

void AssessmentReportWidget::createModuleBreakdownSection() {
    moduleBreakdownSection_->clear();

    auto sectionTitle = moduleBreakdownSection_->addWidget(std::make_unique<Wt::WText>("Module Breakdown"));
    sectionTitle->addStyleClass("section-title");

    moduleTable_ = moduleBreakdownSection_->addWidget(std::make_unique<Wt::WTable>());
    moduleTable_->addStyleClass("module-table");
    moduleTable_->setHeaderCount(1);

    // Header row
    moduleTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Module"));
    moduleTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Score"));
    moduleTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Time Spent"));
    moduleTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Status"));

    // Data rows
    auto moduleScores = report_.getModuleScores();
    int row = 1;
    for (const auto& score : moduleScores) {
        moduleTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(
            "Module " + std::to_string(score.moduleNumber) + ": " + score.moduleTitle));

        std::ostringstream scoreStream;
        scoreStream << std::fixed << std::setprecision(1) << score.score << "%";
        moduleTable_->elementAt(row, 1)->addWidget(std::make_unique<Wt::WText>(scoreStream.str()));

        // Format time
        int hours = score.timeSpentSeconds / 3600;
        int minutes = (score.timeSpentSeconds % 3600) / 60;
        std::ostringstream timeStream;
        timeStream << hours << "h " << minutes << "m";
        moduleTable_->elementAt(row, 2)->addWidget(std::make_unique<Wt::WText>(timeStream.str()));

        auto statusText = moduleTable_->elementAt(row, 3)->addWidget(
            std::make_unique<Wt::WText>(score.completed ? "Complete" : "Incomplete"));
        statusText->addStyleClass(score.completed ? "status-complete" : "status-incomplete");

        row++;
    }
}

void AssessmentReportWidget::createTimeLogSection() {
    timeLogSection_->clear();

    auto sectionTitle = timeLogSection_->addWidget(std::make_unique<Wt::WText>("Time Log Summary"));
    sectionTitle->addStyleClass("section-title");

    auto timeInfo = timeLogSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    timeInfo->addStyleClass("time-info");

    // Total hours
    auto totalHours = timeInfo->addWidget(std::make_unique<Wt::WContainerWidget>());
    totalHours->addStyleClass("time-stat");
    totalHours->addWidget(std::make_unique<Wt::WText>("Total Hours:"))->addStyleClass("time-label");
    std::ostringstream hoursStream;
    hoursStream << std::fixed << std::setprecision(1) << report_.getTotalTimeHours();
    totalHours->addWidget(std::make_unique<Wt::WText>(hoursStream.str()))->addStyleClass("time-value");

    // Note about time tracking
    auto note = timeLogSection_->addWidget(std::make_unique<Wt::WText>(
        "Time logged above represents active engagement with course materials as required for regulatory compliance."));
    note->addStyleClass("time-note");
}

void AssessmentReportWidget::createCertificateSection() {
    certificateSection_->clear();

    // Only show certificate section if passed
    if (!report_.hasPassed()) {
        certificateSection_->hide();
        return;
    }

    certificateSection_->show();

    auto sectionTitle = certificateSection_->addWidget(std::make_unique<Wt::WText>("Certificate"));
    sectionTitle->addStyleClass("section-title");

    if (report_.isCertificateIssued()) {
        auto certInfo = certificateSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
        certInfo->addStyleClass("certificate-info");

        auto statusRow = certInfo->addWidget(std::make_unique<Wt::WContainerWidget>());
        statusRow->addStyleClass("cert-row");
        certificateStatus_ = statusRow->addWidget(std::make_unique<Wt::WText>("Certificate Issued"));
        certificateStatus_->addStyleClass("cert-status issued");

        auto numberRow = certInfo->addWidget(std::make_unique<Wt::WContainerWidget>());
        numberRow->addStyleClass("cert-row");
        numberRow->addWidget(std::make_unique<Wt::WText>("Certificate Number: "))->addStyleClass("cert-label");
        certificateNumber_ = numberRow->addWidget(std::make_unique<Wt::WText>(report_.getCertificateNumber()));
        certificateNumber_->addStyleClass("cert-number");

        auto dateRow = certInfo->addWidget(std::make_unique<Wt::WContainerWidget>());
        dateRow->addStyleClass("cert-row");
        dateRow->addWidget(std::make_unique<Wt::WText>("Issued: "))->addStyleClass("cert-label");
        dateRow->addWidget(std::make_unique<Wt::WText>(report_.getCertificateIssuedAt()))->addStyleClass("cert-date");
    } else {
        auto pendingInfo = certificateSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
        pendingInfo->addStyleClass("certificate-pending");

        pendingInfo->addWidget(std::make_unique<Wt::WText>(
            "Your certificate is ready to be issued. Click the button below to request your certificate."));

        issueCertificateButton_ = certificateSection_->addWidget(
            std::make_unique<Wt::WPushButton>("Request Certificate"));
        issueCertificateButton_->addStyleClass("btn btn-success");
        issueCertificateButton_->clicked().connect([this] {
            if (classroomService_) {
                classroomService_->issueCertificate(report_.getId());
                // Reload report to show certificate
                loadReport(report_.getId());
            }
            certificateRequested_.emit();
        });
    }
}

} // namespace Classroom
} // namespace StudentIntake
