#include "SessionManager.h"
#include <algorithm>

namespace StudentIntake {
namespace Session {

SessionManager& SessionManager::getInstance() {
    static SessionManager instance;
    return instance;
}

SessionManager::SessionManager()
    : sessionTimeoutMinutes_(60) {
}

SessionManager::~SessionManager() {
}

std::shared_ptr<StudentSession> SessionManager::createSession() {
    std::lock_guard<std::mutex> lock(mutex_);

    auto session = std::make_shared<StudentSession>();
    sessions_[session->getSessionId()] = session;

    if (onSessionCreated_) {
        onSessionCreated_(session->getSessionId());
    }

    return session;
}

std::shared_ptr<StudentSession> SessionManager::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        return it->second;
    }
    return nullptr;
}

bool SessionManager::hasSession(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.find(sessionId) != sessions_.end();
}

void SessionManager::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        if (onSessionDestroyed_) {
            onSessionDestroyed_(sessionId);
        }
        sessions_.erase(it);
    }
}

void SessionManager::removeExpiredSessions() {
    std::lock_guard<std::mutex> lock(mutex_);

    // In a production environment, you would track last activity time
    // and remove sessions that have been inactive for sessionTimeoutMinutes_
    // For now, this is a placeholder for the cleanup logic
}

std::shared_ptr<StudentSession> SessionManager::getSessionByStudentId(const std::string& studentId) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& pair : sessions_) {
        if (pair.second->getStudent().getId() == studentId) {
            return pair.second;
        }
    }
    return nullptr;
}

std::shared_ptr<StudentSession> SessionManager::getSessionByEmail(const std::string& email) {
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& pair : sessions_) {
        if (pair.second->getStudent().getEmail() == email) {
            return pair.second;
        }
    }
    return nullptr;
}

void SessionManager::setFormTypeInfos(const std::vector<Models::FormTypeInfo>& formTypes) {
    std::lock_guard<std::mutex> lock(mutex_);
    formTypeInfos_ = formTypes;
}

std::vector<Models::FormTypeInfo> SessionManager::getFormTypeInfos() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return formTypeInfos_;
}

Models::FormTypeInfo SessionManager::getFormTypeInfo(const std::string& formId) const {
    std::lock_guard<std::mutex> lock(mutex_);

    for (const auto& info : formTypeInfos_) {
        if (info.id == formId) {
            return info;
        }
    }
    return Models::FormTypeInfo{};
}

std::vector<std::string> SessionManager::calculateRequiredForms(
    const Models::Student& student,
    const Models::Curriculum& curriculum) const {

    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> requiredForms;
    int studentAge = student.getAge();

    for (const auto& formInfo : formTypeInfos_) {
        bool isRequired = false;

        // Check if universally required
        if (formInfo.isRequired) {
            isRequired = true;
        }

        // Check student type requirements
        if (!formInfo.requiredForStudentTypes.empty()) {
            auto it = std::find(formInfo.requiredForStudentTypes.begin(),
                               formInfo.requiredForStudentTypes.end(),
                               student.getStudentType());
            if (it != formInfo.requiredForStudentTypes.end()) {
                isRequired = true;
            }
        }

        // Check curriculum requirements
        if (!formInfo.requiredForCurriculums.empty()) {
            auto it = std::find(formInfo.requiredForCurriculums.begin(),
                               formInfo.requiredForCurriculums.end(),
                               curriculum.getId());
            if (it != formInfo.requiredForCurriculums.end()) {
                isRequired = true;
            }
        }

        // Also check curriculum's own required forms
        if (curriculum.requiresForm(formInfo.id)) {
            isRequired = true;
        }

        // Check international student requirement
        if (formInfo.requiredForInternational && student.isInternational()) {
            isRequired = true;
        }

        // Check transfer student requirement
        if (formInfo.requiredForTransfer && student.isTransferStudent()) {
            isRequired = true;
        }

        // Check veteran requirement
        if (formInfo.requiredForVeteran && student.isVeteran()) {
            isRequired = true;
        }

        // Check financial aid requirement
        if (formInfo.requiredForFinancialAid && student.requiresFinancialAid()) {
            isRequired = true;
        }

        // Check age requirements
        if (formInfo.minAge > 0 && studentAge < formInfo.minAge) {
            continue;  // Student is too young for this form
        }
        if (formInfo.maxAge > 0 && studentAge > formInfo.maxAge) {
            continue;  // Student is too old for this form
        }

        if (isRequired) {
            requiredForms.push_back(formInfo.id);
        }
    }

    // Sort by display order
    std::sort(requiredForms.begin(), requiredForms.end(),
              [this](const std::string& a, const std::string& b) {
                  int orderA = 0, orderB = 0;
                  for (const auto& info : formTypeInfos_) {
                      if (info.id == a) orderA = info.displayOrder;
                      if (info.id == b) orderB = info.displayOrder;
                  }
                  return orderA < orderB;
              });

    return requiredForms;
}

size_t SessionManager::getActiveSessionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}

std::vector<std::string> SessionManager::getActiveSessionIds() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::string> ids;
    for (const auto& pair : sessions_) {
        ids.push_back(pair.first);
    }
    return ids;
}

} // namespace Session
} // namespace StudentIntake
