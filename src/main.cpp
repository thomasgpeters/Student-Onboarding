#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include "app/StudentIntakeApp.h"

/**
 * @brief Application factory function for Wt
 */
std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env) {
    return std::make_unique<StudentIntake::App::StudentIntakeApp>(env);
}

/**
 * @brief Main entry point for the Student Intake Forms application
 */
int main(int argc, char** argv) {
    try {
        // Create and configure the server
        Wt::WServer server(argc, argv);

        // Add the application entry point
        server.addEntryPoint(Wt::EntryPointType::Application, createApplication);

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
