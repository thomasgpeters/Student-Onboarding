# Instructor Feature

The Instructor feature provides a comprehensive portal for CDL training instructors and examiners to manage students, schedule sessions, record skill validations, and track student progress.

## Overview

The instructor module enables instructors to:
- View and manage assigned students
- Schedule and conduct training sessions
- Record skill validations and assessments
- Provide feedback on student performance
- Track CDL test readiness and results
- Manage their availability for scheduling

## Architecture

### Components

```
src/instructor/
├── InstructorDashboardWidget.h/cpp    # Main dashboard view
├── StudentProgressViewWidget.h/cpp     # Student progress tracking
├── SchedulingWidget.h/cpp              # Session scheduling
├── FeedbackWidget.h/cpp                # Student feedback management
└── ValidationWidget.h/cpp              # Skill validation recording

src/models/
└── Instructor.h/cpp                    # Data models

src/api/
└── InstructorService.h/cpp             # API client

database/migrations/
└── 014_add_instructor_feature.sql      # Database schema

resources/
└── instructor.css                      # UI styling
```

## Database Schema

### Tables

| Table | Purpose |
|-------|---------|
| `instructor` | Core instructor data and CDL qualifications |
| `instructor_qualification` | Certifications and credentials |
| `instructor_assignment` | Instructor-student assignments |
| `scheduled_session` | Training session scheduling |
| `session_attendance` | Attendance tracking with signatures |
| `student_feedback` | Instructor feedback on students |
| `skill_category` | CDL skill categories (5 default) |
| `skill_item` | Individual skills within categories (40 default) |
| `skill_validation` | Records of skill validations |
| `student_skill_progress` | Aggregated skill progress |
| `cdl_test_result` | Official CDL test results |
| `instructor_availability` | Scheduling availability |

### Entity Relationships

```
instructor
    ├── instructor_qualification (1:N)
    ├── instructor_assignment (1:N)
    │       └── student/course/enrollment
    ├── scheduled_session (1:N)
    │       └── session_attendance (1:N)
    ├── student_feedback (1:N)
    ├── skill_validation (1:N)
    └── instructor_availability (1:N)

skill_category
    └── skill_item (1:N)
            └── skill_validation (1:N)
                    └── student_skill_progress (aggregated)
```

## Instructor Types

Instructors can be one of three types:

| Type | Description | Permissions |
|------|-------------|-------------|
| `instructor` | Standard CDL instructor | Training, scheduling, feedback |
| `examiner` | Certified CDL examiner | Above + official testing, CDL issuance |
| `both` | Dual-certified | All capabilities |

## Data Models

### Instructor

```cpp
class Instructor {
    int id_;
    int adminUserId_;
    std::string employeeId_;
    std::string firstName_, lastName_;
    std::string email_;
    InstructorType instructorType_;

    // CDL Information
    std::string cdlNumber_;
    std::string cdlClass_;
    std::string cdlState_;
    std::string cdlExpirationDate_;

    // Examiner Certification (if applicable)
    std::string examinerCertNumber_;
    std::string examinerCertExpiration_;

    // Endorsed classes and endorsements (arrays)
    std::vector<std::string> endorsedClasses_;
    std::vector<std::string> endorsements_;

    // Permissions
    bool isActive_;
    bool canSchedule_;
    bool canValidate_;
    bool canIssueCdl_;
};
```

### InstructorQualification

```cpp
class InstructorQualification {
    std::string type_;           // certification type
    std::string name_;           // certification name
    std::string issuingAuthority_;
    std::string certificationNumber_;
    std::string issueDate_;
    std::string expirationDate_;
    bool isVerified_;
    std::string documentPath_;   // stored qualification document
};
```

### InstructorAssignment

```cpp
class InstructorAssignment {
    int instructorId_;
    int studentId_;          // OR
    int courseId_;           // OR
    int enrollmentId_;

    AssignmentType type_;    // primary, secondary, backup, examiner, observer
    AssignmentScope scope_;  // full, classroom, range, road, exam

    std::string startDate_;
    std::string endDate_;
    int assignedBy_;
};
```

## UI Components

### InstructorDashboardWidget

Main dashboard providing an overview of instructor activities.

**Sections:**
- Welcome message with instructor type display
- Statistics grid:
  - Active students count
  - Sessions today
  - Pending follow-ups
  - Average student progress
- Today's schedule with session details
- Students needing attention (max 5)
- Quick action buttons

**Quick Actions:**
- Schedule Session
- Add Feedback
- View All Students
- View Full Schedule
- Validate Skills

### StudentProgressViewWidget

Detailed view of student progress for assigned students.

**List View:**
- All assigned students with:
  - Name and course
  - Progress percentage
  - Total hours
  - Status indicator

**Detail View:**
- Overall progress bar
- Module progress table
- Skills progress with:
  - Practice counts
  - Validation status
  - Best scores
- Time tracking breakdown:
  - Total hours
  - Practice hours
  - Classroom hours
- Recent feedback history

**Actions:**
- Add Feedback
- Schedule Session
- Validate Skill

### SchedulingWidget

Comprehensive session scheduling and management.

**Filters:**
- Upcoming sessions
- Today's sessions
- This week
- Custom date range

**Session Table Columns:**
- Date and time
- Session type
- Student name
- Location
- Status
- Actions

**Session Types:**
| Type | Description |
|------|-------------|
| `orientation` | Initial student orientation |
| `classroom` | Classroom instruction |
| `pre_trip` | Pre-trip inspection training |
| `basic_control` | Basic vehicle control |
| `range_practice` | Range practice sessions |
| `road_practice` | On-road practice |
| `skills_test` | Skills testing |
| `road_test` | Road test examination |
| `final_exam` | Final examination |

**Session Statuses:**
| Status | Actions Available |
|--------|-------------------|
| `scheduled` | Start, Cancel, Edit |
| `confirmed` | Start, Cancel, Edit |
| `in_progress` | Complete |
| `completed` | View |
| `cancelled` | - |
| `no_show` | Reschedule |
| `rescheduled` | View new session |

**Scheduling Dialog:**
- Student selection
- Session type
- Title and description
- Date and time pickers
- Location
- Notes

### FeedbackWidget

Create and manage student feedback.

**Filters:**
- Feedback type
- Follow-up status (pending, completed, all)

**Feedback Types:**
| Type | Purpose |
|------|---------|
| `progress_update` | General progress notes |
| `session_review` | Post-session review |
| `skill_assessment` | Skill-specific feedback |
| `behavior` | Behavioral observations |
| `safety_concern` | Safety-related issues |
| `commendation` | Positive recognition |
| `general` | General notes |

**Feedback Form Fields:**
- Subject line
- Detailed feedback text
- Performance rating (1-5 scale)
- Strengths (text area)
- Areas for improvement (text area)
- Recommended actions
- Follow-up required (checkbox)
- Follow-up date
- Visible to student (checkbox)

**Table Display:**
- Date
- Student
- Type
- Subject
- Rating
- Follow-up status

### ValidationWidget

Record skill validations for CDL training competencies.

**Skill Categories Display:**
- Category name and code
- Description
- Number of skills in category
- Minimum practice hours required

**CDL Skill Categories (Default):**

| Category | Code | Skills | Min Hours |
|----------|------|--------|-----------|
| Pre-Trip Inspection | PTI | 8 | 4.0 |
| Basic Vehicle Control | BVC | 4 | 8.0 |
| Range Maneuvers | RM | varies | 16.0 |
| Road Skills | RS | 9 | 20.0 |
| Safety Procedures | SP | varies | 2.0 |

**Validation Dialog:**
- Student selection
- Skill selection (grouped by category)
- Result: Pass, Fail, Needs Practice, Deferred, Incomplete
- Score (0-100)
- Errors noted
- Critical error checkbox
- Official test checkbox
- Notes

**Recent Validations Table:**
- Date
- Student
- Skill
- Result (with status badge)
- Score
- Official test indicator

## API Service

The `InstructorService` class provides comprehensive API access.

### Instructor Profile APIs
```cpp
Models::Instructor getInstructorByUserId(int userId);
Models::Instructor getInstructor(int instructorId);
std::vector<Models::Instructor> getAllInstructors();
InstructorResult updateInstructor(const Models::Instructor& instructor);
```

### Assignment APIs
```cpp
std::vector<StudentProgressSummary> getAssignedStudentProgress(int instructorId);
std::vector<Models::InstructorAssignment> getInstructorAssignments(int instructorId);
InstructorResult assignStudent(int instructorId, int studentId, ...);
InstructorResult removeAssignment(int assignmentId);
```

### Session Scheduling APIs
```cpp
std::vector<Models::ScheduledSession> getInstructorSessions(int instructorId);
std::vector<Models::ScheduledSession> getSessionsByDateRange(int instructorId, ...);
std::vector<Models::ScheduledSession> getTodaySessions(int instructorId);
std::vector<Models::ScheduledSession> getUpcomingSessions(int instructorId);
InstructorResult scheduleSession(const Models::ScheduledSession& session);
InstructorResult updateSession(const Models::ScheduledSession& session);
InstructorResult cancelSession(int sessionId, const std::string& reason);
InstructorResult startSession(int sessionId);
InstructorResult completeSession(int sessionId, const std::string& notes);
InstructorResult markNoShow(int sessionId);
```

### Feedback APIs
```cpp
std::vector<Models::StudentFeedback> getInstructorFeedback(int instructorId);
std::vector<Models::StudentFeedback> getStudentFeedback(int studentId);
std::vector<Models::StudentFeedback> getPendingFollowUps(int instructorId);
InstructorResult createFeedback(const Models::StudentFeedback& feedback);
InstructorResult updateFeedback(const Models::StudentFeedback& feedback);
InstructorResult completeFeedbackFollowUp(int feedbackId, const std::string& notes);
```

### Skill Validation APIs
```cpp
std::vector<Models::SkillCategory> getSkillCategories();
std::vector<Models::SkillItem> getAllSkillItems();
std::vector<Models::SkillItem> getSkillItemsByCategory(int categoryId);
std::vector<Models::SkillValidation> getInstructorValidations(int instructorId);
std::vector<Models::StudentSkillProgress> getStudentSkillProgress(int studentId);
InstructorResult createValidation(const Models::SkillValidation& validation);
bool isSkillValidated(int studentId, int skillItemId);
```

### CDL Testing APIs
```cpp
InstructorResult recordCdlTestResult(const Models::CdlTestResult& result);
std::vector<Models::CdlTestResult> getStudentTestResults(int studentId);
bool isStudentReadyForCdlTest(int studentId);
InstructorResult issueCdlRecommendation(int studentId, const std::string& notes);
```

### Dashboard APIs
```cpp
InstructorDashboardStats getDashboardStats(int instructorId);
std::vector<ActivityFeedItem> getRecentActivity(int instructorId);
std::vector<Notification> getNotifications(int instructorId);
```

## Dashboard Statistics

The `InstructorDashboardStats` structure provides:

```cpp
struct InstructorDashboardStats {
    int activeStudents;
    int sessionsToday;
    int sessionsThisWeek;
    int pendingFollowUps;
    double averageStudentProgress;
    int studentsNeedingAttention;
    int validationsThisMonth;
    int upcomingSessions;
};
```

## CSS Classes

Primary CSS classes for instructor UI:

```css
/* Dashboard */
.instructor-dashboard        /* Main dashboard container */
.dashboard-header           /* Welcome section */
.stats-grid                 /* Statistics cards grid */
.instructor-stat-card       /* Individual stat card */

/* Tables */
.instructor-table           /* Standard table styling */
.sessions-table            /* Session listing */
.students-table            /* Student listing */
.feedback-table            /* Feedback listing */
.validations-table         /* Validation listing */

/* Status Badges */
.status-scheduled          /* Scheduled status */
.status-confirmed          /* Confirmed status */
.status-in_progress        /* In progress status */
.status-completed          /* Completed status */
.status-cancelled          /* Cancelled status */
.status-no_show            /* No show status */

/* Widgets */
.scheduling-widget         /* Scheduling interface */
.feedback-widget           /* Feedback interface */
.validation-widget         /* Validation interface */
.progress-view-widget      /* Student progress view */

/* Dialogs */
.schedule-dialog           /* Schedule session dialog */
.feedback-dialog           /* Add feedback dialog */
.validation-dialog         /* Record validation dialog */
```

## Integration with StudentIntakeApp

The instructor feature integrates via application states:

```cpp
enum class AppState {
    // ...
    InstructorDashboard,  // Instructor main view
    InstructorStudents,   // View student progress
    InstructorSchedule,   // Session scheduling
    InstructorFeedback,   // Feedback management
    InstructorValidation  // Skill validation
};
```

Navigation handlers:
```cpp
void handleViewStudents();
void handleViewSchedule();
void handleViewFeedback();
void handleViewValidations();
void handleInstructorBack();
```

## Default Skill Categories and Items

The migration seeds 5 categories with 40 skills:

### Pre-Trip Inspection (PTI)
1. Engine compartment
2. Cab/cab area
3. Coupling system
4. Fuel area
5. Under vehicle
6. Wheels/rims/tires
7. Side of vehicle
8. Rear of vehicle

### Basic Vehicle Control (BVC)
1. Straight line backing
2. Offset backing (left)
3. Offset backing (right)
4. Parallel parking (conventional)

### Road Skills (RS)
1. Left turns
2. Right turns
3. Lane changes
4. Intersections
5. Railroad crossings
6. Expressway driving
7. Urban driving
8. Rural driving
9. Night driving

## Security Considerations

1. **Role Verification** - Only users with instructor role can access
2. **Assignment Scope** - Instructors can only view/manage assigned students
3. **Examiner Privileges** - CDL test results restricted to certified examiners
4. **Audit Trail** - All validations and test results logged with timestamps
5. **Digital Signatures** - Session attendance includes signature capture

## Future Enhancements

1. **Mobile App** - Tablet-optimized interface for in-vehicle use
2. **GPS Integration** - Route tracking for road sessions
3. **Video Recording** - Session recording for review
4. **Fleet Management** - Vehicle assignment and tracking
5. **Automated Scheduling** - AI-powered optimal scheduling
6. **Parent/Sponsor Portal** - Progress sharing with sponsors
