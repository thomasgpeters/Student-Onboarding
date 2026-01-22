# Logging Framework

The Student Onboarding application includes a centralized logging framework that provides configurable log levels for controlling output verbosity.

## Log Levels

| Level | Value | Description |
|-------|-------|-------------|
| `NONE` | 0 | No logging output |
| `ERROR` | 1 | Only error messages |
| `WARN` | 2 | Errors and warnings |
| `INFO` | 3 | Errors, warnings, and informational messages (default) |
| `DEBUG` | 4 | All messages including detailed debug output |

## Configuration

### Environment Variable

Set the `LOG_LEVEL` environment variable before starting the application:

```bash
# Maximum output (all debug messages)
LOG_LEVEL=DEBUG ./student_intake --docroot resources --http-port 8080

# Normal operation (default)
LOG_LEVEL=INFO ./student_intake --docroot resources --http-port 8080

# Warnings and errors only
LOG_LEVEL=WARN ./student_intake --docroot resources --http-port 8080

# Errors only
LOG_LEVEL=ERROR ./student_intake --docroot resources --http-port 8080

# Suppress all logging
LOG_LEVEL=NONE ./student_intake --docroot resources --http-port 8080
```

### Programmatic Configuration

```cpp
#include "utils/Logger.h"

// Set level directly
StudentIntake::Logger::setLevel(StudentIntake::LogLevel::DEBUG);

// Set level from string
StudentIntake::Logger::setLevelFromString("DEBUG");

// Get current level
StudentIntake::LogLevel level = StudentIntake::Logger::getLevel();
std::string levelStr = StudentIntake::Logger::getLevelString();

// Check if a level should be logged
if (StudentIntake::Logger::shouldLog(StudentIntake::LogLevel::DEBUG)) {
    // Perform expensive debug operation
}

// Optional: Show timestamps
StudentIntake::Logger::setShowTimestamp(true);

// Optional: Show/hide level prefix (default: true)
StudentIntake::Logger::setShowLevel(true);
```

## Usage

### Logging Macros

The framework provides four convenience macros with stream syntax support:

```cpp
#include "utils/Logger.h"

// Debug messages - detailed debugging info
LOG_DEBUG("ComponentName", "Processing item: " << itemId);

// Info messages - important state changes
LOG_INFO("ComponentName", "User logged in successfully");

// Warning messages - non-fatal issues
LOG_WARN("ComponentName", "Configuration file not found, using defaults");

// Error messages - errors and failures
LOG_ERROR("ComponentName", "Failed to connect to database: " << errorMsg);
```

### Component Names

Use consistent component names that match the class or module:

| Component | Used In |
|-----------|---------|
| `Main` | main.cpp |
| `StudentIntakeApp` | StudentIntakeApp.cpp |
| `AuthManager` | AuthManager.cpp |
| `RegisterWidget` | RegisterWidget.cpp |
| `ApiClient` | ApiClient.cpp |
| `FormSubmissionService` | FormSubmissionService.cpp |
| `PdfGenerator` | PdfGenerator.cpp |
| `Student::fromJson` | Student.cpp |
| `AcademicHistoryForm` | AcademicHistoryForm.cpp |
| `ConsentForm` | ConsentForm.cpp |
| `EmergencyContactForm` | EmergencyContactForm.cpp |
| `AdminApp` | AdminApp.cpp |
| `AdminAuth` | AdminAuthManager.cpp |
| `AdminDashboard` | AdminDashboard.cpp |
| `FormPdfPreviewWidget` | FormPdfPreviewWidget.cpp |
| `FormSubmissionsWidget` | FormSubmissionsWidget.cpp |
| `FormDetailViewer` | FormDetailViewer.cpp |
| `StudentList` | StudentListWidget.cpp |
| `StudentDetail` | StudentDetailWidget.cpp |
| `FormViewer` | StudentFormViewer.cpp |
| `CurriculumListWidget` | CurriculumListWidget.cpp |
| `CurriculumEditorWidget` | CurriculumEditorWidget.cpp |
| `InstitutionSettings` | InstitutionSettingsWidget.cpp |

## Output Format

Log messages are formatted as:

```
[LEVEL] [Component] Message
```

Example output:
```
[INFO]  [Main] Starting Student Onboarding Application...
[INFO]  [Main]   Log level: INFO
[DEBUG] [ApiClient] POST http://localhost:5656/api/Student
[DEBUG] [ApiClient] Request body: {"data":{"type":"Student",...}}
[INFO]  [AuthManager] Login result - success: 1, student ID: '42'
[WARN]  [StudentList] API service not available
[ERROR] [FormSubmissionService] Failed to submit form: Connection refused
```

With timestamps enabled (`Logger::setShowTimestamp(true)`):
```
2024-01-15 10:30:45.123 [INFO]  [Main] Starting Student Onboarding Application...
```

## Log Level Guidelines

### DEBUG
Use for detailed debugging information that helps trace execution flow:
- API request/response bodies
- Parsing details and intermediate values
- Form field values
- Cache operations
- Record counts and IDs

### INFO
Use for important state changes and milestones:
- Application startup/shutdown
- User login/logout
- Form submissions completed
- Records created/updated/deleted successfully
- Settings loaded/saved

### WARN
Use for non-fatal issues that don't prevent operation:
- API service not available (using fallback)
- Configuration not found (using defaults)
- Unexpected but handled response formats
- Failed to delete/update a record (operation continues)

### ERROR
Use for errors and failures that indicate a problem:
- Failed API calls
- Exception handling
- Failed to initialize resources
- Critical operations that failed

## Architecture

### Header-Only Implementation

The Logger is implemented as a header-only utility in `src/utils/Logger.h`:

```
src/
├── utils/
│   └── Logger.h          # Logger class and macros
```

### Thread Safety

The Logger uses a mutex to ensure thread-safe output when multiple threads log concurrently.

### Automatic Flushing

All log messages are automatically flushed to ensure they appear immediately, which is important for debugging crashes.

## Adding Logging to New Code

1. Include the Logger header:
   ```cpp
   #include "utils/Logger.h"
   ```

2. Use the appropriate macro with a component name:
   ```cpp
   LOG_DEBUG("MyNewComponent", "Initializing with value: " << value);
   ```

3. Choose the right log level based on the guidelines above.

## Migration from std::cout/std::cerr

The logging framework replaced direct `std::cout` and `std::cerr` usage throughout the codebase. The migration pattern was:

| Before | After |
|--------|-------|
| `std::cout << "[Component] message" << std::endl;` | `LOG_DEBUG("Component", "message");` |
| `std::cerr << "[Component] error" << std::endl;` | `LOG_ERROR("Component", "error");` |
| `std::cout.flush();` | (removed - automatic) |

## Files Updated

The logging framework was applied to 27 files across the codebase:

- **models**: Student.cpp
- **api**: ApiClient.cpp, FormSubmissionService.cpp, PdfGenerator.cpp
- **auth**: AuthManager.cpp, RegisterWidget.cpp
- **forms**: AcademicHistoryForm.cpp, ConsentForm.cpp, EmergencyContactForm.cpp
- **app**: StudentIntakeApp.cpp
- **admin**: AdminApp.cpp, AdminAuthManager.cpp, AdminDashboard.cpp
- **admin/forms**: FormPdfPreviewWidget.cpp, FormSubmissionsWidget.cpp, FormDetailViewer.cpp, FormTypesListWidget.cpp, FormTypeDetailWidget.cpp
- **admin/students**: StudentListWidget.cpp, StudentDetailWidget.cpp, StudentFormViewer.cpp
- **admin/curriculum**: CurriculumListWidget.cpp, CurriculumEditorWidget.cpp
- **admin/settings**: InstitutionSettingsWidget.cpp
- **main.cpp**: Application entry point
