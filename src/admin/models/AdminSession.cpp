#include "AdminSession.h"

namespace StudentIntake {
namespace Admin {
namespace Models {

AdminSession::AdminSession()
    : adminUser_()
    , isAuthenticated_(false)
    , token_("")
    , lastActivity_(std::chrono::system_clock::now())
    , sessionTimeoutMinutes_(60) {  // Default 1 hour timeout
}

void AdminSession::setAdminUser(const AdminUser& user) {
    adminUser_ = user;
    updateLastActivity();
}

void AdminSession::updateLastActivity() {
    lastActivity_ = std::chrono::system_clock::now();
}

bool AdminSession::isExpired() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::minutes>(now - lastActivity_);
    return duration.count() >= sessionTimeoutMinutes_;
}

void AdminSession::clear() {
    adminUser_ = AdminUser();
    isAuthenticated_ = false;
    token_ = "";
    lastActivity_ = std::chrono::system_clock::now();
}

} // namespace Models
} // namespace Admin
} // namespace StudentIntake
