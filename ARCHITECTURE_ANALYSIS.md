# Student Onboarding - Architecture Analysis

> Generated: 2026-01-12

## Project Overview

- **Language**: C++17
- **Framework**: Wt (Witty) Web Toolkit
- **Build System**: CMake 3.16+
- **Codebase Size**: ~8,750 lines of C++ code
- **Purpose**: Multi-user student intake forms web application with backend API integration

---

## Directory Structure

```
Student-Onboarding/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Documentation
├── src/                        # Main source code (organized by modules)
│   ├── main.cpp                # Server entry point
│   ├── app/                    # Application core
│   │   ├── StudentIntakeApp.h/cpp
│   │   └── AppConfig.h
│   ├── auth/                   # Authentication module
│   │   ├── AuthManager.h/cpp
│   │   ├── LoginWidget.h/cpp
│   │   └── RegisterWidget.h/cpp
│   ├── forms/                  # Form definitions & factory
│   │   ├── BaseForm.h/cpp
│   │   ├── FormFactory.h/cpp
│   │   ├── PersonalInfoForm.h/cpp
│   │   ├── EmergencyContactForm.h/cpp
│   │   ├── MedicalInfoForm.h/cpp
│   │   ├── AcademicHistoryForm.h/cpp
│   │   ├── FinancialAidForm.h/cpp
│   │   ├── DocumentUploadForm.h/cpp
│   │   ├── ConsentForm.h/cpp
│   │   └── InternationalStudentForm.h/cpp
│   ├── models/                 # Data models
│   │   ├── Student.h/cpp
│   │   ├── FormData.h/cpp
│   │   ├── Curriculum.h/cpp
│   │   └── FormTypeInfo.h/cpp
│   ├── session/                # Session management
│   │   ├── StudentSession.h/cpp
│   │   └── SessionManager.h/cpp
│   ├── api/                    # API client & services
│   │   ├── ApiClient.h/cpp
│   │   └── FormSubmissionService.h/cpp
│   ├── curriculum/             # Curriculum management
│   │   ├── CurriculumManager.h/cpp
│   │   └── CurriculumSelector.h/cpp
│   └── widgets/                # UI components
│       ├── NavigationWidget.h/cpp
│       ├── ProgressWidget.h/cpp
│       ├── FormContainer.h/cpp
│       └── DashboardWidget.h/cpp
├── config/                     # Configuration files
│   ├── forms_config.json       # Form types configuration
│   └── curriculum_config.json  # Curriculum definitions
├── database/                   # Database schemas/migrations
└── resources/                  # Client resources
    └── styles.css              # Application styling
```

---

## Module Architecture

### Module Hierarchy Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                   StudentIntakeApp (Main)                    │
│         Orchestrates all modules and state management        │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  Service Layer (Shared Services for Dependency Injection)    │
├─────────────────────────────────────────────────────────────┤
│ - ApiClient          (HTTP communication with backend)       │
│ - FormSubmissionService (Form submission logic)              │
│ - AuthManager        (User authentication)                   │
│ - CurriculumManager  (Program data management)               │
│ - FormFactory        (Form creation factory)                 │
│ - StudentSession     (Session state tracking)                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  Presentation Layer (UI Components & Widgets)                │
├─────────────────────────────────────────────────────────────┤
│ - NavigationWidget   - ProgressWidget   - FormContainer      │
│ - DashboardWidget    - LoginWidget      - RegisterWidget     │
│ - CurriculumSelector - BaseForm + 8 specialized forms        │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│  Data Layer (Models & Session Management)                    │
├─────────────────────────────────────────────────────────────┤
│ - Student       (Student entity with attributes)             │
│ - FormData      (Form submission data structure)             │
│ - Curriculum    (Program definition)                         │
│ - FormTypeInfo  (Form metadata)                              │
└─────────────────────────────────────────────────────────────┘
```

### Module Responsibilities

| Module | Location | Responsibility |
|--------|----------|----------------|
| **app** | `src/app/` | Main orchestrator, state machine (6 states), configuration singleton |
| **auth** | `src/auth/` | Login, registration, token management, password validation |
| **forms** | `src/forms/` | 8 form types + BaseForm + FormFactory |
| **models** | `src/models/` | Data structures with JSON serialization |
| **session** | `src/session/` | Per-student state, progress tracking, form caching |
| **api** | `src/api/` | HTTP client (libcurl), high-level API abstraction |
| **curriculum** | `src/curriculum/` | Program data loading, filtering by department/degree |
| **widgets** | `src/widgets/` | Reusable UI components |

---

## Application State Machine

```
Login → Register → CurriculumSelection → Dashboard → Forms → Completion
```

---

## Design Patterns Used

| Pattern | Location | Purpose |
|---------|----------|---------|
| **State Machine** | StudentIntakeApp | Manage app states |
| **Factory** | FormFactory | Create form instances on demand |
| **Dependency Injection** | All services | Loose coupling of components |
| **Singleton** | AppConfig, SessionManager | Global access to shared resources |
| **Template Method** | BaseForm | Define form processing skeleton |
| **Service Layer** | ApiClient, FormSubmissionService | Abstract infrastructure concerns |
| **Observer/Signal-Slot** | Wt signals in widgets | Asynchronous event communication |
| **Registry** | FormFactory | Track available form types |

---

## API Endpoint Analysis

### Backend Base URL
```
http://localhost:5656/api
```

### Available Backend Endpoints

| Endpoint | Description |
|----------|-------------|
| AcademicHistory | Academic history records |
| AuditLog | Activity logging |
| Consent | Terms and consent records |
| Curriculum | Program/curriculum data |
| CurriculumFormRequirement | Form requirements per curriculum |
| CurriculumPrerequisite | Course prerequisites |
| Department | Department data |
| Document | Document uploads |
| EmergencyContact | Emergency contact info |
| FinancialAid | Financial aid applications |
| FormField | Form field definitions |
| FormSubmission | Form submission tracking |
| FormType | Form type definitions |
| IntakeSession | Session management |
| MedicalInfo | Medical information |
| Student | Student records |
| StudentAddress | Student addresses |
| TestScore | Standardized test scores |
| auth | Authentication endpoints |

### Currently Used by Application

| Backend Endpoint | App Endpoint | Usage |
|-----------------|--------------|-------|
| Student | `/student` | Registration, profile, personal info |
| EmergencyContact | `/emergency_contact` | Emergency contact form |
| MedicalInfo | `/medical_info` | Medical information form |
| AcademicHistory | `/academic_history` | Academic history form |
| FinancialAid | `/financial_aid` | Financial aid form |
| Document | `/document` | Document upload form |
| Consent | `/consent` | Terms and consent form |
| Curriculum | `/curriculum` | Program/curriculum selection |
| FormType | `/form_type` | Form type configuration |
| FormSubmission | `/form_submission` | Tracking completed forms |
| auth | `/auth/login` | Authentication |

### Backend Endpoints NOT Currently Used

| Endpoint | Potential Use |
|----------|--------------|
| **AuditLog** | Activity logging/compliance tracking |
| **CurriculumFormRequirement** | Dynamic form requirements per program (currently in JSON config) |
| **CurriculumPrerequisite** | Course/program prerequisites |
| **Department** | Department data for curriculum grouping |
| **FormField** | Dynamic form field definitions |
| **IntakeSession** | Server-side session tracking |
| **StudentAddress** | Separate address storage (currently part of personal info) |
| **TestScore** | SAT/ACT/GRE test scores |

---

## Form Type Mapping

Current mapping in `FormSubmissionService.cpp`:

```cpp
formEndpoints_["personal_info"] = "/student";
formEndpoints_["emergency_contact"] = "/emergency_contact";
formEndpoints_["medical_info"] = "/medical_info";
formEndpoints_["academic_history"] = "/academic_history";
formEndpoints_["financial_aid"] = "/financial_aid";
formEndpoints_["documents"] = "/document";
formEndpoints_["consent"] = "/consent";
```

---

## Configuration Files

### forms_config.json
Defines 7 form types with conditional requirements based on:
- Student type (undergraduate, graduate, doctoral, certificate, non-degree)
- Curriculum (specific programs require specific forms)
- International status
- Transfer status
- Veteran status
- Financial aid needs

### curriculum_config.json
Defines 10 programs across 5 departments:
- Each program specifies its required forms
- Supports degree types and prerequisites

---

## External Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| Wt | 4.x | Web framework and UI components |
| libcurl | - | HTTP requests |
| nlohmann_json | 3.11.3 | JSON serialization (auto-fetched) |
| C++ Standard | 17 | Language features |

---

## Recommendations for Enhancement

1. **Use `IntakeSession` endpoint** for server-side session persistence instead of client-only session
2. **Use `CurriculumFormRequirement` endpoint** to fetch form requirements dynamically instead of local JSON config
3. **Use `StudentAddress` endpoint** as a separate entity for better data normalization
4. **Add `TestScore` form** for standardized test submissions (SAT, ACT, GRE)
5. **Integrate `AuditLog` endpoint** for compliance and activity tracking
6. **Use `Department` endpoint** to dynamically load department data for curriculum filtering
7. **Use `FormField` endpoint** for dynamic form field definitions (more flexible than hardcoded forms)

---

## Notes

- The application uses JSON:API format for requests with `data.type` and `data.attributes` structure
- All HTTP communication goes through `ApiClient` with libcurl
- Forms inherit from `BaseForm` which provides common validation and submission logic
- The app supports both synchronous and asynchronous API calls
