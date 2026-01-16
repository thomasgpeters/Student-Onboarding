#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "app/StudentIntakeApp.h"
#include "admin/AdminApp.h"

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
 *   /                  - Student Portal (onboarding forms)
 *   /administration    - Admin Portal (staff dashboard)
 */
int main(int argc, char** argv) {
    try {
        // Create and configure the server
        Wt::WServer server(argc, argv);

        // Add the Student Portal entry point at root path
        server.addEntryPoint(Wt::EntryPointType::Application, createStudentApplication, "/");

        // Add the Admin Portal entry point at /administration path
        server.addEntryPoint(Wt::EntryPointType::Application, createAdminApplication, "/administration");

        std::cerr << "Starting Student Onboarding Application..." << std::endl;
        std::cerr << "  Student Portal: http://localhost:8080/" << std::endl;
        std::cerr << "  Admin Portal:   http://localhost:8080/administration" << std::endl;

        // Run the server
        if (server.start()) {
            // Wait for shutdown signal
            int sig = Wt::WServer::waitForShutdown();
            std::cerr << "Shutdown (signal = " << sig << ")" << std::endl;
            server.stop();
        }
    } catch (const Wt::WServer::Exception& e) {
        std::cerr << "Server exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
