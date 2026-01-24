#include "AppUserAuthService.h"
#include "utils/Logger.h"
#include <regex>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace StudentIntake {
namespace Auth {

AppUserAuthService::AppUserAuthService()
    : apiService_(nullptr) {
}

AppUserAuthService::AppUserAuthService(std::shared_ptr<Api::FormSubmissionService> apiService)
    : apiService_(apiService) {
}

AppUserAuthResult AppUserAuthService::login(const std::string& email, const std::string& password) {
    AppUserAuthResult result;
    result.success = false;

    // Validate inputs
    if (email.empty()) {
        result.message = "Email is required";
        result.errors.push_back("Email is required");
        return result;
    }

    if (password.empty()) {
        result.message = "Password is required";
        result.errors.push_back("Password is required");
        return result;
    }

    if (!isValidEmail(email)) {
        result.message = "Invalid email format";
        result.errors.push_back("Invalid email format");
        return result;
    }

    if (!apiService_) {
        result.message = "API service not configured";
        result.errors.push_back("Internal error: API service not available");
        LOG_ERROR("AppUserAuthService", "API service is null");
        return result;
    }

    try {
        // Query app_user by email
        std::string endpoint = "AppUser?filter[email]=" + email;
        auto apiResult = apiService_->getResource(endpoint);

        if (!apiResult.success) {
            result.message = "Authentication failed";
            result.errors.push_back("Invalid email or password");
            LOG_WARN("AppUserAuthService", "Failed to query app_user for email: " << email);
            return result;
        }

        // Parse response
        auto jsonResponse = nlohmann::json::parse(apiResult.data);

        if (!jsonResponse.contains("data") || !jsonResponse["data"].is_array() ||
            jsonResponse["data"].empty()) {
            result.message = "Authentication failed";
            result.errors.push_back("Invalid email or password");
            return result;
        }

        auto userData = jsonResponse["data"][0];
        Models::AppUser user = parseAppUserFromJson(userData);

        // Check if user is active
        if (!user.isActive()) {
            result.message = "Account is deactivated";
            result.errors.push_back("This account has been deactivated. Please contact an administrator.");
            return result;
        }

        // Check if login is enabled
        if (!user.isLoginEnabled()) {
            result.message = "Login is disabled";
            result.errors.push_back("Login has been disabled for this account. Please contact an administrator.");
            return result;
        }

        // Verify password
        std::string storedHash = user.getPasswordHash();
        if (!verifyPassword(password, storedHash)) {
            result.message = "Authentication failed";
            result.errors.push_back("Invalid email or password");
            return result;
        }

        // Fetch user roles
        auto roles = fetchUserRoles(user.getId());
        user.setRoles(roles);

        // Generate token
        std::string token = generateToken(user.getId());

        // Update last login timestamp
        nlohmann::json updateData;
        updateData["last_login_at"] = []() {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
            return ss.str();
        }();

        apiService_->updateResource("AppUser/" + std::to_string(user.getId()), updateData.dump());

        result.success = true;
        result.message = "Login successful";
        result.token = token;
        result.user = user;

        LOG_INFO("AppUserAuthService", "User logged in successfully: " << email);

    } catch (const std::exception& e) {
        result.message = "Authentication error";
        result.errors.push_back("An error occurred during authentication");
        LOG_ERROR("AppUserAuthService", "Login exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::logout(const std::string& token) {
    AppUserAuthResult result;
    result.success = true;
    result.message = "Logged out successfully";
    // In a production system, we would invalidate the token here
    return result;
}

void AppUserAuthService::loginAsync(const std::string& email, const std::string& password,
                                     AppUserAuthCallback callback) {
    // For now, just call the sync version
    // In production, this would use async I/O
    auto result = login(email, password);
    if (callback) {
        callback(result);
    }
}

AppUserAuthResult AppUserAuthService::createUser(const std::string& email, const std::string& password,
                                                   const std::string& firstName, const std::string& lastName,
                                                   int createdByAdminId) {
    AppUserAuthResult result;
    result.success = false;

    // Validate inputs
    if (email.empty()) {
        result.message = "Email is required";
        result.errors.push_back("Email is required");
        return result;
    }

    if (!isValidEmail(email)) {
        result.message = "Invalid email format";
        result.errors.push_back("Invalid email format");
        return result;
    }

    if (!isValidPassword(password)) {
        result.message = "Password does not meet requirements";
        for (const auto& req : getPasswordRequirements()) {
            result.errors.push_back(req);
        }
        return result;
    }

    if (!apiService_) {
        result.message = "API service not configured";
        result.errors.push_back("Internal error: API service not available");
        return result;
    }

    try {
        // Check if user already exists
        std::string checkEndpoint = "AppUser?filter[email]=" + email;
        auto checkResult = apiService_->getResource(checkEndpoint);

        if (checkResult.success) {
            auto jsonResponse = nlohmann::json::parse(checkResult.data);
            if (jsonResponse.contains("data") && jsonResponse["data"].is_array() &&
                !jsonResponse["data"].empty()) {
                result.message = "User already exists";
                result.errors.push_back("A user with this email already exists");
                return result;
            }
        }

        // Create the user
        nlohmann::json userData;
        userData["email"] = email;
        userData["password_hash"] = hashPassword(password);
        userData["first_name"] = firstName;
        userData["last_name"] = lastName;
        userData["is_active"] = true;
        userData["login_enabled"] = true;
        userData["email_verified"] = false;

        auto createResult = apiService_->createResource("AppUser", userData.dump());

        if (!createResult.success) {
            result.message = "Failed to create user";
            result.errors.push_back("Could not create user account");
            return result;
        }

        auto jsonResponse = nlohmann::json::parse(createResult.data);
        Models::AppUser newUser = parseAppUserFromJson(jsonResponse["data"]);

        result.success = true;
        result.message = "User created successfully";
        result.user = newUser;

        LOG_INFO("AppUserAuthService", "User created: " << email << " by admin " << createdByAdminId);

    } catch (const std::exception& e) {
        result.message = "Error creating user";
        result.errors.push_back("An error occurred while creating the user");
        LOG_ERROR("AppUserAuthService", "Create user exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::assignRole(int appUserId, Models::UserRole role, int assignedByAdminId) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        result.errors.push_back("Internal error: API service not available");
        return result;
    }

    try {
        nlohmann::json roleData;
        roleData["app_user_id"] = appUserId;
        roleData["role"] = Models::RoleAssignment::roleToString(role);
        roleData["assigned_by"] = assignedByAdminId;
        roleData["is_active"] = true;

        auto createResult = apiService_->createResource("UserRole", roleData.dump());

        if (!createResult.success) {
            result.message = "Failed to assign role";
            result.errors.push_back("Could not assign role to user");
            return result;
        }

        result.success = true;
        result.message = "Role assigned successfully";

        LOG_INFO("AppUserAuthService", "Role " << Models::RoleAssignment::roleToString(role)
                 << " assigned to user " << appUserId << " by admin " << assignedByAdminId);

    } catch (const std::exception& e) {
        result.message = "Error assigning role";
        result.errors.push_back("An error occurred while assigning the role");
        LOG_ERROR("AppUserAuthService", "Assign role exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::removeRole(int appUserId, Models::UserRole role) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        result.errors.push_back("Internal error: API service not available");
        return result;
    }

    try {
        // Find the role assignment
        std::string roleStr = Models::RoleAssignment::roleToString(role);
        std::string endpoint = "UserRole?filter[app_user_id]=" + std::to_string(appUserId) +
                               "&filter[role]=" + roleStr;

        auto getResult = apiService_->getResource(endpoint);

        if (!getResult.success) {
            result.message = "Role not found";
            result.errors.push_back("User does not have this role");
            return result;
        }

        auto jsonResponse = nlohmann::json::parse(getResult.data);
        if (!jsonResponse.contains("data") || !jsonResponse["data"].is_array() ||
            jsonResponse["data"].empty()) {
            result.message = "Role not found";
            result.errors.push_back("User does not have this role");
            return result;
        }

        int roleId = jsonResponse["data"][0]["id"].get<int>();

        // Delete or deactivate the role
        nlohmann::json updateData;
        updateData["is_active"] = false;

        apiService_->updateResource("UserRole/" + std::to_string(roleId), updateData.dump());

        result.success = true;
        result.message = "Role removed successfully";

        LOG_INFO("AppUserAuthService", "Role " << roleStr << " removed from user " << appUserId);

    } catch (const std::exception& e) {
        result.message = "Error removing role";
        result.errors.push_back("An error occurred while removing the role");
        LOG_ERROR("AppUserAuthService", "Remove role exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::deactivateUser(int appUserId) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        return result;
    }

    try {
        nlohmann::json updateData;
        updateData["is_active"] = false;

        auto updateResult = apiService_->updateResource("AppUser/" + std::to_string(appUserId),
                                                         updateData.dump());

        if (!updateResult.success) {
            result.message = "Failed to deactivate user";
            return result;
        }

        result.success = true;
        result.message = "User deactivated successfully";

        LOG_INFO("AppUserAuthService", "User " << appUserId << " deactivated");

    } catch (const std::exception& e) {
        result.message = "Error deactivating user";
        LOG_ERROR("AppUserAuthService", "Deactivate user exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::activateUser(int appUserId) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        return result;
    }

    try {
        nlohmann::json updateData;
        updateData["is_active"] = true;

        auto updateResult = apiService_->updateResource("AppUser/" + std::to_string(appUserId),
                                                         updateData.dump());

        if (!updateResult.success) {
            result.message = "Failed to activate user";
            return result;
        }

        result.success = true;
        result.message = "User activated successfully";

        LOG_INFO("AppUserAuthService", "User " << appUserId << " activated");

    } catch (const std::exception& e) {
        result.message = "Error activating user";
        LOG_ERROR("AppUserAuthService", "Activate user exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::getUserByEmail(const std::string& email) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        return result;
    }

    try {
        std::string endpoint = "AppUser?filter[email]=" + email;
        auto apiResult = apiService_->getResource(endpoint);

        if (!apiResult.success) {
            result.message = "User not found";
            return result;
        }

        auto jsonResponse = nlohmann::json::parse(apiResult.data);
        if (!jsonResponse.contains("data") || !jsonResponse["data"].is_array() ||
            jsonResponse["data"].empty()) {
            result.message = "User not found";
            return result;
        }

        Models::AppUser user = parseAppUserFromJson(jsonResponse["data"][0]);
        auto roles = fetchUserRoles(user.getId());
        user.setRoles(roles);

        result.success = true;
        result.user = user;

    } catch (const std::exception& e) {
        result.message = "Error fetching user";
        LOG_ERROR("AppUserAuthService", "Get user exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::getUserById(int appUserId) {
    AppUserAuthResult result;
    result.success = false;

    if (!apiService_) {
        result.message = "API service not configured";
        return result;
    }

    try {
        std::string endpoint = "AppUser/" + std::to_string(appUserId);
        auto apiResult = apiService_->getResource(endpoint);

        if (!apiResult.success) {
            result.message = "User not found";
            return result;
        }

        auto jsonResponse = nlohmann::json::parse(apiResult.data);
        Models::AppUser user = parseAppUserFromJson(jsonResponse["data"]);
        auto roles = fetchUserRoles(user.getId());
        user.setRoles(roles);

        result.success = true;
        result.user = user;

    } catch (const std::exception& e) {
        result.message = "Error fetching user";
        LOG_ERROR("AppUserAuthService", "Get user by id exception: " << e.what());
    }

    return result;
}

AppUserAuthResult AppUserAuthService::requestPasswordReset(const std::string& email) {
    AppUserAuthResult result;
    result.success = false;
    // Implementation would send password reset email
    result.message = "If an account exists with this email, a password reset link will be sent.";
    result.success = true;  // Always return success to not reveal if email exists
    return result;
}

AppUserAuthResult AppUserAuthService::resetPassword(const std::string& token, const std::string& newPassword) {
    AppUserAuthResult result;
    result.success = false;

    if (!isValidPassword(newPassword)) {
        result.message = "Password does not meet requirements";
        for (const auto& req : getPasswordRequirements()) {
            result.errors.push_back(req);
        }
        return result;
    }

    // Implementation would validate token and update password
    result.message = "Password reset not implemented";
    return result;
}

AppUserAuthResult AppUserAuthService::changePassword(int appUserId, const std::string& oldPassword,
                                                       const std::string& newPassword) {
    AppUserAuthResult result;
    result.success = false;

    if (!isValidPassword(newPassword)) {
        result.message = "New password does not meet requirements";
        for (const auto& req : getPasswordRequirements()) {
            result.errors.push_back(req);
        }
        return result;
    }

    if (!apiService_) {
        result.message = "API service not configured";
        return result;
    }

    try {
        // Get current user
        auto userResult = getUserById(appUserId);
        if (!userResult.success) {
            result.message = "User not found";
            return result;
        }

        // Verify old password
        if (!verifyPassword(oldPassword, userResult.user.getPasswordHash())) {
            result.message = "Current password is incorrect";
            result.errors.push_back("Current password is incorrect");
            return result;
        }

        // Update password
        nlohmann::json updateData;
        updateData["password_hash"] = hashPassword(newPassword);

        auto updateResult = apiService_->updateResource("AppUser/" + std::to_string(appUserId),
                                                         updateData.dump());

        if (!updateResult.success) {
            result.message = "Failed to update password";
            return result;
        }

        result.success = true;
        result.message = "Password changed successfully";

    } catch (const std::exception& e) {
        result.message = "Error changing password";
        LOG_ERROR("AppUserAuthService", "Change password exception: " << e.what());
    }

    return result;
}

bool AppUserAuthService::validateToken(const std::string& token) {
    // In production, this would validate a JWT or session token
    return !token.empty();
}

AppUserAuthResult AppUserAuthService::refreshToken(const std::string& token) {
    AppUserAuthResult result;
    result.success = false;
    result.message = "Token refresh not implemented";
    return result;
}

bool AppUserAuthService::isValidEmail(const std::string& email) {
    return Models::AppUser::isValidEmail(email);
}

bool AppUserAuthService::isValidPassword(const std::string& password) {
    return Models::AppUser::isValidPassword(password);
}

std::vector<std::string> AppUserAuthService::getPasswordRequirements() {
    return {
        "At least 8 characters",
        "At least one uppercase letter",
        "At least one lowercase letter",
        "At least one digit"
    };
}

bool AppUserAuthService::hasRole(int appUserId, Models::UserRole role) {
    auto roles = getUserRoles(appUserId);
    return std::find(roles.begin(), roles.end(), role) != roles.end();
}

std::vector<Models::UserRole> AppUserAuthService::getUserRoles(int appUserId) {
    std::vector<Models::UserRole> roles;

    auto roleAssignments = fetchUserRoles(appUserId);
    for (const auto& ra : roleAssignments) {
        if (ra.isActive) {
            roles.push_back(ra.role);
        }
    }

    return roles;
}

// Private helper methods

std::string AppUserAuthService::hashPassword(const std::string& password) const {
    // In production, use bcrypt or argon2
    // This is a simple hash for development only
    std::hash<std::string> hasher;
    size_t hash = hasher(password);
    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

bool AppUserAuthService::verifyPassword(const std::string& password, const std::string& hash) const {
    // In production, use bcrypt or argon2 verification
    return hashPassword(password) == hash;
}

std::string AppUserAuthService::generateToken(int appUserId) const {
    // In production, use JWT
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    const char* hex = "0123456789abcdef";
    std::string token = "token_";
    for (int i = 0; i < 32; ++i) {
        token += hex[dis(gen)];
    }
    token += "_" + std::to_string(appUserId);

    return token;
}

Models::AppUser AppUserAuthService::parseAppUserFromJson(const nlohmann::json& json) {
    // Handle JSON:API format where attributes may be nested
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    Models::AppUser user;

    if (json.contains("id")) {
        user.setId(json["id"].get<int>());
    }

    if (attrs.contains("email") && attrs["email"].is_string()) {
        user.setEmail(attrs["email"].get<std::string>());
    }

    if (attrs.contains("password_hash") && attrs["password_hash"].is_string()) {
        user.setPasswordHash(attrs["password_hash"].get<std::string>());
    }

    if (attrs.contains("first_name") && attrs["first_name"].is_string()) {
        user.setFirstName(attrs["first_name"].get<std::string>());
    }

    if (attrs.contains("last_name") && attrs["last_name"].is_string()) {
        user.setLastName(attrs["last_name"].get<std::string>());
    }

    if (attrs.contains("is_active")) {
        user.setActive(attrs["is_active"].get<bool>());
    }

    if (attrs.contains("login_enabled")) {
        user.setLoginEnabled(attrs["login_enabled"].get<bool>());
    }

    if (attrs.contains("email_verified")) {
        user.setEmailVerified(attrs["email_verified"].get<bool>());
    }

    return user;
}

std::vector<Models::RoleAssignment> AppUserAuthService::fetchUserRoles(int appUserId) {
    std::vector<Models::RoleAssignment> roles;

    if (!apiService_) {
        return roles;
    }

    try {
        std::string endpoint = "UserRole?filter[app_user_id]=" + std::to_string(appUserId);
        auto apiResult = apiService_->getResource(endpoint);

        if (!apiResult.success) {
            return roles;
        }

        auto jsonResponse = nlohmann::json::parse(apiResult.data);
        if (jsonResponse.contains("data") && jsonResponse["data"].is_array()) {
            for (const auto& roleJson : jsonResponse["data"]) {
                roles.push_back(Models::RoleAssignment::fromJson(roleJson));
            }
        }

    } catch (const std::exception& e) {
        LOG_ERROR("AppUserAuthService", "Fetch user roles exception: " << e.what());
    }

    return roles;
}

} // namespace Auth
} // namespace StudentIntake
