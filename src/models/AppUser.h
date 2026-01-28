#ifndef APP_USER_H
#define APP_USER_H

#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief User roles in the system
 */
enum class UserRole {
    Student,        // Can access student intake portal
    Instructor,     // Can view students in assigned programs
    Examiner,       // Can view and grade student assessments
    Staff,          // General staff access
    Manager,        // Can manage staff and view reports
    Admin,          // Can manage students and curriculum
    SuperAdmin      // Full system access, can manage other admins
};

/**
 * @brief Role assignment for a user
 */
struct RoleAssignment {
    int id;
    int appUserId;
    UserRole role;
    int assignedBy;  // app_user_id of the admin who assigned this role
    std::chrono::system_clock::time_point assignedAt;
    bool isActive;

    std::string getRoleString() const;
    static UserRole roleFromString(const std::string& roleStr);
    static std::string roleToString(UserRole role);

    nlohmann::json toJson() const;
    static RoleAssignment fromJson(const nlohmann::json& json);
};

/**
 * @brief AppUser - Single source of truth for user credentials
 *
 * All users (students, instructors, admins) have their credentials stored here.
 * Profile-specific data is stored in student or admin_user tables which reference
 * this table via app_user_id.
 *
 * Only Administrators can create users and assign roles.
 */
class AppUser {
public:
    AppUser();
    AppUser(const std::string& email, const std::string& firstName, const std::string& lastName);
    ~AppUser() = default;

    // Getters
    int getId() const { return id_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPasswordHash() const { return passwordHash_; }
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getLastName() const { return lastName_; }
    std::string getFullName() const;
    bool isActive() const { return isActive_; }
    bool isLoginEnabled() const { return loginEnabled_; }
    bool isEmailVerified() const { return emailVerified_; }
    std::chrono::system_clock::time_point getLastLoginAt() const { return lastLoginAt_; }
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt_; }
    const std::vector<RoleAssignment>& getRoles() const { return roles_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPasswordHash(const std::string& hash) { passwordHash_ = hash; }
    void setFirstName(const std::string& name) { firstName_ = name; }
    void setLastName(const std::string& name) { lastName_ = name; }
    void setActive(bool active) { isActive_ = active; }
    void setLoginEnabled(bool enabled) { loginEnabled_ = enabled; }
    void setEmailVerified(bool verified) { emailVerified_ = verified; }
    void setLastLoginAt(const std::chrono::system_clock::time_point& time) { lastLoginAt_ = time; }
    void setCreatedAt(const std::chrono::system_clock::time_point& time) { createdAt_ = time; }
    void setRoles(const std::vector<RoleAssignment>& roles) { roles_ = roles; }

    // Role management
    void addRole(const RoleAssignment& role);
    void removeRole(UserRole role);
    bool hasRole(UserRole role) const;
    bool hasAnyRole(const std::vector<UserRole>& roles) const;
    UserRole getHighestRole() const;  // Returns the most privileged role
    std::vector<UserRole> getActiveRoles() const;

    // Permission checks (based on roles)
    bool canAccessStudentPortal() const;
    bool canAccessAdminPortal() const;
    bool canManageStudents() const;
    bool canManageCurriculum() const;
    bool canManageAdmins() const;
    bool canCreateUsers() const;
    bool canAssignRoles() const;

    // Validation
    bool isValid() const;
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);

    // Serialization
    nlohmann::json toJson() const;
    static AppUser fromJson(const nlohmann::json& json);

private:
    int id_;
    std::string email_;
    std::string passwordHash_;
    std::string firstName_;
    std::string lastName_;
    bool isActive_;
    bool loginEnabled_;
    bool emailVerified_;
    std::chrono::system_clock::time_point lastLoginAt_;
    std::chrono::system_clock::time_point createdAt_;
    std::vector<RoleAssignment> roles_;
};

} // namespace Models
} // namespace StudentIntake

#endif // APP_USER_H
