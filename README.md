# Student Onboarding Application

A modular C++ web application built with the Wt (Witty) Web Toolkit for processing student onboarding forms during the enrollment process.

**Developed by Imagery Business Systems LLC**

## Features

- **Multi-user Support**: Multiple students can simultaneously process their onboarding forms
- **Modular Form Design**: Each form type is implemented as a separate, reusable module
- **Configurable Required Forms**: Forms required for each student are determined based on:
  - Student type (undergraduate, graduate, doctoral, certificate)
  - Selected curriculum/program
  - Special status (international, transfer, veteran, financial aid)
- **API Integration**: Sends form data to backend via RESTful API (designed for ApiLogicServer)
- **Session Management**: Tracks user progress across multiple forms
- **Progress Tracking**: Visual progress indicator showing completion status
- **Post-Onboarding Forms**: Support for additional forms after completing initial onboarding
- **Completed Forms View**: Students can review and edit previously submitted forms
- **Returning Student Support**: Form data pre-fills automatically when students log back in
- **Completion Tracking**: System remembers which forms students have completed across sessions

## Application Workflow

### 1. Authentication
- New users register with name, email, and password
- Returning users log in to continue their application
- **Returning students**: Previously selected program is automatically loaded
- **Completed students**: If all forms were completed, dashboard shows completion view
- Session persists user progress

### 2. Program Selection
- Students browse available academic programs in a card-based layout
- **Card Grid Layout**: Programs displayed in responsive 3-column grid (2 columns on tablet, 1 on mobile)
- Each program card shows:
  - Program name (blue header)
  - Department name
  - Brief description
  - Degree type, credit hours, and duration badges
  - **Info button (i)**: Opens syllabus popup with detailed program information
  - **Select button**: Immediately selects program and navigates to forms
- Filter options by department and degree type
- No separate "Continue" button needed - selection is immediate

### 3. Dashboard
The dashboard provides a central hub for students:

**Advertisement Banner:**
- Full-width promotional banner at the top of the dashboard
- Blue gradient background with "Featured" badge
- Displays business advertisements and campus promotions
- Call-to-action link in yellow for visibility
- Responsive design adapts to mobile screens

**During Onboarding:**
- Shows selected program information with "Change Program" option
- Displays progress bar with completion percentage
- "Continue Application" button to resume form entry
- **Change Program**: Allows switching to a different program (resets all form progress)

**After Completion:**
- Two-column layout with main content and sidebar panel
- Completion confirmation message
- Right-hand panel displays:
  - **Completed Forms**: List of submitted forms with eye icon (👁) to view/edit
  - **Recommended Forms**: Optional additional forms with arrow (→) to start

```
┌─────────────────────────────────────────────────────────┐
│ [Featured] Campus Bookstore - 15% off textbooks! Shop → │  ← Ad Banner
├─────────────────────────────────────────────────────────┤
│                             │                           │
│  ┌───────────────────────┐  │  Completed Forms          │
│  │ Selected Program      │  │  ✓ Personal Information 👁│
│  │ Bachelor of Science   │  │  ✓ Emergency Contacts   👁│
│  │ in Computer Science   │  │  ✓ Academic History     👁│
│  │ [Change Program]      │  │  ✓ Terms and Consent    👁│
│  └───────────────────────┘  │                           │
│                             │  Recommended Forms        │
│  ┌───────────────────────┐  │  + Medical Information  → │
│  │ ✓ Onboarding Complete!│  │  + Financial Aid        → │
│  │ Thank you for         │  │                           │
│  │ completing your forms │  │                           │
│  └───────────────────────┘  │                           │
│                             │                           │
│  ┌───────────────────────┐  │                           │
│  │ Need Help?            │  │                           │
│  │ admissions@univ.edu   │  │                           │
│  │ (555) 123-4567        │  │                           │
│  └───────────────────────┘  │                           │
│                             │                           │
└─────────────────────────────┴───────────────────────────┘
        Main Content                  Sidebar (light blue)
```

**Dashboard Layout Wireframe (During Onboarding):**
```
┌─────────────────────────────────────────────────────────┐
│ ┌─────────────────────────────────────────────────────┐ │
│ │ [Featured] Campus Bookstore - Get 15% off! Shop →   │ │  ← Blue gradient
│ └─────────────────────────────────────────────────────┘ │     banner
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌───────────────────────────────────────────────────┐  │
│  │ Selected Program                  [Change Program] │  │
│  │ ─────────────────────────────────────────────────  │  │
│  │ Bachelor of Science in Computer Science            │  │
│  │ Comprehensive program covering software...         │  │
│  │ Credit Hours: 120                                  │  │
│  └───────────────────────────────────────────────────┘  │
│                                                         │
│  ┌───────────────────────────────────────────────────┐  │
│  │ Application Progress                               │  │
│  │ ─────────────────────────────────────────────────  │  │
│  │ You have completed 2 of 4 required forms.          │  │
│  │ ████████████░░░░░░░░░░░░  50% Complete             │  │
│  │                                                    │  │
│  │            [ Continue Application ]                │  │
│  └───────────────────────────────────────────────────┘  │
│                                                         │
│  ┌───────────────────────────────────────────────────┐  │
│  │ Need Help?                                         │  │
│  │ Email: admissions@university.edu                   │  │
│  │ Phone: (555) 123-4567                             │  │
│  │ Office Hours: Mon-Fri, 9 AM - 5 PM                │  │
│  └───────────────────────────────────────────────────┘  │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

### 4. Forms View
- Program name header displays selected curriculum
- Left sidebar shows progress through form steps
- Main area displays current form
- Navigation: Previous/Next buttons, click sidebar to jump

### 5. Completion
- Success confirmation when all required forms submitted
- Return to dashboard to view completed forms
- Access optional additional forms

## Project Structure

```
Student-Onboarding/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── WORKFLOW.md                 # Detailed workflow documentation
├── ARCHITECTURE_ANALYSIS.md    # Technical architecture details
├── src/
│   ├── main.cpp                # Application entry point
│   ├── app/
│   │   ├── StudentIntakeApp.cpp/h  # Main application class
│   │   └── AppConfig.h         # Application configuration
│   ├── auth/
│   │   ├── AuthManager.cpp/h   # Authentication logic
│   │   ├── LoginWidget.cpp/h   # Login UI
│   │   └── RegisterWidget.cpp/h # Registration UI
│   ├── forms/
│   │   ├── BaseForm.cpp/h      # Base form class
│   │   ├── FormFactory.cpp/h   # Form creation factory
│   │   ├── PersonalInfoForm.cpp/h
│   │   ├── EmergencyContactForm.cpp/h
│   │   ├── MedicalInfoForm.cpp/h
│   │   ├── AcademicHistoryForm.cpp/h
│   │   ├── FinancialAidForm.cpp/h
│   │   ├── DocumentUploadForm.cpp/h
│   │   └── ConsentForm.cpp/h
│   ├── curriculum/
│   │   ├── CurriculumManager.cpp/h  # Curriculum data management
│   │   └── CurriculumSelector.cpp/h # Curriculum selection UI
│   ├── api/
│   │   ├── ApiClient.cpp/h     # HTTP client for API calls
│   │   └── FormSubmissionService.cpp/h # Form submission logic
│   ├── models/
│   │   ├── Student.cpp/h       # Student data model
│   │   ├── FormData.cpp/h      # Form data model
│   │   └── Curriculum.cpp/h    # Curriculum data model
│   ├── session/
│   │   ├── SessionManager.cpp/h # Session management
│   │   └── StudentSession.cpp/h # Individual session state
│   └── widgets/
│       ├── NavigationWidget.cpp/h   # Navigation bar
│       ├── ProgressWidget.cpp/h     # Progress indicator
│       ├── FormContainer.cpp/h      # Form container/navigation
│       └── DashboardWidget.cpp/h    # Student dashboard
├── config/
│   ├── forms_config.json       # Form type configuration
│   └── curriculum_config.json  # Curriculum configuration
├── database/
│   ├── schema.sql              # Database schema
│   └── postgresql-schema.sql   # PostgreSQL schema
└── resources/
    └── styles.css              # Custom CSS styles
```

## Requirements

- C++17 compatible compiler
- CMake 3.16 or higher
- Wt (Witty) Web Toolkit 4.x
- libcurl
- nlohmann/json (fetched automatically if not found)

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./student_intake --docroot resources --http-address 0.0.0.0 --http-port 8080
```

Then open `http://localhost:8080` in your web browser.

## Configuration

### Form Types Configuration (`config/forms_config.json`)

Configure which forms are required based on student attributes:

```json
{
  "formTypes": [
    {
      "id": "personal_info",
      "name": "Personal Information",
      "isRequired": true,
      "requiredForStudentTypes": [],
      "requiredForInternational": false,
      "requiredForTransfer": false
    }
  ]
}
```

### Curriculum Configuration (`config/curriculum_config.json`)

Define available programs and their required forms:

```json
{
  "curriculums": [
    {
      "id": "cs_bs",
      "name": "Bachelor of Science in Computer Science",
      "description": "Comprehensive program covering software development...",
      "department": "Computer Science",
      "creditHours": 120,
      "requiredForms": ["personal_info", "emergency_contact", "academic_history", "consent"]
    }
  ]
}
```

## API Integration

The application is designed to work with ApiLogicServer backend. Configure the API base URL in `AppConfig.h`:

```cpp
std::string apiBaseUrl = "http://localhost:5656/api";
```

### API Endpoints

The application expects the following API endpoints (note: capitalized resource names for ApiLogicServer/SAFRS):

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/Student` | POST | Student registration |
| `/Student?filter[email]=` | GET | Login (find by email) |
| `/Student/:id` | PATCH | Update student profile |
| `/Curriculum` | GET | List available programs |
| `/FormType` | GET | List form types |
| `/StudentAddress` | GET/POST | List/create student addresses |
| `/StudentAddress/:id` | PATCH/DELETE | Update/delete student address |
| `/EmergencyContact` | GET/POST | List/create emergency contacts |
| `/EmergencyContact/:id` | PATCH/DELETE | Update/delete emergency contact |
| `/MedicalInfo` | POST | Submit medical info |
| `/AcademicHistory` | GET/POST | List/create academic history |
| `/AcademicHistory/:id` | PATCH/DELETE | Update/delete academic history |
| `/FinancialAid` | GET/POST/PATCH | Get/create/update financial aid |
| `/Document` | POST | Submit documents |
| `/Consent` | POST | Submit consent form |

### JSON:API Format

The API uses JSON:API format with nested attributes:

```json
{
  "data": {
    "type": "Student",
    "attributes": {
      "first_name": "John",
      "last_name": "Doe",
      "email": "john@example.com"
    }
  }
}
```

### Student Data Fields

The Student model supports these fields (all use snake_case in API):

| Field | Type | Description |
|-------|------|-------------|
| `id` | int/string | Student ID |
| `email` | string | Email address |
| `first_name` | string | First name |
| `middle_name` | string | Middle name |
| `last_name` | string | Last name |
| `preferred_name` | string | Preferred/nickname |
| `date_of_birth` | date | Date of birth (YYYY-MM-DD) |
| `gender` | string | Gender |
| `preferred_pronouns` | string | Preferred pronouns |
| `phone_number` | string | Primary phone |
| `alternate_phone` | string | Secondary phone |
| `ssn` | string | Social Security Number |
| `citizenship_status` | string | Citizenship status |
| `curriculum_id` | int | Selected program ID |
| `student_type` | string | undergraduate/graduate/doctoral/certificate |
| `is_international` | boolean | International student flag |
| `is_transfer_student` | boolean | Transfer student flag |
| `is_veteran` | boolean | Veteran status |
| `requires_financial_aid` | boolean | Needs financial aid |
| `completed_forms` | array | List of completed form IDs |
| `enrollment_date` | date | Enrollment date |

### Student Address Data Fields

Student addresses are stored in a separate `student_address` table (not on the student table directly). This allows multiple addresses per student (permanent, mailing, billing).

| Field | Type | Description |
|-------|------|-------------|
| `id` | int | Address ID |
| `student_id` | int | Foreign key to student |
| `address_type` | string | Type: 'permanent', 'mailing', 'billing' |
| `street1` | string | Street address line 1 |
| `street2` | string | Street address line 2 (apt/suite) |
| `city` | string | City |
| `state` | string | State/Province |
| `postal_code` | string | ZIP/Postal code |
| `country` | string | Country |
| `is_primary` | boolean | Primary address flag |

### Returning Student Flow

When a student logs in again:

1. API returns full student record including `completed_forms` and `curriculum_id`
2. Application loads previously selected curriculum from `curriculum_id`
3. Dashboard calculates which forms are required for that curriculum
4. If all required forms are in `completed_forms`, shows completion view
5. Otherwise, student continues from where they left off
6. Forms view pre-fills all fields from student data and related records

**Technical Notes:**
- `completed_forms` is stored as a JSON-encoded string in the database TEXT column
- Example: `["personal_info","emergency_contact","academic_history"]`
- The application serializes/deserializes this automatically
- Related records (EmergencyContact, AcademicHistory, FinancialAid) are loaded via separate API calls

## Form Types

### Core Onboarding Forms (Required)

1. **Personal Information** - Basic personal details, contact info, address
2. **Emergency Contacts** - Emergency contact information (supports multiple contacts)
   - **Relationship Limits**: Spouse (max 1), Parent (max 2), Grandparent (max 4), others unlimited
   - **API Integration**: Remove button deletes contact from database immediately
   - **Up to 10 contacts** can be added per student
3. **Academic History** - High school and previous college education
4. **Consent** - Terms of service, privacy policy, FERPA acknowledgment

### Optional/Conditional Forms

5. **Medical Information** - Health insurance, conditions, allergies, medications
6. **Financial Aid** - FAFSA status, income information, aid preferences
7. **Document Upload** - Upload required documents (ID, transcripts, etc.)

## UI Components

### Header/Navigation
- Company logo (48px) with "Student Onboarding" branding
- Clickable logo and title return to dashboard
- User welcome message and navigation buttons (Help, Profile, Logout)

### Advertisement Banner
- Full-width promotional banner at top of dashboard
- Blue gradient background (`#1e40af` to `#3b82f6`)
- "Featured" badge with semi-transparent white background
- Promotional text with business name and offer
- Yellow call-to-action link (`#fbbf24`) for high visibility
- Responsive: stacks vertically on mobile screens
- Easily customizable content in `DashboardWidget.cpp`

```
┌──────────────────────────────────────────────────────────────┐
│  [Featured]  Campus Bookstore — Get 15% off!  Shop Now →     │
└──────────────────────────────────────────────────────────────┘
     ↑              ↑                               ↑
   Badge     Business promo text            Yellow CTA link
```

### Dashboard Widget
- Advertisement banner at top for promotions
- Two-column responsive layout (after completion)
- Main content area with program info and progress
- Right-hand sidebar panel (light blue background) for completed/recommended forms
- Sidebar positioned flush to right edge of screen
- Sidebar visible only after onboarding completion
- Responsive: sidebar moves below main content on mobile

### Forms View
- Program header with gradient background showing selected curriculum
- Progress sidebar with clickable steps
- Form navigation with Previous/Next buttons
- **Data Pre-fill**: All fields automatically populated from student record

### Footer
- Black background with white text
- Copyright notice: "© 2026 Imagery Business Systems LLC. All rights reserved."
- Sticky footer at bottom of page

### Styling
Custom CSS with design tokens:
- Primary color: `#2563eb` (blue)
- Success color: `#22c55e` (green)
- Sidebar color: `#e8f4fc` (light blue)
- Footer color: `#1a1a1a` (dark gray/black)
- Responsive breakpoints for mobile support

## Future Enhancements

- PDF form export
- Print-friendly layouts
- Document attachments
- Email notifications
- Real-time application status tracking

## Documentation

- `README.md` - This overview document
- `WORKFLOW.md` - Detailed workflow and state documentation
- `ARCHITECTURE_ANALYSIS.md` - Technical architecture and module details

## Recent Changes

### Version 1.9.0
- **Advertisement Banner**: Replaced redundant welcome message with promotional banner
  - Full-width banner at top of dashboard for business advertisements
  - Blue gradient background with "Featured" badge
  - Sample Campus Bookstore promotion with 15% off textbooks offer
  - Yellow call-to-action link for high visibility
  - Responsive design: stacks vertically on mobile devices
  - Removed duplicate "Welcome, <name>" message (already shown in navbar)
  - Easily customizable by editing `DashboardWidget.cpp`

### Version 1.8.0
- **Program Selection Card Layout**: Redesigned program selection page with modern card-based UI
  - 3-column responsive grid layout (adapts to 2 columns on tablet, 1 on mobile)
  - Each program displayed as a styled card with white background and subtle shadow
  - Card sections: header (name/department), body (description), meta (badges), footer (buttons)
  - Info button (ℹ) opens syllabus popup dialog with detailed program information
  - Select button immediately navigates to forms (removed separate Continue button)
  - Inline styles applied directly to Wt widgets to ensure consistent rendering
  - Filter dropdowns for department and degree type
- **Syllabus Dialog Styling**: Modal popup with professional appearance
  - Modal dialog blocks interaction with background content
  - Dimmed backdrop overlay (50% opacity) when dialog is visible
  - Blue title bar with white text
  - Styled content sections: program details, description, onboarding requirements
  - Rounded corners and drop shadow for polished look
  - Gray "Close" button in footer

### Version 1.7.0
- **Change Program Feature**: Students can now change their selected program from the dashboard
  - "Change Program" button added to the curriculum section
  - When clicked, resets all completed forms progress
  - Clears curriculum selection and navigates to program selection
  - Changes persisted to API immediately
  - Students must complete all forms again with the new program

### Version 1.6.0
- **Emergency Contact Relationship Limits**: Added validation to enforce maximum contacts per relationship type:
  - Spouse: maximum 1
  - Parent: maximum 2
  - Grandparent: maximum 4
  - Other relationships: no limit
- **Emergency Contact API Deletion**: Remove button now deletes records from database via API
- **Increased Contact Limit**: Support for up to 10 emergency contacts (was 5)

### Version 1.5.0
- **Session Persistence Fix**: Fixed returning student workflow
  - `completed_forms` now properly serialized as JSON string for TEXT column storage
  - `curriculum_id` correctly loaded on login to restore program selection
  - Students now return to dashboard with progress intact
- **Debug Logging**: Added tracing for login and profile update API calls

### Version 1.4.0
- **Academic History Multi-Record Support**: Students can now add multiple education records
  - Separate high school and college/university entries
  - Records identified by `institution_type` field
  - Full CRUD operations via API

### Version 1.3.0
- **Financial Aid Form Fix**: Proper field mapping to database columns
  - Handles UNIQUE constraint on student_id (upsert logic)
  - Check for existing record before POST

### Version 1.2.0
- **Emergency Contact Model**: Added EmergencyContact model with full API integration
- **Data Pre-fill**: Forms automatically populate from existing records

## License

This project is provided as-is for educational and demonstration purposes.
