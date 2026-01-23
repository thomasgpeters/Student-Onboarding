#ifndef ASSESSMENT_REPORT_WIDGET_H
#define ASSESSMENT_REPORT_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>
#include <Wt/WSignal.h>
#include <memory>
#include "api/ClassroomService.h"
#include "models/Assessment.h"

namespace StudentIntake {
namespace Classroom {

/**
 * @brief Widget for displaying the final assessment report
 *
 * Shows a comprehensive report upon course completion including:
 * - Overall score and pass/fail status
 * - Module-by-module breakdown
 * - Time spent summary
 * - Certificate information (if applicable)
 */
class AssessmentReportWidget : public Wt::WContainerWidget {
public:
    AssessmentReportWidget();
    ~AssessmentReportWidget() = default;

    void setClassroomService(std::shared_ptr<Api::ClassroomService> service) { classroomService_ = service; }

    /**
     * @brief Load and display a report
     */
    void loadReport(const std::string& reportId);

    /**
     * @brief Load report by enrollment ID
     */
    void loadReportByEnrollment(const std::string& enrollmentId);

    /**
     * @brief Print the report
     */
    void printReport();

    // Signals
    Wt::Signal<>& backClicked() { return backClicked_; }
    Wt::Signal<>& certificateRequested() { return certificateRequested_; }

private:
    void setupUI();
    void updateDisplay();
    void createSummarySection();
    void createModuleBreakdownSection();
    void createTimeLogSection();
    void createCertificateSection();

    std::shared_ptr<Api::ClassroomService> classroomService_;
    Models::AssessmentReport report_;

    // UI sections
    Wt::WContainerWidget* headerSection_;
    Wt::WContainerWidget* summarySection_;
    Wt::WContainerWidget* moduleBreakdownSection_;
    Wt::WContainerWidget* timeLogSection_;
    Wt::WContainerWidget* certificateSection_;
    Wt::WContainerWidget* actionsSection_;

    // Header widgets
    Wt::WText* reportTitle_;
    Wt::WText* reportNumber_;

    // Summary widgets
    Wt::WText* studentName_;
    Wt::WText* courseName_;
    Wt::WText* completionDate_;
    Wt::WText* overallScore_;
    Wt::WText* passFailStatus_;
    Wt::WText* totalTime_;

    // Module breakdown
    Wt::WTable* moduleTable_;

    // Certificate
    Wt::WText* certificateStatus_;
    Wt::WText* certificateNumber_;
    Wt::WPushButton* issueCertificateButton_;

    // Actions
    Wt::WPushButton* printButton_;
    Wt::WPushButton* backButton_;

    // Signals
    Wt::Signal<> backClicked_;
    Wt::Signal<> certificateRequested_;
};

} // namespace Classroom
} // namespace StudentIntake

#endif // ASSESSMENT_REPORT_WIDGET_H
