#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "app/StudentIntakeApp.h"
#include "admin/AdminApp.h"
#include "utils/Logger.h"
#include <cstdlib>
#include <iostream>

/**
 * @brief Display startup banner
 */
void displayBanner() {
    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                               ║\n";
    std::cout << "  ║   ███████╗████████╗██╗   ██╗██████╗ ███████╗███╗   ██╗████████╗║\n";
    std::cout << "  ║   ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝████╗  ██║╚══██╔══╝║\n";
    std::cout << "  ║   ███████╗   ██║   ██║   ██║██║  ██║█████╗  ██╔██╗ ██║   ██║   ║\n";
    std::cout << "  ║   ╚════██║   ██║   ██║   ██║██║  ██║██╔══╝  ██║╚██╗██║   ██║   ║\n";
    std::cout << "  ║   ███████║   ██║   ╚██████╔╝██████╔╝███████╗██║ ╚████║   ██║   ║\n";
    std::cout << "  ║   ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝   ╚═╝   ║\n";
    std::cout << "  ║                                                               ║\n";
    std::cout << "  ║    ██████╗ ███╗   ██╗██████╗  ██████╗  █████╗ ██████╗ ██████╗  ║\n";
    std::cout << "  ║   ██╔═══██╗████╗  ██║██╔══██╗██╔═══██╗██╔══██╗██╔══██╗██╔══██╗ ║\n";
    std::cout << "  ║   ██║   ██║██╔██╗ ██║██████╔╝██║   ██║███████║██████╔╝██║  ██║ ║\n";
    std::cout << "  ║   ██║   ██║██║╚██╗██║██╔══██╗██║   ██║██╔══██║██╔══██╗██║  ██║ ║\n";
    std::cout << "  ║   ╚██████╔╝██║ ╚████║██████╔╝╚██████╔╝██║  ██║██║  ██║██████╔╝ ║\n";
    std::cout << "  ║    ╚═════╝ ╚═╝  ╚═══╝╚═════╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═════╝  ║\n";
    std::cout << "  ║                                                               ║\n";
    std::cout << "  ║               Student Intake Management System                ║\n";
    std::cout << "  ║                        v1.0.0                                 ║\n";
    std::cout << "  ║                                                               ║\n";
    std::cout << "  ╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

/**
 * @brief Application factory function for Student Portal
 */
std::unique_ptr<Wt::WApplication> createStudentApplication(const Wt::WEnvironment& env) {
    return std::make_unique<StudentIntake::App::StudentIntakeApp>(env);
}

/**
 * @brief Application factory function for Admin Portal
 */
std::unique_ptr<Wt::WApplication> createAdminApplication(const Wt::WEnvironment& env) {
    return std::make_unique<StudentIntake::Admin::AdminApp>(env);
}

/**
 * @brief Main entry point for the Student Intake Forms application
 *
 * URL Routing:
 *   /                  - Unified Login (redirects based on role)
 *   /student           - Student Portal (onboarding forms)
 *   /classroom         - Instructor/Classroom Portal
 *   /administration    - Admin Portal (staff dashboard)
 */
int main(int argc, char** argv) {
    try {
        // Display startup banner
        displayBanner();

        // Configure logging level from environment variable or default to INFO
        // Set LOG_LEVEL=DEBUG for verbose output, LOG_LEVEL=NONE to suppress
        const char* logLevel = std::getenv("LOG_LEVEL");
        if (logLevel) {
            StudentIntake::Logger::setLevelFromString(logLevel);
        } else {
            StudentIntake::Logger::setLevel(StudentIntake::LogLevel::INFO);
        }

        // Show log level in output (useful for startup)
        StudentIntake::Logger::setShowLevel(true);

        // Create and configure the server
        Wt::WServer server(argc, argv);

        // Add the unified login at root path
        server.addEntryPoint(Wt::EntryPointType::Application, createStudentApplication, "/");

        // Add the Student Portal at /student path
        server.addEntryPoint(Wt::EntryPointType::Application, createStudentApplication, "/student");

        // Add the Classroom/Instructor Portal at /classroom path
        server.addEntryPoint(Wt::EntryPointType::Application, createStudentApplication, "/classroom");

        // Add the Admin Portal entry point at /administration path
        server.addEntryPoint(Wt::EntryPointType::Application, createAdminApplication, "/administration");

        LOG_INFO("Main", "Starting Student Onboarding Application...");
        LOG_INFO("Main", "  Log level: " << StudentIntake::Logger::getLevelString());
        LOG_INFO("Main", "  Unified Login:     http://localhost:8080/");
        LOG_INFO("Main", "  Student Portal:    http://localhost:8080/student");
        LOG_INFO("Main", "  Classroom Portal:  http://localhost:8080/classroom");
        LOG_INFO("Main", "  Admin Portal:      http://localhost:8080/administration");

        // Run the server
        if (server.start()) {
            // Wait for shutdown signal
            int sig = Wt::WServer::waitForShutdown();
            LOG_INFO("Main", "Shutdown (signal = " << sig << ")");
            server.stop();
        }
    } catch (const Wt::WServer::Exception& e) {
        LOG_ERROR("Main", "Server exception: " << e.what());
        return 1;
    } catch (const std::exception& e) {
        LOG_ERROR("Main", "Exception: " << e.what());
        return 1;
    }

    return 0;
}
