#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>

namespace StudentIntake {
namespace App {

/**
 * @brief Application configuration settings
 */
struct AppConfig {
    // API settings
    std::string apiBaseUrl = "http://localhost:5656/api";
    int apiTimeout = 30;

    // Session settings
    int sessionTimeoutMinutes = 60;

    // UI settings
    std::string applicationTitle = "Student Onboarding";
    std::string applicationSubtitle = "Complete your enrollment forms";

    // File upload settings
    int maxUploadSizeMB = 10;
    std::string uploadDirectory = "/tmp/uploads";

    // Feature flags
    bool enableRegistration = true;
    bool enablePasswordReset = true;
    bool enableDraftSaving = true;
    bool enableProgressTracking = true;

    // Paths
    std::string configPath = "config/";
    std::string resourcesPath = "resources/";

    static AppConfig& getInstance() {
        static AppConfig instance;
        return instance;
    }

private:
    AppConfig() = default;
};

} // namespace App
} // namespace StudentIntake

#endif // APP_CONFIG_H
