#include "AppUser.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Models {

// =====================================================
// RoleAssignment implementation
// =====================================================

std::string RoleAssignment::getRoleString() const {
    return roleToString(role);
}

UserRole RoleAssignment::roleFromString(const std::string& roleStr) {
    if (roleStr == "student") return UserRole::Student;
    if (roleStr == "instructor") return UserRole::Instructor;
    if (roleStr == "examiner") return UserRole::Examiner;
    if (roleStr == "staff") return UserRole::Staff;
    if (roleStr == "manager") return UserRole::Manager;
    if (roleStr == "admin") return UserRole::Admin;
    if (roleStr == "super_admin") return UserRole::SuperAdmin;
    // Default to Student for unknown roles
    return UserRole::Student;
}

std::string RoleAssignment::roleToString(UserRole role) {
    switch (role) {
        case UserRole::Student: return "student";
        case UserRole::Instructor: return "instructor";
        case UserRole::Examiner: return "examiner";
        case UserRole::Staff: return "staff";
        case UserRole::Manager: return "manager";
        case UserRole::Admin: return "admin";
        case UserRole::SuperAdmin: return "super_admin";
    }
    return "student";
}

nlohmann::json RoleAssignment::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["app_user_id"] = appUserId;
    j["role"] = getRoleString();
    j["assigned_by"] = assignedBy;
    j["is_active"] = isActive;

    // Format timestamp
    auto time_t = std::chrono::system_clock::to_time_t(assignedAt);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    j["assigned_at"] = ss.str();

    return j;
}

RoleAssignment RoleAssignment::fromJson(const nlohmann::json& json) {
    RoleAssignment ra;

    if (json.contains("id") && !json["id"].is_null()) {
        ra.id = json["id"].get<int>();
    }

    if (json.contains("app_user_id") && !json["app_user_id"].is_null()) {
        ra.appUserId = json["app_user_id"].get<int>();
    }

    if (json.contains("role") && json["role"].is_string()) {
        ra.role = roleFromString(json["role"].get<std::string>());
    }

    if (json.contains("assigned_by") && !json["assigned_by"].is_null()) {
        ra.assignedBy = json["assigned_by"].get<int>();
    }

    if (json.contains("is_active")) {
        ra.isActive = json["is_active"].get<bool>();
    } else {
        ra.isActive = true;
    }

    if (json.contains("assigned_at") && json["assigned_at"].is_string()) {
        std::string dateStr = json["assigned_at"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        ra.assignedAt = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    return ra;
}

// =====================================================
// AppUser implementation
// =====================================================

AppUser::AppUser()
    : id_(0)
    , email_("")
    , passwordHash_("")
    , firstName_("")
    , lastName_("")
    , isActive_(true)
    , loginEnabled_(true)
    , emailVerified_(false)
    , lastLoginAt_()
    , createdAt_(std::chrono::system_clock::now())
{
}

AppUser::AppUser(const std::string& email, const std::string& firstName, const std::string& lastName)
    : id_(0)
    , email_(email)
    , passwordHash_("")
    , firstName_(firstName)
    , lastName_(lastName)
    , isActive_(true)
    , loginEnabled_(true)
    , emailVerified_(false)
    , lastLoginAt_()
    , createdAt_(std::chrono::system_clock::now())
{
}

std::string AppUser::getFullName() const {
    if (firstName_.empty() && lastName_.empty()) {
        return email_;
    }
    if (firstName_.empty()) {
        return lastName_;
    }
    if (lastName_.empty()) {
        return firstName_;
    }
    return firstName_ + " " + lastName_;
}

// Role management
void AppUser::addRole(const RoleAssignment& role) {
    // Check if role already exists
    auto it = std::find_if(roles_.begin(), roles_.end(),
        [&role](const RoleAssignment& ra) {
            return ra.role == role.role;
        });

    if (it != roles_.end()) {
        // Update existing role
        *it = role;
    } else {
        // Add new role
        roles_.push_back(role);
    }
}

void AppUser::removeRole(UserRole role) {
    roles_.erase(
        std::remove_if(roles_.begin(), roles_.end(),
            [role](const RoleAssignment& ra) {
                return ra.role == role;
            }),
        roles_.end()
    );
}

bool AppUser::hasRole(UserRole role) const {
    return std::any_of(roles_.begin(), roles_.end(),
        [role](const RoleAssignment& ra) {
            return ra.role == role && ra.isActive;
        });
}

bool AppUser::hasAnyRole(const std::vector<UserRole>& roles) const {
    for (const auto& role : roles) {
        if (hasRole(role)) {
            return true;
        }
    }
    return false;
}

UserRole AppUser::getHighestRole() const {
    // Check from highest to lowest privilege
    if (hasRole(UserRole::SuperAdmin)) return UserRole::SuperAdmin;
    if (hasRole(UserRole::Admin)) return UserRole::Admin;
    if (hasRole(UserRole::Manager)) return UserRole::Manager;
    if (hasRole(UserRole::Staff)) return UserRole::Staff;
    if (hasRole(UserRole::Examiner)) return UserRole::Examiner;
    if (hasRole(UserRole::Instructor)) return UserRole::Instructor;
    return UserRole::Student;
}

std::vector<UserRole> AppUser::getActiveRoles() const {
    std::vector<UserRole> activeRoles;
    for (const auto& ra : roles_) {
        if (ra.isActive) {
            activeRoles.push_back(ra.role);
        }
    }
    return activeRoles;
}

// Permission checks
bool AppUser::canAccessStudentPortal() const {
    return hasRole(UserRole::Student);
}

bool AppUser::canAccessAdminPortal() const {
    return hasAnyRole({UserRole::Instructor, UserRole::Examiner, UserRole::Staff,
                       UserRole::Manager, UserRole::Admin, UserRole::SuperAdmin});
}

bool AppUser::canManageStudents() const {
    return hasAnyRole({UserRole::Admin, UserRole::SuperAdmin});
}

bool AppUser::canManageCurriculum() const {
    return hasAnyRole({UserRole::Admin, UserRole::SuperAdmin});
}

bool AppUser::canManageAdmins() const {
    return hasRole(UserRole::SuperAdmin);
}

bool AppUser::canCreateUsers() const {
    return hasAnyRole({UserRole::Admin, UserRole::SuperAdmin});
}

bool AppUser::canAssignRoles() const {
    return hasAnyRole({UserRole::Admin, UserRole::SuperAdmin});
}

// Validation
bool AppUser::isValid() const {
    return !email_.empty() && isValidEmail(email_);
}

bool AppUser::isValidEmail(const std::string& email) {
    if (email.empty()) return false;

    // Basic email validation regex
    static const std::regex emailRegex(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );

    return std::regex_match(email, emailRegex);
}

bool AppUser::isValidPassword(const std::string& password) {
    // Password requirements:
    // - At least 8 characters
    // - At least one uppercase letter
    // - At least one lowercase letter
    // - At least one digit
    if (password.length() < 8) return false;

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;

    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        if (std::islower(c)) hasLower = true;
        if (std::isdigit(c)) hasDigit = true;
    }

    return hasUpper && hasLower && hasDigit;
}

// Serialization
nlohmann::json AppUser::toJson() const {
    nlohmann::json j;
    j["id"] = id_;
    j["email"] = email_;
    j["first_name"] = firstName_;
    j["last_name"] = lastName_;
    j["is_active"] = isActive_;
    j["login_enabled"] = loginEnabled_;
    j["email_verified"] = emailVerified_;

    // Format timestamps
    if (lastLoginAt_.time_since_epoch().count() > 0) {
        auto time_t = std::chrono::system_clock::to_time_t(lastLoginAt_);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
        j["last_login_at"] = ss.str();
    }

    auto created_time_t = std::chrono::system_clock::to_time_t(createdAt_);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&created_time_t), "%Y-%m-%dT%H:%M:%SZ");
    j["created_at"] = ss.str();

    // Serialize roles
    nlohmann::json rolesJson = nlohmann::json::array();
    for (const auto& role : roles_) {
        rolesJson.push_back(role.toJson());
    }
    j["roles"] = rolesJson;

    return j;
}

AppUser AppUser::fromJson(const nlohmann::json& json) {
    AppUser user;

    if (json.contains("id") && !json["id"].is_null()) {
        user.id_ = json["id"].get<int>();
    }

    if (json.contains("email") && json["email"].is_string()) {
        user.email_ = json["email"].get<std::string>();
    }

    if (json.contains("password_hash") && json["password_hash"].is_string()) {
        user.passwordHash_ = json["password_hash"].get<std::string>();
    }

    if (json.contains("first_name") && json["first_name"].is_string()) {
        user.firstName_ = json["first_name"].get<std::string>();
    }

    if (json.contains("last_name") && json["last_name"].is_string()) {
        user.lastName_ = json["last_name"].get<std::string>();
    }

    if (json.contains("is_active")) {
        user.isActive_ = json["is_active"].get<bool>();
    }

    if (json.contains("login_enabled")) {
        user.loginEnabled_ = json["login_enabled"].get<bool>();
    }

    if (json.contains("email_verified")) {
        user.emailVerified_ = json["email_verified"].get<bool>();
    }

    if (json.contains("last_login_at") && json["last_login_at"].is_string()) {
        std::string dateStr = json["last_login_at"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        user.lastLoginAt_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    if (json.contains("created_at") && json["created_at"].is_string()) {
        std::string dateStr = json["created_at"].get<std::string>();
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        user.createdAt_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    if (json.contains("roles") && json["roles"].is_array()) {
        for (const auto& roleJson : json["roles"]) {
            user.roles_.push_back(RoleAssignment::fromJson(roleJson));
        }
    }

    return user;
}

} // namespace Models
} // namespace StudentIntake
