# Administration User Guide

## Student Onboarding System - Administrative Portal

**Version:** 1.0
**Last Updated:** January 2026

---

## Table of Contents

1. [Introduction](#1-introduction)
2. [Getting Started](#2-getting-started)
   - [System Requirements](#system-requirements)
   - [Accessing the Admin Portal](#accessing-the-admin-portal)
   - [Logging In](#logging-in)
3. [User Roles and Permissions](#3-user-roles-and-permissions)
   - [Super Administrator](#super-administrator)
   - [Administrator](#administrator)
   - [Instructor](#instructor)
4. [Dashboard Overview](#4-dashboard-overview)
   - [Navigation Elements](#navigation-elements)
   - [Statistics Cards](#statistics-cards)
   - [Quick Actions](#quick-actions)
5. [Student Management](#5-student-management)
   - [Student List View](#student-list-view)
   - [Filtering and Searching](#filtering-and-searching-students)
   - [Student Detail View](#student-detail-view)
   - [Managing Student Access](#managing-student-access)
   - [Viewing Student Forms](#viewing-student-forms)
6. [Form Submission Management](#6-form-submission-management)
   - [Form Submissions List](#form-submissions-list)
   - [Filtering Submissions](#filtering-submissions)
   - [Reviewing Form Details](#reviewing-form-details)
   - [Approving Forms](#approving-forms)
   - [Rejecting Forms](#rejecting-forms)
   - [Requesting Revisions](#requesting-revisions)
7. [Curriculum Management](#7-curriculum-management)
   - [Curriculum List View](#curriculum-list-view)
   - [Filtering Programs](#filtering-programs)
   - [Creating a New Program](#creating-a-new-program)
   - [Editing Programs](#editing-programs)
   - [Managing Required Forms](#managing-required-forms)
8. [Settings](#8-settings)
   - [System Configuration](#system-configuration)
   - [Admin User Management](#admin-user-management)
9. [Common Workflows](#9-common-workflows)
   - [Processing New Student Enrollments](#processing-new-student-enrollments)
   - [Handling Form Revisions](#handling-form-revisions)
   - [Setting Up a New Program](#setting-up-a-new-program)
10. [Troubleshooting](#10-troubleshooting)
11. [Keyboard Shortcuts](#11-keyboard-shortcuts)
12. [Glossary](#12-glossary)

---

## 1. Introduction

The Student Onboarding Administration Portal provides a comprehensive interface for managing student enrollments, reviewing form submissions, and configuring academic and vocational programs. This guide covers all administrative functions available in the system.

### Purpose of This Guide

This guide is designed to help administrators:
- Navigate the administrative interface efficiently
- Manage student records and access
- Review and process form submissions
- Configure curriculum and program requirements
- Understand role-based permissions and workflows

### Document Conventions

| Convention | Meaning |
|------------|---------|
| **Bold text** | UI elements (buttons, menu items) |
| `Monospace` | System values, codes, or user input |
| > Note | Important information or tips |
| ⚠️ Warning | Critical information requiring attention |

---

## 2. Getting Started

### System Requirements

The Administration Portal is a web-based application that requires:
- Modern web browser (Chrome, Firefox, Safari, or Edge)
- Minimum screen resolution: 1280 x 720
- Stable internet connection
- Valid administrator credentials

### Accessing the Admin Portal

The Administration Portal is accessed through a dedicated URL separate from the student portal:

```
https://[your-institution-domain]/administration
```

> Note: The exact URL will be provided by your system administrator.

![Screenshot: Admin Portal URL in Browser Address Bar]
<!-- SCREENSHOT_PLACEHOLDER: admin_portal_url.png -->
<!-- Caption: Accessing the Administration Portal via the /administration path -->

### Logging In

1. Navigate to the Administration Portal URL
2. Enter your administrator email address
3. Enter your password
4. Click the **Login** button

![Screenshot: Admin Login Screen]
<!-- SCREENSHOT_PLACEHOLDER: admin_login_screen.png -->
<!-- Caption: The Administration Portal login screen with email and password fields -->

#### Login Credentials

Your login credentials are separate from student accounts. Contact your Super Administrator if you need:
- Initial account setup
- Password reset
- Role or permission changes

#### Login Errors

| Error Message | Resolution |
|---------------|------------|
| "Invalid email or password" | Verify credentials and try again |
| "Account locked" | Contact Super Administrator |
| "Session expired" | Log in again |

![Screenshot: Login Error Message]
<!-- SCREENSHOT_PLACEHOLDER: admin_login_error.png -->
<!-- Caption: Example of login error message display -->

---

## 3. User Roles and Permissions

The system implements three administrative roles with different access levels:

### Super Administrator

**Full system access** - Can perform all administrative functions.

| Permission | Access |
|------------|--------|
| Manage Students | ✅ Full |
| Review Forms | ✅ Full |
| Manage Curriculum | ✅ Full |
| Revoke Student Access | ✅ Yes |
| Manage Admin Users | ✅ Yes |
| System Settings | ✅ Yes |

### Administrator

**Content management** - Can manage students, forms, and curriculum.

| Permission | Access |
|------------|--------|
| Manage Students | ✅ Full |
| Review Forms | ✅ Full |
| Manage Curriculum | ✅ Full |
| Revoke Student Access | ✅ Yes |
| Manage Admin Users | ❌ No |
| System Settings | ❌ No |

### Instructor

**Program-specific access** - Limited to assigned programs only.

| Permission | Access |
|------------|--------|
| View Students | 📋 Assigned programs only |
| View Forms | 📋 Assigned programs only |
| Manage Curriculum | ❌ No |
| Revoke Student Access | ❌ No |
| Manage Admin Users | ❌ No |
| System Settings | ❌ No |

![Screenshot: Role Indicator in Navigation]
<!-- SCREENSHOT_PLACEHOLDER: admin_role_indicator.png -->
<!-- Caption: The navigation bar displays your name and current role -->

---

## 4. Dashboard Overview

After logging in, you'll see the Administration Dashboard - your central hub for monitoring and managing the onboarding system.

![Screenshot: Admin Dashboard Overview]
<!-- SCREENSHOT_PLACEHOLDER: admin_dashboard_full.png -->
<!-- Caption: The Administration Dashboard showing navigation, statistics, and quick actions -->

### Navigation Elements

The interface consists of three main navigation components:

#### Top Navigation Bar

Located at the top of the screen, the navigation bar displays:
- **Home** button - Returns to Dashboard
- **Admin name** and role indicator
- **Logout** button

![Screenshot: Top Navigation Bar]
<!-- SCREENSHOT_PLACEHOLDER: admin_top_navigation.png -->
<!-- Caption: Top navigation bar with home button, user info, and logout -->

#### Sidebar Navigation

The left sidebar provides access to main sections:

| Menu Item | Description | Required Role |
|-----------|-------------|---------------|
| **Dashboard** | Overview and statistics | All |
| **Students** | Student management | All |
| **Forms** | Form submission review | All |
| **Curriculum** | Program management | Administrator+ |
| **Settings** | System configuration | Super Administrator |

![Screenshot: Sidebar Navigation]
<!-- SCREENSHOT_PLACEHOLDER: admin_sidebar.png -->
<!-- Caption: Sidebar navigation with section indicators -->

> Note: Menu items may be hidden based on your role permissions.

### Statistics Cards

The Dashboard displays real-time statistics in clickable cards:

| Card | Description |
|------|-------------|
| **Total Students** | Number of enrolled students |
| **Pending Forms** | Forms awaiting review |
| **Active Programs** | Currently active curriculum programs |

![Screenshot: Dashboard Statistics Cards]
<!-- SCREENSHOT_PLACEHOLDER: admin_dashboard_stats.png -->
<!-- Caption: Statistics cards showing key metrics at a glance -->

### Quick Actions

Click any statistics card to navigate directly to that section with relevant filters applied:
- Click **Pending Forms** → Opens Forms list filtered to pending status
- Click **Total Students** → Opens Student list
- Click **Active Programs** → Opens Curriculum list

---

## 5. Student Management

The Student Management section allows you to view, search, and manage student records.

### Student List View

Access the Student List by clicking **Students** in the sidebar.

![Screenshot: Student List View]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_list_full.png -->
<!-- Caption: Full Student List view with statistics, filters, and data table -->

#### Statistics Cards

Four mini-cards display student counts by status:

| Card | Color | Description |
|------|-------|-------------|
| **Active** | Blue | Students with active enrollment |
| **Pending** | Yellow | Students in onboarding process |
| **Completed** | Green | Students who completed onboarding |
| **Revoked** | Red | Students with revoked access |

![Screenshot: Student Statistics Cards]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_stats.png -->
<!-- Caption: Student statistics showing counts for each status -->

#### Student Table

The table displays:
- **Name** - Student's full name
- **Email** - Student email address
- **Program** - Enrolled curriculum/program
- **Status** - Current enrollment status (badge)
- **Enrollment Date** - Date of enrollment

![Screenshot: Student Table]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_table.png -->
<!-- Caption: Student data table with status badges and enrollment information -->

### Filtering and Searching Students

Use the filter controls above the table to find specific students:

#### Search Field

Type in the search field to filter by name or email. Results update automatically as you type.

![Screenshot: Student Search]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_search.png -->
<!-- Caption: Using the search field to filter students by name or email -->

#### Program Filter

Select a program from the dropdown to show only students enrolled in that program.

![Screenshot: Program Filter Dropdown]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_program_filter.png -->
<!-- Caption: Program filter dropdown showing available programs -->

#### Status Filter

Filter students by their enrollment status:
- **All Statuses** - Show all students
- **Active** - Currently active students
- **Pending** - Awaiting completion
- **Completed** - Finished onboarding
- **Revoked** - Access revoked

![Screenshot: Status Filter Dropdown]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_status_filter.png -->
<!-- Caption: Status filter dropdown with available options -->

#### Clear Filters

Click the **Clear** button to reset all filters and show all students.

![Screenshot: Clear Filters Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_clear_filters.png -->
<!-- Caption: Clear button location in the filter bar -->

### Student Detail View

Click any row in the Student List to view detailed information.

![Screenshot: Student Detail View]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_detail_full.png -->
<!-- Caption: Complete Student Detail view with all information panels -->

#### Student Header

Displays the student's name, email, and current status badge.

![Screenshot: Student Detail Header]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_detail_header.png -->
<!-- Caption: Student header with name and status badge -->

#### Information Cards

| Card | Information |
|------|-------------|
| **Program** | Enrolled curriculum program |
| **Enrollment Date** | When the student enrolled |
| **Phone** | Contact phone number |
| **Address** | Primary address |

![Screenshot: Student Information Cards]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_info_cards.png -->
<!-- Caption: Student information displayed in organized cards -->

#### Action Buttons

| Button | Action | Availability |
|--------|--------|--------------|
| **View Forms** | See all forms submitted by this student | Always |
| **Revoke Access** | Disable student login | When access is active |
| **Restore Access** | Re-enable student login | When access is revoked |
| **Back** | Return to Student List | Always |

![Screenshot: Student Action Buttons]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_actions.png -->
<!-- Caption: Action buttons available in Student Detail view -->

### Managing Student Access

Administrators can control student access to the system.

#### Revoking Access

1. Navigate to the Student Detail view
2. Click the **Revoke Access** button
3. Confirm the action when prompted

![Screenshot: Revoke Access Confirmation]
<!-- SCREENSHOT_PLACEHOLDER: admin_revoke_access_dialog.png -->
<!-- Caption: Confirmation dialog when revoking student access -->

⚠️ **Warning:** Revoking access immediately prevents the student from logging in. Use this for students who:
- Have withdrawn from the program
- Violated institutional policies
- Need temporary access suspension

#### Restoring Access

1. Navigate to the Student Detail view (student will show "Revoked" status)
2. Click the **Restore Access** button
3. Confirm the action when prompted

![Screenshot: Restore Access Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_restore_access.png -->
<!-- Caption: Restore Access button shown for revoked students -->

### Viewing Student Forms

To see all forms submitted by a specific student:

1. Open the Student Detail view
2. Click **View Forms**

![Screenshot: Student Form Viewer]
<!-- SCREENSHOT_PLACEHOLDER: admin_student_forms.png -->
<!-- Caption: List of forms submitted by the selected student -->

The Student Form Viewer displays:
- **Form Name** - Type of form
- **Status** - Review status (pending, approved, rejected, needs_revision)
- **Submitted Date** - When the form was submitted
- **Reviewed Date** - When it was reviewed (if applicable)
- **Actions** - Link to view form details

---

## 6. Form Submission Management

The Forms section allows administrators to review, approve, and manage student form submissions.

### Form Submissions List

Access the Form Submissions by clicking **Forms** in the sidebar.

![Screenshot: Form Submissions List]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_list_full.png -->
<!-- Caption: Full Form Submissions view with statistics and filters -->

#### Submission Statistics Cards

Four cards display submission counts by status:

| Card | Color | Description |
|------|-------|-------------|
| **Pending** | Yellow | Awaiting review |
| **Approved** | Green | Approved submissions |
| **Rejected** | Red | Rejected submissions |
| **Needs Revision** | Purple | Returned for revision |

![Screenshot: Form Statistics Cards]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_stats.png -->
<!-- Caption: Form submission statistics by status -->

#### Submissions Table

The table displays:
- **Student Name** - Who submitted the form
- **Email** - Student's email address
- **Form Type** - Type of form submitted
- **Status** - Current review status (badge)
- **Submitted** - Submission date
- **Program** - Student's enrolled program
- **Actions** - View/Review buttons

![Screenshot: Form Submissions Table]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_table.png -->
<!-- Caption: Form submissions table with status badges and actions -->

### Filtering Submissions

Use multiple filters to find specific submissions:

#### Search Field

Search by student name or email address.

![Screenshot: Form Search Field]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_search.png -->
<!-- Caption: Search field for filtering by student name or email -->

#### Form Type Filter

Filter by specific form types:
- Personal Information
- Emergency Contact
- Medical Information
- Academic History
- Financial Aid
- Documents
- Consent

![Screenshot: Form Type Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_type_filter.png -->
<!-- Caption: Form type dropdown filter -->

#### Status Filter

Filter by submission status:
- **All Statuses** - Show all submissions
- **Pending** - Not yet reviewed
- **Approved** - Approved by administrator
- **Rejected** - Rejected by administrator
- **Needs Revision** - Returned for student revision

![Screenshot: Form Status Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_status_filter.png -->
<!-- Caption: Status filter dropdown for form submissions -->

#### Program Filter

Filter submissions by student's enrolled program.

![Screenshot: Form Program Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_forms_program_filter.png -->
<!-- Caption: Program filter for form submissions -->

### Reviewing Form Details

Click **View** or click a row to open the Form Detail Viewer.

![Screenshot: Form Detail Viewer]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_detail_full.png -->
<!-- Caption: Complete Form Detail view with all sections -->

#### Header Section

Shows the form type, current status badge, and back button.

![Screenshot: Form Detail Header]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_detail_header.png -->
<!-- Caption: Form Detail header with status badge -->

#### Student Information Panel

Displays:
- Student Name
- Email Address
- Enrolled Program
- Submission Date

![Screenshot: Form Student Info]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_student_info.png -->
<!-- Caption: Student information panel in form detail -->

#### Form Data Section

Displays all submitted form fields and values:

| Field Type | Display Format |
|------------|----------------|
| Text | Plain text value |
| Date | Formatted date (MM/DD/YYYY) |
| Email | Email address |
| Phone | Formatted phone number |
| Select | Selected option |
| Textarea | Multi-line text |
| File | File name with download link |

![Screenshot: Form Data Fields]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_data_fields.png -->
<!-- Caption: Submitted form data displayed in organized format -->

#### Review Section

For already-reviewed forms, displays:
- **Reviewed By** - Administrator who reviewed
- **Review Date** - When it was reviewed
- **Review Notes** - Any notes added during review

![Screenshot: Form Review Info]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_review_info.png -->
<!-- Caption: Review information for processed forms -->

### Approving Forms

To approve a pending form submission:

1. Open the Form Detail view
2. Review all submitted information
3. (Optional) Add review notes in the text area
4. Click the **Approve** button

![Screenshot: Approve Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_approve.png -->
<!-- Caption: Approve button in form review actions -->

The form status changes to "Approved" and the student is notified.

### Rejecting Forms

To reject a form submission:

1. Open the Form Detail view
2. Review the submitted information
3. Add review notes explaining the reason for rejection
4. Click the **Reject** button

![Screenshot: Reject Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_reject.png -->
<!-- Caption: Reject button with review notes area -->

> Note: Always include clear rejection reasons in the review notes to help students understand what went wrong.

### Requesting Revisions

To request corrections without fully rejecting:

1. Open the Form Detail view
2. Identify issues that need correction
3. Add detailed notes explaining required changes
4. Click the **Request Revision** button

![Screenshot: Request Revision Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_form_revision.png -->
<!-- Caption: Request Revision button for sending forms back to students -->

The form status changes to "Needs Revision" and the student can resubmit with corrections.

---

## 7. Curriculum Management

The Curriculum section allows administrators to manage academic and vocational programs.

> Note: Curriculum management requires Administrator or Super Administrator role.

### Curriculum List View

Access Curriculum Management by clicking **Curriculum** in the sidebar.

![Screenshot: Curriculum List View]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_list_full.png -->
<!-- Caption: Full Curriculum List view with statistics and program table -->

#### Program Statistics Cards

Four cards display program metrics:

| Card | Color | Description |
|------|-------|-------------|
| **Active** | Blue | Currently active programs |
| **Inactive** | Gray | Deactivated programs |
| **Online** | Cyan | Programs available online |
| **On-Campus** | Orange | On-campus only programs |

![Screenshot: Curriculum Statistics Cards]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_stats.png -->
<!-- Caption: Curriculum statistics showing program counts -->

#### Add Program Button

Click **Add Program** to create a new curriculum entry.

![Screenshot: Add Program Button]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_add_button.png -->
<!-- Caption: Add Program button location -->

#### Program Table

The table displays:
- **Program Name** - Full program name
- **Department** - Associated department
- **Degree Type** - Bachelor, Master, Doctoral, Certificate (with colored badge)
- **Credits** - Total credit/training hours
- **Duration** - Program length
- **Status** - Active/Inactive indicator

![Screenshot: Curriculum Table]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_table.png -->
<!-- Caption: Curriculum table with degree type badges -->

### Filtering Programs

#### Search Field

Search programs by name or code.

![Screenshot: Curriculum Search]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_search.png -->
<!-- Caption: Search field for finding programs -->

#### Department Filter

Filter by department:
- Computer Science
- Business Administration
- Engineering
- Nursing
- Arts and Sciences
- Adult Education - CDL

![Screenshot: Department Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_dept_filter.png -->
<!-- Caption: Department filter dropdown -->

#### Degree Type Filter

Filter by degree level:
- All Types
- Bachelor (Undergraduate)
- Master (Graduate)
- Doctoral
- Certificate

![Screenshot: Degree Type Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_degree_filter.png -->
<!-- Caption: Degree type filter dropdown -->

#### Status Filter

Filter by active status:
- All
- Active
- Inactive

![Screenshot: Curriculum Status Filter]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_status_filter.png -->
<!-- Caption: Active status filter -->

### Creating a New Program

1. Click the **Add Program** button
2. Complete the Curriculum Editor form (see below)
3. Click **Save**

![Screenshot: New Program Form]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_new.png -->
<!-- Caption: Empty Curriculum Editor for new program creation -->

### Editing Programs

1. Click any program row in the Curriculum List
2. Modify fields in the Curriculum Editor
3. Click **Save** to apply changes

![Screenshot: Edit Program Form]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_edit.png -->
<!-- Caption: Curriculum Editor with existing program data -->

#### Curriculum Editor Sections

##### Basic Information

| Field | Description | Required |
|-------|-------------|----------|
| **Program Name** | Full name of the program | Yes |
| **Description** | Detailed program description | No |
| **Department** | Associated department | Yes |
| **Degree Type** | Type of credential awarded | Yes |

![Screenshot: Basic Information Section]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_basic_info.png -->
<!-- Caption: Basic information fields in Curriculum Editor -->

##### Program Details

| Field | Description | Default |
|-------|-------------|---------|
| **Credit Hours** | Total credits or training hours | 0 |
| **Duration (months)** | Program length in months | 0 |
| **Is Active** | Whether program accepts enrollments | Yes |
| **Is Online** | Whether program is available online | No |

![Screenshot: Program Details Section]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_details.png -->
<!-- Caption: Program details configuration -->

##### CDL Program Details (Vocational Only)

For CDL programs, additional fields appear:

| Field | Description |
|-------|-------------|
| **CDL Class** | Class A or Class B |
| **Endorsements** | T, N, H, P, S, Air Brakes |
| **Classroom Hours** | Hours of classroom instruction |
| **Range Hours** | Hours of range/yard training |
| **Road Hours** | Hours of on-road training |

![Screenshot: CDL Details Section]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_cdl_details.png -->
<!-- Caption: CDL-specific fields for vocational programs -->

### Managing Required Forms

The Required Forms section determines which forms students must complete for the program.

![Screenshot: Required Forms Section]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_forms.png -->
<!-- Caption: Required Forms checklist in Curriculum Editor -->

#### Available Form Types

| Form | Typical Use |
|------|-------------|
| **Personal Information** | Basic student data (usually required for all) |
| **Emergency Contact** | Emergency contact details |
| **Medical Information** | Health information (required for nursing, CDL) |
| **Academic History** | Previous education (required for degree programs) |
| **Financial Aid** | FAFSA and financial information |
| **Documents** | Supporting document uploads |
| **Consent** | Agreements and signatures |

#### Configuring Required Forms

1. Check the box next to each form that should be required
2. Uncheck forms that are optional or not needed
3. Click **Save** to apply changes

![Screenshot: Form Requirements Configuration]
<!-- SCREENSHOT_PLACEHOLDER: admin_curriculum_form_config.png -->
<!-- Caption: Checking and unchecking required forms -->

---

## 8. Settings

The Settings section provides system configuration options.

> Note: Settings access requires Super Administrator role.

### System Configuration

![Screenshot: System Settings]
<!-- SCREENSHOT_PLACEHOLDER: admin_settings_system.png -->
<!-- Caption: System configuration options -->

### Admin User Management

Super Administrators can manage other admin accounts:

#### Viewing Admin Users

![Screenshot: Admin User List]
<!-- SCREENSHOT_PLACEHOLDER: admin_settings_users.png -->
<!-- Caption: List of administrator accounts -->

#### Creating Admin Users

1. Click **Add Admin**
2. Enter user details:
   - Email address
   - First and last name
   - Role (Super Administrator, Administrator, Instructor)
   - Assigned programs (for Instructors)
3. Click **Save**

![Screenshot: Create Admin User]
<!-- SCREENSHOT_PLACEHOLDER: admin_settings_add_user.png -->
<!-- Caption: New administrator creation form -->

#### Editing Admin Users

1. Click an admin user row
2. Modify fields as needed
3. Click **Save**

![Screenshot: Edit Admin User]
<!-- SCREENSHOT_PLACEHOLDER: admin_settings_edit_user.png -->
<!-- Caption: Admin user editing form -->

#### Assigning Programs to Instructors

For Instructor accounts, specify which programs they can access:

1. Edit the Instructor account
2. In the **Assigned Programs** section, check the programs
3. Click **Save**

![Screenshot: Program Assignment]
<!-- SCREENSHOT_PLACEHOLDER: admin_settings_program_assign.png -->
<!-- Caption: Assigning programs to an Instructor account -->

---

## 9. Common Workflows

### Processing New Student Enrollments

**Workflow for reviewing a new student's submissions:**

1. Navigate to **Forms** in the sidebar
2. Filter by **Status: Pending** to see forms needing review
3. Sort by submission date (oldest first) to process in order
4. Click the first pending submission
5. Review all submitted data for accuracy and completeness
6. Take action:
   - **Approve** if all information is correct
   - **Request Revision** if minor corrections needed
   - **Reject** if major issues or fraudulent information
7. Repeat for remaining forms

![Screenshot: Enrollment Processing Workflow]
<!-- SCREENSHOT_PLACEHOLDER: admin_workflow_enrollment.png -->
<!-- Caption: Step-by-step form review process -->

### Handling Form Revisions

**When a student resubmits a form marked "Needs Revision":**

1. Navigate to **Forms**
2. The resubmitted form appears with **Pending** status
3. Open the form to review changes
4. Compare with your previous review notes
5. Verify the student addressed all issues
6. **Approve** if corrections are satisfactory, or **Request Revision** again if needed

![Screenshot: Revision Review Workflow]
<!-- SCREENSHOT_PLACEHOLDER: admin_workflow_revision.png -->
<!-- Caption: Reviewing a resubmitted form -->

### Setting Up a New Program

**Complete workflow for adding a new academic or vocational program:**

1. Navigate to **Curriculum**
2. Click **Add Program**
3. Enter Basic Information:
   - Program name
   - Description
   - Select department
   - Choose degree type
4. Configure Program Details:
   - Set credit/training hours
   - Set duration
   - Enable **Is Active**
   - Enable **Is Online** if applicable
5. For CDL programs, configure:
   - CDL class (A or B)
   - Endorsements
   - Training hour breakdown
6. Select Required Forms:
   - Check all forms students must complete
7. Click **Save**
8. Verify the program appears in the Curriculum List

![Screenshot: New Program Setup Workflow]
<!-- SCREENSHOT_PLACEHOLDER: admin_workflow_new_program.png -->
<!-- Caption: Complete program creation process -->

---

## 10. Troubleshooting

### Common Issues and Solutions

#### Cannot Log In

| Problem | Solution |
|---------|----------|
| "Invalid credentials" | Verify email and password are correct |
| Forgot password | Contact Super Administrator for reset |
| Account locked | Contact Super Administrator |

#### Menu Items Missing

| Problem | Solution |
|---------|----------|
| Curriculum not visible | Requires Administrator or Super Administrator role |
| Settings not visible | Requires Super Administrator role |
| Limited student access | Instructor role - contact admin for role upgrade |

#### Forms Not Loading

| Problem | Solution |
|---------|----------|
| Blank form list | Check filter settings, click **Clear** |
| "Error loading data" | Refresh the page, check network connection |
| Slow loading | Large datasets take time, wait for completion |

#### Cannot Save Changes

| Problem | Solution |
|---------|----------|
| "Validation error" | Check required fields marked with asterisk |
| "Permission denied" | Your role may not allow this action |
| Changes not persisting | Check network connection, try again |

![Screenshot: Error Message Example]
<!-- SCREENSHOT_PLACEHOLDER: admin_troubleshooting_error.png -->
<!-- Caption: Example error message with resolution guidance -->

### Getting Help

If you encounter issues not covered here:

1. Note the exact error message
2. Document the steps that led to the issue
3. Contact your Super Administrator or IT support
4. Provide:
   - Your username
   - Time of the issue
   - Browser and operating system
   - Screenshots if possible

---

## 11. Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Tab` | Move to next field |
| `Shift + Tab` | Move to previous field |
| `Enter` | Submit form / Confirm action |
| `Escape` | Cancel dialog / Close modal |

---

## 12. Glossary

| Term | Definition |
|------|------------|
| **Accredited Program** | Academic program recognized by accrediting bodies |
| **CDL** | Commercial Driver's License |
| **Curriculum** | A program of study or training |
| **Endorsement** | Additional CDL qualification (T, N, H, P, S) |
| **Form Submission** | A completed form submitted by a student |
| **Instructor** | Admin role with read-only access to assigned programs |
| **Intake Status** | Student's progress in the onboarding process |
| **Pending** | Awaiting review or action |
| **Revoked** | Student access has been disabled |
| **Vocational Program** | Non-accredited training program (e.g., CDL) |

---

## Document History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | January 2026 | System | Initial release |

---

## Screenshot Reference Guide

For documentation maintainers, the following screenshots are needed:

| Placeholder | Description | Dimensions |
|-------------|-------------|------------|
| `admin_portal_url.png` | Browser showing admin URL | 800x100 |
| `admin_login_screen.png` | Login page full view | 1280x720 |
| `admin_login_error.png` | Login with error message | 600x400 |
| `admin_role_indicator.png` | Top nav with role display | 400x60 |
| `admin_dashboard_full.png` | Complete dashboard view | 1280x720 |
| `admin_top_navigation.png` | Top navigation bar | 1280x60 |
| `admin_sidebar.png` | Sidebar navigation | 250x500 |
| `admin_dashboard_stats.png` | Dashboard stat cards | 800x200 |
| `admin_student_list_full.png` | Student list complete | 1280x720 |
| `admin_student_stats.png` | Student stat cards | 800x100 |
| `admin_student_table.png` | Student data table | 1000x400 |
| `admin_student_search.png` | Search field in use | 400x60 |
| `admin_student_program_filter.png` | Program dropdown open | 300x200 |
| `admin_student_status_filter.png` | Status dropdown open | 300x200 |
| `admin_student_clear_filters.png` | Clear button highlight | 200x60 |
| `admin_student_detail_full.png` | Student detail view | 1280x720 |
| `admin_student_detail_header.png` | Student header section | 800x120 |
| `admin_student_info_cards.png` | Student info cards | 800x200 |
| `admin_student_actions.png` | Action buttons | 400x60 |
| `admin_revoke_access_dialog.png` | Revoke confirmation | 400x200 |
| `admin_restore_access.png` | Restore access button | 200x60 |
| `admin_student_forms.png` | Student forms list | 800x400 |
| `admin_forms_list_full.png` | Forms list complete | 1280x720 |
| `admin_forms_stats.png` | Forms stat cards | 800x100 |
| `admin_forms_table.png` | Forms data table | 1000x400 |
| `admin_forms_search.png` | Forms search field | 400x60 |
| `admin_forms_type_filter.png` | Form type dropdown | 300x300 |
| `admin_forms_status_filter.png` | Forms status dropdown | 300x200 |
| `admin_forms_program_filter.png` | Forms program filter | 300x200 |
| `admin_form_detail_full.png` | Form detail complete | 1280x720 |
| `admin_form_detail_header.png` | Form detail header | 800x100 |
| `admin_form_student_info.png` | Student info panel | 400x200 |
| `admin_form_data_fields.png` | Form field display | 600x400 |
| `admin_form_review_info.png` | Review information | 400x150 |
| `admin_form_approve.png` | Approve button | 300x100 |
| `admin_form_reject.png` | Reject with notes | 500x200 |
| `admin_form_revision.png` | Request revision | 500x200 |
| `admin_curriculum_list_full.png` | Curriculum list complete | 1280x720 |
| `admin_curriculum_stats.png` | Curriculum stat cards | 800x100 |
| `admin_curriculum_add_button.png` | Add program button | 200x60 |
| `admin_curriculum_table.png` | Curriculum table | 1000x400 |
| `admin_curriculum_search.png` | Curriculum search | 400x60 |
| `admin_curriculum_dept_filter.png` | Department dropdown | 300x250 |
| `admin_curriculum_degree_filter.png` | Degree type dropdown | 300x200 |
| `admin_curriculum_status_filter.png` | Curriculum status filter | 300x150 |
| `admin_curriculum_new.png` | New program editor | 1000x700 |
| `admin_curriculum_edit.png` | Edit program editor | 1000x700 |
| `admin_curriculum_basic_info.png` | Basic info section | 600x300 |
| `admin_curriculum_details.png` | Program details section | 600x200 |
| `admin_curriculum_cdl_details.png` | CDL details section | 600x250 |
| `admin_curriculum_forms.png` | Required forms section | 600x300 |
| `admin_curriculum_form_config.png` | Form checkboxes | 400x300 |
| `admin_settings_system.png` | System settings | 800x500 |
| `admin_settings_users.png` | Admin user list | 800x400 |
| `admin_settings_add_user.png` | Add admin form | 600x400 |
| `admin_settings_edit_user.png` | Edit admin form | 600x400 |
| `admin_settings_program_assign.png` | Program assignment | 500x300 |
| `admin_workflow_enrollment.png` | Enrollment workflow | 800x400 |
| `admin_workflow_revision.png` | Revision workflow | 800x300 |
| `admin_workflow_new_program.png` | New program workflow | 800x400 |
| `admin_troubleshooting_error.png` | Error message example | 500x150 |

---

*End of Administration User Guide*
