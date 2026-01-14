#include "DashboardWidget.h"
#include "session/SessionManager.h"
#include <Wt/WBreak.h>

namespace StudentIntake {
namespace Widgets {

DashboardWidget::DashboardWidget()
    : WContainerWidget()
    , mainLayout_(nullptr)
    , leftColumn_(nullptr)
    , rightPanel_(nullptr)
    , welcomeText_(nullptr)
    , statusText_(nullptr)
    , curriculumText_(nullptr)
    , progressSection_(nullptr)
    , progressText_(nullptr)
    , continueButton_(nullptr)
    , completionSection_(nullptr)
    , completedFormsSection_(nullptr)
    , completedFormsList_(nullptr)
    , recommendedFormsSection_(nullptr)
    , recommendedFormsList_(nullptr) {
    setupUI();
}

DashboardWidget::~DashboardWidget() {
}

void DashboardWidget::setupUI() {
    addStyleClass("dashboard-widget");

    // Welcome section (full width, above the columns)
    auto welcomeSection = addWidget(std::make_unique<Wt::WContainerWidget>());
    welcomeSection->addStyleClass("welcome-section");

    welcomeText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>("<h2>Welcome to Student Onboarding</h2>"));

    statusText_ = welcomeSection->addWidget(std::make_unique<Wt::WText>());
    statusText_->addStyleClass("status-text");

    // Main layout container (two columns)
    mainLayout_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    mainLayout_->addStyleClass("dashboard-layout");

    // Left column - main content
    leftColumn_ = mainLayout_->addWidget(std::make_unique<Wt::WContainerWidget>());
    leftColumn_->addStyleClass("dashboard-main");

    // Curriculum info
    auto curriculumSection = leftColumn_->addWidget(std::make_unique<Wt::WContainerWidget>());
    curriculumSection->addStyleClass("curriculum-section card");

    curriculumText_ = curriculumSection->addWidget(std::make_unique<Wt::WText>());
    curriculumText_->setTextFormat(Wt::TextFormat::XHTML);
    curriculumText_->addStyleClass("curriculum-info");

    // Change Program link
    auto changeProgramBtn = curriculumSection->addWidget(std::make_unique<Wt::WPushButton>("Change Program"));
    changeProgramBtn->addStyleClass("btn btn-link btn-sm text-primary");
    changeProgramBtn->clicked().connect([this]() {
        changeProgramClicked_.emit();
    });

    // Progress section
    progressSection_ = leftColumn_->addWidget(std::make_unique<Wt::WContainerWidget>());
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
    completionSection_ = leftColumn_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completionSection_->addStyleClass("completion-section card");
    completionSection_->hide();

    completionSection_->addWidget(std::make_unique<Wt::WText>(
        "<div class='completion-icon'>&#10003;</div>"
        "<h3>Onboarding Complete!</h3>"
        "<p>Thank you for completing your student onboarding forms. "
        "Our admissions team will review your submission and contact you soon.</p>"
        "<p>You will receive a confirmation email with next steps.</p>"));

    // Help section
    auto helpSection = leftColumn_->addWidget(std::make_unique<Wt::WContainerWidget>());
    helpSection->addStyleClass("help-section card");
    helpSection->addWidget(std::make_unique<Wt::WText>(
        "<h4>Need Help?</h4>"
        "<p>If you have questions about the onboarding process, please contact:</p>"
        "<ul>"
        "<li>Email: admissions@university.edu</li>"
        "<li>Phone: (555) 123-4567</li>"
        "<li>Office Hours: Mon-Fri, 9 AM - 5 PM</li>"
        "</ul>"));

    // Right panel - forms sidebar (visible after completion)
    // Using CSS class-based visibility instead of Wt's hide()/show() to prevent inline style conflicts
    rightPanel_ = mainLayout_->addWidget(std::make_unique<Wt::WContainerWidget>());
    rightPanel_->addStyleClass("dashboard-sidebar sidebar-hidden");

    // Completed forms section
    completedFormsSection_ = rightPanel_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completedFormsSection_->addStyleClass("sidebar-section");

    completedFormsSection_->addWidget(std::make_unique<Wt::WText>("<h4>Completed Forms</h4>"));
    completedFormsList_ = completedFormsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    completedFormsList_->addStyleClass("sidebar-forms-list");

    // Recommended/Additional forms section
    recommendedFormsSection_ = rightPanel_->addWidget(std::make_unique<Wt::WContainerWidget>());
    recommendedFormsSection_->addStyleClass("sidebar-section");

    recommendedFormsSection_->addWidget(std::make_unique<Wt::WText>("<h4>Recommended Forms</h4>"));
    recommendedFormsList_ = recommendedFormsSection_->addWidget(std::make_unique<Wt::WContainerWidget>());
    recommendedFormsList_->addStyleClass("sidebar-forms-list");
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

    // Show/hide sections based on completion status
    // Using CSS class-based visibility for the sidebar to prevent Wt inline style conflicts
    if (session_->isIntakeComplete()) {
        completionSection_->show();
        progressSection_->hide();
        // Show right panel with completed forms and recommendations using CSS classes
        rightPanel_->removeStyleClass("sidebar-hidden");
        rightPanel_->addStyleClass("sidebar-visible");
        updateCompletedFormsDisplay();
        updateRecommendedFormsDisplay();
    } else {
        completionSection_->hide();
        progressSection_->show();
        // Hide right panel using CSS classes
        rightPanel_->removeStyleClass("sidebar-visible");
        rightPanel_->addStyleClass("sidebar-hidden");
    }
}

void DashboardWidget::updateCompletedFormsDisplay() {
    if (!session_ || !completedFormsList_) return;

    completedFormsList_->clear();

    // Get form type info from session manager
    auto formTypeInfos = Session::SessionManager::getInstance().getFormTypeInfos();
    auto completedFormIds = session_->getStudent().getCompletedForms();

    if (completedFormIds.empty()) {
        completedFormsList_->addWidget(std::make_unique<Wt::WText>(
            "<p class='no-forms-text'>No forms completed yet.</p>"));
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
        formItem->addStyleClass("sidebar-form-item");

        // Add checkmark icon and form name
        auto formLabel = formItem->addWidget(std::make_unique<Wt::WText>(
            "<span class='form-status-icon completed'>&#10003;</span>" + formName));
        formLabel->addStyleClass("sidebar-form-label");

        // Add view button with eye icon
        auto viewBtn = formItem->addWidget(std::make_unique<Wt::WPushButton>());
        viewBtn->setText("<span class='eye-icon'>&#128065;</span>");
        viewBtn->setTextFormat(Wt::TextFormat::XHTML);
        viewBtn->addStyleClass("btn-icon");
        viewBtn->setToolTip("View form");
        viewBtn->clicked().connect([this, formId]() {
            viewFormClicked_.emit(formId);
        });
    }
}

void DashboardWidget::updateRecommendedFormsDisplay() {
    if (!session_ || !recommendedFormsList_) return;

    recommendedFormsList_->clear();

    // Get form type info from session manager
    auto formTypeInfos = Session::SessionManager::getInstance().getFormTypeInfos();
    auto completedFormIds = session_->getStudent().getCompletedForms();

    // Find forms that are not required but available (optional forms)
    std::vector<Models::FormTypeInfo> recommendedForms;
    for (const auto& info : formTypeInfos) {
        // Check if form is not already completed and is optional
        bool isCompleted = std::find(completedFormIds.begin(), completedFormIds.end(), info.id) != completedFormIds.end();
        if (!isCompleted && !info.isRequired) {
            recommendedForms.push_back(info);
        }
    }

    if (recommendedForms.empty()) {
        recommendedFormsList_->addWidget(std::make_unique<Wt::WText>(
            "<p class='no-forms-text'>No additional forms available.</p>"));
        return;
    }

    for (const auto& info : recommendedForms) {
        auto formItem = recommendedFormsList_->addWidget(std::make_unique<Wt::WContainerWidget>());
        formItem->addStyleClass("sidebar-form-item");

        // Add plus icon and form name
        auto formLabel = formItem->addWidget(std::make_unique<Wt::WText>(
            "<span class='form-status-icon optional'>+</span>" + info.name));
        formLabel->addStyleClass("sidebar-form-label");

        // Add arrow button to start form
        auto startBtn = formItem->addWidget(std::make_unique<Wt::WPushButton>());
        startBtn->setText("<span class='arrow-icon'>&#8594;</span>");
        startBtn->setTextFormat(Wt::TextFormat::XHTML);
        startBtn->addStyleClass("btn-icon");
        startBtn->setToolTip("Start form");
        startBtn->clicked().connect([this, id = info.id]() {
            viewFormClicked_.emit(id);
        });
    }
}

void DashboardWidget::showCompletionMessage() {
    completionSection_->show();
    progressSection_->hide();
    // Show right panel using CSS classes to avoid inline style conflicts
    rightPanel_->removeStyleClass("sidebar-hidden");
    rightPanel_->addStyleClass("sidebar-visible");
    updateCompletedFormsDisplay();
    updateRecommendedFormsDisplay();
}

} // namespace Widgets
} // namespace StudentIntake
