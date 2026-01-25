# Classroom Feature

The Classroom feature provides a complete online learning environment for students enrolled in courses that have an online component, such as the ELD (Entry Level Driver) prep-course for CDL training.

## Overview

The classroom module enables students to:
- Access course modules and content
- Track progress through sequential learning materials
- Complete assessments (quizzes and exams)
- Log time spent on activities for compliance tracking
- Generate completion reports and certificates

## Architecture

### Components

```
src/classroom/
├── ClassroomWidget.h/cpp         # Main classroom container
├── AssessmentWidget.h/cpp        # Quiz/exam taking interface
└── AssessmentReportWidget.h/cpp  # Completion report display

src/api/
└── ClassroomService.h/cpp        # API client for classroom operations

database/migrations/
└── 013_add_classroom_feature.sql # Database schema

resources/
└── classroom.css                 # UI styling
```

## Database Schema

### Tables

| Table | Purpose |
|-------|---------|
| `course` | Online courses linked to curriculum |
| `course_module` | Modules within courses (~32 for ELD) |
| `module_content` | Content items (reading, video, quiz, interactive, document) |
| `student_course_enrollment` | Student enrollment tracking |
| `student_module_progress` | Module-level progress tracking |
| `student_content_progress` | Content-level progress tracking |
| `student_time_log` | Detailed time tracking for compliance |
| `assessment` | Quiz and exam definitions |
| `assessment_question` | Individual questions with shuffling support |
| `student_assessment_attempt` | Attempt records with timing |
| `student_assessment_answer` | Individual answers given |
| `assessment_report` | Final completion reports |

### Entity Relationships

```
curriculum
    └── course (1:1)
            ├── course_module (1:N)
            │       └── module_content (1:N)
            └── assessment (1:N)
                    └── assessment_question (1:N)

student
    └── student_course_enrollment (1:N)
            ├── student_module_progress (1:N)
            ├── student_content_progress (1:N)
            ├── student_time_log (1:N)
            ├── student_assessment_attempt (1:N)
            │       └── student_assessment_answer (1:N)
            └── assessment_report (1:1)
```

## UI Components

### ClassroomWidget

The main container widget that manages the learning experience.

**Sub-components:**
- `ModuleListWidget` - Displays list of course modules with progress status
- `ContentViewerWidget` - Views reading, video, and quiz content
- `ProgressSidebarWidget` - Shows course progress percentage and completion stats

**Key Features:**
- Module navigation with sequential completion requirements
- Content viewing for various media types
- Assessment launching
- Time tracking per activity
- Progress signals for UI updates

### AssessmentWidget

Interactive quiz and exam interface.

**Features:**
- Multi-section display (instructions, questions, results)
- Question types: multiple choice, true/false, multiple select
- Timer with countdown and warning states (yellow at 5 min, red at 1 min)
- Answer shuffling and review options
- Auto-submission on timer expiration
- Multiple attempt support with attempt limits

**Question Display Modes:**
- One at a time with navigation
- All questions at once

### AssessmentReportWidget

Displays final course completion report.

**Sections:**
- Summary (student/course info, overall scores, completion status)
- Module breakdown table with individual scores and time spent
- Time log summary
- Certificate status and issuance
- Print functionality

## API Service

The `ClassroomService` class provides a comprehensive API client for all classroom operations.

### Endpoint Categories

#### Course APIs
```cpp
std::vector<Models::Course> getCourses();
Models::Course getCourse(int courseId);
std::vector<Models::Course> getCoursesForCurriculum(const std::string& curriculumId);
bool curriculumHasOnlineCourse(const std::string& curriculumId);
```

#### Module APIs
```cpp
std::vector<Models::CourseModule> getCourseModules(int courseId);
Models::CourseModule getModule(int moduleId);
Models::CourseModule getNextModule(int enrollmentId, int currentModuleId);
```

#### Content APIs
```cpp
std::vector<Models::ModuleContent> getModuleContents(int moduleId);
Models::ModuleContent getContent(int contentId);
```

#### Enrollment APIs
```cpp
std::vector<Models::Enrollment> getStudentEnrollments(int studentId);
Models::Enrollment getEnrollment(int enrollmentId);
Models::Enrollment enrollStudent(int studentId, int courseId);
bool startCourse(int enrollmentId);
```

#### Progress APIs
```cpp
std::vector<Models::ModuleProgress> getModuleProgressList(int enrollmentId);
bool saveModuleProgress(const Models::ModuleProgress& progress);
bool completeModule(int enrollmentId, int moduleId);
bool completeContent(int enrollmentId, int contentId);
```

#### Time Logging
```cpp
bool logTime(int enrollmentId, int moduleId, int contentId,
             const std::string& activityType, int seconds);
std::vector<Models::TimeLog> getTimeLogs(int enrollmentId);
int getTotalTimeSpent(int enrollmentId);
int getModuleTimeSpent(int enrollmentId, int moduleId);
```

#### Assessment APIs
```cpp
std::vector<Models::Assessment> getCourseAssessments(int courseId);
Models::Assessment getAssessment(int assessmentId);
std::vector<Models::AssessmentQuestion> getAssessmentQuestions(int assessmentId);
```

#### Attempt APIs
```cpp
Models::AssessmentAttempt startAssessmentAttempt(int enrollmentId, int assessmentId);
bool submitAnswer(int attemptId, int questionId, const std::string& answer);
Models::AssessmentResult submitAssessment(int attemptId);
std::vector<Models::AttemptAnswer> getAttemptAnswers(int attemptId);
```

#### Report APIs
```cpp
Models::AssessmentReport generateReport(int enrollmentId);
Models::AssessmentReport getReport(int reportId);
std::vector<Models::AssessmentReport> getStudentReports(int studentId);
bool issueCertificate(int reportId);
```

## Content Types

The classroom supports multiple content types:

| Type | Description |
|------|-------------|
| `reading` | Text-based learning content |
| `video` | Video lessons with playback controls |
| `quiz` | Interactive assessment within a module |
| `interactive` | Interactive exercises |
| `document` | Downloadable documents (PDF, etc.) |

## Time Tracking

Time tracking is essential for compliance with training requirements.

### Activity Types
- `reading` - Time spent on reading content
- `video` - Time spent watching videos
- `quiz` - Time spent on module quizzes
- `assessment` - Time spent on formal assessments
- `review` - Time spent reviewing completed content

### Implementation
```cpp
void ClassroomWidget::logTimeForCurrentActivity() {
    // Called periodically and on content transitions
    classroomService_->logTime(
        enrollmentId_,
        currentModuleId_,
        currentContentId_,
        activityType_,
        elapsedSeconds_
    );
}
```

## Assessment Grading

Assessments are automatically graded upon submission.

### Grading Process
1. Student submits assessment
2. System compares answers to correct answers
3. Score calculated as percentage
4. Passing threshold checked (configurable per assessment)
5. Result recorded in `student_assessment_attempt`
6. Module/content marked complete if passing

### Attempt Management
- Configurable maximum attempts per assessment
- Best score tracking across attempts
- Remaining attempts validation before starting

## Certificate Issuance

Upon successful course completion:

1. Report generated with all progress data
2. Certificate eligibility checked (all modules complete, passing scores)
3. Certificate number generated (unique identifier)
4. Certificate status updated in `assessment_report`
5. Print-ready report available

## CSS Classes

The classroom uses these primary CSS classes:

```css
.classroom-widget          /* Main container */
.classroom-header          /* Course title and navigation */
.module-list-*            /* Module listing components */
.content-viewer-*         /* Content display area */
.progress-sidebar-*       /* Progress tracking sidebar */
.assessment-*             /* Assessment taking interface */
.assessment-report-*      /* Completion report */
.timer-warning            /* Timer warning state */
.timer-caution            /* Timer caution state */
.status-completed         /* Completed status badge */
.status-in-progress       /* In progress status badge */
.status-locked            /* Locked status badge */
.status-not-started       /* Not started status badge */
```

## Integration with StudentIntakeApp

The classroom is integrated into the main application via:

```cpp
// State enum
enum class AppState {
    // ...
    Classroom,  // Online course learning environment
    // ...
};

// Show classroom
void StudentIntakeApp::showClassroom() {
    hideAllViews();
    classroomWidget_->show();
    classroomWidget_->loadCourse(courseId);
}

// Enter classroom handler
void StudentIntakeApp::handleEnterClassroom() {
    setState(AppState::Classroom);
}
```

## Future Enhancements

Potential improvements for the classroom feature:

1. **Offline Support** - Cache content for offline access
2. **Bookmarking** - Save position within content
3. **Notes** - Allow students to take notes per module
4. **Discussion** - Forum or chat for student questions
5. **Gamification** - Badges and achievements
6. **Analytics** - Detailed learning analytics dashboard
