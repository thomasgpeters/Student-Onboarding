#ifndef STUDENT_SESSION_H
#define STUDENT_SESSION_H

#include <string>
#include <memory>
#include <map>
#include <Wt/Auth/Login.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>
#include "models/Student.h"
#include "models/FormData.h"
#include "models/Curriculum.h"

namespace StudentIntake {
namespace Session {

/**
 * @brief Manages the session state for a single student
 */
class StudentSession {
public:
    StudentSession();
    ~StudentSession();

    // Session identification
    std::string getSessionId() const { return sessionId_; }
    void setSessionId(const std::string& id) { sessionId_ = id; }

    // Authentication state
    bool isLoggedIn() const { return isLoggedIn_; }
    void setLoggedIn(bool loggedIn) { isLoggedIn_ = loggedIn; }

    std::string getAuthToken() const { return authToken_; }
    void setAuthToken(const std::string& token) { authToken_ = token; }

    // Student data
    Models::Student& getStudent() { return student_; }
    const Models::Student& getStudent() const { return student_; }
    void setStudent(const Models::Student& student) { student_ = student; }

    // Curriculum
    Models::Curriculum& getCurrentCurriculum() { return currentCurriculum_; }
    const Models::Curriculum& getCurrentCurriculum() const { return currentCurriculum_; }
    void setCurrentCurriculum(const Models::Curriculum& curriculum) { currentCurriculum_ = curriculum; }
    bool hasCurriculumSelected() const { return !currentCurriculum_.getId().empty(); }

    // Form data management
    void setFormData(const std::string& formId, const Models::FormData& data);
    Models::FormData getFormData(const std::string& formId) const;
    bool hasFormData(const std::string& formId) const;
    void clearFormData(const std::string& formId);
    void clearAllFormData();
    std::vector<std::string> getFormDataIds() const;

    // Required forms based on student data
    std::vector<std::string> getRequiredFormIds() const { return requiredFormIds_; }
    void setRequiredFormIds(const std::vector<std::string>& formIds) { requiredFormIds_ = formIds; }
    void addRequiredFormId(const std::string& formId);
    bool isFormRequired(const std::string& formId) const;

    // Current form navigation
    std::string getCurrentFormId() const { return currentFormId_; }
    void setCurrentFormId(const std::string& formId) { currentFormId_ = formId; }

    // Progress tracking
    int getCompletedFormsCount() const;
    int getTotalRequiredFormsCount() const;
    double getProgressPercentage() const;
    bool isIntakeComplete() const;

    // Session state
    void reset();
    void logout();

private:
    std::string sessionId_;
    bool isLoggedIn_;
    std::string authToken_;
    Models::Student student_;
    Models::Curriculum currentCurriculum_;
    std::map<std::string, Models::FormData> formDataCache_;
    std::vector<std::string> requiredFormIds_;
    std::string currentFormId_;
};

} // namespace Session
} // namespace StudentIntake

#endif // STUDENT_SESSION_H
