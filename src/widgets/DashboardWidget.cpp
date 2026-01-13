#include "DashboardWidget.h"
#include "session/SessionManager.h"
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
    , completionSection_(nullptr)
    , completedFormsSection_(nullptr)
    , completedFormsList_(nullptr)
    , additionalFormsSection_(nullptr) {
    setupUI();
}

DashboardWidget::~DashboardWidget() {
}

void DashboardWidget::setupUI() {
    addStyleClass("dashboard-widget");

    // Welcome section
    auto welcomeSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    welcomeSection->addStyleClass("welcome-section");

    welcomeText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>("<h2>Welcome to Student Onboarding</h2>"));

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

    // Completed forms section (shows when forms have been submitted)
    completedFormsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    completedFormsSection_->addStyleClass("completed-forms-section card");
    completedFormsSection_->hide();

    completedFormsSection_->addWidget(std::make_unique<Wt::WText>("<h4>Completed Forms</h4>"));
    completedFormsList_ = completedFormsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completedFormsList_->addStyleClass("completed-forms-list");

    // Additional forms section (post-onboarding forms)
    additionalFormsSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    additionalFormsSection_->addStyleClass("additional-forms-section card");
    additionalFormsSection_->hide();

    additionalFormsSection_->addWidget(std::make_unique<Wt::WText>(
        "<h4>Additional Forms</h4>"
        "<p>The following optional forms are available for your program:</p>"));

    auto additionalFormsButtons = additionalFormsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    additionalFormsButtons->addStyleClass("button-container");

    auto viewAdditionalBtn = additionalFormsButtons->addWidget(std::make_unique<Wt::WPushButton>("View Additional Forms"));
    viewAdditionalBtn->addStyleClass("btn btn-outline-primary");
    viewAdditionalBtn->clicked().connect([this]() {
        additionalFormsClicked_.emit();
    });

    // Completion section (initially hidden)
    completionSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    completionSection_->addStyleClass("completion-section card");
    completionSection_->hide();

    completionSection_->addWidget(std::make_unique<Wt::WText>(
        "<div class='completion-icon'>&#10003;</div>"
        "<h3>Onboarding Complete!</h3>"
        "<p>Thank you for completing your student onboarding forms. "
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
        "<p>If you have questions about the onboarding process, please contact:</p>"
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

    // Update completed forms display
    updateCompletedFormsDisplay();

    // Show/hide sections based on completion status
    if (session_->isIntakeComplete()) {
        completionSection_->show();
        progressSection_->hide();
        // Show additional forms section if there are optional forms available
        additionalFormsSection_->show();
    } else {
        completionSection_->hide();
        progressSection_->show();
        additionalFormsSection_->hide();
    }

    // Always show completed forms if any exist
    if (completed > 0) {
        completedFormsSection_->show();
    } else {
        completedFormsSection_->hide();
    }
}

void DashboardWidget::updateCompletedFormsDisplay() {
    if (!session_ || !completedFormsList_) return;

    completedFormsList_->clear();

    // Get form type info from session manager
    auto& formTypeInfos = Session::SessionManager::getInstance().getFormTypeInfos();
    auto completedFormIds = session_->getStudent().getCompletedFormIds();

    if (completedFormIds.empty()) {
        return;
    }

    for (const auto& formId : completedFormIds) {
        // Find form name from form type info
        std::string formName = formId;
        for (const auto& info : formTypeInfos) {
            if (info.id == formId) {
                formName = info.name;
                break;
            }
        }

        auto formItem = completedFormsList_->addWidget(std::make_unique<Wt::WContainerWidget>());
        formItem->addStyleClass("completed-form-item");

        // Add checkmark icon and form name
        auto formLabel = formItem->addWidget(std::make_unique<Wt::WText>(
            "<span class='form-check-icon'>&#10003;</span> " + formName));
        formLabel->addStyleClass("form-label");

        // Add view/edit button
        auto viewBtn = formItem->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->addStyleClass("btn btn-sm btn-outline-secondary");
        viewBtn->clicked().connect([this, formId]() {
            viewFormClicked_.emit(formId);
        });
    }
}

void DashboardWidget::showCompletionMessage() {
    completionSection_->show();
    progressSection_->hide();
}

} // namespace Widgets
} // namespace StudentIntake
