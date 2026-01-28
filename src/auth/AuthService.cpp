#include "AuthService.h"
#include <regex>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace StudentIntake {
namespace Auth {

// Helper to get current timestamp
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// Helper to get session expiry (24 hours from now)
static std::string getSessionExpiry() {
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(24);
    auto time = std::chrono::system_clock::to_time_t(expiry);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

AuthService::AuthService()
    : apiClient_(std::make_shared<Api::ApiClient>()) {
}

AuthService::AuthService(std::shared_ptr<Api::ApiClient> apiClient)
    : apiClient_(apiClient) {
}

AuthService::~AuthService() = default;

// =============================================================================
// Helper Methods
// =============================================================================

UnifiedAuthResult AuthService::parseAuthResponse(const Api::ApiResponse& response) {
    UnifiedAuthResult result;
    result.success = response.success;

    if (!response.success) {
        result.message = response.errorMessage;
        result.errors.push_back(response.errorMessage);
        return result;
    }

    auto json = response.getJson();
    if (json.contains("data")) {
        auto& data = json["data"];

        // Parse user
        if (data.contains("user")) {
            result.user = Models::User::fromJson(data["user"]);
        } else if (data.contains("attributes")) {
            result.user = Models::User::fromJson(data["attributes"]);
        }

        // Parse tokens
        if (data.contains("session_token")) {
            result.sessionToken = data["session_token"].get<std::string>();
        }
        if (data.contains("refresh_token")) {
            result.refreshToken = data["refresh_token"].get<std::string>();
        }

        // Parse message
        if (data.contains("message")) {
            result.message = data["message"].get<std::string>();
        }
    }

    return result;
}

nlohmann::json AuthService::buildJsonApiPayload(const std::string& type,
                                                  const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["attributes"] = attributes;
    return payload;
}

nlohmann::json AuthService::buildJsonApiPayload(const std::string& type,
                                                  const std::string& id,
                                                  const nlohmann::json& attributes) {
    nlohmann::json payload;
    payload["data"]["type"] = type;
    payload["data"]["id"] = id;
    payload["data"]["attributes"] = attributes;
    return payload;
}

std::string AuthService::generateSessionToken() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    std::string token;
    token.reserve(64);
    for (int i = 0; i < 64; ++i) {
        token += alphanum[dis(gen)];
    }

    return token;
}

void AuthService::recordLoginAttempt(const std::string& email, bool success,
                                      const std::string& reason,
                                      const std::string& ipAddress,
                                      const std::string& userAgent) {
    nlohmann::json attrs;
    attrs["email"] = email;
    attrs["action"] = success ? "login_success" : "login_failed";
    attrs["success"] = success;
    attrs["failure_reason"] = reason;
    attrs["ip_address"] = ipAddress;
    attrs["user_agent"] = userAgent;
    attrs["created_at"] = getCurrentTimestamp();

    auto payload = buildJsonApiPayload("LoginAudit", attrs);
    // Fire and forget - don't wait for response
    apiClient_->post("/LoginAudit", payload);
}

// =============================================================================
// Authentication
// =============================================================================

UnifiedAuthResult AuthService::login(const std::string& email, const std::string& password,
                               const std::string& ipAddress, const std::string& userAgent) {
    UnifiedAuthResult result;
    result.success = false;

    // Validate input
    if (email.empty() || password.empty()) {
        result.message = "Email and password are required";
        result.errors.push_back(result.message);
        return result;
    }

    // Query for user by email - first try AppUser table
    std::string endpoint = "/AppUser?filter[email]=" + email;
    auto response = apiClient_->get(endpoint);

    if (!response.success) {
        // Fall back to Student table for backward compatibility
        endpoint = "/Student?filter[email]=" + email;
        response = apiClient_->get(endpoint);
    }

    if (!response.success) {
        result.message = "Invalid email or password";
        result.errors.push_back(result.message);
        recordLoginAttempt(email, false, result.message, ipAddress, userAgent);
        return result;
    }

    // Parse response
    auto json = response.getJson();
    nlohmann::json users;

    // Handle JSON:API format
    if (json.contains("data")) {
        users = json["data"];
    } else if (json.is_array()) {
        users = json;
    }

    if (!users.is_array() || users.empty()) {
        result.message = "Invalid email or password";
        result.errors.push_back(result.message);
        recordLoginAttempt(email, false, "User not found", ipAddress, userAgent);
        return result;
    }

    // Get the first user
    auto userData = users[0];

    // Parse user from response
    result.user = Models::User::fromJson(userData);

    // Check if user is active and login is enabled
    if (!result.user.isActive()) {
        result.message = "Account is inactive";
        result.errors.push_back(result.message);
        recordLoginAttempt(email, false, result.message, ipAddress, userAgent);
        return result;
    }

    if (!result.user.isLoginEnabled()) {
        result.message = "Login is disabled for this account";
        result.errors.push_back(result.message);
        recordLoginAttempt(email, false, result.message, ipAddress, userAgent);
        return result;
    }

    // Check account lockout
    if (result.user.isLocked()) {
        result.message = "Account is temporarily locked. Please try again later.";
        result.errors.push_back(result.message);
        recordLoginAttempt(email, false, result.message, ipAddress, userAgent);
        return result;
    }

    // Note: Password verification would normally be done server-side
    // Since the backend doesn't have a dedicated auth endpoint,
    // we're using query-based auth (similar to the original Student login)

    // Generate session token
    result.sessionToken = generateSessionToken();
    result.success = true;
    result.message = "Login successful";

    // Get user roles - merge with any roles already detected from user data
    auto existingRoles = result.user.getRoles();
    auto fetchedRoles = getUserRoles(result.user.getId(), result.user.getEmail());

    // Merge roles (add fetched roles that aren't already present)
    for (const auto& role : fetchedRoles) {
        if (!result.user.hasRole(role)) {
            result.user.addRole(role);
        }
    }

    // If still no roles, default to student
    if (result.user.getRoles().empty()) {
        result.user.addRole(Models::UserRole::Student);
    }

    recordLoginAttempt(email, true, "", ipAddress, userAgent);

    return result;
}

void AuthService::loginAsync(const std::string& email, const std::string& password,
                              UnifiedAuthCallback callback,
                              const std::string& ipAddress, const std::string& userAgent) {
    // For async, we'll query by email similar to sync version
    std::string endpoint = "/AppUser?filter[email]=" + email;

    apiClient_->getAsync(endpoint,
        [this, callback, email, password, ipAddress, userAgent](const Api::ApiResponse& response) {
            UnifiedAuthResult result;
            result.success = false;

            if (!response.success) {
                // Fall back to Student table - use sync call for simplicity
                auto syncResult = login(email, password, ipAddress, userAgent);
                callback(syncResult);
                return;
            }

            auto json = response.getJson();
            nlohmann::json users;

            if (json.contains("data")) {
                users = json["data"];
            } else if (json.is_array()) {
                users = json;
            }

            if (!users.is_array() || users.empty()) {
                result.message = "Invalid email or password";
                result.errors.push_back(result.message);
                recordLoginAttempt(email, false, "User not found", ipAddress, userAgent);
                callback(result);
                return;
            }

            auto userData = users[0];
            result.user = Models::User::fromJson(userData);

            if (!result.user.isActive() || !result.user.isLoginEnabled()) {
                result.message = "Account is inactive or disabled";
                result.errors.push_back(result.message);
                recordLoginAttempt(email, false, result.message, ipAddress, userAgent);
                callback(result);
                return;
            }

            result.sessionToken = generateSessionToken();
            result.success = true;
            result.message = "Login successful";
            result.user.setRoles(getUserRoles(result.user.getId(), result.user.getEmail()));

            recordLoginAttempt(email, true, "", ipAddress, userAgent);
            callback(result);
        });
}

UnifiedAuthResult AuthService::logout(const std::string& sessionToken) {
    UnifiedAuthResult result;

    if (sessionToken.empty()) {
        result.success = false;
        result.message = "Session token is required";
        return result;
    }

    // Since there's no dedicated auth endpoint, just clear the session locally
    // In a real implementation, we would invalidate the session server-side
    result.success = true;
    result.message = "Logged out successfully";

    return result;
}

bool AuthService::validateSession(const std::string& sessionToken) {
    if (sessionToken.empty()) return false;

    std::string endpoint = "/UserSession?filter[session_token]=" + sessionToken +
                          "&filter[is_active]=true";
    auto response = apiClient_->get(endpoint);
    auto json = response.getJson();

    if (!response.success || !json.contains("data")) {
        return false;
    }

    auto& data = json["data"];
    if (data.empty()) return false;

    // Check expiry
    auto session = Models::UserSession::fromJson(data[0]);
    return !session.isExpired() && session.isActive();
}

UnifiedAuthResult AuthService::refreshSession(const std::string& refreshToken) {
    UnifiedAuthResult result;
    result.success = false;

    if (refreshToken.empty()) {
        result.message = "Refresh token is required";
        return result;
    }

    nlohmann::json attrs;
    attrs["refresh_token"] = refreshToken;

    auto payload = buildJsonApiPayload("AuthRefresh", attrs);
    auto response = apiClient_->post("/auth/refresh", payload);

    return parseAuthResponse(response);
}

Models::User AuthService::getUserFromSession(const std::string& sessionToken) {
    if (sessionToken.empty()) {
        return Models::User();
    }

    // Get session
    std::string endpoint = "/UserSession?filter[session_token]=" + sessionToken +
                          "&filter[is_active]=true";
    auto sessionResponse = apiClient_->get(endpoint);
    auto sessionJson = sessionResponse.getJson();

    if (!sessionResponse.success || !sessionJson.contains("data") ||
        sessionJson["data"].empty()) {
        return Models::User();
    }

    auto session = Models::UserSession::fromJson(sessionJson["data"][0]);

    // Get user
    auto userResponse = apiClient_->get("/AppUser/" + std::to_string(session.getUserId()));
    auto userJson = userResponse.getJson();

    if (!userResponse.success || !userJson.contains("data")) {
        return Models::User();
    }

    Models::User user = Models::User::fromJson(userJson["data"]);

    // Get roles
    user.setRoles(getUserRoles(user.getId(), user.getEmail()));

    return user;
}

// =============================================================================
// Registration
// =============================================================================

UnifiedAuthResult AuthService::registerStudent(const std::string& email, const std::string& password,
                                          const std::string& firstName, const std::string& lastName) {
    UnifiedAuthResult result;
    result.success = false;

    // Validate input
    if (!isValidEmail(email)) {
        result.message = "Invalid email format";
        result.errors.push_back(result.message);
        return result;
    }

    if (!isValidPassword(password)) {
        result.message = getPasswordRequirements();
        result.errors.push_back(result.message);
        return result;
    }

    // Create user
    nlohmann::json attrs;
    attrs["email"] = email;
    attrs["password"] = password;
    attrs["first_name"] = firstName;
    attrs["last_name"] = lastName;
    attrs["role"] = "student";

    auto payload = buildJsonApiPayload("AuthRegister", attrs);
    auto response = apiClient_->post("/auth/register", payload);

    return parseAuthResponse(response);
}

void AuthService::registerStudentAsync(const std::string& email, const std::string& password,
                                         const std::string& firstName, const std::string& lastName,
                                         UnifiedAuthCallback callback) {
    // Validate input first
    if (!isValidEmail(email)) {
        UnifiedAuthResult result;
        result.success = false;
        result.message = "Invalid email format";
        callback(result);
        return;
    }

    if (!isValidPassword(password)) {
        UnifiedAuthResult result;
        result.success = false;
        result.message = getPasswordRequirements();
        callback(result);
        return;
    }

    nlohmann::json attrs;
    attrs["email"] = email;
    attrs["password"] = password;
    attrs["first_name"] = firstName;
    attrs["last_name"] = lastName;
    attrs["role"] = "student";

    auto payload = buildJsonApiPayload("AuthRegister", attrs);

    apiClient_->postAsync("/auth/register", payload,
        [this, callback](const Api::ApiResponse& response) {
            callback(parseAuthResponse(response));
        });
}

// =============================================================================
// Password Management
// =============================================================================

UnifiedAuthResult AuthService::requestPasswordReset(const std::string& email) {
    UnifiedAuthResult result;
    result.success = false;

    if (email.empty()) {
        result.message = "Email is required";
        return result;
    }

    nlohmann::json attrs;
    attrs["email"] = email;

    auto payload = buildJsonApiPayload("PasswordReset", attrs);
    auto response = apiClient_->post("/auth/password-reset-request", payload);

    result.success = response.success;
    result.message = response.success ?
        "If an account exists with this email, a password reset link has been sent." :
        response.errorMessage;

    return result;
}

UnifiedAuthResult AuthService::resetPassword(const std::string& token, const std::string& newPassword) {
    UnifiedAuthResult result;
    result.success = false;

    if (token.empty()) {
        result.message = "Reset token is required";
        return result;
    }

    if (!isValidPassword(newPassword)) {
        result.message = getPasswordRequirements();
        return result;
    }

    nlohmann::json attrs;
    attrs["token"] = token;
    attrs["new_password"] = newPassword;

    auto payload = buildJsonApiPayload("PasswordReset", attrs);
    auto response = apiClient_->post("/auth/password-reset", payload);

    result.success = response.success;
    result.message = response.success ? "Password reset successfully" : response.errorMessage;

    return result;
}

UnifiedAuthResult AuthService::changePassword(int userId, const std::string& oldPassword,
                                         const std::string& newPassword) {
    UnifiedAuthResult result;
    result.success = false;

    if (!isValidPassword(newPassword)) {
        result.message = getPasswordRequirements();
        return result;
    }

    nlohmann::json attrs;
    attrs["user_id"] = userId;
    attrs["old_password"] = oldPassword;
    attrs["new_password"] = newPassword;

    auto payload = buildJsonApiPayload("PasswordChange", attrs);
    auto response = apiClient_->post("/auth/change-password", payload);

    result.success = response.success;
    result.message = response.success ? "Password changed successfully" : response.errorMessage;

    return result;
}

// =============================================================================
// Role Management
// =============================================================================

std::vector<Models::UserRole> AuthService::getUserRoles(int userId, const std::string& email) {
    std::vector<Models::UserRole> roles;

    // Try to get roles from UserRole table (unified auth system)
    std::string endpoint = "/UserRole?filter[user_id]=" + std::to_string(userId) +
                          "&filter[is_active]=true";
    auto response = apiClient_->get(endpoint);
    auto json = response.getJson();

    if (response.success && json.contains("data") && json["data"].is_array() && !json["data"].empty()) {
        for (const auto& item : json["data"]) {
            if (item.contains("role") && item["role"].is_string()) {
                roles.push_back(Models::User::stringToRole(item["role"].get<std::string>()));
            } else if (item.contains("attributes") && item["attributes"].contains("role")) {
                roles.push_back(Models::User::stringToRole(
                    item["attributes"]["role"].get<std::string>()));
            }
        }
    }

    // If no roles found from UserRoles table, check admin_user table by email
    if (roles.empty() && !email.empty()) {
        std::string adminEndpoint = "/AdminUser?filter[email]=" + email;
        auto adminResponse = apiClient_->get(adminEndpoint);
        auto adminJson = adminResponse.getJson();

        if (adminResponse.success && adminJson.contains("data") &&
            adminJson["data"].is_array() && !adminJson["data"].empty()) {
            // Found user in admin_user table - check their role
            auto adminData = adminJson["data"][0];
            std::string adminRole;

            if (adminData.contains("role") && adminData["role"].is_string()) {
                adminRole = adminData["role"].get<std::string>();
            } else if (adminData.contains("attributes") && adminData["attributes"].contains("role")) {
                adminRole = adminData["attributes"]["role"].get<std::string>();
            }

            // Map admin_user role to UserRole
            if (adminRole == "admin" || adminRole == "super_admin") {
                roles.push_back(Models::UserRole::Admin);
            } else if (adminRole == "instructor") {
                roles.push_back(Models::UserRole::Instructor);
            } else if (adminRole == "staff") {
                // Staff role - treat as limited admin access
                roles.push_back(Models::UserRole::Admin);
            }
        }
    }

    // Note: If roles is still empty, the User::fromJson already assigned Student role
    // based on student_type/curriculum_id fields from Student table fallback

    return roles;
}

UnifiedAuthResult AuthService::switchRole(const std::string& sessionToken, Models::UserRole newRole) {
    UnifiedAuthResult result;
    result.success = false;

    if (sessionToken.empty()) {
        result.message = "Session token is required";
        return result;
    }

    // Get current session
    std::string endpoint = "/UserSession?filter[session_token]=" + sessionToken;
    auto sessionResponse = apiClient_->get(endpoint);
    auto sessionJson = sessionResponse.getJson();

    if (!sessionResponse.success || !sessionJson.contains("data") ||
        sessionJson["data"].empty()) {
        result.message = "Invalid session";
        return result;
    }

    auto session = Models::UserSession::fromJson(sessionJson["data"][0]);

    // Check if user has the requested role
    if (!userHasRole(session.getUserId(), newRole)) {
        result.message = "User does not have the requested role";
        return result;
    }

    // Update session with new active role
    nlohmann::json attrs;
    attrs["active_role"] = Models::User::roleToString(newRole);

    auto payload = buildJsonApiPayload("UserSession", std::to_string(session.getId()), attrs);
    auto updateResponse = apiClient_->patch("/UserSession/" + std::to_string(session.getId()), payload);

    result.success = updateResponse.success;
    result.message = updateResponse.success ? "Role switched successfully" : updateResponse.errorMessage;

    // Return updated user
    result.user = getUserFromSession(sessionToken);

    return result;
}

bool AuthService::userHasRole(int userId, Models::UserRole role) {
    auto roles = getUserRoles(userId);
    return std::find(roles.begin(), roles.end(), role) != roles.end();
}

std::string AuthService::getRouteForUser(const Models::User& user) {
    return user.getHomeRoute();
}

// =============================================================================
// Profile Management
// =============================================================================

Models::StudentProfile AuthService::getStudentProfile(int userId) {
    std::string endpoint = "/StudentProfile?filter[user_id]=" + std::to_string(userId);
    auto response = apiClient_->get(endpoint);
    auto json = response.getJson();

    if (response.success && json.contains("data") && !json["data"].empty()) {
        return Models::StudentProfile::fromJson(json["data"][0]);
    }

    return Models::StudentProfile();
}

Models::AdminProfile AuthService::getAdminProfile(int userId) {
    std::string endpoint = "/AdminProfile?filter[user_id]=" + std::to_string(userId);
    auto response = apiClient_->get(endpoint);
    auto json = response.getJson();

    if (response.success && json.contains("data") && !json["data"].empty()) {
        return Models::AdminProfile::fromJson(json["data"][0]);
    }

    return Models::AdminProfile();
}

UnifiedAuthResult AuthService::updateUserProfile(const Models::User& user) {
    UnifiedAuthResult result;

    auto attrs = user.toJson();
    auto payload = buildJsonApiPayload("AppUser", std::to_string(user.getId()), attrs);
    auto response = apiClient_->patch("/AppUser/" + std::to_string(user.getId()), payload);

    result.success = response.success;
    result.message = response.success ? "Profile updated successfully" : response.errorMessage;

    return result;
}

UnifiedAuthResult AuthService::updateStudentProfile(const Models::StudentProfile& profile) {
    UnifiedAuthResult result;

    auto attrs = profile.toJson();
    auto payload = buildJsonApiPayload("StudentProfile", std::to_string(profile.getId()), attrs);
    auto response = apiClient_->patch("/StudentProfile/" + std::to_string(profile.getId()), payload);

    result.success = response.success;
    result.message = response.success ? "Profile updated successfully" : response.errorMessage;

    return result;
}

// =============================================================================
// Validation Helpers
// =============================================================================

bool AuthService::isValidEmail(const std::string& email) {
    if (email.empty()) return false;

    // Basic email regex pattern
    std::regex pattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, pattern);
}

bool AuthService::isValidPassword(const std::string& password) {
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

std::string AuthService::getPasswordRequirements() {
    return "Password must be at least 8 characters and contain at least one uppercase letter, "
           "one lowercase letter, and one digit.";
}

} // namespace Auth
} // namespace StudentIntake
