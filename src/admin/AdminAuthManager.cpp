#include "AdminAuthManager.h"
#include <regex>
#include "utils/Logger.h"

namespace StudentIntake {
namespace Admin {

AdminAuthManager::AdminAuthManager()
    : apiService_(nullptr) {
}

AdminAuthManager::AdminAuthManager(std::shared_ptr<Api::FormSubmissionService> apiService)
    : apiService_(apiService) {
}

AdminAuthManager::~AdminAuthManager() {
}

AdminAuthResult AdminAuthManager::login(const std::string& email, const std::string& password) {
    AdminAuthResult result;
    result.success = false;

    // Validate input
    if (email.empty() || password.empty()) {
        result.message = "Email and password are required";
        result.errors.push_back(result.message);
        return result;
    }

    if (!isValidEmail(email)) {
        result.message = "Invalid email format";
        result.errors.push_back(result.message);
        return result;
    }

    if (!apiService_) {
        result.message = "API service not available";
        result.errors.push_back(result.message);
        return result;
    }

    // Development fallback: Allow login with default admin credentials
    // even if API is not available. REMOVE IN PRODUCTION!
    if (email == "admin@university.edu" && password == "admin123") {
        LOG_WARN("AdminAuth", "Using development fallback login for admin");
        result.adminUser.setId(1);
        result.adminUser.setEmail(email);
        result.adminUser.setFirstName("System");
        result.adminUser.setLastName("Administrator");
        result.adminUser.setRole(Models::AdminRole::SuperAdmin);
        result.adminUser.setActive(true);
        result.success = true;
        result.message = "Login successful";
        result.token = "admin_token_dev_1";
        return result;
    }

    try {
        // Query the AdminUser API endpoint (PascalCase for ApiLogicServer)
        std::string endpoint = "/AdminUser?filter[email]=" + email;
        LOG_DEBUG("AdminAuth", "Querying endpoint: " << endpoint);
        auto response = apiService_->getApiClient()->get(endpoint);

        if (!response.success) {
            LOG_ERROR("AdminAuth", "API request failed: " << response.errorMessage);
            result.message = "Failed to connect to server";
            result.errors.push_back(response.errorMessage);
            return result;
        }

        LOG_DEBUG("AdminAuth", "API response: " << response.body.substr(0, 500));

        // Parse JSON response
        auto jsonResponse = nlohmann::json::parse(response.body);

        // Check if any admin users found
        if (!jsonResponse.contains("data") || jsonResponse["data"].empty()) {
            result.message = "Invalid credentials";
            result.errors.push_back("Admin user not found");
            return result;
        }

        // Get first matching admin user
        auto adminData = jsonResponse["data"][0];

        // Parse admin user
        result.adminUser = parseAdminUserFromJson(adminData);

        // Verify password (in production, compare bcrypt hashes)
        std::string storedHash = "";
        if (adminData.contains("attributes") && adminData["attributes"].contains("password_hash")) {
            storedHash = adminData["attributes"]["password_hash"].get<std::string>();
        }

        // Simple password check (in production, use proper bcrypt comparison)
        std::string inputHash = hashPassword(password);

        // For demo purposes, also allow a simple password check
        // In production, always use bcrypt comparison
        if (storedHash.empty() || (inputHash != storedHash && password != "admin123")) {
            result.message = "Invalid credentials";
            result.errors.push_back("Password mismatch");
            return result;
        }

        // Check if account is active
        if (!result.adminUser.isActive()) {
            result.message = "Account is disabled";
            result.errors.push_back("Admin account has been deactivated");
            return result;
        }

        // Success
        result.success = true;
        result.message = "Login successful";
        result.token = "admin_token_" + std::to_string(result.adminUser.getId());

        LOG_INFO("AdminAuth", "Login successful for: " << email
                  << " (Role: " << result.adminUser.getRoleString() << ")");

    } catch (const std::exception& e) {
        result.message = "Login failed";
        result.errors.push_back(std::string("Exception: ") + e.what());
        LOG_ERROR("AdminAuth", "Login exception: " << e.what());
    }

    return result;
}

AdminAuthResult AdminAuthManager::logout(Models::AdminSession& session) {
    AdminAuthResult result;
    result.success = true;
    result.message = "Logout successful";

    session.clear();

    return result;
}

bool AdminAuthManager::isValidEmail(const std::string& email) const {
    // Basic email validation
    std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return std::regex_match(email, emailRegex);
}

bool AdminAuthManager::isValidPassword(const std::string& password) const {
    // Admin passwords should be at least 8 characters
    return password.length() >= 8;
}

std::string AdminAuthManager::hashPassword(const std::string& password) const {
    // Simple hash for demo - in production use bcrypt
    std::hash<std::string> hasher;
    return std::to_string(hasher(password));
}

Models::AdminUser AdminAuthManager::parseAdminUserFromJson(const nlohmann::json& json) {
    Models::AdminUser user;

    // Parse ID
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            user.setId(std::stoi(json["id"].get<std::string>()));
        } else {
            user.setId(json["id"].get<int>());
        }
    }

    // Parse attributes
    if (json.contains("attributes")) {
        auto& attrs = json["attributes"];

        if (attrs.contains("email") && !attrs["email"].is_null()) {
            user.setEmail(attrs["email"].get<std::string>());
        }

        if (attrs.contains("first_name") && !attrs["first_name"].is_null()) {
            user.setFirstName(attrs["first_name"].get<std::string>());
        }

        if (attrs.contains("last_name") && !attrs["last_name"].is_null()) {
            user.setLastName(attrs["last_name"].get<std::string>());
        }

        if (attrs.contains("role") && !attrs["role"].is_null()) {
            user.setRoleFromString(attrs["role"].get<std::string>());
        }

        if (attrs.contains("department_id") && !attrs["department_id"].is_null()) {
            user.setDepartmentId(attrs["department_id"].get<int>());
        }

        if (attrs.contains("is_active") && !attrs["is_active"].is_null()) {
            user.setActive(attrs["is_active"].get<bool>());
        } else {
            user.setActive(true);  // Default to active
        }
    }

    return user;
}

} // namespace Admin
} // namespace StudentIntake
