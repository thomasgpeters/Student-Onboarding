#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <string>
#include <memory>
#include <functional>
#include "api/FormSubmissionService.h"
#include "session/StudentSession.h"
#include "models/Student.h"

namespace StudentIntake {
namespace Auth {

/**
 * @brief Authentication result
 */
struct AuthResult {
    bool success;
    std::string message;
    std::string token;
    Models::Student student;
    std::vector<std::string> errors;
};

/**
 * @brief Callback for async auth operations
 */
using AuthCallback = std::function<void(const AuthResult&)>;

/**
 * @brief Manages user authentication
 */
class AuthManager {
public:
    AuthManager();
    explicit AuthManager(std::shared_ptr<Api::FormSubmissionService> apiService);
    ~AuthManager();

    // Service dependency
    void setApiService(std::shared_ptr<Api::FormSubmissionService> service) { apiService_ = service; }
    std::shared_ptr<Api::FormSubmissionService> getApiService() const { return apiService_; }

    // Authentication operations
    AuthResult login(const std::string& email, const std::string& password);
    AuthResult registerStudent(const std::string& email, const std::string& password,
                               const std::string& firstName, const std::string& lastName);
    AuthResult logout(Session::StudentSession& session);

    // Async operations
    void loginAsync(const std::string& email, const std::string& password, AuthCallback callback);
    void registerAsync(const std::string& email, const std::string& password,
                       const std::string& firstName, const std::string& lastName,
                       AuthCallback callback);

    // Token management
    bool validateToken(const std::string& token);
    bool refreshToken(Session::StudentSession& session);

    // Password operations
    AuthResult requestPasswordReset(const std::string& email);
    AuthResult resetPassword(const std::string& token, const std::string& newPassword);
    AuthResult changePassword(const std::string& studentId, const std::string& oldPassword,
                              const std::string& newPassword);

    // Validation
    bool isValidEmail(const std::string& email) const;
    bool isValidPassword(const std::string& password) const;
    std::vector<std::string> getPasswordRequirements() const;

private:
    std::shared_ptr<Api::FormSubmissionService> apiService_;

    // Helper methods
    std::string hashPassword(const std::string& password) const;
    AuthResult parseApiResult(const Api::SubmissionResult& result);
};

} // namespace Auth
} // namespace StudentIntake

#endif // AUTH_MANAGER_H
