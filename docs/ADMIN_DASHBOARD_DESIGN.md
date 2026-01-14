# Admin Dashboard Portal - Design Document

## Overview

The Admin Dashboard is a separate web portal for university staff (administrators and instructors) to manage students, their onboarding forms, and curriculum configuration. This portal is accessible at a different URL path from the student-facing application.

## Access Points

| Portal | URL Path | Users |
|--------|----------|-------|
| Student Onboarding | `/` | Students |
| Admin Dashboard | `/administration` | Staff, Instructors, Admins |

## User Roles

### Role Hierarchy

```
┌─────────────────────────────────────────────────────────┐
│                    Super Admin                          │
│  - Full system access                                   │
│  - Manage other admins                                  │
│  - System configuration                                 │
├─────────────────────────────────────────────────────────┤
│                    Administrator                        │
│  - Manage students                                      │
│  - Manage curriculum                                    │
│  - View all forms                                       │
│  - Revoke student access                                │
├─────────────────────────────────────────────────────────┤
│                     Instructor                          │
│  - View students in their programs                      │
│  - View submitted forms (read-only)                     │
│  - Cannot modify curriculum                             │
└─────────────────────────────────────────────────────────┘
```

### Role Permissions Matrix

| Feature | Super Admin | Administrator | Instructor |
|---------|-------------|---------------|------------|
| View Students | All | All | Own Programs |
| Edit Students | Yes | Yes | No |
| Revoke Login | Yes | Yes | No |
| View Forms | All | All | Own Programs |
| Approve Forms | Yes | Yes | No |
| Manage Curriculum | Yes | Yes | No |
| Edit Syllabus | Yes | Yes | No |
| Reorder Forms | Yes | Yes | No |
| Manage Admins | Yes | No | No |
| System Config | Yes | No | No |

---

## Application Architecture

### URL Routing Structure

```
/                           → Student Portal (existing)
/administration             → Admin Portal Entry
/administration/login       → Admin Login Page
/administration/dashboard   → Admin Dashboard Home
/administration/students    → Student Management
/administration/students/:id → Student Detail View
/administration/forms       → Form Submissions
/administration/curriculum  → Curriculum Management
/administration/curriculum/:id → Edit Curriculum
/administration/settings    → System Settings (Super Admin)
```

### Component Structure

```
src/
├── admin/
│   ├── AdminApp.cpp/h              # Admin application class
│   ├── AdminAuthManager.cpp/h      # Admin authentication
│   ├── AdminLoginWidget.cpp/h      # Admin login UI
│   ├── AdminDashboard.cpp/h        # Admin dashboard home
│   ├── AdminNavigation.cpp/h       # Admin navigation bar
│   └── AdminSidebar.cpp/h          # Section navigation sidebar
├── admin/students/
│   ├── StudentListWidget.cpp/h     # Student list with filters
│   ├── StudentDetailWidget.cpp/h   # Individual student view
│   └── StudentFormViewer.cpp/h     # View student's forms
├── admin/curriculum/
│   ├── CurriculumListWidget.cpp/h  # List all programs
│   ├── CurriculumEditor.cpp/h      # Edit program details
│   ├── SyllabusEditor.cpp/h        # Edit syllabus content
│   └── FormRequirementsEditor.cpp/h # Edit/reorder required forms
├── admin/forms/
│   ├── FormSubmissionsList.cpp/h   # All form submissions
│   ├── FormReviewWidget.cpp/h      # Review/approve forms
│   └── PostRegistrationManager.cpp/h # Manage post-reg forms
└── admin/models/
    ├── AdminUser.cpp/h             # Admin user model
    └── AdminSession.cpp/h          # Admin session management
```

---

## Database Schema Changes

### New Tables

#### `admin_user` Table

```sql
CREATE TABLE admin_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(200) NOT NULL UNIQUE,
    password_hash VARCHAR(500) NOT NULL,
    first_name VARCHAR(100),
    last_name VARCHAR(100),
    role VARCHAR(50) NOT NULL DEFAULT 'instructor',  -- 'super_admin', 'administrator', 'instructor'
    department_id INTEGER REFERENCES department(id),
    is_active BOOLEAN DEFAULT TRUE,
    last_login TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### `admin_program_assignment` Table (for instructors)

```sql
CREATE TABLE admin_program_assignment (
    id SERIAL PRIMARY KEY,
    admin_user_id INTEGER NOT NULL REFERENCES admin_user(id),
    curriculum_id INTEGER NOT NULL REFERENCES curriculum(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(admin_user_id, curriculum_id)
);
```

### Modified Tables

#### `student` Table - Add login control

```sql
ALTER TABLE student ADD COLUMN is_login_revoked BOOLEAN DEFAULT FALSE;
ALTER TABLE student ADD COLUMN revoked_at TIMESTAMP;
ALTER TABLE student ADD COLUMN revoked_by INTEGER REFERENCES admin_user(id);
ALTER TABLE student ADD COLUMN revocation_reason TEXT;
```

#### `curriculum` Table - Add syllabus content

```sql
ALTER TABLE curriculum ADD COLUMN syllabus_content TEXT;
ALTER TABLE curriculum ADD COLUMN syllabus_updated_at TIMESTAMP;
ALTER TABLE curriculum ADD COLUMN syllabus_updated_by INTEGER REFERENCES admin_user(id);
```

#### `curriculum_form_requirement` Table - Add ordering

```sql
ALTER TABLE curriculum_form_requirement ADD COLUMN display_order INTEGER DEFAULT 0;
ALTER TABLE curriculum_form_requirement ADD COLUMN is_required BOOLEAN DEFAULT TRUE;
ALTER TABLE curriculum_form_requirement ADD COLUMN added_by INTEGER REFERENCES admin_user(id);
```

---

## UI Design

### Admin Portal Layout

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   Main Content Area                              │
│  ──────────  │                                                  │
│  Students    │   ┌──────────────────────────────────────────┐   │
│  Forms       │   │                                          │   │
│  Curriculum  │   │        (Section-specific content)        │   │
│  ──────────  │   │                                          │   │
│  Settings *  │   │                                          │   │
│              │   └──────────────────────────────────────────┘   │
│              │                                                  │
│  * Super     │                                                  │
│    Admin     │                                                  │
│    only      │                                                  │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘
     Sidebar                    Content Area
```

### Admin Login Page

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│                    ┌─────────────────────┐                      │
│                    │   [University Logo]  │                      │
│                    │                      │                      │
│                    │   Admin Portal       │                      │
│                    │   ────────────────   │                      │
│                    │                      │                      │
│                    │   Email:             │                      │
│                    │   ┌───────────────┐  │                      │
│                    │   │               │  │                      │
│                    │   └───────────────┘  │                      │
│                    │                      │                      │
│                    │   Password:          │                      │
│                    │   ┌───────────────┐  │                      │
│                    │   │               │  │                      │
│                    │   └───────────────┘  │                      │
│                    │                      │                      │
│                    │   [    Log In    ]   │                      │
│                    │                      │                      │
│                    │   Forgot Password?   │                      │
│                    └─────────────────────┘                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Dashboard Home

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  ▶ Dashboard │   Welcome, Administrator Name                    │
│  Students    │   ─────────────────────────────────────────────  │
│  Forms       │                                                  │
│  Curriculum  │   ┌─────────────┐ ┌─────────────┐ ┌────────────┐ │
│              │   │   ▲ 124     │ │   ▲ 45      │ │   ▲ 8      │ │
│              │   │  Students   │ │  Pending    │ │  Programs  │ │
│              │   │  Enrolled   │ │  Forms      │ │  Active    │ │
│              │   └─────────────┘ └─────────────┘ └────────────┘ │
│              │                                                  │
│              │   Recent Activity                                │
│              │   ─────────────────────────────────────────────  │
│              │   • John Doe submitted Personal Information      │
│              │   • Jane Smith completed onboarding              │
│              │   • CS-BS syllabus updated                       │
│              │   • New student: Mike Johnson registered         │
│              │                                                  │
│              │   Quick Actions                                  │
│              │   ─────────────────────────────────────────────  │
│              │   [Review Pending Forms]  [Add New Program]      │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘
```

### Student Management

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   Student Management                             │
│  ▶ Students  │   ─────────────────────────────────────────────  │
│  Forms       │                                                  │
│  Curriculum  │   Search: [________________]  Program: [All ▼]   │
│              │   Status: [All ▼]                                │
│              │                                                  │
│              │   ┌──────────────────────────────────────────┐   │
│              │   │ Name          │ Email       │ Program   │   │
│              │   ├──────────────────────────────────────────┤   │
│              │   │ John Doe      │ john@...    │ CS-BS     │ ⋮ │
│              │   │ Jane Smith    │ jane@...    │ BUS-MBA   │ ⋮ │
│              │   │ Mike Johnson  │ mike@...    │ ENG-BSME  │ ⋮ │
│              │   │ Sarah Wilson  │ sarah@...   │ CS-MS     │ ⋮ │
│              │   │ [REVOKED] Tom │ tom@...     │ CS-BS     │ ⋮ │
│              │   └──────────────────────────────────────────┘   │
│              │                                                  │
│              │   Showing 1-10 of 124  [< Prev] [Next >]         │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘

⋮ = Actions Menu (View, Edit, Revoke Access, Delete)
```

### Student Detail View

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   ← Back to Students                             │
│  ▶ Students  │                                                  │
│  Forms       │   John Doe                    [Revoke Access]    │
│  Curriculum  │   john.doe@email.com          Status: Active     │
│              │   ─────────────────────────────────────────────  │
│              │                                                  │
│              │   ┌─────────────────────┬────────────────────┐   │
│              │   │ Personal Info       │ Program Info       │   │
│              │   ├─────────────────────┼────────────────────┤   │
│              │   │ Phone: 555-1234     │ Program: CS-BS     │   │
│              │   │ DOB: 1998-05-15     │ Type: Undergrad    │   │
│              │   │ SSN: ***-**-1234    │ Enrolled: 2024-01  │   │
│              │   │ Citizenship: US     │ Progress: 75%      │   │
│              │   └─────────────────────┴────────────────────┘   │
│              │                                                  │
│              │   Submitted Forms                                │
│              │   ─────────────────────────────────────────────  │
│              │   ✓ Personal Information    [View] [Edit]        │
│              │   ✓ Emergency Contacts      [View] [Edit]        │
│              │   ✓ Academic History        [View] [Edit]        │
│              │   ○ Consent Form            Pending              │
│              │                                                  │
│              │   Post-Registration Forms                        │
│              │   ─────────────────────────────────────────────  │
│              │   + Medical Information     [Request]            │
│              │   + Financial Aid           [Request]            │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘
```

### Curriculum Management

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   Curriculum Management          [+ Add Program] │
│  Students    │   ─────────────────────────────────────────────  │
│  Forms       │                                                  │
│  ▶ Curriculum│   Department: [All ▼]   Degree: [All ▼]          │
│              │                                                  │
│              │   ┌──────────────────────────────────────────┐   │
│              │   │ Program         │ Dept   │ Forms │ Enroll│   │
│              │   ├──────────────────────────────────────────┤   │
│              │   │ CS-BS           │ CS     │ 4     │ 45    │ ⋮ │
│              │   │ CS-MS           │ CS     │ 5     │ 23    │ ⋮ │
│              │   │ BUS-MBA         │ BUS    │ 6     │ 34    │ ⋮ │
│              │   │ ENG-BSME        │ ENG    │ 4     │ 28    │ ⋮ │
│              │   └──────────────────────────────────────────┘   │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘

⋮ = Actions Menu (Edit, Edit Syllabus, Manage Forms, Deactivate)
```

### Curriculum Editor - Syllabus Tab

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   ← Back to Curriculum                           │
│  Students    │                                                  │
│  Forms       │   CS-BS - Computer Science (Bachelor)            │
│  ▶ Curriculum│   ─────────────────────────────────────────────  │
│              │                                                  │
│              │   [Details] [Syllabus] [Required Forms]          │
│              │            ─────────                             │
│              │                                                  │
│              │   Syllabus Content                               │
│              │   ┌──────────────────────────────────────────┐   │
│              │   │                                          │   │
│              │   │  # Program Overview                      │   │
│              │   │                                          │   │
│              │   │  The Bachelor of Science in Computer     │   │
│              │   │  Science program prepares students...    │   │
│              │   │                                          │   │
│              │   │  ## Core Requirements                    │   │
│              │   │  - Programming Fundamentals              │   │
│              │   │  - Data Structures                       │   │
│              │   │  - Algorithms                            │   │
│              │   │                                          │   │
│              │   │  ## Career Paths                         │   │
│              │   │  - Software Engineer                     │   │
│              │   │  - Data Scientist                        │   │
│              │   │                                          │   │
│              │   └──────────────────────────────────────────┘   │
│              │                                                  │
│              │   [Save Changes]  [Preview]  Last saved: 5m ago  │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘
```

### Curriculum Editor - Required Forms Tab

```
┌─────────────────────────────────────────────────────────────────┐
│  [Logo] Admin Portal                    [Admin Name] [Logout]   │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                  │
│  Dashboard   │   ← Back to Curriculum                           │
│  Students    │                                                  │
│  Forms       │   CS-BS - Computer Science (Bachelor)            │
│  ▶ Curriculum│   ─────────────────────────────────────────────  │
│              │                                                  │
│              │   [Details] [Syllabus] [Required Forms]          │
│              │                        ──────────────            │
│              │                                                  │
│              │   Drag to reorder • Toggle required status       │
│              │                                                  │
│              │   ┌──────────────────────────────────────────┐   │
│              │   │ ≡ │ 1. Personal Information    │ [✓] Req │   │
│              │   │ ≡ │ 2. Emergency Contacts      │ [✓] Req │   │
│              │   │ ≡ │ 3. Academic History        │ [✓] Req │   │
│              │   │ ≡ │ 4. Terms and Consent       │ [✓] Req │   │
│              │   │ ≡ │ 5. Medical Information     │ [ ] Opt │   │
│              │   │ ≡ │ 6. Financial Aid           │ [ ] Opt │   │
│              │   └──────────────────────────────────────────┘   │
│              │                                                  │
│              │   [+ Add Form]                                   │
│              │                                                  │
│              │   Available Forms Not Assigned:                  │
│              │   • Document Upload                              │
│              │                                                  │
│              │   [Save Order]                                   │
│              │                                                  │
└──────────────┴──────────────────────────────────────────────────┘

≡ = Drag handle for reordering
```

### Revoke Access Dialog

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│         ┌───────────────────────────────────────────┐           │
│         │                                           │           │
│         │   Revoke Student Access                   │           │
│         │   ─────────────────────────────────────   │           │
│         │                                           │           │
│         │   Student: John Doe                       │           │
│         │   Email: john.doe@email.com               │           │
│         │                                           │           │
│         │   ⚠ This will prevent the student from   │           │
│         │   logging into the system.                │           │
│         │                                           │           │
│         │   Reason for revocation:                  │           │
│         │   ┌─────────────────────────────────────┐ │           │
│         │   │                                     │ │           │
│         │   │                                     │ │           │
│         │   └─────────────────────────────────────┘ │           │
│         │                                           │           │
│         │   [ ] Send notification email to student  │           │
│         │                                           │           │
│         │        [Cancel]  [Revoke Access]          │           │
│         │                                           │           │
│         └───────────────────────────────────────────┘           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## API Endpoints

### Admin Authentication

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/AdminUser` | POST | Create admin user |
| `/AdminUser?filter[email]=` | GET | Login (find by email) |
| `/AdminUser/:id` | PATCH | Update admin profile |

### Student Management

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/Student` | GET | List all students (with filters) |
| `/Student/:id` | GET | Get student details |
| `/Student/:id` | PATCH | Update student (including revoke) |
| `/Student/:id/forms` | GET | Get student's form submissions |

### Curriculum Management

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/Curriculum` | GET | List all programs |
| `/Curriculum` | POST | Create new program |
| `/Curriculum/:id` | PATCH | Update program (syllabus, etc.) |
| `/Curriculum/:id` | DELETE | Deactivate program |
| `/CurriculumFormRequirement` | GET | Get form requirements |
| `/CurriculumFormRequirement` | POST | Add form requirement |
| `/CurriculumFormRequirement/:id` | PATCH | Update order/required status |
| `/CurriculumFormRequirement/:id` | DELETE | Remove form requirement |

---

## Implementation Phases

### Phase 1: Foundation
- [ ] Create AdminApp class and routing
- [ ] Implement admin authentication (separate from students)
- [ ] Create admin login page
- [ ] Basic admin navigation/layout

### Phase 2: Student Management
- [ ] Student list with search and filters
- [ ] Student detail view
- [ ] View student submitted forms
- [ ] Revoke/restore student access

### Phase 3: Curriculum Management
- [ ] Curriculum list view
- [ ] Curriculum editor (details tab)
- [ ] Syllabus editor with rich text
- [ ] Required forms editor with drag-and-drop reorder

### Phase 4: Forms Management
- [ ] Form submissions list
- [ ] Form review and approval workflow
- [ ] Post-registration form requests

### Phase 5: Advanced Features
- [ ] Admin user management (Super Admin)
- [ ] Activity logging and audit trail
- [ ] Reports and analytics
- [ ] Email notifications

---

## Security Considerations

1. **Separate Authentication**: Admin users stored in separate table from students
2. **Role-Based Access**: Permissions enforced at API and UI level
3. **Session Isolation**: Admin sessions separate from student sessions
4. **Audit Logging**: All admin actions logged with timestamp and user
5. **Revocation Records**: Track who revoked access and why
6. **Password Requirements**: Stronger requirements for admin accounts

---

## Files to Create/Modify

### New Files

```
src/admin/
├── AdminApp.cpp
├── AdminApp.h
├── AdminAuthManager.cpp
├── AdminAuthManager.h
├── AdminLoginWidget.cpp
├── AdminLoginWidget.h
├── AdminDashboard.cpp
├── AdminDashboard.h
├── AdminNavigation.cpp
├── AdminNavigation.h
├── AdminSidebar.cpp
├── AdminSidebar.h
├── students/
│   ├── StudentListWidget.cpp
│   ├── StudentListWidget.h
│   ├── StudentDetailWidget.cpp
│   ├── StudentDetailWidget.h
│   ├── StudentFormViewer.cpp
│   └── StudentFormViewer.h
├── curriculum/
│   ├── CurriculumListWidget.cpp
│   ├── CurriculumListWidget.h
│   ├── CurriculumEditor.cpp
│   ├── CurriculumEditor.h
│   ├── SyllabusEditor.cpp
│   ├── SyllabusEditor.h
│   ├── FormRequirementsEditor.cpp
│   └── FormRequirementsEditor.h
└── models/
    ├── AdminUser.cpp
    ├── AdminUser.h
    ├── AdminSession.cpp
    └── AdminSession.h

database/migrations/
└── 002_add_admin_tables.sql

resources/
└── admin-styles.css
```

### Modified Files

```
src/main.cpp                 # Add admin app routing
CMakeLists.txt               # Add new source files
database/schema.sql          # Add admin tables
```

---

## Estimated Effort

| Phase | Components | Complexity |
|-------|------------|------------|
| Phase 1 | 8 files | Medium |
| Phase 2 | 6 files | Medium |
| Phase 3 | 8 files | High |
| Phase 4 | 4 files | Medium |
| Phase 5 | 4 files | High |

---

## Next Steps

1. Review and approve this design document
2. Create database migration for admin tables
3. Begin Phase 1 implementation
4. Iterate based on feedback
