#ifndef ADMIN_USER_H
#define ADMIN_USER_H

#include <string>
#include <vector>
#include <chrono>

namespace StudentIntake {
namespace Admin {
namespace Models {

/**
 * @brief Admin user roles
 */
enum class AdminRole {
    Instructor,      // Can view students in assigned programs
    Administrator,   // Can manage students and curriculum
    SuperAdmin       // Full system access
};

/**
 * @brief Admin user model
 *
 * The AdminUser class stores admin-specific profile data. Authentication
 * credentials are stored in the AppUser table, referenced via appUserId_.
 * Only SuperAdmins can create admin accounts by first creating an
 * AppUser with the appropriate role, then creating the AdminUser profile.
 */
class AdminUser {
public:
    AdminUser();
    ~AdminUser() = default;

    // Getters
    int getId() const { return id_; }
    int getAppUserId() const { return appUserId_; }
    const std::string& getEmail() const { return email_; }
    const std::string& getPasswordHash() const { return passwordHash_; }
    const std::string& getFirstName() const { return firstName_; }
    const std::string& getLastName() const { return lastName_; }
    AdminRole getRole() const { return role_; }
    std::string getRoleString() const;
    int getDepartmentId() const { return departmentId_; }
    bool isActive() const { return isActive_; }
    const std::vector<int>& getAssignedPrograms() const { return assignedPrograms_; }

    std::string getFullName() const;

    // Setters
    void setId(int id) { id_ = id; }
    void setAppUserId(int appUserId) { appUserId_ = appUserId; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPasswordHash(const std::string& hash) { passwordHash_ = hash; }
    void setFirstName(const std::string& name) { firstName_ = name; }
    void setLastName(const std::string& name) { lastName_ = name; }
    void setRole(AdminRole role) { role_ = role; }
    void setRoleFromString(const std::string& roleStr);
    void setDepartmentId(int id) { departmentId_ = id; }
    void setActive(bool active) { isActive_ = active; }
    void setAssignedPrograms(const std::vector<int>& programs) { assignedPrograms_ = programs; }

    // Permission checks
    bool canManageStudents() const;
    bool canManageCurriculum() const;
    bool canRevokeAccess() const;
    bool canManageAdmins() const;
    bool canViewProgram(int curriculumId) const;

    // Validation
    bool isValid() const;

private:
    int id_;
    int appUserId_;  // Reference to app_user for credentials
    std::string email_;
    std::string passwordHash_;
    std::string firstName_;
    std::string lastName_;
    AdminRole role_;
    int departmentId_;
    bool isActive_;
    std::vector<int> assignedPrograms_;  // For instructors - which programs they can view
};

} // namespace Models
} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_USER_H
