#include "StudentSession.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>

namespace StudentIntake {
namespace Session {

namespace {
    std::string generateSessionId() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < 32; ++i) {
            ss << dis(gen);
        }
        return ss.str();
    }
}

StudentSession::StudentSession()
    : sessionId_(generateSessionId())
    , isLoggedIn_(false)
    , authToken_("")
    , currentFormId_("") {
}

StudentSession::~StudentSession() {
}

void StudentSession::setFormData(const std::string& formId, const Models::FormData& data) {
    Models::FormData mutableData = data;
    mutableData.setSessionId(sessionId_);
    formDataCache_[formId] = mutableData;
}

Models::FormData StudentSession::getFormData(const std::string& formId) const {
    auto it = formDataCache_.find(formId);
    if (it != formDataCache_.end()) {
        return it->second;
    }
    return Models::FormData(formId, student_.getId());
}

bool StudentSession::hasFormData(const std::string& formId) const {
    return formDataCache_.find(formId) != formDataCache_.end();
}

void StudentSession::clearFormData(const std::string& formId) {
    formDataCache_.erase(formId);
}

void StudentSession::clearAllFormData() {
    formDataCache_.clear();
}

std::vector<std::string> StudentSession::getFormDataIds() const {
    std::vector<std::string> ids;
    for (const auto& pair : formDataCache_) {
        ids.push_back(pair.first);
    }
    return ids;
}

void StudentSession::addRequiredFormId(const std::string& formId) {
    if (!isFormRequired(formId)) {
        requiredFormIds_.push_back(formId);
    }
}

bool StudentSession::isFormRequired(const std::string& formId) const {
    return std::find(requiredFormIds_.begin(), requiredFormIds_.end(), formId)
           != requiredFormIds_.end();
}

int StudentSession::getCompletedFormsCount() const {
    int count = 0;
    for (const auto& formId : requiredFormIds_) {
        if (student_.hasCompletedForm(formId)) {
            count++;
        }
    }
    return count;
}

int StudentSession::getTotalRequiredFormsCount() const {
    return static_cast<int>(requiredFormIds_.size());
}

double StudentSession::getProgressPercentage() const {
    if (requiredFormIds_.empty()) {
        return 0.0;
    }
    return (static_cast<double>(getCompletedFormsCount()) /
            static_cast<double>(getTotalRequiredFormsCount())) * 100.0;
}

bool StudentSession::isIntakeComplete() const {
    if (requiredFormIds_.empty()) {
        return false;
    }
    return getCompletedFormsCount() == getTotalRequiredFormsCount();
}

void StudentSession::reset() {
    sessionId_ = generateSessionId();
    isLoggedIn_ = false;
    authToken_.clear();
    student_ = Models::Student();
    currentCurriculum_ = Models::Curriculum();
    formDataCache_.clear();
    requiredFormIds_.clear();
    currentFormId_.clear();
}

void StudentSession::logout() {
    isLoggedIn_ = false;
    authToken_.clear();
    // Keep student data for potential re-login
    formDataCache_.clear();
    currentFormId_.clear();
}

} // namespace Session
} // namespace StudentIntake
