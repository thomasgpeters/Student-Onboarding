#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief User roles in the system
 */
enum class UserRole {
    Student,
    Instructor,
    Examiner,
    Staff,
    Manager,
    Admin
};

/**
 * @brief Admin permission levels
 */
enum class AdminLevel {
    Staff,
    Manager,
    SuperAdmin
};

/**
 * @brief Represents a user in the unified authentication system
 */
class User {
public:
    User();

    // Getters
    int getId() const { return id_; }
    std::string getEmail() const { return email_; }
    std::string getPasswordHash() const { return passwordHash_; }
    std::string getFirstName() const { return firstName_; }
    std::string getLastName() const { return lastName_; }
    std::string getMiddleName() const { return middleName_; }
    std::string getPreferredName() const { return preferredName_; }
    std::string getPhoneNumber() const { return phoneNumber_; }
    bool isActive() const { return isActive_; }
    bool isLoginEnabled() const { return loginEnabled_; }
    bool isEmailVerified() const { return emailVerified_; }
    std::string getLastLoginAt() const { return lastLoginAt_; }
    int getFailedLoginAttempts() const { return failedLoginAttempts_; }
    std::string getLockedUntil() const { return lockedUntil_; }
    std::string getCreatedAt() const { return createdAt_; }
    std::string getUpdatedAt() const { return updatedAt_; }

    // Role management
    std::vector<UserRole> getRoles() const { return roles_; }
    UserRole getPrimaryRole() const;
    bool hasRole(UserRole role) const;
    bool hasMultipleRoles() const { return roles_.size() > 1; }
    bool isStudent() const { return hasRole(UserRole::Student); }
    bool isInstructor() const { return hasRole(UserRole::Instructor); }
    bool isExaminer() const { return hasRole(UserRole::Examiner); }
    bool isStaff() const { return hasRole(UserRole::Staff); }
    bool isManager() const { return hasRole(UserRole::Manager); }
    bool isAdmin() const { return hasRole(UserRole::Admin); }

    // Setters
    void setId(int id) { id_ = id; }
    void setEmail(const std::string& email) { email_ = email; }
    void setPasswordHash(const std::string& hash) { passwordHash_ = hash; }
    void setFirstName(const std::string& name) { firstName_ = name; }
    void setLastName(const std::string& name) { lastName_ = name; }
    void setMiddleName(const std::string& name) { middleName_ = name; }
    void setPreferredName(const std::string& name) { preferredName_ = name; }
    void setPhoneNumber(const std::string& phone) { phoneNumber_ = phone; }
    void setActive(bool active) { isActive_ = active; }
    void setLoginEnabled(bool enabled) { loginEnabled_ = enabled; }
    void setEmailVerified(bool verified) { emailVerified_ = verified; }
    void setLastLoginAt(const std::string& time) { lastLoginAt_ = time; }
    void setFailedLoginAttempts(int attempts) { failedLoginAttempts_ = attempts; }
    void setLockedUntil(const std::string& time) { lockedUntil_ = time; }
    void setCreatedAt(const std::string& time) { createdAt_ = time; }
    void setUpdatedAt(const std::string& time) { updatedAt_ = time; }

    // Role setters
    void setRoles(const std::vector<UserRole>& roles) { roles_ = roles; }
    void addRole(UserRole role);
    void removeRole(UserRole role);
    void clearRoles() { roles_.clear(); }

    // Utility
    std::string getFullName() const;
    std::string getDisplayName() const;
    bool isLocked() const;
    bool canLogin() const;

    // Serialization
    nlohmann::json toJson() const;
    static User fromJson(const nlohmann::json& json);

    // Role conversion helpers
    static UserRole stringToRole(const std::string& role);
    static std::string roleToString(UserRole role);
    static std::string roleToDisplayString(UserRole role);

    // Route helpers
    std::string getHomeRoute() const;

private:
    int id_;
    std::string email_;
    std::string passwordHash_;
    std::string firstName_;
    std::string lastName_;
    std::string middleName_;
    std::string preferredName_;
    std::string phoneNumber_;
    bool isActive_;
    bool loginEnabled_;
    bool emailVerified_;
    std::string lastLoginAt_;
    int failedLoginAttempts_;
    std::string lockedUntil_;
    std::string createdAt_;
    std::string updatedAt_;
    std::vector<UserRole> roles_;
};

/**
 * @brief Extended profile data for students
 */
class StudentProfile {
public:
    StudentProfile();

    // Getters
    int getId() const { return id_; }
    int getUserId() const { return userId_; }
    std::string getPreferredPronouns() const { return preferredPronouns_; }
    std::string getDateOfBirth() const { return dateOfBirth_; }
    std::string getGender() const { return gender_; }
    std::string getAlternatePhone() const { return alternatePhone_; }
    std::string getAddressLine1() const { return addressLine1_; }
    std::string getAddressLine2() const { return addressLine2_; }
    std::string getCity() const { return city_; }
    std::string getState() const { return state_; }
    std::string getZipCode() const { return zipCode_; }
    std::string getSsn() const { return ssn_; }
    std::string getCitizenshipStatus() const { return citizenshipStatus_; }
    std::string getCitizenshipCountry() const { return citizenshipCountry_; }
    std::string getVisaType() const { return visaType_; }
    int getCurriculumId() const { return curriculumId_; }
    std::string getStudentType() const { return studentType_; }
    std::string getEnrollmentDate() const { return enrollmentDate_; }
    std::string getExpectedGraduation() const { return expectedGraduation_; }
    bool isInternational() const { return isInternational_; }
    bool isTransferStudent() const { return isTransferStudent_; }
    bool isVeteran() const { return isVeteran_; }
    bool requiresFinancialAid() const { return requiresFinancialAid_; }
    std::string getIntakeStatus() const { return intakeStatus_; }
    std::string getCompletedForms() const { return completedForms_; }

    // Setters
    void setId(int id) { id_ = id; }
    void setUserId(int id) { userId_ = id; }
    void setPreferredPronouns(const std::string& pronouns) { preferredPronouns_ = pronouns; }
    void setDateOfBirth(const std::string& dob) { dateOfBirth_ = dob; }
    void setGender(const std::string& gender) { gender_ = gender; }
    void setAlternatePhone(const std::string& phone) { alternatePhone_ = phone; }
    void setAddressLine1(const std::string& addr) { addressLine1_ = addr; }
    void setAddressLine2(const std::string& addr) { addressLine2_ = addr; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setZipCode(const std::string& zip) { zipCode_ = zip; }
    void setSsn(const std::string& ssn) { ssn_ = ssn; }
    void setCitizenshipStatus(const std::string& status) { citizenshipStatus_ = status; }
    void setCitizenshipCountry(const std::string& country) { citizenshipCountry_ = country; }
    void setVisaType(const std::string& type) { visaType_ = type; }
    void setCurriculumId(int id) { curriculumId_ = id; }
    void setStudentType(const std::string& type) { studentType_ = type; }
    void setEnrollmentDate(const std::string& date) { enrollmentDate_ = date; }
    void setExpectedGraduation(const std::string& date) { expectedGraduation_ = date; }
    void setInternational(bool intl) { isInternational_ = intl; }
    void setTransferStudent(bool transfer) { isTransferStudent_ = transfer; }
    void setVeteran(bool veteran) { isVeteran_ = veteran; }
    void setRequiresFinancialAid(bool aid) { requiresFinancialAid_ = aid; }
    void setIntakeStatus(const std::string& status) { intakeStatus_ = status; }
    void setCompletedForms(const std::string& forms) { completedForms_ = forms; }

    // Serialization
    nlohmann::json toJson() const;
    static StudentProfile fromJson(const nlohmann::json& json);

private:
    int id_;
    int userId_;
    std::string preferredPronouns_;
    std::string dateOfBirth_;
    std::string gender_;
    std::string alternatePhone_;
    std::string addressLine1_;
    std::string addressLine2_;
    std::string city_;
    std::string state_;
    std::string zipCode_;
    std::string ssn_;
    std::string citizenshipStatus_;
    std::string citizenshipCountry_;
    std::string visaType_;
    int curriculumId_;
    std::string studentType_;
    std::string enrollmentDate_;
    std::string expectedGraduation_;
    bool isInternational_;
    bool isTransferStudent_;
    bool isVeteran_;
    bool requiresFinancialAid_;
    std::string intakeStatus_;
    std::string completedForms_;
};

/**
 * @brief Extended profile data for admins
 */
class AdminProfile {
public:
    AdminProfile();

    int getId() const { return id_; }
    int getUserId() const { return userId_; }
    std::string getEmployeeId() const { return employeeId_; }
    int getDepartmentId() const { return departmentId_; }
    AdminLevel getAdminLevel() const { return adminLevel_; }
    std::string getAdminLevelString() const;
    bool canManageStudents() const { return canManageStudents_; }
    bool canManageCurriculum() const { return canManageCurriculum_; }
    bool canManageInstructors() const { return canManageInstructors_; }
    bool canManageAdmins() const { return canManageAdmins_; }
    bool canViewReports() const { return canViewReports_; }
    bool canRevokeAccess() const { return canRevokeAccess_; }
    bool isActive() const { return isActive_; }

    void setId(int id) { id_ = id; }
    void setUserId(int id) { userId_ = id; }
    void setEmployeeId(const std::string& id) { employeeId_ = id; }
    void setDepartmentId(int id) { departmentId_ = id; }
    void setAdminLevel(AdminLevel level) { adminLevel_ = level; }
    void setAdminLevelFromString(const std::string& level);
    void setCanManageStudents(bool can) { canManageStudents_ = can; }
    void setCanManageCurriculum(bool can) { canManageCurriculum_ = can; }
    void setCanManageInstructors(bool can) { canManageInstructors_ = can; }
    void setCanManageAdmins(bool can) { canManageAdmins_ = can; }
    void setCanViewReports(bool can) { canViewReports_ = can; }
    void setCanRevokeAccess(bool can) { canRevokeAccess_ = can; }
    void setActive(bool active) { isActive_ = active; }

    nlohmann::json toJson() const;
    static AdminProfile fromJson(const nlohmann::json& json);

    static AdminLevel stringToAdminLevel(const std::string& level);
    static std::string adminLevelToString(AdminLevel level);

private:
    int id_;
    int userId_;
    std::string employeeId_;
    int departmentId_;
    AdminLevel adminLevel_;
    bool canManageStudents_;
    bool canManageCurriculum_;
    bool canManageInstructors_;
    bool canManageAdmins_;
    bool canViewReports_;
    bool canRevokeAccess_;
    bool isActive_;
};

/**
 * @brief User session information
 */
class UserSession {
public:
    UserSession();

    int getId() const { return id_; }
    int getUserId() const { return userId_; }
    std::string getSessionToken() const { return sessionToken_; }
    std::string getRefreshToken() const { return refreshToken_; }
    std::string getIpAddress() const { return ipAddress_; }
    std::string getUserAgent() const { return userAgent_; }
    UserRole getActiveRole() const { return activeRole_; }
    std::string getCreatedAt() const { return createdAt_; }
    std::string getExpiresAt() const { return expiresAt_; }
    std::string getLastActivityAt() const { return lastActivityAt_; }
    bool isActive() const { return isActive_; }

    void setId(int id) { id_ = id; }
    void setUserId(int id) { userId_ = id; }
    void setSessionToken(const std::string& token) { sessionToken_ = token; }
    void setRefreshToken(const std::string& token) { refreshToken_ = token; }
    void setIpAddress(const std::string& ip) { ipAddress_ = ip; }
    void setUserAgent(const std::string& agent) { userAgent_ = agent; }
    void setActiveRole(UserRole role) { activeRole_ = role; }
    void setCreatedAt(const std::string& time) { createdAt_ = time; }
    void setExpiresAt(const std::string& time) { expiresAt_ = time; }
    void setLastActivityAt(const std::string& time) { lastActivityAt_ = time; }
    void setActive(bool active) { isActive_ = active; }

    bool isExpired() const;

    nlohmann::json toJson() const;
    static UserSession fromJson(const nlohmann::json& json);

private:
    int id_;
    int userId_;
    std::string sessionToken_;
    std::string refreshToken_;
    std::string ipAddress_;
    std::string userAgent_;
    UserRole activeRole_;
    std::string createdAt_;
    std::string expiresAt_;
    std::string lastActivityAt_;
    bool isActive_;
};

} // namespace Models
} // namespace StudentIntake

#endif // USER_H
