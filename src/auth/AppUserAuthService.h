#ifndef APP_USER_AUTH_SERVICE_H
#define APP_USER_AUTH_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "api/FormSubmissionService.h"
#include "models/AppUser.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Result of an authentication operation
 */
struct AppUserAuthResult {
    bool success;
    std::string message;
    std::string token;
    Models::AppUser user;
    std::vector<std::string> errors;
};

/**
 * @brief Callback for async auth operations
 */
using AppUserAuthCallback = std::function<void(const AppUserAuthResult&)>;

/**
 * @brief Unified authentication service for all user types
 *
 * This service authenticates against the app_user table, which is the
 * single source of truth for user credentials. It supports:
 * - Login for all user types (students, instructors, admins)
 * - User creation (admin-only)
 * - Role assignment (admin-only)
 * - Password management
 *
 * User creation workflow:
 * 1. Administrator creates AppUser with email and password
 * 2. Administrator assigns roles to the AppUser
 * 3. If user has 'student' role, create Student profile
 * 4. If user has admin role, create AdminUser profile
 */
class AppUserAuthService {
public:
    AppUserAuthService();
    explicit AppUserAuthService(std::shared_ptr<Api::FormSubmissionService> apiService);
    ~AppUserAuthService() = default;

    // Service dependency
    void setApiService(std::shared_ptr<Api::FormSubmissionService> service) { apiService_ = service; }
    std::shared_ptr<Api::FormSubmissionService> getApiService() const { return apiService_; }

    // Authentication operations
    AppUserAuthResult login(const std::string& email, const std::string& password);
    AppUserAuthResult logout(const std::string& token);

    // Async operations
    void loginAsync(const std::string& email, const std::string& password, AppUserAuthCallback callback);

    // User management (requires admin role)
    AppUserAuthResult createUser(const std::string& email, const std::string& password,
                                  const std::string& firstName, const std::string& lastName,
                                  int createdByAdminId);
    AppUserAuthResult assignRole(int appUserId, Models::UserRole role, int assignedByAdminId);
    AppUserAuthResult removeRole(int appUserId, Models::UserRole role);
    AppUserAuthResult deactivateUser(int appUserId);
    AppUserAuthResult activateUser(int appUserId);

    // Password operations
    AppUserAuthResult requestPasswordReset(const std::string& email);
    AppUserAuthResult resetPassword(const std::string& token, const std::string& newPassword);
    AppUserAuthResult changePassword(int appUserId, const std::string& oldPassword,
                                      const std::string& newPassword);

    // User lookup
    AppUserAuthResult getUserByEmail(const std::string& email);
    AppUserAuthResult getUserById(int appUserId);
    std::vector<Models::AppUser> getUsersByRole(Models::UserRole role);

    // Token management
    bool validateToken(const std::string& token);
    AppUserAuthResult refreshToken(const std::string& token);

    // Validation
    static bool isValidEmail(const std::string& email);
    static bool isValidPassword(const std::string& password);
    static std::vector<std::string> getPasswordRequirements();

    // Role checking helpers
    bool hasRole(int appUserId, Models::UserRole role);
    std::vector<Models::UserRole> getUserRoles(int appUserId);

private:
    std::shared_ptr<Api::FormSubmissionService> apiService_;

    // Helper methods
    std::string hashPassword(const std::string& password) const;
    bool verifyPassword(const std::string& password, const std::string& hash) const;
    std::string generateToken(int appUserId) const;
    Models::AppUser parseAppUserFromJson(const nlohmann::json& json);
    std::vector<Models::RoleAssignment> fetchUserRoles(int appUserId);
};

} // namespace Auth
} // namespace StudentIntake

#endif // APP_USER_AUTH_SERVICE_H
