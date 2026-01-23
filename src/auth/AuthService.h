#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "api/ApiClient.h"
#include "models/User.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Result of an authentication operation
 */
struct AuthResult {
    bool success;
    std::string message;
    std::string sessionToken;
    std::string refreshToken;
    Models::User user;
    std::vector<std::string> errors;
};

/**
 * @brief Callback for async auth operations
 */
using AuthCallback = std::function<void(const AuthResult&)>;

/**
 * @brief Unified authentication service for all user types
 *
 * Handles:
 * - Login for students, instructors, and admins
 * - Session management
 * - Role-based routing
 * - Password management
 */
class AuthService {
public:
    AuthService();
    explicit AuthService(std::shared_ptr<Api::ApiClient> apiClient);
    ~AuthService();

    void setApiClient(std::shared_ptr<Api::ApiClient> client) { apiClient_ = client; }
    std::shared_ptr<Api::ApiClient> getApiClient() const { return apiClient_; }

    // =========================================================================
    // Authentication
    // =========================================================================

    /**
     * @brief Authenticate a user with email and password
     * @return AuthResult containing user info, roles, and session token
     */
    AuthResult login(const std::string& email, const std::string& password,
                     const std::string& ipAddress = "", const std::string& userAgent = "");

    /**
     * @brief Async login
     */
    void loginAsync(const std::string& email, const std::string& password,
                    AuthCallback callback,
                    const std::string& ipAddress = "", const std::string& userAgent = "");

    /**
     * @brief Logout and invalidate session
     */
    AuthResult logout(const std::string& sessionToken);

    /**
     * @brief Validate a session token
     */
    bool validateSession(const std::string& sessionToken);

    /**
     * @brief Refresh an expired session
     */
    AuthResult refreshSession(const std::string& refreshToken);

    /**
     * @brief Get user from session token
     */
    Models::User getUserFromSession(const std::string& sessionToken);

    // =========================================================================
    // Registration
    // =========================================================================

    /**
     * @brief Register a new student user
     */
    AuthResult registerStudent(const std::string& email, const std::string& password,
                               const std::string& firstName, const std::string& lastName);

    /**
     * @brief Async registration
     */
    void registerStudentAsync(const std::string& email, const std::string& password,
                              const std::string& firstName, const std::string& lastName,
                              AuthCallback callback);

    // =========================================================================
    // Password Management
    // =========================================================================

    /**
     * @brief Request a password reset
     */
    AuthResult requestPasswordReset(const std::string& email);

    /**
     * @brief Reset password with token
     */
    AuthResult resetPassword(const std::string& token, const std::string& newPassword);

    /**
     * @brief Change password for logged-in user
     */
    AuthResult changePassword(int userId, const std::string& oldPassword,
                              const std::string& newPassword);

    // =========================================================================
    // Role Management
    // =========================================================================

    /**
     * @brief Get all roles for a user
     */
    std::vector<Models::UserRole> getUserRoles(int userId);

    /**
     * @brief Switch active role for session (for multi-role users)
     */
    AuthResult switchRole(const std::string& sessionToken, Models::UserRole newRole);

    /**
     * @brief Check if user has a specific role
     */
    bool userHasRole(int userId, Models::UserRole role);

    /**
     * @brief Get the redirect route based on user's primary role
     */
    std::string getRouteForUser(const Models::User& user);

    // =========================================================================
    // Profile Management
    // =========================================================================

    /**
     * @brief Get student profile for a user
     */
    Models::StudentProfile getStudentProfile(int userId);

    /**
     * @brief Get admin profile for a user
     */
    Models::AdminProfile getAdminProfile(int userId);

    /**
     * @brief Update user profile
     */
    AuthResult updateUserProfile(const Models::User& user);

    /**
     * @brief Update student profile
     */
    AuthResult updateStudentProfile(const Models::StudentProfile& profile);

    // =========================================================================
    // Validation Helpers
    // =========================================================================

    /**
     * @brief Validate email format
     */
    static bool isValidEmail(const std::string& email);

    /**
     * @brief Validate password strength
     * Requirements: 8+ chars, uppercase, lowercase, digit
     */
    static bool isValidPassword(const std::string& password);

    /**
     * @brief Get password requirements message
     */
    static std::string getPasswordRequirements();

private:
    std::shared_ptr<Api::ApiClient> apiClient_;

    // Helper methods
    AuthResult parseAuthResponse(const Api::ApiResponse& response);
    nlohmann::json buildJsonApiPayload(const std::string& type, const nlohmann::json& attributes);
    std::string generateSessionToken();
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
    void recordLoginAttempt(const std::string& email, bool success,
                            const std::string& reason = "",
                            const std::string& ipAddress = "",
                            const std::string& userAgent = "");
};

} // namespace Auth
} // namespace StudentIntake

#endif // AUTH_SERVICE_H
