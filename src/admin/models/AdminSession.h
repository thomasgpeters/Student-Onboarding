#ifndef ADMIN_SESSION_H
#define ADMIN_SESSION_H

#include <string>
#include <memory>
#include <chrono>
#include "AdminUser.h"

namespace StudentIntake {
namespace Admin {
namespace Models {

/**
 * @brief Admin session state management
 */
class AdminSession {
public:
    AdminSession();
    ~AdminSession() = default;

    // Session management
    void setAdminUser(const AdminUser& user);
    AdminUser& getAdminUser() { return adminUser_; }
    const AdminUser& getAdminUser() const { return adminUser_; }

    bool isAuthenticated() const { return isAuthenticated_; }
    void setAuthenticated(bool auth) { isAuthenticated_ = auth; }

    // Token management
    const std::string& getToken() const { return token_; }
    void setToken(const std::string& token) { token_ = token; }

    // Session timing
    void updateLastActivity();
    bool isExpired() const;
    void setSessionTimeout(int minutes) { sessionTimeoutMinutes_ = minutes; }

    // Clear session
    void clear();

private:
    AdminUser adminUser_;
    bool isAuthenticated_;
    std::string token_;
    std::chrono::system_clock::time_point lastActivity_;
    int sessionTimeoutMinutes_;
};

} // namespace Models
} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_SESSION_H
