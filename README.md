# Student Onboarding Application

A modular C++ web application built with the Wt (Witty) Web Toolkit for processing student onboarding forms during the enrollment process.

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

## Application Workflow

### 1. Authentication
- New users register with name, email, and password
- Returning users log in to continue their application
- Session persists user progress

### 2. Program Selection
- Students browse available academic programs
- Programs display name, description, department, and credit hours
- Selected program determines required onboarding forms

### 3. Dashboard
The dashboard provides a central hub for students:

**During Onboarding:**
- Shows selected program information
- Displays progress bar with completion percentage
- "Continue Application" button to resume form entry

**After Completion:**
- Two-column layout with main content and sidebar panel
- Completion confirmation message
- Right-hand panel displays:
  - **Completed Forms**: List of submitted forms with eye icon (👁) to view/edit
  - **Recommended Forms**: Optional additional forms with arrow (→) to start

```
┌─────────────────────────────────────────────────────────┐
│               Welcome, Student Name                      │
├─────────────────────────────┬───────────────────────────┤
│                             │  Completed Forms          │
│  Selected Program           │  ✓ Personal Information 👁│
│  Bachelor of Science...     │  ✓ Emergency Contacts   👁│
│                             │  ✓ Academic History     👁│
│  ┌─────────────────────┐    │  ✓ Terms and Consent    👁│
│  │ Onboarding Complete!│    │                           │
│  │ Thank you for...    │    │  Recommended Forms        │
│  └─────────────────────┘    │  + Medical Information  → │
│                             │  + Financial Aid        → │
│  Need Help?                 │                           │
│  Contact admissions...      │                           │
└─────────────────────────────┴───────────────────────────┘
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
| `/EmergencyContact` | POST | Submit emergency contact |
| `/MedicalInfo` | POST | Submit medical info |
| `/AcademicHistory` | POST | Submit academic history |
| `/FinancialAid` | POST | Submit financial aid |
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

## Form Types

### Core Onboarding Forms (Required)

1. **Personal Information** - Basic personal details, contact info, address
2. **Emergency Contacts** - Emergency contact information (supports multiple contacts)
3. **Academic History** - High school and previous college education
4. **Consent** - Terms of service, privacy policy, FERPA acknowledgment

### Optional/Conditional Forms

5. **Medical Information** - Health insurance, conditions, allergies, medications
6. **Financial Aid** - FAFSA status, income information, aid preferences
7. **Document Upload** - Upload required documents (ID, transcripts, etc.)

## UI Components

### Dashboard Widget
- Two-column responsive layout
- Main content area with program info and progress
- Collapsible sidebar for completed/recommended forms
- Sticky sidebar on desktop, stacked on mobile

### Forms View
- Program header with gradient background showing selected curriculum
- Progress sidebar with clickable steps
- Form navigation with Previous/Next buttons

### Styling
Custom CSS with design tokens:
- Primary color: `#2563eb` (blue)
- Success color: `#22c55e` (green)
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

## License

This project is provided as-is for educational and demonstration purposes.
