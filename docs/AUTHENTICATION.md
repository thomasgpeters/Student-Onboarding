# Unified Authentication Feature

The Unified Authentication feature provides a single, consolidated login system for all user types (students, instructors, and administrators) with role-based access control and routing.

## Overview

The authentication system enables:
- Single login point at "/" for all user types
- Role-based access control (RBAC)
- Multi-role support for administrators
- Automatic routing based on user's primary role
- Role switching for users with multiple roles
- Secure session management with tokens
- Login audit trail for security monitoring

## Architecture

### Components

```
src/auth/
├── AuthService.h/cpp           # Unified authentication service
├── UnifiedLoginWidget.h/cpp    # Single login form at "/"
├── AuthManager.h/cpp           # Legacy auth manager (backwards compatibility)
├── LoginWidget.h/cpp           # Legacy login widget
└── RegisterWidget.h/cpp        # Student registration

src/models/
└── User.h/cpp                  # User model with role support

src/widgets/
└── RoleNavigationWidget.h/cpp  # Role switching for multi-role users

database/migrations/
└── 015_unified_user_authentication.sql  # Database schema
```

## User Roles

The system supports three user roles:

| Role | Description | Access |
|------|-------------|--------|
| `student` | Students enrolled in programs | Student dashboard, forms, classroom |
| `instructor` | CDL instructors and examiners | Instructor portal |
| `admin` | System administrators | Administration dashboard |

### Role Assignment Rules

- **Students**: Single role only (`student`)
- **Instructors**: Single role only (`instructor`)
- **Administrators**: Can have multiple roles (`admin` + `instructor`, or `admin` + `student`)

## Database Schema

### Tables

| Table | Purpose |
|-------|---------|
| `app_user` | Unified user table for all authentication |
| `user_roles` | Junction table for user-role assignments |
| `student_profile` | Extended student-specific data |
| `instructor_profile` | Extended instructor-specific data |
| `admin_profile` | Extended admin-specific data |
| `user_session` | Session token management |
| `login_audit` | Security audit trail |

### app_user Table

```sql
CREATE TABLE app_user (
    id SERIAL PRIMARY KEY,
    email VARCHAR(255) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    middle_name VARCHAR(100),
    preferred_name VARCHAR(100),
    phone_number VARCHAR(20),

    -- Account status
    is_active BOOLEAN DEFAULT true,
    login_enabled BOOLEAN DEFAULT true,
    email_verified BOOLEAN DEFAULT false,

    -- Authentication tracking
    last_login_at TIMESTAMP WITH TIME ZONE,
    failed_login_attempts INTEGER DEFAULT 0,
    locked_until TIMESTAMP WITH TIME ZONE,
    password_changed_at TIMESTAMP WITH TIME ZONE,

    -- Timestamps
    created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);
```

### user_roles Table

```sql
CREATE TABLE user_roles (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES app_user(id),
    role user_role NOT NULL,
    granted_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
    granted_by INTEGER REFERENCES app_user(id),
    is_active BOOLEAN DEFAULT true,
    UNIQUE(user_id, role)
);
```

### Helper Functions

```sql
-- Check if user has a specific role
CREATE FUNCTION user_has_role(p_user_id INTEGER, p_role user_role)
RETURNS BOOLEAN;

-- Get user's primary role (priority: admin > instructor > student)
CREATE FUNCTION get_user_primary_role(p_user_id INTEGER)
RETURNS user_role;

-- Get all active roles for a user
CREATE FUNCTION get_user_roles(p_user_id INTEGER)
RETURNS TABLE(role user_role);
```

### Convenience View

```sql
CREATE VIEW user_with_roles AS
SELECT
    u.*,
    get_user_primary_role(u.id) as primary_role,
    (SELECT COUNT(*) FROM user_roles WHERE user_id = u.id AND is_active = true) as role_count
FROM app_user u;
```

## Data Models

### User Class

```cpp
class User {
public:
    // Profile
    int id_;
    std::string email_;
    std::string passwordHash_;
    std::string firstName_;
    std::string lastName_;
    std::string middleName_;
    std::string preferredName_;
    std::string phoneNumber_;

    // Account status
    bool isActive_;
    bool loginEnabled_;
    bool emailVerified_;

    // Authentication tracking
    std::string lastLoginAt_;
    int failedLoginAttempts_;
    std::string lockedUntil_;

    // Roles
    std::vector<UserRole> roles_;

    // Role methods
    UserRole getPrimaryRole() const;
    bool hasRole(UserRole role) const;
    bool hasMultipleRoles() const;
    bool isStudent() const;
    bool isInstructor() const;
    bool isAdmin() const;

    // Utility methods
    std::string getFullName() const;
    std::string getDisplayName() const;
    bool isLocked() const;
    bool canLogin() const;
    std::string getHomeRoute() const;
};
```

### UserRole Enum

```cpp
enum class UserRole {
    Student,
    Instructor,
    Admin
};
```

### Role Priority

When determining the primary role for users with multiple roles:

```cpp
UserRole User::getPrimaryRole() const {
    // Priority: Admin > Instructor > Student
    if (hasRole(UserRole::Admin)) return UserRole::Admin;
    if (hasRole(UserRole::Instructor)) return UserRole::Instructor;
    return UserRole::Student;
}
```

### UserSession Class

```cpp
class UserSession {
    int id_;
    int userId_;
    std::string sessionToken_;
    std::string refreshToken_;
    std::string ipAddress_;
    std::string userAgent_;
    std::string deviceInfo_;
    UserRole activeRole_;      // Current active role for multi-role users
    std::string createdAt_;
    std::string expiresAt_;
    std::string lastActivityAt_;
    bool isActive_;
    std::string revokedAt_;
    std::string revokedReason_;
};
```

## Authentication Flow

### Login Flow

```
1. User visits "/" (root endpoint)
2. UnifiedLoginWidget displays login form
3. User enters email and password
4. AuthService.login() called
   - Validates credentials
   - Checks account status (active, not locked)
   - Retrieves user roles
   - Creates session token
   - Records login audit
5. On success, loginSuccess signal emits User object
6. StudentIntakeApp.handleUnifiedLoginSuccess() called
7. routeUserByRole() determines destination based on primary role
8. User redirected to appropriate dashboard
```

### Routing Logic

```cpp
void StudentIntakeApp::routeUserByRole(const Models::User& user) {
    // Set up role navigation for multi-role users
    if (user.hasMultipleRoles()) {
        roleNavigationWidget_->setUser(user);
        roleNavigationWidget_->show();
    }

    // Route based on primary role
    switch (user.getPrimaryRole()) {
        case Models::UserRole::Admin:
            setState(AppState::Administration);  // -> /administration
            break;
        case Models::UserRole::Instructor:
            setState(AppState::InstructorDashboard);  // -> /instructor
            break;
        case Models::UserRole::Student:
        default:
            // Follow existing student flow
            if (session_->hasCurriculumSelected()) {
                setState(AppState::Dashboard);  // -> /student
            } else {
                setState(AppState::CurriculumSelection);
            }
            break;
    }
}
```

## UI Components

### UnifiedLoginWidget

The primary login interface at "/" endpoint.

**Structure:**
```
┌─────────────────────────────────────┐
│           Student Intake            │
│       Sign in to your account       │
│                                     │
│  ┌─────────────────────────────┐   │
│  │ Email                        │   │
│  └─────────────────────────────┘   │
│                                     │
│  ┌─────────────────────────────┐   │
│  │ Password                     │   │
│  └─────────────────────────────┘   │
│                                     │
│  ┌─────────────────────────────┐   │
│  │         Sign In              │   │
│  └─────────────────────────────┘   │
│                                     │
│  Forgot password?   Need account?   │
└─────────────────────────────────────┘
```

**Signals:**
```cpp
Wt::Signal<Models::User>& loginSuccess();   // User with roles
Wt::Signal<>& registerRequested();
Wt::Signal<>& forgotPasswordRequested();
```

**Features:**
- Loading indicator during authentication
- Error message display
- Form validation
- Remember me option (future)

### RoleNavigationWidget

Navigation bar for users with multiple roles.

**Display (for Admin+Instructor user):**
```
┌────────────────────────────────────────────────────────────┐
│ Current view: Administrator  |  Switch to: [Instructor Portal]
└────────────────────────────────────────────────────────────┘
```

**Features:**
- Shows only when user has multiple roles
- Displays current active role
- Buttons for switching to other roles
- Color-coded role buttons

**Signals:**
```cpp
Wt::Signal<Models::UserRole>& roleSelected();
```

**Methods:**
```cpp
void setUser(const Models::User& user);
void setActiveRole(Models::UserRole role);
Models::UserRole getActiveRole() const;
bool shouldBeVisible() const;
```

## AuthService API

### Authentication

```cpp
// Login with email and password
AuthResult login(const std::string& email,
                const std::string& password,
                const std::string& ipAddress = "",
                const std::string& userAgent = "");

// Async login
void loginAsync(const std::string& email,
               const std::string& password,
               std::function<void(const AuthResult&)> callback);

// Logout and invalidate session
bool logout(const std::string& sessionToken);

// Validate session token
AuthResult validateSession(const std::string& sessionToken);

// Refresh session token
AuthResult refreshSession(const std::string& refreshToken);
```

### Registration

```cpp
// Register new student
AuthResult registerStudent(const std::string& email,
                          const std::string& password,
                          const std::string& firstName,
                          const std::string& lastName);

// Async registration
void registerStudentAsync(...,
                         std::function<void(const AuthResult&)> callback);
```

### Password Management

```cpp
// Request password reset email
bool requestPasswordReset(const std::string& email);

// Reset password with token
AuthResult resetPassword(const std::string& token,
                        const std::string& newPassword);

// Change password for logged-in user
AuthResult changePassword(int userId,
                         const std::string& currentPassword,
                         const std::string& newPassword);
```

### Role Management

```cpp
// Get all roles for a user
std::vector<Models::UserRole> getUserRoles(int userId);

// Switch active role (for multi-role users)
AuthResult switchRole(const std::string& sessionToken,
                     Models::UserRole newRole);

// Check if user has specific role
bool userHasRole(int userId, Models::UserRole role);

// Get route for user based on primary role
std::string getRouteForUser(const Models::User& user);
```

### Profile Management

```cpp
// Get profiles
Models::StudentProfile getStudentProfile(int userId);
Models::AdminProfile getAdminProfile(int userId);

// Update profiles
AuthResult updateUserProfile(const Models::User& user);
AuthResult updateStudentProfile(int userId, const Models::StudentProfile& profile);
```

### Validation Helpers

```cpp
bool isValidEmail(const std::string& email);
bool isValidPassword(const std::string& password);
std::string getPasswordRequirements();
```

## AuthResult Structure

```cpp
struct AuthResult {
    bool success;
    std::string message;
    std::string sessionToken;
    std::string refreshToken;
    Models::User user;
    std::vector<std::string> errors;
};
```

## Security Features

### Password Security

- Passwords hashed using bcrypt
- Minimum password requirements enforced
- Password history tracking (optional)

### Account Lockout

```cpp
// After 5 failed attempts, account locked for 15 minutes
if (user.failedLoginAttempts_ >= 5) {
    user.lockedUntil_ = now + 15_minutes;
}
```

### Session Management

- 24-hour session expiry
- Refresh token for extending sessions
- Session revocation support
- IP and user agent tracking

### Audit Trail

Every login attempt is recorded:

```sql
INSERT INTO login_audit (
    user_id,
    action,      -- 'login_success', 'login_failed', 'logout', 'password_reset'
    success,
    failure_reason,
    ip_address,
    user_agent,
    created_at
);
```

## CSS Classes

```css
/* Unified Login Widget */
.unified-login-widget        /* Main container */
.login-header               /* Title section */
.login-title                /* Main title */
.login-subtitle             /* Subtitle text */
.login-form                 /* Form container */
.form-group                 /* Input group */
.form-label                 /* Input label */
.form-input                 /* Input field */
.login-button               /* Submit button */
.error-message              /* Error display */
.loading-indicator          /* Loading state */

/* Role Navigation */
.role-navigation            /* Navigation bar */
.role-nav-container         /* Inner container */
.current-role               /* Current role display */
.role-label                 /* "Current view:" label */
.role-name                  /* Role name text */
.role-separator             /* Separator */
.switch-label               /* "Switch to:" label */
.role-buttons               /* Button container */
.role-switch-btn            /* Switch button */
.btn-admin                  /* Admin button style */
.btn-instructor             /* Instructor button style */
.btn-student                /* Student button style */
```

## Integration

### StudentIntakeApp Updates

```cpp
// New members
std::shared_ptr<Auth::AuthService> authService_;
Models::User currentUser_;
Auth::UnifiedLoginWidget* unifiedLoginWidget_;
Widgets::RoleNavigationWidget* roleNavigationWidget_;

// New state
enum class AppState {
    // ...
    Administration,  // Admin main view
    // ...
};

// New handlers
void handleUnifiedLoginSuccess(const Models::User& user);
void handleRoleSwitch(Models::UserRole role);
void routeUserByRole(const Models::User& user);
```

### Service Setup

```cpp
void StudentIntakeApp::setupServices() {
    // ...
    authService_ = std::make_shared<Auth::AuthService>(apiClient_);
    // ...
}
```

### UI Setup

```cpp
void StudentIntakeApp::setupUI() {
    // Role navigation (for multi-role admins)
    roleNavigationWidget_ = mainContainer_->addWidget(
        std::make_unique<Widgets::RoleNavigationWidget>());
    roleNavigationWidget_->roleSelected().connect(
        this, &StudentIntakeApp::handleRoleSwitch);
    roleNavigationWidget_->hide();

    // Unified login widget
    unifiedLoginWidget_ = contentContainer_->addWidget(
        std::make_unique<Auth::UnifiedLoginWidget>());
    unifiedLoginWidget_->setAuthService(authService_);
    unifiedLoginWidget_->loginSuccess().connect(
        this, &StudentIntakeApp::handleUnifiedLoginSuccess);
}
```

## Default Users

The migration creates a default super admin:

| Field | Value |
|-------|-------|
| Email | admin@system.local |
| Password | Admin123! |
| Roles | admin |
| Admin Level | super_admin |

**Important:** Change this password immediately in production!

## Migration from Legacy System

The migration includes a commented script for migrating existing students:

```sql
-- Uncomment and run to migrate existing students
/*
INSERT INTO app_user (email, password_hash, first_name, last_name, ...)
SELECT email, password_hash, first_name, last_name, ...
FROM student
WHERE email IS NOT NULL;

INSERT INTO user_roles (user_id, role)
SELECT au.id, 'student'
FROM app_user au
JOIN student s ON s.email = au.email;

INSERT INTO student_profile (user_id, ...)
SELECT au.id, ...
FROM app_user au
JOIN student s ON s.email = au.email;
*/
```

## Future Enhancements

1. **Two-Factor Authentication** - SMS or authenticator app
2. **OAuth Integration** - Google, Microsoft login
3. **Single Sign-On (SSO)** - SAML/OIDC support
4. **Password Policies** - Configurable complexity rules
5. **Session Management UI** - View/revoke active sessions
6. **Role Request Workflow** - Request additional roles
7. **Delegation** - Temporary role assignment
