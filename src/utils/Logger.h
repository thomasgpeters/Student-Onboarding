#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace StudentIntake {

/**
 * Log levels for controlling output verbosity
 * NONE = No logging output
 * ERROR = Only error messages
 * WARN = Errors and warnings
 * INFO = Errors, warnings, and informational messages
 * DEBUG = All messages including detailed debug output
 */
enum class LogLevel {
    NONE = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
};

/**
 * Logger class for centralized logging with configurable levels
 *
 * Usage:
 *   Logger::setLevel(LogLevel::DEBUG);  // Set globally
 *   LOG_DEBUG("MyComponent", "Debug message: " << value);
 *   LOG_INFO("MyComponent", "Info message");
 *   LOG_WARN("MyComponent", "Warning message");
 *   LOG_ERROR("MyComponent", "Error message");
 */
class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    // Set the global log level
    static void setLevel(LogLevel level) {
        instance().currentLevel_ = level;
    }

    // Get the current log level
    static LogLevel getLevel() {
        return instance().currentLevel_;
    }

    // Set level from string (useful for config files)
    static void setLevelFromString(const std::string& level) {
        if (level == "NONE" || level == "none") {
            setLevel(LogLevel::NONE);
        } else if (level == "ERROR" || level == "error") {
            setLevel(LogLevel::ERROR);
        } else if (level == "WARN" || level == "warn" || level == "WARNING" || level == "warning") {
            setLevel(LogLevel::WARN);
        } else if (level == "INFO" || level == "info") {
            setLevel(LogLevel::INFO);
        } else if (level == "DEBUG" || level == "debug") {
            setLevel(LogLevel::DEBUG);
        }
    }

    // Get level as string
    static std::string getLevelString() {
        switch (instance().currentLevel_) {
            case LogLevel::NONE: return "NONE";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::WARN: return "WARN";
            case LogLevel::INFO: return "INFO";
            case LogLevel::DEBUG: return "DEBUG";
            default: return "UNKNOWN";
        }
    }

    // Check if a level should be logged
    static bool shouldLog(LogLevel level) {
        return static_cast<int>(level) <= static_cast<int>(instance().currentLevel_);
    }

    // Enable/disable timestamps
    static void setShowTimestamp(bool show) {
        instance().showTimestamp_ = show;
    }

    // Enable/disable log level prefix
    static void setShowLevel(bool show) {
        instance().showLevel_ = show;
    }

    // Log a message at the specified level
    template<typename T>
    static void log(LogLevel level, const std::string& component, const T& message) {
        if (!shouldLog(level)) return;

        std::lock_guard<std::mutex> lock(instance().mutex_);

        std::ostream& out = (level == LogLevel::ERROR) ? std::cerr : std::cout;

        // Optional timestamp
        if (instance().showTimestamp_) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
            out << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
                << "." << std::setfill('0') << std::setw(3) << ms.count() << " ";
        }

        // Optional level prefix
        if (instance().showLevel_) {
            switch (level) {
                case LogLevel::DEBUG: out << "[DEBUG] "; break;
                case LogLevel::INFO:  out << "[INFO]  "; break;
                case LogLevel::WARN:  out << "[WARN]  "; break;
                case LogLevel::ERROR: out << "[ERROR] "; break;
                default: break;
            }
        }

        // Component tag
        if (!component.empty()) {
            out << "[" << component << "] ";
        }

        // Message
        out << message << std::endl;
        out.flush();
    }

    // Convenience methods
    template<typename T>
    static void debug(const std::string& component, const T& message) {
        log(LogLevel::DEBUG, component, message);
    }

    template<typename T>
    static void info(const std::string& component, const T& message) {
        log(LogLevel::INFO, component, message);
    }

    template<typename T>
    static void warn(const std::string& component, const T& message) {
        log(LogLevel::WARN, component, message);
    }

    template<typename T>
    static void error(const std::string& component, const T& message) {
        log(LogLevel::ERROR, component, message);
    }

private:
    Logger() : currentLevel_(LogLevel::INFO), showTimestamp_(false), showLevel_(true) {}
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    LogLevel currentLevel_;
    bool showTimestamp_;
    bool showLevel_;
    std::mutex mutex_;
};

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& component)
        : level_(level), component_(component), active_(Logger::shouldLog(level)) {}

    ~LogStream() {
        if (active_) {
            Logger::log(level_, component_, stream_.str());
        }
    }

    template<typename T>
    LogStream& operator<<(const T& value) {
        if (active_) {
            stream_ << value;
        }
        return *this;
    }

private:
    LogLevel level_;
    std::string component_;
    bool active_;
    std::ostringstream stream_;
};

} // namespace StudentIntake

// Convenience macros for logging with stream syntax
// Usage: LOG_DEBUG("Component", "message " << variable << " more text");
#define LOG_DEBUG(component, message) \
    do { \
        if (StudentIntake::Logger::shouldLog(StudentIntake::LogLevel::DEBUG)) { \
            std::ostringstream _log_ss; \
            _log_ss << message; \
            StudentIntake::Logger::debug(component, _log_ss.str()); \
        } \
    } while(0)

#define LOG_INFO(component, message) \
    do { \
        if (StudentIntake::Logger::shouldLog(StudentIntake::LogLevel::INFO)) { \
            std::ostringstream _log_ss; \
            _log_ss << message; \
            StudentIntake::Logger::info(component, _log_ss.str()); \
        } \
    } while(0)

#define LOG_WARN(component, message) \
    do { \
        if (StudentIntake::Logger::shouldLog(StudentIntake::LogLevel::WARN)) { \
            std::ostringstream _log_ss; \
            _log_ss << message; \
            StudentIntake::Logger::warn(component, _log_ss.str()); \
        } \
    } while(0)

#define LOG_ERROR(component, message) \
    do { \
        if (StudentIntake::Logger::shouldLog(StudentIntake::LogLevel::ERROR)) { \
            std::ostringstream _log_ss; \
            _log_ss << message; \
            StudentIntake::Logger::error(component, _log_ss.str()); \
        } \
    } while(0)
