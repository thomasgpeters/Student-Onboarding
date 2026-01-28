#include "User.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Models {

// =============================================================================
// User Implementation
// =============================================================================

User::User()
    : id_(0)
    , isActive_(true)
    , loginEnabled_(true)
    , emailVerified_(false)
    , failedLoginAttempts_(0)
{
}

UserRole User::getPrimaryRole() const {
    // Priority: Admin > Manager > Staff > Examiner > Instructor > Student
    if (hasRole(UserRole::Admin)) return UserRole::Admin;
    if (hasRole(UserRole::Manager)) return UserRole::Manager;
    if (hasRole(UserRole::Staff)) return UserRole::Staff;
    if (hasRole(UserRole::Examiner)) return UserRole::Examiner;
    if (hasRole(UserRole::Instructor)) return UserRole::Instructor;
    if (hasRole(UserRole::Student)) return UserRole::Student;

    // Default to student if no roles
    return UserRole::Student;
}

bool User::hasRole(UserRole role) const {
    return std::find(roles_.begin(), roles_.end(), role) != roles_.end();
}

void User::addRole(UserRole role) {
    if (!hasRole(role)) {
        roles_.push_back(role);
    }
}

void User::removeRole(UserRole role) {
    roles_.erase(std::remove(roles_.begin(), roles_.end(), role), roles_.end());
}

std::string User::getFullName() const {
    std::string name = firstName_;
    if (!middleName_.empty()) {
        name += " " + middleName_;
    }
    if (!lastName_.empty()) {
        name += " " + lastName_;
    }
    return name;
}

std::string User::getDisplayName() const {
    if (!preferredName_.empty()) {
        return preferredName_;
    }
    if (!firstName_.empty()) {
        return firstName_;
    }
    return email_;
}

bool User::isLocked() const {
    if (lockedUntil_.empty()) return false;

    // Compare with current time
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");

    return lockedUntil_ > oss.str();
}

bool User::canLogin() const {
    return isActive_ && loginEnabled_ && !isLocked();
}

std::string User::getHomeRoute() const {
    UserRole primary = getPrimaryRole();
    switch (primary) {
        case UserRole::Admin: return "/administration";
        case UserRole::Manager: return "/administration";
        case UserRole::Staff: return "/administration";
        case UserRole::Examiner: return "/administration";
        case UserRole::Instructor: return "/administration";
        case UserRole::Student: return "/student";
    }
    return "/student";
}

UserRole User::stringToRole(const std::string& role) {
    if (role == "admin") return UserRole::Admin;
    if (role == "manager") return UserRole::Manager;
    if (role == "staff") return UserRole::Staff;
    if (role == "examiner") return UserRole::Examiner;
    if (role == "instructor") return UserRole::Instructor;
    return UserRole::Student;
}

std::string User::roleToString(UserRole role) {
    switch (role) {
        case UserRole::Admin: return "admin";
        case UserRole::Manager: return "manager";
        case UserRole::Staff: return "staff";
        case UserRole::Examiner: return "examiner";
        case UserRole::Instructor: return "instructor";
        case UserRole::Student: return "student";
    }
    return "student";
}

std::string User::roleToDisplayString(UserRole role) {
    switch (role) {
        case UserRole::Admin: return "Administrator";
        case UserRole::Manager: return "Manager";
        case UserRole::Staff: return "Staff";
        case UserRole::Examiner: return "Examiner";
        case UserRole::Instructor: return "Instructor";
        case UserRole::Student: return "Student";
    }
    return "Student";
}

nlohmann::json User::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["email"] = email_;
    j["first_name"] = firstName_;
    j["last_name"] = lastName_;
    j["middle_name"] = middleName_;
    j["preferred_name"] = preferredName_;
    j["phone_number"] = phoneNumber_;
    j["is_active"] = isActive_;
    j["login_enabled"] = loginEnabled_;
    j["email_verified"] = emailVerified_;
    j["last_login_at"] = lastLoginAt_;
    j["failed_login_attempts"] = failedLoginAttempts_;
    j["locked_until"] = lockedUntil_;
    j["created_at"] = createdAt_;
    j["updated_at"] = updatedAt_;

    // Roles as array of strings
    nlohmann::json rolesArray = nlohmann::json::array();
    for (const auto& role : roles_) {
        rolesArray.push_back(roleToString(role));
    }
    j["roles"] = rolesArray;

    return j;
}

User User::fromJson(const nlohmann::json& json) {
    User user;

    // Handle JSON:API format - get attributes if present
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // Parse ID - can be at top level (JSON:API) or in attributes, can be string or int
    if (json.contains("id")) {
        if (json["id"].is_number()) {
            user.id_ = json["id"].get<int>();
        } else if (json["id"].is_string()) {
            try {
                user.id_ = std::stoi(json["id"].get<std::string>());
            } catch (...) {
                user.id_ = 0;
            }
        }
    }

    // Parse email
    if (attrs.contains("email") && attrs["email"].is_string()) {
        user.email_ = attrs["email"].get<std::string>();
    }

    // Parse password hash
    if (attrs.contains("password_hash") && attrs["password_hash"].is_string()) {
        user.passwordHash_ = attrs["password_hash"].get<std::string>();
    }

    // Parse name fields
    if (attrs.contains("first_name") && attrs["first_name"].is_string()) {
        user.firstName_ = attrs["first_name"].get<std::string>();
    }
    if (attrs.contains("last_name") && attrs["last_name"].is_string()) {
        user.lastName_ = attrs["last_name"].get<std::string>();
    }
    if (attrs.contains("middle_name") && attrs["middle_name"].is_string()) {
        user.middleName_ = attrs["middle_name"].get<std::string>();
    }
    if (attrs.contains("preferred_name") && attrs["preferred_name"].is_string()) {
        user.preferredName_ = attrs["preferred_name"].get<std::string>();
    }
    if (attrs.contains("phone_number") && attrs["phone_number"].is_string()) {
        user.phoneNumber_ = attrs["phone_number"].get<std::string>();
    }

    // Parse account status flags - default to true if not present (for Student fallback)
    if (attrs.contains("is_active") && attrs["is_active"].is_boolean()) {
        user.isActive_ = attrs["is_active"].get<bool>();
    } else {
        user.isActive_ = true;  // Default active for Student records
    }

    if (attrs.contains("login_enabled") && attrs["login_enabled"].is_boolean()) {
        user.loginEnabled_ = attrs["login_enabled"].get<bool>();
    } else if (attrs.contains("is_login_revoked") && attrs["is_login_revoked"].is_boolean()) {
        // For Student records, is_login_revoked is the inverse of login_enabled
        user.loginEnabled_ = !attrs["is_login_revoked"].get<bool>();
    } else {
        user.loginEnabled_ = true;  // Default enabled for Student records
    }

    // Also check status field for Student records - "revoked" means no login
    if (attrs.contains("status") && attrs["status"].is_string()) {
        std::string status = attrs["status"].get<std::string>();
        if (status == "revoked") {
            user.isActive_ = false;
        }
    }

    if (attrs.contains("email_verified") && attrs["email_verified"].is_boolean()) {
        user.emailVerified_ = attrs["email_verified"].get<bool>();
    }

    // Parse timestamps
    if (attrs.contains("last_login_at") && attrs["last_login_at"].is_string()) {
        user.lastLoginAt_ = attrs["last_login_at"].get<std::string>();
    }
    if (attrs.contains("created_at") && attrs["created_at"].is_string()) {
        user.createdAt_ = attrs["created_at"].get<std::string>();
    }
    if (attrs.contains("updated_at") && attrs["updated_at"].is_string()) {
        user.updatedAt_ = attrs["updated_at"].get<std::string>();
    }

    // Parse login attempt tracking
    if (attrs.contains("failed_login_attempts") && attrs["failed_login_attempts"].is_number()) {
        user.failedLoginAttempts_ = attrs["failed_login_attempts"].get<int>();
    }
    if (attrs.contains("locked_until") && attrs["locked_until"].is_string()) {
        user.lockedUntil_ = attrs["locked_until"].get<std::string>();
    }

    // Parse roles - from array or default to Student if from Student table
    if (attrs.contains("roles") && attrs["roles"].is_array()) {
        for (const auto& role : attrs["roles"]) {
            if (role.is_string()) {
                user.roles_.push_back(stringToRole(role.get<std::string>()));
            }
        }
    } else if (attrs.contains("student_type") || attrs.contains("curriculum_id")) {
        // This appears to be from Student table - default to Student role
        user.roles_.push_back(UserRole::Student);
    }

    return user;
}

// =============================================================================
// StudentProfile Implementation
// =============================================================================

StudentProfile::StudentProfile()
    : id_(0)
    , userId_(0)
    , curriculumId_(0)
    , isInternational_(false)
    , isTransferStudent_(false)
    , isVeteran_(false)
    , requiresFinancialAid_(false)
    , intakeStatus_("in_progress")
{
}

nlohmann::json StudentProfile::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["user_id"] = userId_;
    j["preferred_pronouns"] = preferredPronouns_;
    j["date_of_birth"] = dateOfBirth_;
    j["gender"] = gender_;
    j["alternate_phone"] = alternatePhone_;
    j["address_line1"] = addressLine1_;
    j["address_line2"] = addressLine2_;
    j["city"] = city_;
    j["state"] = state_;
    j["zip_code"] = zipCode_;
    j["ssn"] = ssn_;
    j["citizenship_status"] = citizenshipStatus_;
    j["citizenship_country"] = citizenshipCountry_;
    j["visa_type"] = visaType_;
    j["curriculum_id"] = curriculumId_;
    j["student_type"] = studentType_;
    j["enrollment_date"] = enrollmentDate_;
    j["expected_graduation"] = expectedGraduation_;
    j["is_international"] = isInternational_;
    j["is_transfer_student"] = isTransferStudent_;
    j["is_veteran"] = isVeteran_;
    j["requires_financial_aid"] = requiresFinancialAid_;
    j["intake_status"] = intakeStatus_;
    j["completed_forms"] = completedForms_;
    return j;
}

StudentProfile StudentProfile::fromJson(const nlohmann::json& json) {
    StudentProfile profile;

    if (json.contains("id") && json["id"].is_number()) {
        profile.id_ = json["id"].get<int>();
    }
    if (json.contains("user_id") && json["user_id"].is_number()) {
        profile.userId_ = json["user_id"].get<int>();
    }
    if (json.contains("preferred_pronouns") && json["preferred_pronouns"].is_string()) {
        profile.preferredPronouns_ = json["preferred_pronouns"].get<std::string>();
    }
    if (json.contains("date_of_birth") && json["date_of_birth"].is_string()) {
        profile.dateOfBirth_ = json["date_of_birth"].get<std::string>();
    }
    if (json.contains("gender") && json["gender"].is_string()) {
        profile.gender_ = json["gender"].get<std::string>();
    }
    if (json.contains("alternate_phone") && json["alternate_phone"].is_string()) {
        profile.alternatePhone_ = json["alternate_phone"].get<std::string>();
    }
    if (json.contains("address_line1") && json["address_line1"].is_string()) {
        profile.addressLine1_ = json["address_line1"].get<std::string>();
    }
    if (json.contains("address_line2") && json["address_line2"].is_string()) {
        profile.addressLine2_ = json["address_line2"].get<std::string>();
    }
    if (json.contains("city") && json["city"].is_string()) {
        profile.city_ = json["city"].get<std::string>();
    }
    if (json.contains("state") && json["state"].is_string()) {
        profile.state_ = json["state"].get<std::string>();
    }
    if (json.contains("zip_code") && json["zip_code"].is_string()) {
        profile.zipCode_ = json["zip_code"].get<std::string>();
    }
    if (json.contains("ssn") && json["ssn"].is_string()) {
        profile.ssn_ = json["ssn"].get<std::string>();
    }
    if (json.contains("citizenship_status") && json["citizenship_status"].is_string()) {
        profile.citizenshipStatus_ = json["citizenship_status"].get<std::string>();
    }
    if (json.contains("citizenship_country") && json["citizenship_country"].is_string()) {
        profile.citizenshipCountry_ = json["citizenship_country"].get<std::string>();
    }
    if (json.contains("visa_type") && json["visa_type"].is_string()) {
        profile.visaType_ = json["visa_type"].get<std::string>();
    }
    if (json.contains("curriculum_id") && json["curriculum_id"].is_number()) {
        profile.curriculumId_ = json["curriculum_id"].get<int>();
    }
    if (json.contains("student_type") && json["student_type"].is_string()) {
        profile.studentType_ = json["student_type"].get<std::string>();
    }
    if (json.contains("enrollment_date") && json["enrollment_date"].is_string()) {
        profile.enrollmentDate_ = json["enrollment_date"].get<std::string>();
    }
    if (json.contains("expected_graduation") && json["expected_graduation"].is_string()) {
        profile.expectedGraduation_ = json["expected_graduation"].get<std::string>();
    }
    if (json.contains("is_international") && json["is_international"].is_boolean()) {
        profile.isInternational_ = json["is_international"].get<bool>();
    }
    if (json.contains("is_transfer_student") && json["is_transfer_student"].is_boolean()) {
        profile.isTransferStudent_ = json["is_transfer_student"].get<bool>();
    }
    if (json.contains("is_veteran") && json["is_veteran"].is_boolean()) {
        profile.isVeteran_ = json["is_veteran"].get<bool>();
    }
    if (json.contains("requires_financial_aid") && json["requires_financial_aid"].is_boolean()) {
        profile.requiresFinancialAid_ = json["requires_financial_aid"].get<bool>();
    }
    if (json.contains("intake_status") && json["intake_status"].is_string()) {
        profile.intakeStatus_ = json["intake_status"].get<std::string>();
    }
    if (json.contains("completed_forms") && json["completed_forms"].is_string()) {
        profile.completedForms_ = json["completed_forms"].get<std::string>();
    }

    return profile;
}

// =============================================================================
// AdminProfile Implementation
// =============================================================================

AdminProfile::AdminProfile()
    : id_(0)
    , userId_(0)
    , departmentId_(0)
    , adminLevel_(AdminLevel::Staff)
    , canManageStudents_(true)
    , canManageCurriculum_(false)
    , canManageInstructors_(false)
    , canManageAdmins_(false)
    , canViewReports_(true)
    , canRevokeAccess_(false)
    , isActive_(true)
{
}

std::string AdminProfile::getAdminLevelString() const {
    return adminLevelToString(adminLevel_);
}

void AdminProfile::setAdminLevelFromString(const std::string& level) {
    adminLevel_ = stringToAdminLevel(level);
}

AdminLevel AdminProfile::stringToAdminLevel(const std::string& level) {
    if (level == "manager") return AdminLevel::Manager;
    if (level == "super_admin") return AdminLevel::SuperAdmin;
    return AdminLevel::Staff;
}

std::string AdminProfile::adminLevelToString(AdminLevel level) {
    switch (level) {
        case AdminLevel::Manager: return "manager";
        case AdminLevel::SuperAdmin: return "super_admin";
        case AdminLevel::Staff: return "staff";
    }
    return "staff";
}

nlohmann::json AdminProfile::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["user_id"] = userId_;
    j["employee_id"] = employeeId_;
    j["department_id"] = departmentId_;
    j["admin_level"] = adminLevelToString(adminLevel_);
    j["can_manage_students"] = canManageStudents_;
    j["can_manage_curriculum"] = canManageCurriculum_;
    j["can_manage_instructors"] = canManageInstructors_;
    j["can_manage_admins"] = canManageAdmins_;
    j["can_view_reports"] = canViewReports_;
    j["can_revoke_access"] = canRevokeAccess_;
    j["is_active"] = isActive_;
    return j;
}

AdminProfile AdminProfile::fromJson(const nlohmann::json& json) {
    AdminProfile profile;

    if (json.contains("id") && json["id"].is_number()) {
        profile.id_ = json["id"].get<int>();
    }
    if (json.contains("user_id") && json["user_id"].is_number()) {
        profile.userId_ = json["user_id"].get<int>();
    }
    if (json.contains("employee_id") && json["employee_id"].is_string()) {
        profile.employeeId_ = json["employee_id"].get<std::string>();
    }
    if (json.contains("department_id") && json["department_id"].is_number()) {
        profile.departmentId_ = json["department_id"].get<int>();
    }
    if (json.contains("admin_level") && json["admin_level"].is_string()) {
        profile.setAdminLevelFromString(json["admin_level"].get<std::string>());
    }
    if (json.contains("can_manage_students") && json["can_manage_students"].is_boolean()) {
        profile.canManageStudents_ = json["can_manage_students"].get<bool>();
    }
    if (json.contains("can_manage_curriculum") && json["can_manage_curriculum"].is_boolean()) {
        profile.canManageCurriculum_ = json["can_manage_curriculum"].get<bool>();
    }
    if (json.contains("can_manage_instructors") && json["can_manage_instructors"].is_boolean()) {
        profile.canManageInstructors_ = json["can_manage_instructors"].get<bool>();
    }
    if (json.contains("can_manage_admins") && json["can_manage_admins"].is_boolean()) {
        profile.canManageAdmins_ = json["can_manage_admins"].get<bool>();
    }
    if (json.contains("can_view_reports") && json["can_view_reports"].is_boolean()) {
        profile.canViewReports_ = json["can_view_reports"].get<bool>();
    }
    if (json.contains("can_revoke_access") && json["can_revoke_access"].is_boolean()) {
        profile.canRevokeAccess_ = json["can_revoke_access"].get<bool>();
    }
    if (json.contains("is_active") && json["is_active"].is_boolean()) {
        profile.isActive_ = json["is_active"].get<bool>();
    }

    return profile;
}

// =============================================================================
// UserSession Implementation
// =============================================================================

UserSession::UserSession()
    : id_(0)
    , userId_(0)
    , activeRole_(UserRole::Student)
    , isActive_(true)
{
}

bool UserSession::isExpired() const {
    if (expiresAt_.empty()) return true;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");

    return expiresAt_ < oss.str();
}

nlohmann::json UserSession::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["user_id"] = userId_;
    j["session_token"] = sessionToken_;
    j["refresh_token"] = refreshToken_;
    j["ip_address"] = ipAddress_;
    j["user_agent"] = userAgent_;
    j["active_role"] = User::roleToString(activeRole_);
    j["created_at"] = createdAt_;
    j["expires_at"] = expiresAt_;
    j["last_activity_at"] = lastActivityAt_;
    j["is_active"] = isActive_;
    return j;
}

UserSession UserSession::fromJson(const nlohmann::json& json) {
    UserSession session;

    if (json.contains("id") && json["id"].is_number()) {
        session.id_ = json["id"].get<int>();
    }
    if (json.contains("user_id") && json["user_id"].is_number()) {
        session.userId_ = json["user_id"].get<int>();
    }
    if (json.contains("session_token") && json["session_token"].is_string()) {
        session.sessionToken_ = json["session_token"].get<std::string>();
    }
    if (json.contains("refresh_token") && json["refresh_token"].is_string()) {
        session.refreshToken_ = json["refresh_token"].get<std::string>();
    }
    if (json.contains("ip_address") && json["ip_address"].is_string()) {
        session.ipAddress_ = json["ip_address"].get<std::string>();
    }
    if (json.contains("user_agent") && json["user_agent"].is_string()) {
        session.userAgent_ = json["user_agent"].get<std::string>();
    }
    if (json.contains("active_role") && json["active_role"].is_string()) {
        session.activeRole_ = User::stringToRole(json["active_role"].get<std::string>());
    }
    if (json.contains("created_at") && json["created_at"].is_string()) {
        session.createdAt_ = json["created_at"].get<std::string>();
    }
    if (json.contains("expires_at") && json["expires_at"].is_string()) {
        session.expiresAt_ = json["expires_at"].get<std::string>();
    }
    if (json.contains("last_activity_at") && json["last_activity_at"].is_string()) {
        session.lastActivityAt_ = json["last_activity_at"].get<std::string>();
    }
    if (json.contains("is_active") && json["is_active"].is_boolean()) {
        session.isActive_ = json["is_active"].get<bool>();
    }

    return session;
}

} // namespace Models
} // namespace StudentIntake
