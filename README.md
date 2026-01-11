# Student Intake Forms Application

A modular C++ web application built with the Wt (Witty) Web Toolkit for processing student intake forms during the enrollment process.

## Features

- **Multi-user Support**: Multiple students can simultaneously process their intake forms
- **Modular Form Design**: Each form type is implemented as a separate, reusable module
- **Configurable Required Forms**: Forms required for each student are determined based on:
  - Student type (undergraduate, graduate, doctoral, certificate)
  - Selected curriculum/program
  - Special status (international, transfer, veteran, financial aid)
- **API Integration**: Sends form data to backend via RESTful API (designed for ApiLogicServer)
- **Session Management**: Tracks user progress across multiple forms
- **Progress Tracking**: Visual progress indicator showing completion status

## Project Structure

```
Student-Onboarding/
├── CMakeLists.txt              # Build configuration
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
      "requiredForms": ["personal_info", "emergency_contact", "academic_history"]
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

The application expects the following API endpoints:

- `POST /auth/login` - User authentication
- `POST /Student` - Student registration
- `GET /Curriculum` - List curriculums
- `GET /FormType` - List form types
- `POST /PersonalInfo` - Submit personal info form
- `POST /EmergencyContact` - Submit emergency contact form
- `POST /MedicalInfo` - Submit medical info form
- `POST /AcademicHistory` - Submit academic history form
- `POST /FinancialAid` - Submit financial aid form
- `POST /Documents` - Submit documents
- `POST /Consent` - Submit consent form

## Form Types

1. **Personal Information** - Basic personal details, contact info, address
2. **Emergency Contacts** - Emergency contact information (supports multiple contacts)
3. **Medical Information** - Health insurance, conditions, allergies, medications
4. **Academic History** - High school and previous college education
5. **Financial Aid** - FAFSA status, income information, aid preferences
6. **Document Upload** - Upload required documents (ID, transcripts, etc.)
7. **Consent** - Terms of service, privacy policy, FERPA acknowledgment

## License

This project is provided as-is for educational and demonstration purposes.
