#ifndef INSTRUCTOR_H
#define INSTRUCTOR_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Instructor type enumeration
 */
enum class InstructorType {
    Instructor,  // CDL Instructor - can teach
    Examiner,    // Certified examiner - can test and issue CDL
    Both         // Can do both
};

/**
 * @brief Represents a CDL instructor or examiner
 */
class Instructor {
public:
    Instructor();
    Instructor(const std::string& id, const std::string& name);

    // Getters
    std::string getId() const { return id_; }
    int getAdminUserId() const { return adminUserId_; }
    std::string getEmployeeId() const { return employeeId_; }
    std::string getName() const { return name_; }
    std::string getEmail() const { return email_; }
    std::string getHireDate() const { return hireDate_; }
    InstructorType getInstructorType() const { return instructorType_; }
    std::string getInstructorTypeString() const;

    // CDL info
    std::string getCdlNumber() const { return cdlNumber_; }
    std::string getCdlClass() const { return cdlClass_; }
    std::string getCdlState() const { return cdlState_; }
    std::string getCdlExpiration() const { return cdlExpiration_; }

    // Examiner info
    bool isCertifiedExaminer() const { return isCertifiedExaminer_; }
    std::string getExaminerCertificationNumber() const { return examinerCertificationNumber_; }
    std::string getExaminerCertificationExpiration() const { return examinerCertificationExpiration_; }
    std::string getExaminerIssuingAuthority() const { return examinerIssuingAuthority_; }

    // Endorsements
    std::vector<std::string> getEndorsedClasses() const { return endorsedClasses_; }
    std::vector<std::string> getEndorsedEndorsements() const { return endorsedEndorsements_; }

    // Permissions
    bool isActive() const { return isActive_; }
    bool canSchedule() const { return canSchedule_; }
    bool canValidate() const { return canValidate_; }
    bool canIssueCdl() const { return canIssueCdl_; }

    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setAdminUserId(int id) { adminUserId_ = id; }
    void setEmployeeId(const std::string& id) { employeeId_ = id; }
    void setName(const std::string& name) { name_ = name; }
    void setEmail(const std::string& email) { email_ = email; }
    void setHireDate(const std::string& date) { hireDate_ = date; }
    void setInstructorType(InstructorType type) { instructorType_ = type; }
    void setInstructorTypeFromString(const std::string& type);

    void setCdlNumber(const std::string& num) { cdlNumber_ = num; }
    void setCdlClass(const std::string& cls) { cdlClass_ = cls; }
    void setCdlState(const std::string& state) { cdlState_ = state; }
    void setCdlExpiration(const std::string& date) { cdlExpiration_ = date; }

    void setCertifiedExaminer(bool certified) { isCertifiedExaminer_ = certified; }
    void setExaminerCertificationNumber(const std::string& num) { examinerCertificationNumber_ = num; }
    void setExaminerCertificationExpiration(const std::string& date) { examinerCertificationExpiration_ = date; }
    void setExaminerIssuingAuthority(const std::string& authority) { examinerIssuingAuthority_ = authority; }

    void setEndorsedClasses(const std::vector<std::string>& classes) { endorsedClasses_ = classes; }
    void setEndorsedEndorsements(const std::vector<std::string>& endorsements) { endorsedEndorsements_ = endorsements; }

    void setActive(bool active) { isActive_ = active; }
    void setCanSchedule(bool can) { canSchedule_ = can; }
    void setCanValidate(bool can) { canValidate_ = can; }
    void setCanIssueCdl(bool can) { canIssueCdl_ = can; }

    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static Instructor fromJson(const nlohmann::json& json);

    // Helper methods
    static InstructorType stringToInstructorType(const std::string& type);
    static std::string instructorTypeToString(InstructorType type);

    // Utility
    bool canTeachClass(const std::string& cdlClass) const;
    bool canTeachEndorsement(const std::string& endorsement) const;

private:
    std::string id_;
    int adminUserId_;
    std::string employeeId_;
    std::string name_;
    std::string email_;
    std::string hireDate_;
    InstructorType instructorType_;

    // CDL info
    std::string cdlNumber_;
    std::string cdlClass_;
    std::string cdlState_;
    std::string cdlExpiration_;

    // Examiner info
    bool isCertifiedExaminer_;
    std::string examinerCertificationNumber_;
    std::string examinerCertificationExpiration_;
    std::string examinerIssuingAuthority_;

    // Endorsements
    std::vector<std::string> endorsedClasses_;
    std::vector<std::string> endorsedEndorsements_;

    // Permissions
    bool isActive_;
    bool canSchedule_;
    bool canValidate_;
    bool canIssueCdl_;

    std::string notes_;
};

/**
 * @brief Represents an instructor qualification/certification
 */
class InstructorQualification {
public:
    InstructorQualification();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    std::string getQualificationType() const { return qualificationType_; }
    std::string getQualificationName() const { return qualificationName_; }
    std::string getIssuingAuthority() const { return issuingAuthority_; }
    std::string getCertificationNumber() const { return certificationNumber_; }
    std::string getIssuedDate() const { return issuedDate_; }
    std::string getExpirationDate() const { return expirationDate_; }
    bool isActive() const { return isActive_; }
    bool isVerified() const { return isVerified_; }
    std::string getDocumentPath() const { return documentPath_; }
    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setQualificationType(const std::string& type) { qualificationType_ = type; }
    void setQualificationName(const std::string& name) { qualificationName_ = name; }
    void setIssuingAuthority(const std::string& authority) { issuingAuthority_ = authority; }
    void setCertificationNumber(const std::string& num) { certificationNumber_ = num; }
    void setIssuedDate(const std::string& date) { issuedDate_ = date; }
    void setExpirationDate(const std::string& date) { expirationDate_ = date; }
    void setActive(bool active) { isActive_ = active; }
    void setVerified(bool verified) { isVerified_ = verified; }
    void setDocumentPath(const std::string& path) { documentPath_ = path; }
    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static InstructorQualification fromJson(const nlohmann::json& json);

    // Utility
    bool isExpired() const;

private:
    std::string id_;
    int instructorId_;
    std::string qualificationType_;
    std::string qualificationName_;
    std::string issuingAuthority_;
    std::string certificationNumber_;
    std::string issuedDate_;
    std::string expirationDate_;
    bool isActive_;
    bool isVerified_;
    std::string documentPath_;
    std::string notes_;
};

/**
 * @brief Represents an instructor assignment to a student or course
 */
class InstructorAssignment {
public:
    InstructorAssignment();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    int getStudentId() const { return studentId_; }
    int getCourseId() const { return courseId_; }
    int getEnrollmentId() const { return enrollmentId_; }
    std::string getAssignmentType() const { return assignmentType_; }
    std::string getAssignmentScope() const { return assignmentScope_; }
    std::string getEffectiveFrom() const { return effectiveFrom_; }
    std::string getEffectiveUntil() const { return effectiveUntil_; }
    bool isActive() const { return isActive_; }
    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setStudentId(int id) { studentId_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setEnrollmentId(int id) { enrollmentId_ = id; }
    void setAssignmentType(const std::string& type) { assignmentType_ = type; }
    void setAssignmentScope(const std::string& scope) { assignmentScope_ = scope; }
    void setEffectiveFrom(const std::string& date) { effectiveFrom_ = date; }
    void setEffectiveUntil(const std::string& date) { effectiveUntil_ = date; }
    void setActive(bool active) { isActive_ = active; }
    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static InstructorAssignment fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int instructorId_;
    int studentId_;
    int courseId_;
    int enrollmentId_;
    std::string assignmentType_;
    std::string assignmentScope_;
    std::string effectiveFrom_;
    std::string effectiveUntil_;
    bool isActive_;
    std::string notes_;
};

/**
 * @brief Represents instructor availability for scheduling
 */
class InstructorAvailability {
public:
    InstructorAvailability();

    // Getters
    std::string getId() const { return id_; }
    int getInstructorId() const { return instructorId_; }
    int getDayOfWeek() const { return dayOfWeek_; }
    std::string getSpecificDate() const { return specificDate_; }
    std::string getStartTime() const { return startTime_; }
    std::string getEndTime() const { return endTime_; }
    std::string getAvailabilityType() const { return availabilityType_; }
    std::string getPreferredLocation() const { return preferredLocation_; }
    std::string getNotes() const { return notes_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setInstructorId(int id) { instructorId_ = id; }
    void setDayOfWeek(int day) { dayOfWeek_ = day; }
    void setSpecificDate(const std::string& date) { specificDate_ = date; }
    void setStartTime(const std::string& time) { startTime_ = time; }
    void setEndTime(const std::string& time) { endTime_ = time; }
    void setAvailabilityType(const std::string& type) { availabilityType_ = type; }
    void setPreferredLocation(const std::string& location) { preferredLocation_ = location; }
    void setNotes(const std::string& notes) { notes_ = notes; }

    // Serialization
    nlohmann::json toJson() const;
    static InstructorAvailability fromJson(const nlohmann::json& json);

    // Utility
    bool isRecurring() const { return dayOfWeek_ >= 0; }
    std::string getDayOfWeekName() const;

private:
    std::string id_;
    int instructorId_;
    int dayOfWeek_;  // -1 for specific date, 0-6 for Sun-Sat
    std::string specificDate_;
    std::string startTime_;
    std::string endTime_;
    std::string availabilityType_;
    std::string preferredLocation_;
    std::string notes_;
};

} // namespace Models
} // namespace StudentIntake

#endif // INSTRUCTOR_H
