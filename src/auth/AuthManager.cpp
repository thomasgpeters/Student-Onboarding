#include "AuthManager.h"
#include <regex>
#include <thread>
#include <functional>
#include <iostream>

namespace StudentIntake {
namespace Auth {

AuthManager::AuthManager()
    : apiService_(std::make_shared<Api::FormSubmissionService>()) {
}

AuthManager::AuthManager(std::shared_ptr<Api::FormSubmissionService> apiService)
    : apiService_(apiService) {
}

AuthManager::~AuthManager() {
}

AuthResult AuthManager::parseApiResult(const Api::SubmissionResult& result) {
    AuthResult authResult;
    authResult.success = result.success;
    authResult.message = result.message;
    authResult.errors = result.errors;

    if (result.success && result.responseData.contains("token")) {
        authResult.token = result.responseData["token"].get<std::string>();
    }

    if (result.success && result.responseData.contains("student")) {
        authResult.student = Models::Student::fromJson(result.responseData["student"]);
    } else if (result.success && result.responseData.contains("data")) {
        authResult.student = Models::Student::fromJson(result.responseData["data"]);
    }

    return authResult;
}

AuthResult AuthManager::login(const std::string& email, const std::string& password) {
    AuthResult result;

    // Validate inputs
    if (!isValidEmail(email)) {
        result.success = false;
        result.message = "Invalid email format";
        result.errors.push_back("Please enter a valid email address");
        return result;
    }

    if (password.empty()) {
        result.success = false;
        result.message = "Password is required";
        result.errors.push_back("Please enter your password");
        return result;
    }

    // Call API
    Api::SubmissionResult apiResult = apiService_->loginStudent(email, password);
    return parseApiResult(apiResult);
}

AuthResult AuthManager::registerStudent(const std::string& email, const std::string& password,
                                         const std::string& firstName, const std::string& lastName) {
    std::cout << "[AuthManager] registerStudent called for: " << email << std::endl;
    std::cout.flush();

    AuthResult result;

    // Validate inputs
    if (!isValidEmail(email)) {
        std::cout << "[AuthManager] Invalid email format" << std::endl;
        result.success = false;
        result.message = "Invalid email format";
        result.errors.push_back("Please enter a valid email address");
        return result;
    }

    if (!isValidPassword(password)) {
        std::cout << "[AuthManager] Password does not meet requirements" << std::endl;
        result.success = false;
        result.message = "Password does not meet requirements";
        result.errors = getPasswordRequirements();
        return result;
    }

    if (firstName.empty()) {
        result.success = false;
        result.message = "First name is required";
        result.errors.push_back("Please enter your first name");
        return result;
    }

    if (lastName.empty()) {
        result.success = false;
        result.message = "Last name is required";
        result.errors.push_back("Please enter your last name");
        return result;
    }

    std::cout << "[AuthManager] Validation passed, creating student object" << std::endl;
    std::cout.flush();

    // Create student object
    Models::Student student;
    student.setEmail(email);
    student.setFirstName(firstName);
    student.setLastName(lastName);

    std::cout << "[AuthManager] Calling apiService_->registerStudent" << std::endl;
    std::cout.flush();

    // Call API
    Api::SubmissionResult apiResult = apiService_->registerStudent(student, password);

    std::cout << "[AuthManager] API result - success: " << apiResult.success
              << ", message: " << apiResult.message << std::endl;
    if (apiResult.success) {
        std::cout << "[AuthManager] Response data: " << apiResult.responseData.dump() << std::endl;
    }
    std::cout.flush();

    result = parseApiResult(apiResult);

    // If parseApiResult didn't extract student from response, use local student
    // but try to get ID from response data
    if (result.success && result.student.getId().empty()) {
        result.student = student;
        // Try to extract ID from JSON:API response
        if (apiResult.responseData.contains("data")) {
            auto data = apiResult.responseData["data"];
            if (data.contains("id")) {
                result.student.setId(data["id"].get<std::string>());
            }
        }
    }

    std::cout << "[AuthManager] Final student ID: " << result.student.getId() << std::endl;
    std::cout.flush();

    return result;
}

AuthResult AuthManager::logout(Session::StudentSession& session) {
    AuthResult result;
    result.success = true;
    result.message = "Logged out successfully";

    session.logout();

    return result;
}

void AuthManager::loginAsync(const std::string& email, const std::string& password,
                              AuthCallback callback) {
    std::thread([this, email, password, callback]() {
        AuthResult result = login(email, password);
        if (callback) {
            callback(result);
        }
    }).detach();
}

void AuthManager::registerAsync(const std::string& email, const std::string& password,
                                 const std::string& firstName, const std::string& lastName,
                                 AuthCallback callback) {
    std::thread([this, email, password, firstName, lastName, callback]() {
        AuthResult result = registerStudent(email, password, firstName, lastName);
        if (callback) {
            callback(result);
        }
    }).detach();
}

bool AuthManager::validateToken(const std::string& token) {
    if (token.empty()) {
        return false;
    }

    // In a production environment, you would validate the token with the backend
    // For now, we just check if it's non-empty
    return true;
}

bool AuthManager::refreshToken(Session::StudentSession& session) {
    if (!session.isLoggedIn()) {
        return false;
    }

    // In a production environment, you would call the refresh token endpoint
    // and update the session with the new token
    return true;
}

AuthResult AuthManager::requestPasswordReset(const std::string& email) {
    AuthResult result;

    if (!isValidEmail(email)) {
        result.success = false;
        result.message = "Invalid email format";
        result.errors.push_back("Please enter a valid email address");
        return result;
    }

    // Call API for password reset
    auto apiClient = apiService_->getApiClient();
    nlohmann::json payload;
    payload["email"] = email;

    Api::ApiResponse response = apiClient->post("/auth/password-reset-request", payload);

    result.success = response.isSuccess();
    if (result.success) {
        result.message = "Password reset email sent. Please check your inbox.";
    } else {
        result.message = "Failed to send password reset email";
        result.errors.push_back(response.errorMessage);
    }

    return result;
}

AuthResult AuthManager::resetPassword(const std::string& token, const std::string& newPassword) {
    AuthResult result;

    if (!isValidPassword(newPassword)) {
        result.success = false;
        result.message = "Password does not meet requirements";
        result.errors = getPasswordRequirements();
        return result;
    }

    auto apiClient = apiService_->getApiClient();
    nlohmann::json payload;
    payload["token"] = token;
    payload["newPassword"] = newPassword;

    Api::ApiResponse response = apiClient->post("/auth/password-reset", payload);

    result.success = response.isSuccess();
    if (result.success) {
        result.message = "Password reset successfully";
    } else {
        result.message = "Failed to reset password";
        auto json = response.getJson();
        if (json.contains("message")) {
            result.errors.push_back(json["message"].get<std::string>());
        }
    }

    return result;
}

AuthResult AuthManager::changePassword(const std::string& studentId,
                                        const std::string& oldPassword,
                                        const std::string& newPassword) {
    AuthResult result;

    if (!isValidPassword(newPassword)) {
        result.success = false;
        result.message = "New password does not meet requirements";
        result.errors = getPasswordRequirements();
        return result;
    }

    auto apiClient = apiService_->getApiClient();
    nlohmann::json payload;
    payload["studentId"] = studentId;
    payload["oldPassword"] = oldPassword;
    payload["newPassword"] = newPassword;

    Api::ApiResponse response = apiClient->post("/auth/change-password", payload);

    result.success = response.isSuccess();
    if (result.success) {
        result.message = "Password changed successfully";
    } else {
        result.message = "Failed to change password";
        auto json = response.getJson();
        if (json.contains("message")) {
            result.errors.push_back(json["message"].get<std::string>());
        }
    }

    return result;
}

bool AuthManager::isValidEmail(const std::string& email) const {
    // Basic email validation regex
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool AuthManager::isValidPassword(const std::string& password) const {
    // Password requirements:
    // - At least 8 characters
    // - At least one uppercase letter
    // - At least one lowercase letter
    // - At least one digit

    if (password.length() < 8) {
        return false;
    }

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

std::vector<std::string> AuthManager::getPasswordRequirements() const {
    return {
        "Password must be at least 8 characters long",
        "Password must contain at least one uppercase letter",
        "Password must contain at least one lowercase letter",
        "Password must contain at least one digit"
    };
}

std::string AuthManager::hashPassword(const std::string& password) const {
    // In a production environment, use a proper hashing library like bcrypt
    // This is a placeholder - actual hashing should happen on the server
    return password;
}

} // namespace Auth
} // namespace StudentIntake
