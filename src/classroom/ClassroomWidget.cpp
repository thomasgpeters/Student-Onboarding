#include "ClassroomWidget.h"
#include "AssessmentWidget.h"
#include <Wt/WBreak.h>
#include <Wt/WImage.h>
#include <Wt/WProgressBar.h>
#include <Wt/WTemplate.h>
#include <Wt/WVideo.h>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Classroom {

// =============================================================================
// ClassroomWidget Implementation
// =============================================================================

ClassroomWidget::ClassroomWidget()
    : currentModuleId_(0)
    , currentContentId_(0)
    , currentActivity_(Models::ActivityType::Navigation)
    , isTracking_(false)
    , headerContainer_(nullptr)
    , mainContainer_(nullptr)
    , contentArea_(nullptr)
    , sidebarArea_(nullptr)
    , courseTitle_(nullptr)
    , courseDescription_(nullptr)
    , backButton_(nullptr)
    , contentStack_(nullptr)
    , moduleListWidget_(nullptr)
    , contentViewerWidget_(nullptr)
    , assessmentWidget_(nullptr)
    , progressSidebar_(nullptr) {
    setupUI();
}

ClassroomWidget::~ClassroomWidget() {
    stopTimeTracking();
}

void ClassroomWidget::setupUI() {
    addStyleClass("classroom-widget");

    setupHeader();
    setupMainContent();
}

void ClassroomWidget::setupHeader() {
    headerContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    headerContainer_->addStyleClass("classroom-header");

    auto headerRow = headerContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    headerRow->addStyleClass("classroom-header-row");

    // Back button
    backButton_ = headerRow->addWidget(std::make_unique<Wt::WPushButton>("Back to Dashboard"));
    backButton_->addStyleClass("btn btn-secondary");
    backButton_->clicked().connect([this] {
        onBackClicked();
    });

    // Course info
    auto courseInfo = headerRow->addWidget(std::make_unique<Wt::WContainerWidget>());
    courseInfo->addStyleClass("course-info");

    courseTitle_ = courseInfo->addWidget(std::make_unique<Wt::WText>());
    courseTitle_->addStyleClass("course-title");

    courseDescription_ = courseInfo->addWidget(std::make_unique<Wt::WText>());
    courseDescription_->addStyleClass("course-description");
}

void ClassroomWidget::setupMainContent() {
    mainContainer_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    mainContainer_->addStyleClass("classroom-main");

    // Content area (left side)
    contentArea_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    contentArea_->addStyleClass("classroom-content");

    // Stacked widget for different views
    contentStack_ = contentArea_->addWidget(std::make_unique<Wt::WStackedWidget>());
    contentStack_->addStyleClass("content-stack");

    // Module list view
    moduleListWidget_ = contentStack_->addWidget(std::make_unique<ModuleListWidget>());
    moduleListWidget_->moduleClicked().connect([this](int moduleId) {
        onModuleClicked(moduleId);
    });

    // Content viewer
    contentViewerWidget_ = contentStack_->addWidget(std::make_unique<ContentViewerWidget>());
    contentViewerWidget_->contentCompleted().connect([this](int contentId) {
        onContentCompleted(contentId);
    });
    contentViewerWidget_->assessmentRequested().connect([this](int assessmentId) {
        onAssessmentClicked(assessmentId);
    });
    contentViewerWidget_->backToModule().connect([this] {
        contentStack_->setCurrentWidget(moduleListWidget_);
        currentActivity_ = Models::ActivityType::Navigation;
    });

    // Assessment widget
    assessmentWidget_ = contentStack_->addWidget(std::make_unique<AssessmentWidget>());
    assessmentWidget_->assessmentCompleted().connect([this](int assessmentId, double score, bool passed) {
        onAssessmentCompleted(assessmentId, score, passed);
    });
    assessmentWidget_->backClicked().connect([this] {
        contentStack_->setCurrentWidget(moduleListWidget_);
        currentActivity_ = Models::ActivityType::Navigation;
    });

    // Show module list by default
    contentStack_->setCurrentWidget(moduleListWidget_);

    // Sidebar (right side)
    setupSidebar();
}

void ClassroomWidget::setupSidebar() {
    sidebarArea_ = mainContainer_->addWidget(std::make_unique<Wt::WContainerWidget>());
    sidebarArea_->addStyleClass("classroom-sidebar");

    progressSidebar_ = sidebarArea_->addWidget(std::make_unique<ProgressSidebarWidget>());
}

void ClassroomWidget::loadCourse(int courseId) {
    if (!classroomService_) {
        return;
    }

    // Load course data
    currentCourse_ = classroomService_->getCourse(std::to_string(courseId));

    // Get or create enrollment
    int studentId = std::stoi(session_->getStudent().getId());
    currentEnrollment_ = classroomService_->getStudentCourseEnrollment(studentId, courseId);

    if (currentEnrollment_.getId().empty()) {
        // Enroll the student
        auto result = classroomService_->enrollStudent(studentId, courseId);
        if (result.success) {
            currentEnrollment_ = classroomService_->getStudentCourseEnrollment(studentId, courseId);
        }
    }

    // Start course if not started
    if (currentEnrollment_.getCompletionStatus() == Models::CompletionStatus::NotStarted) {
        classroomService_->startCourse(currentEnrollment_.getId());
        currentEnrollment_ = classroomService_->getEnrollment(currentEnrollment_.getId());
    }

    // Load modules
    modules_ = classroomService_->getCourseModules(std::to_string(courseId));

    // Update UI
    updateCourseInfo();
    updateModuleList();
    updateProgress();

    // Start time tracking
    startTimeTracking();
}

void ClassroomWidget::refresh() {
    if (!classroomService_ || currentCourse_.getId().empty()) {
        return;
    }

    // Reload enrollment
    currentEnrollment_ = classroomService_->getEnrollment(currentEnrollment_.getId());

    // Update UI
    updateModuleList();
    updateProgress();
}

void ClassroomWidget::navigateToModule(int moduleId) {
    currentModuleId_ = moduleId;
    moduleSelected_.emit(moduleId);

    // Load module contents and show in content viewer
    auto contents = classroomService_->getModuleContents(std::to_string(moduleId));
    if (!contents.empty()) {
        navigateToContent(std::stoi(contents[0].getId()));
    }
}

void ClassroomWidget::navigateToContent(int contentId) {
    currentContentId_ = contentId;
    contentSelected_.emit(contentId);

    auto content = classroomService_->getContent(std::to_string(contentId));
    contentViewerWidget_->setContent(content);
    contentViewerWidget_->setClassroomService(classroomService_);
    contentStack_->setCurrentWidget(contentViewerWidget_);

    // Update activity type based on content
    switch (content.getContentType()) {
        case Models::ContentType::Reading:
            currentActivity_ = Models::ActivityType::Reading;
            break;
        case Models::ContentType::Video:
            currentActivity_ = Models::ActivityType::Video;
            break;
        case Models::ContentType::Quiz:
            currentActivity_ = Models::ActivityType::Quiz;
            break;
        default:
            currentActivity_ = Models::ActivityType::Navigation;
    }
}

void ClassroomWidget::startAssessment(int assessmentId) {
    assessmentSelected_.emit(assessmentId);

    auto assessment = classroomService_->getAssessment(std::to_string(assessmentId));
    assessmentWidget_->setAssessment(assessment);
    assessmentWidget_->setClassroomService(classroomService_);
    assessmentWidget_->setEnrollmentId(std::stoi(currentEnrollment_.getId()));
    assessmentWidget_->setStudentId(std::stoi(session_->getStudent().getId()));
    assessmentWidget_->startAssessment();

    contentStack_->setCurrentWidget(assessmentWidget_);
    currentActivity_ = Models::ActivityType::Quiz;
}

void ClassroomWidget::updateCourseInfo() {
    if (courseTitle_) {
        courseTitle_->setText(currentCourse_.getName());
    }
    if (courseDescription_) {
        courseDescription_->setText(currentCourse_.getDescription());
    }
}

void ClassroomWidget::updateModuleList() {
    if (!moduleListWidget_) {
        return;
    }

    moduleListWidget_->setModules(modules_);

    // Get progress for all modules
    auto progressList = classroomService_->getModuleProgressList(currentEnrollment_.getId());
    moduleListWidget_->setProgress(progressList);
}

void ClassroomWidget::updateProgress() {
    if (!progressSidebar_) {
        return;
    }

    progressSidebar_->setCourse(currentCourse_);
    progressSidebar_->setEnrollment(currentEnrollment_);

    auto progressList = classroomService_->getModuleProgressList(currentEnrollment_.getId());
    progressSidebar_->setModuleProgress(progressList);
    progressSidebar_->updateProgress();
}

void ClassroomWidget::onModuleClicked(int moduleId) {
    logTimeForCurrentActivity();
    navigateToModule(moduleId);
}

void ClassroomWidget::onContentClicked(int contentId) {
    logTimeForCurrentActivity();
    navigateToContent(contentId);
}

void ClassroomWidget::onAssessmentClicked(int assessmentId) {
    logTimeForCurrentActivity();
    startAssessment(assessmentId);
}

void ClassroomWidget::onBackClicked() {
    stopTimeTracking();
    backToDashboard_.emit();
}

void ClassroomWidget::onContentCompleted(int contentId) {
    if (!classroomService_ || !session_) {
        return;
    }

    int studentId = std::stoi(session_->getStudent().getId());

    // Get module progress
    auto moduleProgress = classroomService_->getModuleProgress(studentId, currentModuleId_);

    // Mark content as completed
    classroomService_->completeContent(studentId, contentId,
                                        std::stoi(moduleProgress.getId()));

    // Check if module is complete
    auto contents = classroomService_->getModuleContents(std::to_string(currentModuleId_));
    auto contentProgress = classroomService_->getModuleProgressList(currentEnrollment_.getId());

    bool allComplete = true;
    for (const auto& content : contents) {
        if (content.isRequired()) {
            auto progress = classroomService_->getContentProgress(studentId, std::stoi(content.getId()));
            if (!progress.isCompleted()) {
                allComplete = false;
                break;
            }
        }
    }

    if (allComplete) {
        onModuleCompleted(currentModuleId_);
    }

    updateProgress();
}

void ClassroomWidget::onModuleCompleted(int moduleId) {
    if (!classroomService_ || !session_) {
        return;
    }

    int studentId = std::stoi(session_->getStudent().getId());
    classroomService_->completeModule(studentId, moduleId);

    // Check if course is complete
    auto progressList = classroomService_->getModuleProgressList(currentEnrollment_.getId());
    int completedCount = 0;
    for (const auto& prog : progressList) {
        if (prog.isCompleted()) {
            completedCount++;
        }
    }

    if (completedCount >= static_cast<int>(modules_.size())) {
        // Course complete - generate report
        classroomService_->generateReport(studentId,
                                           std::stoi(currentEnrollment_.getId()),
                                           std::stoi(currentCourse_.getId()));
        courseCompleted_.emit();
    }

    updateModuleList();
    updateProgress();
}

void ClassroomWidget::onAssessmentCompleted(int assessmentId, double score, bool passed) {
    logTimeForCurrentActivity();

    // Return to module list
    contentStack_->setCurrentWidget(moduleListWidget_);
    currentActivity_ = Models::ActivityType::Navigation;

    updateProgress();
    updateModuleList();

    // Check if this was the final exam
    auto assessment = classroomService_->getAssessment(std::to_string(assessmentId));
    if (assessment.isFinalExam() && passed) {
        courseCompleted_.emit();
    }
}

void ClassroomWidget::startTimeTracking() {
    sessionStartTime_ = std::chrono::steady_clock::now();
    isTracking_ = true;
}

void ClassroomWidget::stopTimeTracking() {
    if (isTracking_) {
        logTimeForCurrentActivity();
        isTracking_ = false;
    }
}

void ClassroomWidget::logTimeForCurrentActivity() {
    if (!isTracking_ || !classroomService_ || !session_) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - sessionStartTime_);
    int durationSeconds = static_cast<int>(duration.count());

    if (durationSeconds > 0) {
        int studentId = std::stoi(session_->getStudent().getId());
        classroomService_->logTime(
            studentId,
            std::stoi(currentEnrollment_.getId()),
            std::stoi(currentCourse_.getId()),
            currentModuleId_,
            currentContentId_,
            currentActivity_,
            durationSeconds
        );
    }

    // Reset timer
    sessionStartTime_ = now;
}

// =============================================================================
// ModuleListWidget Implementation
// =============================================================================

ModuleListWidget::ModuleListWidget()
    : moduleList_(nullptr) {
    setupUI();
}

void ModuleListWidget::setupUI() {
    addStyleClass("module-list-widget");

    auto header = addWidget(std::make_unique<Wt::WText>("Course Modules"));
    header->addStyleClass("module-list-header");

    moduleList_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    moduleList_->addStyleClass("module-list");
}

void ModuleListWidget::setModules(const std::vector<Models::CourseModule>& modules) {
    modules_ = modules;
    updateDisplay();
}

void ModuleListWidget::setProgress(const std::vector<Models::StudentModuleProgress>& progress) {
    progressList_ = progress;
    updateDisplay();
}

void ModuleListWidget::updateDisplay() {
    moduleList_->clear();

    for (const auto& module : modules_) {
        auto moduleCard = moduleList_->addWidget(std::make_unique<Wt::WContainerWidget>());
        moduleCard->addStyleClass("module-card");

        // Find progress for this module
        Models::StudentModuleProgress progress;
        for (const auto& p : progressList_) {
            if (p.getModuleId() == std::stoi(module.getId())) {
                progress = p;
                break;
            }
        }

        // Status indicator
        std::string statusClass = "status-not-started";
        std::string statusText = "Not Started";
        if (progress.isCompleted()) {
            statusClass = "status-completed";
            statusText = "Completed";
        } else if (progress.getStatus() == Models::ProgressStatus::InProgress) {
            statusClass = "status-in-progress";
            statusText = "In Progress";
        } else if (progress.isLocked()) {
            statusClass = "status-locked";
            statusText = "Locked";
        }

        auto statusBadge = moduleCard->addWidget(std::make_unique<Wt::WText>(statusText));
        statusBadge->addStyleClass("module-status " + statusClass);

        // Module info
        auto moduleInfo = moduleCard->addWidget(std::make_unique<Wt::WContainerWidget>());
        moduleInfo->addStyleClass("module-info");

        auto moduleNumber = moduleInfo->addWidget(std::make_unique<Wt::WText>(
            "Module " + std::to_string(module.getModuleNumber())));
        moduleNumber->addStyleClass("module-number");

        auto moduleTitle = moduleInfo->addWidget(std::make_unique<Wt::WText>(module.getTitle()));
        moduleTitle->addStyleClass("module-title");

        if (!module.getDescription().empty()) {
            auto moduleDesc = moduleInfo->addWidget(std::make_unique<Wt::WText>(module.getDescription()));
            moduleDesc->addStyleClass("module-description");
        }

        // Progress bar (if in progress)
        if (progress.getStatus() == Models::ProgressStatus::InProgress) {
            auto progressBar = moduleInfo->addWidget(std::make_unique<Wt::WProgressBar>());
            progressBar->setRange(0, 100);
            progressBar->setValue(static_cast<int>(progress.getProgressPercentage()));
            progressBar->addStyleClass("module-progress-bar");
        }

        // Click handler (if not locked)
        if (!progress.isLocked()) {
            int moduleId = std::stoi(module.getId());
            moduleCard->clicked().connect([this, moduleId] {
                moduleClicked_.emit(moduleId);
            });
            moduleCard->addStyleClass("clickable");
        }
    }
}

// =============================================================================
// ContentViewerWidget Implementation
// =============================================================================

ContentViewerWidget::ContentViewerWidget()
    : contentTitle_(nullptr)
    , contentBody_(nullptr)
    , completeButton_(nullptr)
    , backButton_(nullptr) {
    setupUI();
}

void ContentViewerWidget::setupUI() {
    addStyleClass("content-viewer-widget");

    // Header with back button
    auto header = addWidget(std::make_unique<Wt::WContainerWidget>());
    header->addStyleClass("content-header");

    backButton_ = header->addWidget(std::make_unique<Wt::WPushButton>("Back to Modules"));
    backButton_->addStyleClass("btn btn-link");
    backButton_->clicked().connect([this] {
        backToModule_.emit();
    });

    contentTitle_ = header->addWidget(std::make_unique<Wt::WText>());
    contentTitle_->addStyleClass("content-title");

    // Content body
    contentBody_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    contentBody_->addStyleClass("content-body");

    // Footer with complete button
    auto footer = addWidget(std::make_unique<Wt::WContainerWidget>());
    footer->addStyleClass("content-footer");

    completeButton_ = footer->addWidget(std::make_unique<Wt::WPushButton>("Mark as Complete"));
    completeButton_->addStyleClass("btn btn-primary");
    completeButton_->clicked().connect([this] {
        onMarkComplete();
    });
}

void ContentViewerWidget::setContent(const Models::ModuleContent& content) {
    currentContent_ = content;

    contentTitle_->setText(content.getTitle());
    contentBody_->clear();

    switch (content.getContentType()) {
        case Models::ContentType::Reading:
            displayReadingContent();
            break;
        case Models::ContentType::Video:
            displayVideoContent();
            break;
        case Models::ContentType::Quiz:
            displayQuizContent();
            break;
        default:
            displayReadingContent();
    }
}

void ContentViewerWidget::displayReadingContent() {
    auto readingContainer = contentBody_->addWidget(std::make_unique<Wt::WContainerWidget>());
    readingContainer->addStyleClass("reading-content");

    // Display content text (could be HTML or markdown)
    auto text = readingContainer->addWidget(std::make_unique<Wt::WText>(currentContent_.getContentText()));
    text->setTextFormat(Wt::TextFormat::XHTML);

    completeButton_->show();
}

void ContentViewerWidget::displayVideoContent() {
    auto videoContainer = contentBody_->addWidget(std::make_unique<Wt::WContainerWidget>());
    videoContainer->addStyleClass("video-content");

    // Video player
    auto video = videoContainer->addWidget(std::make_unique<Wt::WVideo>());
    video->addSource(Wt::WLink(currentContent_.getContentUrl()));
    video->setOptions(Wt::PlayerOption::Controls);
    video->addStyleClass("video-player");

    // Description if any
    if (!currentContent_.getDescription().empty()) {
        auto desc = videoContainer->addWidget(std::make_unique<Wt::WText>(currentContent_.getDescription()));
        desc->addStyleClass("video-description");
    }

    completeButton_->show();
}

void ContentViewerWidget::displayQuizContent() {
    auto quizContainer = contentBody_->addWidget(std::make_unique<Wt::WContainerWidget>());
    quizContainer->addStyleClass("quiz-content");

    auto quizInfo = quizContainer->addWidget(std::make_unique<Wt::WText>(
        "This module includes a quiz. Click the button below to start."));
    quizInfo->addStyleClass("quiz-info");

    auto startQuizBtn = quizContainer->addWidget(std::make_unique<Wt::WPushButton>("Start Quiz"));
    startQuizBtn->addStyleClass("btn btn-success");
    startQuizBtn->clicked().connect([this] {
        assessmentRequested_.emit(currentContent_.getAssessmentId());
    });

    completeButton_->hide();  // Quiz completion is handled by assessment
}

void ContentViewerWidget::onMarkComplete() {
    contentCompleted_.emit(std::stoi(currentContent_.getId()));
}

// =============================================================================
// ProgressSidebarWidget Implementation
// =============================================================================

ProgressSidebarWidget::ProgressSidebarWidget()
    : progressPercentage_(nullptr)
    , modulesCompleted_(nullptr)
    , timeSpent_(nullptr)
    , progressBar_(nullptr) {
    setupUI();
}

void ProgressSidebarWidget::setupUI() {
    addStyleClass("progress-sidebar");

    auto header = addWidget(std::make_unique<Wt::WText>("Your Progress"));
    header->addStyleClass("sidebar-header");

    // Progress percentage
    progressPercentage_ = addWidget(std::make_unique<Wt::WText>("0%"));
    progressPercentage_->addStyleClass("progress-percentage");

    // Progress bar
    progressBar_ = addWidget(std::make_unique<Wt::WContainerWidget>());
    progressBar_->addStyleClass("sidebar-progress-bar");
    auto innerBar = progressBar_->addWidget(std::make_unique<Wt::WContainerWidget>());
    innerBar->addStyleClass("progress-bar-inner");
    innerBar->setWidth(Wt::WLength(0, Wt::LengthUnit::Percentage));

    // Stats
    auto statsContainer = addWidget(std::make_unique<Wt::WContainerWidget>());
    statsContainer->addStyleClass("progress-stats");

    modulesCompleted_ = statsContainer->addWidget(std::make_unique<Wt::WText>("0 / 0 modules"));
    modulesCompleted_->addStyleClass("stat-item");

    timeSpent_ = statsContainer->addWidget(std::make_unique<Wt::WText>("Time: 00:00:00"));
    timeSpent_->addStyleClass("stat-item");
}

void ProgressSidebarWidget::setEnrollment(const Models::StudentCourseEnrollment& enrollment) {
    enrollment_ = enrollment;
}

void ProgressSidebarWidget::setCourse(const Models::Course& course) {
    course_ = course;
}

void ProgressSidebarWidget::setModuleProgress(const std::vector<Models::StudentModuleProgress>& progress) {
    moduleProgress_ = progress;
}

void ProgressSidebarWidget::updateProgress() {
    // Calculate completion
    int completedCount = 0;
    for (const auto& p : moduleProgress_) {
        if (p.isCompleted()) {
            completedCount++;
        }
    }

    int totalModules = course_.getTotalModules();
    if (totalModules == 0) {
        totalModules = static_cast<int>(moduleProgress_.size());
    }

    double percentage = totalModules > 0 ?
        (static_cast<double>(completedCount) / totalModules) * 100.0 : 0.0;

    // Update UI
    std::ostringstream pctStream;
    pctStream << std::fixed << std::setprecision(0) << percentage << "%";
    progressPercentage_->setText(pctStream.str());

    modulesCompleted_->setText(std::to_string(completedCount) + " / " +
                               std::to_string(totalModules) + " modules");

    // Update progress bar
    if (progressBar_ && !progressBar_->children().empty()) {
        auto innerBar = dynamic_cast<Wt::WContainerWidget*>(progressBar_->children()[0]);
        if (innerBar) {
            innerBar->setWidth(Wt::WLength(percentage, Wt::LengthUnit::Percentage));
        }
    }

    // Update time spent
    timeSpent_->setText("Time: " + enrollment_.getFormattedTimeSpent());
}

} // namespace Classroom
} // namespace StudentIntake
