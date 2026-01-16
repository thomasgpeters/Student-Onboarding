#ifndef ADMIN_AUTH_MANAGER_H
#define ADMIN_AUTH_MANAGER_H

#include <string>
#include <memory>
#include <functional>
#include "api/FormSubmissionService.h"
#include "admin/models/AdminUser.h"
#include "admin/models/AdminSession.h"

namespace StudentIntake {
namespace Admin {

/**
 * @brief Admin authentication result
 */
struct AdminAuthResult {
    bool success;
    std::string message;
    std::string token;
    Models::AdminUser adminUser;
    std::vector<std::string> errors;
};

/**
 * @brief Callback for async auth operations
 */
using AdminAuthCallback = std::function<void(const AdminAuthResult&)>;

/**
 * @brief Manages admin user authentication (separate from student auth)
 */
class AdminAuthManager {
public:
    AdminAuthManager();
    explicit AdminAuthManager(std::shared_ptr<Api::FormSubmissionService> apiService);
    ~AdminAuthManager();

    // Service dependency
    void setApiService(std::shared_ptr<Api::FormSubmissionService> service) { apiService_ = service; }

    // Authentication operations
    AdminAuthResult login(const std::string& email, const std::string& password);
    AdminAuthResult logout(Models::AdminSession& session);

    // Validation
    bool isValidEmail(const std::string& email) const;
    bool isValidPassword(const std::string& password) const;

private:
    std::shared_ptr<Api::FormSubmissionService> apiService_;

    // Helper methods
    std::string hashPassword(const std::string& password) const;
    Models::AdminUser parseAdminUserFromJson(const nlohmann::json& json);
};

} // namespace Admin
} // namespace StudentIntake

#endif // ADMIN_AUTH_MANAGER_H
