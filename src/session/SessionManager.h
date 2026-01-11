#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <functional>
#include "StudentSession.h"
#include "models/FormData.h"

namespace StudentIntake {
namespace Session {

/**
 * @brief Manages all active student sessions
 * Thread-safe singleton for session management
 */
class SessionManager {
public:
    // Singleton access
    static SessionManager& getInstance();

    // Prevent copying
    SessionManager(const SessionManager&) = delete;
    SessionManager& operator=(const SessionManager&) = delete;

    // Session lifecycle
    std::shared_ptr<StudentSession> createSession();
    std::shared_ptr<StudentSession> getSession(const std::string& sessionId);
    bool hasSession(const std::string& sessionId) const;
    void removeSession(const std::string& sessionId);
    void removeExpiredSessions();

    // Session lookup
    std::shared_ptr<StudentSession> getSessionByStudentId(const std::string& studentId);
    std::shared_ptr<StudentSession> getSessionByEmail(const std::string& email);

    // Form configuration
    void setFormTypeInfos(const std::vector<Models::FormTypeInfo>& formTypes);
    std::vector<Models::FormTypeInfo> getFormTypeInfos() const;
    Models::FormTypeInfo getFormTypeInfo(const std::string& formId) const;

    // Required forms calculation based on student data
    std::vector<std::string> calculateRequiredForms(const Models::Student& student,
                                                     const Models::Curriculum& curriculum) const;

    // Statistics
    size_t getActiveSessionCount() const;
    std::vector<std::string> getActiveSessionIds() const;

    // Configuration
    void setSessionTimeout(int minutes) { sessionTimeoutMinutes_ = minutes; }
    int getSessionTimeout() const { return sessionTimeoutMinutes_; }

    // Callbacks for session events
    using SessionCallback = std::function<void(const std::string&)>;
    void setOnSessionCreated(SessionCallback callback) { onSessionCreated_ = callback; }
    void setOnSessionDestroyed(SessionCallback callback) { onSessionDestroyed_ = callback; }

private:
    SessionManager();
    ~SessionManager();

    std::map<std::string, std::shared_ptr<StudentSession>> sessions_;
    std::vector<Models::FormTypeInfo> formTypeInfos_;
    mutable std::mutex mutex_;
    int sessionTimeoutMinutes_;
    SessionCallback onSessionCreated_;
    SessionCallback onSessionDestroyed_;
};

} // namespace Session
} // namespace StudentIntake

#endif // SESSION_MANAGER_H
