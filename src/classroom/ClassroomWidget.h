#ifndef CLASSROOM_WIDGET_H
#define CLASSROOM_WIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <Wt/WStackedWidget.h>
#include <memory>
#include "session/StudentSession.h"
#include "api/ClassroomService.h"
#include "models/Course.h"
#include "models/StudentProgress.h"

namespace StudentIntake {
namespace Classroom {

// Forward declarations
class ModuleListWidget;
class ContentViewerWidget;
class AssessmentWidget;
class ProgressSidebarWidget;

/**
 * @brief Main classroom widget for online course learning
 *
 * This widget provides the student interface for:
 * - Viewing course modules and progress
 * - Accessing reading assignments, videos, and quizzes
 * - Taking assessments (quizzes and final exam)
 * - Tracking time spent in the course
 * - Viewing completion reports
 */
class ClassroomWidget : public Wt::WContainerWidget {
public:
    ClassroomWidget();
    ~ClassroomWidget();

    // Session and service management
    void setSession(std::shared_ptr<Session::StudentSession> session) { session_ = session; }
    void setClassroomService(std::shared_ptr<Api::ClassroomService> service) { classroomService_ = service; }

    /**
     * @brief Initialize the classroom for a specific course
     */
    void loadCourse(int courseId);

    /**
     * @brief Refresh the current view
     */
    void refresh();

    /**
     * @brief Navigate to a specific module
     */
    void navigateToModule(int moduleId);

    /**
     * @brief Navigate to specific content within a module
     */
    void navigateToContent(int contentId);

    /**
     * @brief Start an assessment
     */
    void startAssessment(int assessmentId);

    // Signals for navigation
    Wt::Signal<>& backToDashboard() { return backToDashboard_; }
    Wt::Signal<>& courseCompleted() { return courseCompleted_; }
    Wt::Signal<int>& moduleSelected() { return moduleSelected_; }
    Wt::Signal<int>& contentSelected() { return contentSelected_; }
    Wt::Signal<int>& assessmentSelected() { return assessmentSelected_; }

private:
    void setupUI();
    void setupHeader();
    void setupMainContent();
    void setupSidebar();

    void updateCourseInfo();
    void updateModuleList();
    void updateProgress();

    void onModuleClicked(int moduleId);
    void onContentClicked(int contentId);
    void onAssessmentClicked(int assessmentId);
    void onBackClicked();
    void onContentCompleted(int contentId);
    void onModuleCompleted(int moduleId);
    void onAssessmentCompleted(int assessmentId, double score, bool passed);

    // Time tracking
    void startTimeTracking();
    void stopTimeTracking();
    void logTimeForCurrentActivity();

    std::shared_ptr<Session::StudentSession> session_;
    std::shared_ptr<Api::ClassroomService> classroomService_;

    // Current state
    Models::Course currentCourse_;
    Models::StudentCourseEnrollment currentEnrollment_;
    std::vector<Models::CourseModule> modules_;
    int currentModuleId_;
    int currentContentId_;

    // Time tracking
    std::chrono::steady_clock::time_point sessionStartTime_;
    Models::ActivityType currentActivity_;
    bool isTracking_;

    // Main layout
    Wt::WContainerWidget* headerContainer_;
    Wt::WContainerWidget* mainContainer_;
    Wt::WContainerWidget* contentArea_;
    Wt::WContainerWidget* sidebarArea_;

    // Header widgets
    Wt::WText* courseTitle_;
    Wt::WText* courseDescription_;
    Wt::WPushButton* backButton_;

    // Content area (stacked views)
    Wt::WStackedWidget* contentStack_;
    ModuleListWidget* moduleListWidget_;
    ContentViewerWidget* contentViewerWidget_;
    AssessmentWidget* assessmentWidget_;

    // Sidebar
    ProgressSidebarWidget* progressSidebar_;

    // Signals
    Wt::Signal<> backToDashboard_;
    Wt::Signal<> courseCompleted_;
    Wt::Signal<int> moduleSelected_;
    Wt::Signal<int> contentSelected_;
    Wt::Signal<int> assessmentSelected_;
};

/**
 * @brief Widget for displaying the list of modules in a course
 */
class ModuleListWidget : public Wt::WContainerWidget {
public:
    ModuleListWidget();

    void setModules(const std::vector<Models::CourseModule>& modules);
    void setProgress(const std::vector<Models::StudentModuleProgress>& progress);

    Wt::Signal<int>& moduleClicked() { return moduleClicked_; }

private:
    void setupUI();
    void updateDisplay();

    std::vector<Models::CourseModule> modules_;
    std::vector<Models::StudentModuleProgress> progressList_;

    Wt::WContainerWidget* moduleList_;
    Wt::Signal<int> moduleClicked_;
};

/**
 * @brief Widget for viewing module content (reading, video)
 */
class ContentViewerWidget : public Wt::WContainerWidget {
public:
    ContentViewerWidget();

    void setContent(const Models::ModuleContent& content);
    void setClassroomService(std::shared_ptr<Api::ClassroomService> service) { classroomService_ = service; }

    Wt::Signal<int>& contentCompleted() { return contentCompleted_; }
    Wt::Signal<int>& assessmentRequested() { return assessmentRequested_; }
    Wt::Signal<>& backToModule() { return backToModule_; }

private:
    void setupUI();
    void displayReadingContent();
    void displayVideoContent();
    void displayQuizContent();
    void onMarkComplete();

    std::shared_ptr<Api::ClassroomService> classroomService_;
    Models::ModuleContent currentContent_;

    Wt::WText* contentTitle_;
    Wt::WContainerWidget* contentBody_;
    Wt::WPushButton* completeButton_;
    Wt::WPushButton* backButton_;

    Wt::Signal<int> contentCompleted_;
    Wt::Signal<int> assessmentRequested_;
    Wt::Signal<> backToModule_;
};

/**
 * @brief Sidebar widget showing course progress
 */
class ProgressSidebarWidget : public Wt::WContainerWidget {
public:
    ProgressSidebarWidget();

    void setEnrollment(const Models::StudentCourseEnrollment& enrollment);
    void setCourse(const Models::Course& course);
    void setModuleProgress(const std::vector<Models::StudentModuleProgress>& progress);

    void updateProgress();

private:
    void setupUI();

    Models::StudentCourseEnrollment enrollment_;
    Models::Course course_;
    std::vector<Models::StudentModuleProgress> moduleProgress_;

    Wt::WText* progressPercentage_;
    Wt::WText* modulesCompleted_;
    Wt::WText* timeSpent_;
    Wt::WContainerWidget* progressBar_;
};

} // namespace Classroom
} // namespace StudentIntake

#endif // CLASSROOM_WIDGET_H
