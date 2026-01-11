#include "DashboardWidget.h"
#include <Wt/WBreak.h>

namespace StudentIntake {
namespace Widgets {

DashboardWidget::DashboardWidget()
    : WContainerWidget()
    , welcomeText_(nullptr)
    , statusText_(nullptr)
    , curriculumText_(nullptr)
    , progressSection_(nullptr)
    , progressText_(nullptr)
    , continueButton_(nullptr)
    , completionSection_(nullptr) {
    setupUI();
}

DashboardWidget::~DashboardWidget() {
}

void DashboardWidget::setupUI() {
    addStyleClass("dashboard-widget");

    // Welcome section
    auto welcomeSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    welcomeSection->addStyleClass("welcome-section");

    welcomeText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>("<h2>Welcome to the Student Intake Portal</h2>"));

    statusText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>());
    statusText_->addStyleClass("status-text");

    // Curriculum info
    auto curriculumSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumSection->addStyleClass("curriculum-section card");

    curriculumText_ = curriculumSection->addWidget(std::make_unique<Wt::WText>());
    curriculumText_->addStyleClass("curriculum-info");

    // Progress section
    progressSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    progressSection_->addStyleClass("progress-section card");

    progressSection_->addWidget(std::make_unique<Wt::WText>("<h4>Application Progress</h4>"));

    progressText_ = progressSection_->addWidget(std::make_unique<Wt::WText>());
    progressText_->addStyleClass("progress-info");

    auto buttonContainer = progressSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    buttonContainer->addStyleClass("button-container");

    continueButton_ = buttonContainer->addWidget(std::make_unique<Wt::WPushButton>("Continue Application"));
    continueButton_->addStyleClass("btn btn-primary btn-lg");
    continueButton_->clicked().connect([this]() {
        continueClicked_.emit();
    });

    // Completion section (initially hidden)
    completionSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    completionSection_->addStyleClass("completion-section card");
    completionSection_->hide();

    completionSection_->addWidget(std::make_unique<Wt::WText>(
        "<div class='completion-icon'>&#10003;</div>"
        "<h3>Application Complete!</h3>"
        "<p>Thank you for completing your student intake application. "
        "Our admissions team will review your submission and contact you soon.</p>"
        "<p>You will receive a confirmation email with next steps.</p>"));

    auto completionButtons = completionSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionButtons->addStyleClass("button-container");

    auto reviewButton = completionButtons->addWidget(std::make_unique<Wt::WPushButton>("Review Submitted Forms"));
    reviewButton->addStyleClass("btn btn-outline-primary");
    reviewButton->clicked().connect([this]() {
        continueClicked_.emit();
    });

    // Help section
    auto helpSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    helpSection->addStyleClass("help-section card");
    helpSection->addWidget(std::make_unique<Wt::WText>(
        "<h4>Need Help?</h4>"
        "<p>If you have questions about the intake process, please contact:</p>"
        "<ul>"
        "<li>Email: admissions@university.edu</li>"
        "<li>Phone: (555) 123-4567</li>"
        "<li>Office Hours: Mon-Fri, 9 AM - 5 PM</li>"
        "</ul>"));
}

void DashboardWidget::refresh() {
    updateDisplay();
}

void DashboardWidget::updateDisplay() {
    if (!session_) return;

    // Update welcome message
    std::string name = session_->getStudent().getFullName();
    if (name.empty() || name == " ") {
        name = session_->getStudent().getEmail();
    }
    welcomeText_->setText("<h2>Welcome, " + name + "</h2>");

    // Update curriculum info
    if (session_->hasCurriculumSelected()) {
        auto& curriculum = session_->getCurrentCurriculum();
        std::string curriculumHtml =
            "<h4>Selected Program</h4>"
            "<p><strong>" + curriculum.getName() + "</strong></p>"
            "<p>" + curriculum.getDescription() + "</p>"
            "<p>Credit Hours: " + std::to_string(curriculum.getCreditHours()) + "</p>";
        curriculumText_->setText(curriculumHtml);
    } else {
        curriculumText_->setText("<p>No program selected yet.</p>");
    }

    // Update progress
    int completed = session_->getCompletedFormsCount();
    int total = session_->getTotalRequiredFormsCount();
    double percentage = session_->getProgressPercentage();

    std::string progressHtml =
        "<p>You have completed <strong>" + std::to_string(completed) +
        " of " + std::to_string(total) + "</strong> required forms.</p>"
        "<div class='progress'>"
        "<div class='progress-bar' style='width: " + std::to_string(static_cast<int>(percentage)) + "%'></div>"
        "</div>"
        "<p class='progress-percentage'>" + std::to_string(static_cast<int>(percentage)) + "% Complete</p>";
    progressText_->setText(progressHtml);

    // Update button text based on progress
    if (completed == 0) {
        continueButton_->setText("Start Application");
    } else if (completed == total) {
        continueButton_->setText("Review Application");
    } else {
        continueButton_->setText("Continue Application");
    }

    // Show/hide completion section
    if (session_->isIntakeComplete()) {
        completionSection_->show();
        progressSection_->hide();
    } else {
        completionSection_->hide();
        progressSection_->show();
    }
}

void DashboardWidget::showCompletionMessage() {
    completionSection_->show();
    progressSection_->hide();
}

} // namespace Widgets
} // namespace StudentIntake
