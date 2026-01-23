#include "FeedbackWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WMessageBox.h>

namespace StudentIntake {
namespace Instructor {

FeedbackWidget::FeedbackWidget()
    : instructorId_(0)
    , feedbackDialog_(nullptr)
{
    setStyleClass("feedback-widget");
    setupUI();
}

void FeedbackWidget::setupUI() {
    // Header
    headerSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerSection_->setStyleClass("widget-header");

    backButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Back"));
    backButton_->setStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this] { backClicked_.emit(); });

    titleText_ = headerSection_->addWidget(std::make_unique<Wt::WText>("Student Feedback"));
    titleText_->setStyleClass("widget-title");

    addFeedbackButton_ = headerSection_->addWidget(std::make_unique<Wt::WPushButton>("Add Feedback"));
    addFeedbackButton_->setStyleClass("btn btn-primary");
    addFeedbackButton_->clicked().connect([this] { showAddFeedbackDialog(); });

    // Filter section
    filterSection_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    filterSection_->setStyleClass("filter-section");

    auto typeLabel = filterSection_->addWidget(std::make_unique<Wt::WText>("Type: "));
    typeLabel->setStyleClass("filter-label");

    typeFilter_ = filterSection_->addWidget(std::make_unique<Wt::WComboBox>());
    typeFilter_->addItem("All Types");
    typeFilter_->addItem("Progress Update");
    typeFilter_->addItem("Session Review");
    typeFilter_->addItem("Skill Assessment");
    typeFilter_->addItem("Behavior");
    typeFilter_->addItem("Safety Concern");
    typeFilter_->addItem("Commendation");
    typeFilter_->setStyleClass("filter-combo");
    typeFilter_->changed().connect([this] { updateFeedbackTable(); });

    followUpFilter_ = filterSection_->addWidget(std::make_unique<Wt::WCheckBox>("Show only pending follow-ups"));
    followUpFilter_->setStyleClass("filter-checkbox");
    followUpFilter_->changed().connect([this] { updateFeedbackTable(); });

    // Feedback table
    createFeedbackTable();

    noFeedbackText_ = addWidget(std::make_unique<Wt::WText>("No feedback found"));
    noFeedbackText_->setStyleClass("no-data-text");
    noFeedbackText_->hide();
}

void FeedbackWidget::createFeedbackTable() {
    feedbackTable_ = addWidget(std::make_unique<Wt::WTable>());
    feedbackTable_->setStyleClass("feedback-table");
    feedbackTable_->setHeaderCount(1);

    feedbackTable_->elementAt(0, 0)->addWidget(std::make_unique<Wt::WText>("Date"));
    feedbackTable_->elementAt(0, 1)->addWidget(std::make_unique<Wt::WText>("Student"));
    feedbackTable_->elementAt(0, 2)->addWidget(std::make_unique<Wt::WText>("Type"));
    feedbackTable_->elementAt(0, 3)->addWidget(std::make_unique<Wt::WText>("Subject"));
    feedbackTable_->elementAt(0, 4)->addWidget(std::make_unique<Wt::WText>("Rating"));
    feedbackTable_->elementAt(0, 5)->addWidget(std::make_unique<Wt::WText>("Follow-up"));
    feedbackTable_->elementAt(0, 6)->addWidget(std::make_unique<Wt::WText>("Actions"));
}

void FeedbackWidget::loadFeedback() {
    updateFeedbackTable();
}

void FeedbackWidget::updateFeedbackTable() {
    // Clear existing rows
    while (feedbackTable_->rowCount() > 1) {
        feedbackTable_->removeRow(1);
    }

    if (!instructorService_) {
        noFeedbackText_->show();
        feedbackTable_->hide();
        return;
    }

    std::vector<Models::StudentFeedback> feedbackList;

    if (followUpFilter_->isChecked()) {
        feedbackList = instructorService_->getPendingFollowUps(instructorId_);
    } else {
        feedbackList = instructorService_->getInstructorFeedback(instructorId_);
    }

    // Filter by type if needed
    int typeIndex = typeFilter_->currentIndex();
    if (typeIndex > 0) {
        Models::FeedbackType filterType;
        switch (typeIndex) {
            case 1: filterType = Models::FeedbackType::ProgressUpdate; break;
            case 2: filterType = Models::FeedbackType::SessionReview; break;
            case 3: filterType = Models::FeedbackType::SkillAssessment; break;
            case 4: filterType = Models::FeedbackType::Behavior; break;
            case 5: filterType = Models::FeedbackType::SafetyConcern; break;
            case 6: filterType = Models::FeedbackType::Commendation; break;
            default: filterType = Models::FeedbackType::General; break;
        }

        std::vector<Models::StudentFeedback> filtered;
        for (const auto& fb : feedbackList) {
            if (fb.getFeedbackType() == filterType) {
                filtered.push_back(fb);
            }
        }
        feedbackList = filtered;
    }

    if (feedbackList.empty()) {
        noFeedbackText_->show();
        feedbackTable_->hide();
        return;
    }

    noFeedbackText_->hide();
    feedbackTable_->show();

    for (const auto& feedback : feedbackList) {
        int row = feedbackTable_->rowCount();

        // Date
        std::string dateStr = feedback.getCreatedAt();
        if (dateStr.length() > 10) dateStr = dateStr.substr(0, 10);
        feedbackTable_->elementAt(row, 0)->addWidget(std::make_unique<Wt::WText>(dateStr));

        // Student
        feedbackTable_->elementAt(row, 1)->addWidget(
            std::make_unique<Wt::WText>("Student #" + std::to_string(feedback.getStudentId())));

        // Type
        auto typeText = feedbackTable_->elementAt(row, 2)->addWidget(
            std::make_unique<Wt::WText>(feedback.getFeedbackTypeString()));
        typeText->setStyleClass("feedback-type type-" + feedback.getFeedbackTypeString());

        // Subject
        feedbackTable_->elementAt(row, 3)->addWidget(std::make_unique<Wt::WText>(feedback.getSubject()));

        // Rating
        if (feedback.getPerformanceRating() > 0) {
            std::string ratingStr = std::to_string(feedback.getPerformanceRating()) + "/" +
                                   std::to_string(feedback.getRatingScale());
            feedbackTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>(ratingStr));
        } else {
            feedbackTable_->elementAt(row, 4)->addWidget(std::make_unique<Wt::WText>("-"));
        }

        // Follow-up status
        auto followUpCell = feedbackTable_->elementAt(row, 5);
        if (feedback.requiresFollowUp()) {
            if (feedback.isFollowUpCompleted()) {
                followUpCell->addWidget(std::make_unique<Wt::WText>("Completed"))->setStyleClass("followup-completed");
            } else {
                followUpCell->addWidget(std::make_unique<Wt::WText>("Pending"))->setStyleClass("followup-pending");
            }
        } else {
            followUpCell->addWidget(std::make_unique<Wt::WText>("-"));
        }

        // Actions
        auto actionsCell = feedbackTable_->elementAt(row, 6);
        actionsCell->setStyleClass("actions-cell");

        std::string feedbackId = feedback.getId();

        auto viewBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("View"));
        viewBtn->setStyleClass("btn btn-sm btn-secondary");
        viewBtn->clicked().connect([this, feedbackId] { editFeedback(feedbackId); });

        if (feedback.requiresFollowUp() && !feedback.isFollowUpCompleted()) {
            auto followUpBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Complete Follow-up"));
            followUpBtn->setStyleClass("btn btn-sm btn-success");
            followUpBtn->clicked().connect([this, feedbackId] { completeFollowUp(feedbackId); });
        }

        auto deleteBtn = actionsCell->addWidget(std::make_unique<Wt::WPushButton>("Delete"));
        deleteBtn->setStyleClass("btn btn-sm btn-danger");
        deleteBtn->clicked().connect([this, feedbackId] { deleteFeedback(feedbackId); });
    }
}

void FeedbackWidget::createFeedbackDialog() {
    feedbackDialog_ = addChild(std::make_unique<Wt::WDialog>("Add Feedback"));
    feedbackDialog_->setStyleClass("feedback-dialog");
    feedbackDialog_->setModal(true);
    feedbackDialog_->setClosable(true);

    auto content = feedbackDialog_->contents();
    content->setStyleClass("dialog-content");

    // Student selection
    auto studentRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    studentRow->setStyleClass("form-row");
    studentRow->addWidget(std::make_unique<Wt::WText>("Student:"))->setStyleClass("form-label");
    studentCombo_ = studentRow->addWidget(std::make_unique<Wt::WComboBox>());
    studentCombo_->setStyleClass("form-input");

    // Feedback type
    auto typeRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    typeRow->setStyleClass("form-row");
    typeRow->addWidget(std::make_unique<Wt::WText>("Type:"))->setStyleClass("form-label");
    feedbackTypeCombo_ = typeRow->addWidget(std::make_unique<Wt::WComboBox>());
    feedbackTypeCombo_->setStyleClass("form-input");
    populateFeedbackTypes();

    // Subject
    auto subjectRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    subjectRow->setStyleClass("form-row");
    subjectRow->addWidget(std::make_unique<Wt::WText>("Subject:"))->setStyleClass("form-label");
    subjectEdit_ = subjectRow->addWidget(std::make_unique<Wt::WLineEdit>());
    subjectEdit_->setStyleClass("form-input");

    // Feedback text
    auto textRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    textRow->setStyleClass("form-row");
    textRow->addWidget(std::make_unique<Wt::WText>("Feedback:"))->setStyleClass("form-label");
    feedbackTextEdit_ = textRow->addWidget(std::make_unique<Wt::WTextArea>());
    feedbackTextEdit_->setStyleClass("form-input");
    feedbackTextEdit_->setRows(4);

    // Rating
    auto ratingRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    ratingRow->setStyleClass("form-row");
    ratingRow->addWidget(std::make_unique<Wt::WText>("Performance Rating (1-5):"))->setStyleClass("form-label");
    ratingEdit_ = ratingRow->addWidget(std::make_unique<Wt::WSpinBox>());
    ratingEdit_->setRange(0, 5);
    ratingEdit_->setValue(0);
    ratingEdit_->setStyleClass("form-input rating-input");

    // Strengths
    auto strengthsRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    strengthsRow->setStyleClass("form-row");
    strengthsRow->addWidget(std::make_unique<Wt::WText>("Strengths:"))->setStyleClass("form-label");
    strengthsEdit_ = strengthsRow->addWidget(std::make_unique<Wt::WTextArea>());
    strengthsEdit_->setStyleClass("form-input");
    strengthsEdit_->setRows(2);

    // Areas for improvement
    auto improvementRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    improvementRow->setStyleClass("form-row");
    improvementRow->addWidget(std::make_unique<Wt::WText>("Areas for Improvement:"))->setStyleClass("form-label");
    improvementEdit_ = improvementRow->addWidget(std::make_unique<Wt::WTextArea>());
    improvementEdit_->setStyleClass("form-input");
    improvementEdit_->setRows(2);

    // Recommended actions
    auto actionsRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    actionsRow->setStyleClass("form-row");
    actionsRow->addWidget(std::make_unique<Wt::WText>("Recommended Actions:"))->setStyleClass("form-label");
    actionsEdit_ = actionsRow->addWidget(std::make_unique<Wt::WTextArea>());
    actionsEdit_->setStyleClass("form-input");
    actionsEdit_->setRows(2);

    // Options
    auto optionsRow = content->addWidget(std::make_unique<Wt::WContainerWidget>());
    optionsRow->setStyleClass("form-row options-row");

    visibleToStudentCheck_ = optionsRow->addWidget(std::make_unique<Wt::WCheckBox>("Visible to student"));
    visibleToStudentCheck_->setChecked(true);

    requiresFollowUpCheck_ = optionsRow->addWidget(std::make_unique<Wt::WCheckBox>("Requires follow-up"));

    // Dialog buttons
    auto footer = feedbackDialog_->footer();
    footer->setStyleClass("dialog-footer");

    dialogCancelButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Cancel"));
    dialogCancelButton_->setStyleClass("btn btn-secondary");
    dialogCancelButton_->clicked().connect([this] { feedbackDialog_->reject(); });

    saveButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Save"));
    saveButton_->setStyleClass("btn btn-primary");
    saveButton_->clicked().connect([this] { saveFeedback(); });
}

void FeedbackWidget::showAddFeedbackDialog(int studentId) {
    editingFeedbackId_ = "";

    if (!feedbackDialog_) {
        createFeedbackDialog();
    }

    populateStudentCombo();

    // Select student if provided
    if (studentId > 0) {
        for (int i = 0; i < studentCombo_->count(); ++i) {
            if (studentCombo_->itemText(i).toUTF8().find(std::to_string(studentId)) != std::string::npos) {
                studentCombo_->setCurrentIndex(i);
                break;
            }
        }
    }

    // Reset form
    feedbackTypeCombo_->setCurrentIndex(0);
    subjectEdit_->setText("");
    feedbackTextEdit_->setText("");
    ratingEdit_->setValue(0);
    strengthsEdit_->setText("");
    improvementEdit_->setText("");
    actionsEdit_->setText("");
    visibleToStudentCheck_->setChecked(true);
    requiresFollowUpCheck_->setChecked(false);

    feedbackDialog_->setWindowTitle("Add Feedback");
    feedbackDialog_->show();
}

void FeedbackWidget::editFeedback(const std::string& feedbackId) {
    if (!instructorService_) return;

    auto feedback = instructorService_->getFeedback(feedbackId);
    if (feedback.getId().empty()) return;

    editingFeedbackId_ = feedbackId;

    if (!feedbackDialog_) {
        createFeedbackDialog();
    }

    populateStudentCombo();

    // Set form values
    subjectEdit_->setText(feedback.getSubject());
    feedbackTextEdit_->setText(feedback.getFeedbackText());
    ratingEdit_->setValue(feedback.getPerformanceRating());
    strengthsEdit_->setText(feedback.getStrengths());
    improvementEdit_->setText(feedback.getAreasForImprovement());
    actionsEdit_->setText(feedback.getRecommendedActions());
    visibleToStudentCheck_->setChecked(feedback.isVisibleToStudent());
    requiresFollowUpCheck_->setChecked(feedback.requiresFollowUp());

    // Set feedback type
    int typeIndex = static_cast<int>(feedback.getFeedbackType());
    if (typeIndex >= 0 && typeIndex < feedbackTypeCombo_->count()) {
        feedbackTypeCombo_->setCurrentIndex(typeIndex);
    }

    feedbackDialog_->setWindowTitle("Edit Feedback");
    feedbackDialog_->show();
}

void FeedbackWidget::saveFeedback() {
    if (!instructorService_) return;

    Models::StudentFeedback feedback;

    if (!editingFeedbackId_.empty()) {
        feedback = instructorService_->getFeedback(editingFeedbackId_);
    }

    feedback.setInstructorId(instructorId_);
    feedback.setSubject(subjectEdit_->text().toUTF8());
    feedback.setFeedbackText(feedbackTextEdit_->text().toUTF8());
    feedback.setPerformanceRating(ratingEdit_->value());
    feedback.setRatingScale(5);
    feedback.setStrengths(strengthsEdit_->text().toUTF8());
    feedback.setAreasForImprovement(improvementEdit_->text().toUTF8());
    feedback.setRecommendedActions(actionsEdit_->text().toUTF8());
    feedback.setVisibleToStudent(visibleToStudentCheck_->isChecked());
    feedback.setRequiresFollowUp(requiresFollowUpCheck_->isChecked());

    // Get selected feedback type
    int typeIndex = feedbackTypeCombo_->currentIndex();
    Models::FeedbackType types[] = {
        Models::FeedbackType::ProgressUpdate,
        Models::FeedbackType::SessionReview,
        Models::FeedbackType::SkillAssessment,
        Models::FeedbackType::Behavior,
        Models::FeedbackType::SafetyConcern,
        Models::FeedbackType::Commendation,
        Models::FeedbackType::General
    };
    if (typeIndex >= 0 && typeIndex < 7) {
        feedback.setFeedbackType(types[typeIndex]);
    }

    Api::InstructorResult result;
    if (editingFeedbackId_.empty()) {
        result = instructorService_->createFeedback(feedback);
    } else {
        result = instructorService_->updateFeedback(feedback);
    }

    if (result.success) {
        feedbackDialog_->accept();
        updateFeedbackTable();
    } else {
        Wt::WMessageBox::show("Error", "Failed to save feedback: " + result.message,
                              Wt::StandardButton::Ok);
    }
}

void FeedbackWidget::deleteFeedback(const std::string& feedbackId) {
    if (!instructorService_) return;

    auto result = Wt::WMessageBox::show("Delete Feedback",
        "Are you sure you want to delete this feedback?",
        Wt::StandardButton::Yes | Wt::StandardButton::No);

    if (result == Wt::StandardButton::Yes) {
        auto apiResult = instructorService_->deleteFeedback(feedbackId);
        if (apiResult.success) {
            updateFeedbackTable();
        }
    }
}

void FeedbackWidget::completeFollowUp(const std::string& feedbackId) {
    if (!instructorService_) return;

    auto result = instructorService_->completeFollowUp(feedbackId);
    if (result.success) {
        updateFeedbackTable();
    }
}

void FeedbackWidget::populateStudentCombo() {
    studentCombo_->clear();

    if (!instructorService_) return;

    auto students = instructorService_->getAssignedStudentProgress(instructorId_);
    for (const auto& student : students) {
        std::string label = student.studentName.empty() ?
            "Student #" + std::to_string(student.studentId) : student.studentName;
        studentCombo_->addItem(label);
    }
}

void FeedbackWidget::populateFeedbackTypes() {
    feedbackTypeCombo_->addItem("Progress Update");
    feedbackTypeCombo_->addItem("Session Review");
    feedbackTypeCombo_->addItem("Skill Assessment");
    feedbackTypeCombo_->addItem("Behavior");
    feedbackTypeCombo_->addItem("Safety Concern");
    feedbackTypeCombo_->addItem("Commendation");
    feedbackTypeCombo_->addItem("General");
}

} // namespace Instructor
} // namespace StudentIntake
