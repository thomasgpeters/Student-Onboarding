#include "ProgressWidget.h"

namespace StudentIntake {
namespace Widgets {

ProgressWidget::ProgressWidget()
    : WContainerWidget()
    , headerContainer_(nullptr)
    , progressBar_(nullptr)
    , progressText_(nullptr)
    , stepsContainer_(nullptr) {
    setupUI();
}

ProgressWidget::~ProgressWidget() {
}

void ProgressWidget::setupUI() {
    addStyleClass("progress-widget");

    // Header with progress bar
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("progress-header");

    auto titleText = headerContainer_->addWidget(std::make_unique<Wt::WText>("<h4>Your Progress</h4>"));
    titleText->setTextFormat(Wt::TextFormat::XHTML);

    // Progress percentage and bar in a row
    auto progressRow = headerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    progressRow->addStyleClass("progress-row");
    progressRow->setAttributeValue("style",
        "display: flex; align-items: center; gap: 0.75rem; margin: 0.75rem 0;");

    progressText_ = progressRow->addWidget(std::make_unique<Wt::WText>("0%"));
    progressText_->addStyleClass("progress-percentage");
    progressText_->setAttributeValue("style",
        "font-size: 1.25rem; font-weight: 700; color: #2563eb; min-width: 50px;");

    // Progress bar container
    auto progressContainer = progressRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    progressContainer->addStyleClass("progress-bar-container");
    progressContainer->setAttributeValue("style", "flex: 1;");

    progressBar_ = progressContainer->addWidget(std::make_unique<Wt::WProgressBar>());
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->addStyleClass("main-progress-bar");
    progressBar_->setFormat("");  // Hide the default percentage text inside bar

    // Steps list
    stepsContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    stepsContainer_->addStyleClass("steps-list");
}

void ProgressWidget::setRequiredForms(const std::vector<std::string>& formIds) {
    requiredFormIds_ = formIds;
    updateSteps();
}

void ProgressWidget::updateSteps() {
    stepsContainer_->clear();
    steps_.clear();

    int stepNumber = 1;
    for (const auto& formId : requiredFormIds_) {
        StepInfo step;
        step.formId = formId;
        step.completed = false;
        step.current = (formId == currentFormId_);

        if (session_) {
            step.completed = session_->getStudent().hasCompletedForm(formId);
        }

        step.container = stepsContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
        step.container->addStyleClass("step-item");

        if (step.completed) {
            step.container->addStyleClass("completed");
        }
        if (step.current) {
            step.container->addStyleClass("current");
        }

        // Step number
        step.numberText = step.container->addWidget(std::make_unique<Wt::WText>(
            std::to_string(stepNumber)));
        step.numberText->addStyleClass("step-number");

        // Step content
        auto contentContainer = step.container->addWidget(std::make_unique<Wt::WContainerWidget>());
        contentContainer->addStyleClass("step-content");

        std::string title = formId;
        if (formFactory_) {
            title = formFactory_->getFormTitle(formId);
        }
        step.titleText = contentContainer->addWidget(std::make_unique<Wt::WText>(title));
        step.titleText->addStyleClass("step-title");

        // Status icon
        std::string statusIcon;
        std::string statusClass;
        if (step.completed) {
            statusIcon = "&#10003;";  // Checkmark
            statusClass = "status-complete";
        } else if (step.current) {
            statusIcon = "&#9679;";  // Filled circle
            statusClass = "status-current";
        } else {
            statusIcon = "&#9675;";  // Empty circle
            statusClass = "status-pending";
        }
        step.statusIcon = step.container->addWidget(std::make_unique<Wt::WText>(statusIcon));
        step.statusIcon->addStyleClass("step-status " + statusClass);

        // Click handler (only for completed steps to allow review)
        step.container->clicked().connect([this, formId]() {
            stepClicked_.emit(formId);
        });

        steps_.push_back(step);
        stepNumber++;
    }
}

void ProgressWidget::setCurrentStep(const std::string& formId) {
    currentFormId_ = formId;
    refresh();
}

void ProgressWidget::refresh() {
    if (!session_) return;

    // Update progress bar
    double percentage = session_->getProgressPercentage();
    progressBar_->setValue(static_cast<int>(percentage));
    progressText_->setText(std::to_string(static_cast<int>(percentage)) + "%");

    // Update step states
    for (auto& step : steps_) {
        step.completed = session_->getStudent().hasCompletedForm(step.formId);
        step.current = (step.formId == currentFormId_);

        step.container->removeStyleClass("completed");
        step.container->removeStyleClass("current");

        if (step.completed) {
            step.container->addStyleClass("completed");
            step.statusIcon->setText("&#10003;");
            step.statusIcon->setStyleClass("step-status status-complete");
        } else if (step.current) {
            step.container->addStyleClass("current");
            step.statusIcon->setText("&#9679;");
            step.statusIcon->setStyleClass("step-status status-current");
        } else {
            step.statusIcon->setText("&#9675;");
            step.statusIcon->setStyleClass("step-status status-pending");
        }
    }
}

} // namespace Widgets
} // namespace StudentIntake
