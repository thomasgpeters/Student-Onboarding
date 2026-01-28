#include "AdminUser.h"
#include <algorithm>

namespace StudentIntake {
namespace Admin {
namespace Models {

AdminUser::AdminUser()
    : id_(0)
    , appUserId_(0)
    , email_("")
    , passwordHash_("")
    , firstName_("")
    , lastName_("")
    , role_(AdminRole::Instructor)
    , departmentId_(0)
    , isActive_(true)
    , assignedPrograms_() {
}

std::string AdminUser::getFullName() const {
    if (firstName_.empty() && lastName_.empty()) {
        return email_;
    }
    return firstName_ + " " + lastName_;
}

std::string AdminUser::getRoleString() const {
    switch (role_) {
        case AdminRole::SuperAdmin:
            return "super_admin";
        case AdminRole::Administrator:
            return "admin";
        case AdminRole::Manager:
            return "manager";
        case AdminRole::Staff:
            return "staff";
        case AdminRole::Examiner:
            return "examiner";
        case AdminRole::Instructor:
        default:
            return "instructor";
    }
}

void AdminUser::setRoleFromString(const std::string& roleStr) {
    if (roleStr == "super_admin" || roleStr == "super_user") {
        role_ = AdminRole::SuperAdmin;
    } else if (roleStr == "admin" || roleStr == "administrator") {
        role_ = AdminRole::Administrator;
    } else if (roleStr == "manager") {
        role_ = AdminRole::Manager;
    } else if (roleStr == "staff") {
        role_ = AdminRole::Staff;
    } else if (roleStr == "examiner") {
        role_ = AdminRole::Examiner;
    } else if (roleStr == "instructor") {
        role_ = AdminRole::Instructor;
    } else {
        role_ = AdminRole::Instructor;
    }
}

bool AdminUser::canManageStudents() const {
    return role_ == AdminRole::SuperAdmin || role_ == AdminRole::Administrator;
}

bool AdminUser::canManageCurriculum() const {
    return role_ == AdminRole::SuperAdmin || role_ == AdminRole::Administrator;
}

bool AdminUser::canRevokeAccess() const {
    return role_ == AdminRole::SuperAdmin || role_ == AdminRole::Administrator;
}

bool AdminUser::canManageAdmins() const {
    return role_ == AdminRole::SuperAdmin;
}

bool AdminUser::canViewProgram(int curriculumId) const {
    // Super admins and administrators can view all programs
    if (role_ == AdminRole::SuperAdmin || role_ == AdminRole::Administrator) {
        return true;
    }

    // Instructors can only view assigned programs
    return std::find(assignedPrograms_.begin(), assignedPrograms_.end(), curriculumId)
           != assignedPrograms_.end();
}

bool AdminUser::isValid() const {
    return !email_.empty() && id_ > 0 && isActive_;
}

} // namespace Models
} // namespace Admin
} // namespace StudentIntake
