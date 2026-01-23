#include "Instructor.h"
#include <algorithm>
#include <ctime>
#include <sstream>

namespace StudentIntake {
namespace Models {

// =============================================================================
// Instructor Implementation
// =============================================================================

Instructor::Instructor()
    : id_("")
    , adminUserId_(0)
    , employeeId_("")
    , name_("")
    , email_("")
    , hireDate_("")
    , instructorType_(InstructorType::Instructor)
    , cdlNumber_("")
    , cdlClass_("")
    , cdlState_("")
    , cdlExpiration_("")
    , isCertifiedExaminer_(false)
    , examinerCertificationNumber_("")
    , examinerCertificationExpiration_("")
    , examinerIssuingAuthority_("")
    , isActive_(true)
    , canSchedule_(true)
    , canValidate_(true)
    , canIssueCdl_(false)
    , notes_("") {
}

Instructor::Instructor(const std::string& id, const std::string& name)
    : id_(id)
    , adminUserId_(0)
    , employeeId_("")
    , name_(name)
    , email_("")
    , hireDate_("")
    , instructorType_(InstructorType::Instructor)
    , cdlNumber_("")
    , cdlClass_("")
    , cdlState_("")
    , cdlExpiration_("")
    , isCertifiedExaminer_(false)
    , examinerCertificationNumber_("")
    , examinerCertificationExpiration_("")
    , examinerIssuingAuthority_("")
    , isActive_(true)
    , canSchedule_(true)
    , canValidate_(true)
    , canIssueCdl_(false)
    , notes_("") {
}

std::string Instructor::getInstructorTypeString() const {
    return instructorTypeToString(instructorType_);
}

void Instructor::setInstructorTypeFromString(const std::string& type) {
    instructorType_ = stringToInstructorType(type);
}

InstructorType Instructor::stringToInstructorType(const std::string& type) {
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "instructor") return InstructorType::Instructor;
    if (lower == "examiner") return InstructorType::Examiner;
    if (lower == "both") return InstructorType::Both;
    return InstructorType::Instructor;
}

std::string Instructor::instructorTypeToString(InstructorType type) {
    switch (type) {
        case InstructorType::Instructor: return "instructor";
        case InstructorType::Examiner: return "examiner";
        case InstructorType::Both: return "both";
        default: return "instructor";
    }
}

bool Instructor::canTeachClass(const std::string& cdlClass) const {
    return std::find(endorsedClasses_.begin(), endorsedClasses_.end(), cdlClass)
           != endorsedClasses_.end();
}

bool Instructor::canTeachEndorsement(const std::string& endorsement) const {
    return std::find(endorsedEndorsements_.begin(), endorsedEndorsements_.end(), endorsement)
           != endorsedEndorsements_.end();
}

nlohmann::json Instructor::toJson() const {
    nlohmann::json j;
    j["admin_user_id"] = adminUserId_;
    j["employee_id"] = employeeId_;
    j["instructor_type"] = instructorTypeToString(instructorType_);
    j["cdl_number"] = cdlNumber_;
    j["cdl_class"] = cdlClass_;
    j["cdl_state"] = cdlState_;
    if (!cdlExpiration_.empty()) j["cdl_expiration"] = cdlExpiration_;
    j["is_certified_examiner"] = isCertifiedExaminer_;
    if (!examinerCertificationNumber_.empty()) j["examiner_certification_number"] = examinerCertificationNumber_;
    if (!examinerCertificationExpiration_.empty()) j["examiner_certification_expiration"] = examinerCertificationExpiration_;
    if (!examinerIssuingAuthority_.empty()) j["examiner_issuing_authority"] = examinerIssuingAuthority_;
    j["endorsed_classes"] = endorsedClasses_;
    j["endorsed_endorsements"] = endorsedEndorsements_;
    j["is_active"] = isActive_;
    j["can_schedule"] = canSchedule_;
    j["can_validate"] = canValidate_;
    j["can_issue_cdl"] = canIssueCdl_;
    if (!notes_.empty()) j["notes"] = notes_;
    if (!hireDate_.empty()) j["hire_date"] = hireDate_;
    return j;
}

Instructor Instructor::fromJson(const nlohmann::json& json) {
    Instructor instructor;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            instructor.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            instructor.id_ = std::to_string(json["id"].get<int>());
        }
    }

    auto parseIntField = [&attrs](const std::string& field) -> int {
        if (attrs.contains(field) && !attrs[field].is_null()) {
            if (attrs[field].is_number())
                return attrs[field].get<int>();
            else if (attrs[field].is_string())
                return std::stoi(attrs[field].get<std::string>());
        }
        return 0;
    };

    instructor.adminUserId_ = parseIntField("admin_user_id");

    // String fields
    if (attrs.contains("employee_id") && !attrs["employee_id"].is_null())
        instructor.employeeId_ = attrs["employee_id"].get<std::string>();
    if (attrs.contains("name") && !attrs["name"].is_null())
        instructor.name_ = attrs["name"].get<std::string>();
    if (attrs.contains("email") && !attrs["email"].is_null())
        instructor.email_ = attrs["email"].get<std::string>();
    if (attrs.contains("hire_date") && !attrs["hire_date"].is_null())
        instructor.hireDate_ = attrs["hire_date"].get<std::string>();
    if (attrs.contains("instructor_type") && !attrs["instructor_type"].is_null())
        instructor.instructorType_ = stringToInstructorType(attrs["instructor_type"].get<std::string>());

    // CDL fields
    if (attrs.contains("cdl_number") && !attrs["cdl_number"].is_null())
        instructor.cdlNumber_ = attrs["cdl_number"].get<std::string>();
    if (attrs.contains("cdl_class") && !attrs["cdl_class"].is_null())
        instructor.cdlClass_ = attrs["cdl_class"].get<std::string>();
    if (attrs.contains("cdl_state") && !attrs["cdl_state"].is_null())
        instructor.cdlState_ = attrs["cdl_state"].get<std::string>();
    if (attrs.contains("cdl_expiration") && !attrs["cdl_expiration"].is_null())
        instructor.cdlExpiration_ = attrs["cdl_expiration"].get<std::string>();

    // Examiner fields
    if (attrs.contains("is_certified_examiner") && !attrs["is_certified_examiner"].is_null())
        instructor.isCertifiedExaminer_ = attrs["is_certified_examiner"].get<bool>();
    if (attrs.contains("examiner_certification_number") && !attrs["examiner_certification_number"].is_null())
        instructor.examinerCertificationNumber_ = attrs["examiner_certification_number"].get<std::string>();
    if (attrs.contains("examiner_certification_expiration") && !attrs["examiner_certification_expiration"].is_null())
        instructor.examinerCertificationExpiration_ = attrs["examiner_certification_expiration"].get<std::string>();
    if (attrs.contains("examiner_issuing_authority") && !attrs["examiner_issuing_authority"].is_null())
        instructor.examinerIssuingAuthority_ = attrs["examiner_issuing_authority"].get<std::string>();

    // Arrays
    if (attrs.contains("endorsed_classes") && !attrs["endorsed_classes"].is_null())
        instructor.endorsedClasses_ = attrs["endorsed_classes"].get<std::vector<std::string>>();
    if (attrs.contains("endorsed_endorsements") && !attrs["endorsed_endorsements"].is_null())
        instructor.endorsedEndorsements_ = attrs["endorsed_endorsements"].get<std::vector<std::string>>();

    // Permissions
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        instructor.isActive_ = attrs["is_active"].get<bool>();
    if (attrs.contains("can_schedule") && !attrs["can_schedule"].is_null())
        instructor.canSchedule_ = attrs["can_schedule"].get<bool>();
    if (attrs.contains("can_validate") && !attrs["can_validate"].is_null())
        instructor.canValidate_ = attrs["can_validate"].get<bool>();
    if (attrs.contains("can_issue_cdl") && !attrs["can_issue_cdl"].is_null())
        instructor.canIssueCdl_ = attrs["can_issue_cdl"].get<bool>();

    if (attrs.contains("notes") && !attrs["notes"].is_null())
        instructor.notes_ = attrs["notes"].get<std::string>();

    return instructor;
}

// =============================================================================
// InstructorQualification Implementation
// =============================================================================

InstructorQualification::InstructorQualification()
    : id_("")
    , instructorId_(0)
    , qualificationType_("")
    , qualificationName_("")
    , issuingAuthority_("")
    , certificationNumber_("")
    , issuedDate_("")
    , expirationDate_("")
    , isActive_(true)
    , isVerified_(false)
    , documentPath_("")
    , notes_("") {
}

bool InstructorQualification::isExpired() const {
    if (expirationDate_.empty()) return false;

    // Simple date comparison (assumes YYYY-MM-DD format)
    time_t now = time(nullptr);
    struct tm* ltm = localtime(&now);
    char today[11];
    strftime(today, sizeof(today), "%Y-%m-%d", ltm);

    return expirationDate_ < std::string(today);
}

nlohmann::json InstructorQualification::toJson() const {
    nlohmann::json j;
    j["instructor_id"] = instructorId_;
    j["qualification_type"] = qualificationType_;
    j["qualification_name"] = qualificationName_;
    if (!issuingAuthority_.empty()) j["issuing_authority"] = issuingAuthority_;
    if (!certificationNumber_.empty()) j["certification_number"] = certificationNumber_;
    if (!issuedDate_.empty()) j["issued_date"] = issuedDate_;
    if (!expirationDate_.empty()) j["expiration_date"] = expirationDate_;
    j["is_active"] = isActive_;
    j["is_verified"] = isVerified_;
    if (!documentPath_.empty()) j["document_path"] = documentPath_;
    if (!notes_.empty()) j["notes"] = notes_;
    return j;
}

InstructorQualification InstructorQualification::fromJson(const nlohmann::json& json) {
    InstructorQualification qual;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            qual.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            qual.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (attrs.contains("instructor_id") && !attrs["instructor_id"].is_null()) {
        if (attrs["instructor_id"].is_number())
            qual.instructorId_ = attrs["instructor_id"].get<int>();
        else if (attrs["instructor_id"].is_string())
            qual.instructorId_ = std::stoi(attrs["instructor_id"].get<std::string>());
    }

    if (attrs.contains("qualification_type") && !attrs["qualification_type"].is_null())
        qual.qualificationType_ = attrs["qualification_type"].get<std::string>();
    if (attrs.contains("qualification_name") && !attrs["qualification_name"].is_null())
        qual.qualificationName_ = attrs["qualification_name"].get<std::string>();
    if (attrs.contains("issuing_authority") && !attrs["issuing_authority"].is_null())
        qual.issuingAuthority_ = attrs["issuing_authority"].get<std::string>();
    if (attrs.contains("certification_number") && !attrs["certification_number"].is_null())
        qual.certificationNumber_ = attrs["certification_number"].get<std::string>();
    if (attrs.contains("issued_date") && !attrs["issued_date"].is_null())
        qual.issuedDate_ = attrs["issued_date"].get<std::string>();
    if (attrs.contains("expiration_date") && !attrs["expiration_date"].is_null())
        qual.expirationDate_ = attrs["expiration_date"].get<std::string>();
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        qual.isActive_ = attrs["is_active"].get<bool>();
    if (attrs.contains("is_verified") && !attrs["is_verified"].is_null())
        qual.isVerified_ = attrs["is_verified"].get<bool>();
    if (attrs.contains("document_path") && !attrs["document_path"].is_null())
        qual.documentPath_ = attrs["document_path"].get<std::string>();
    if (attrs.contains("notes") && !attrs["notes"].is_null())
        qual.notes_ = attrs["notes"].get<std::string>();

    return qual;
}

// =============================================================================
// InstructorAssignment Implementation
// =============================================================================

InstructorAssignment::InstructorAssignment()
    : id_("")
    , instructorId_(0)
    , studentId_(0)
    , courseId_(0)
    , enrollmentId_(0)
    , assignmentType_("primary")
    , assignmentScope_("full")
    , effectiveFrom_("")
    , effectiveUntil_("")
    , isActive_(true)
    , notes_("") {
}

nlohmann::json InstructorAssignment::toJson() const {
    nlohmann::json j;
    j["instructor_id"] = instructorId_;
    if (studentId_ > 0) j["student_id"] = studentId_;
    if (courseId_ > 0) j["course_id"] = courseId_;
    if (enrollmentId_ > 0) j["enrollment_id"] = enrollmentId_;
    j["assignment_type"] = assignmentType_;
    j["assignment_scope"] = assignmentScope_;
    if (!effectiveFrom_.empty()) j["effective_from"] = effectiveFrom_;
    if (!effectiveUntil_.empty()) j["effective_until"] = effectiveUntil_;
    j["is_active"] = isActive_;
    if (!notes_.empty()) j["notes"] = notes_;
    return j;
}

InstructorAssignment InstructorAssignment::fromJson(const nlohmann::json& json) {
    InstructorAssignment assignment;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            assignment.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            assignment.id_ = std::to_string(json["id"].get<int>());
        }
    }

    auto parseIntField = [&attrs](const std::string& field) -> int {
        if (attrs.contains(field) && !attrs[field].is_null()) {
            if (attrs[field].is_number())
                return attrs[field].get<int>();
            else if (attrs[field].is_string())
                return std::stoi(attrs[field].get<std::string>());
        }
        return 0;
    };

    assignment.instructorId_ = parseIntField("instructor_id");
    assignment.studentId_ = parseIntField("student_id");
    assignment.courseId_ = parseIntField("course_id");
    assignment.enrollmentId_ = parseIntField("enrollment_id");

    if (attrs.contains("assignment_type") && !attrs["assignment_type"].is_null())
        assignment.assignmentType_ = attrs["assignment_type"].get<std::string>();
    if (attrs.contains("assignment_scope") && !attrs["assignment_scope"].is_null())
        assignment.assignmentScope_ = attrs["assignment_scope"].get<std::string>();
    if (attrs.contains("effective_from") && !attrs["effective_from"].is_null())
        assignment.effectiveFrom_ = attrs["effective_from"].get<std::string>();
    if (attrs.contains("effective_until") && !attrs["effective_until"].is_null())
        assignment.effectiveUntil_ = attrs["effective_until"].get<std::string>();
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        assignment.isActive_ = attrs["is_active"].get<bool>();
    if (attrs.contains("notes") && !attrs["notes"].is_null())
        assignment.notes_ = attrs["notes"].get<std::string>();

    return assignment;
}

// =============================================================================
// InstructorAvailability Implementation
// =============================================================================

InstructorAvailability::InstructorAvailability()
    : id_("")
    , instructorId_(0)
    , dayOfWeek_(-1)
    , specificDate_("")
    , startTime_("")
    , endTime_("")
    , availabilityType_("available")
    , preferredLocation_("")
    , notes_("") {
}

std::string InstructorAvailability::getDayOfWeekName() const {
    static const std::vector<std::string> days = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
    };
    if (dayOfWeek_ >= 0 && dayOfWeek_ < 7) {
        return days[dayOfWeek_];
    }
    return "";
}

nlohmann::json InstructorAvailability::toJson() const {
    nlohmann::json j;
    j["instructor_id"] = instructorId_;
    if (dayOfWeek_ >= 0) j["day_of_week"] = dayOfWeek_;
    if (!specificDate_.empty()) j["specific_date"] = specificDate_;
    j["start_time"] = startTime_;
    j["end_time"] = endTime_;
    j["availability_type"] = availabilityType_;
    if (!preferredLocation_.empty()) j["preferred_location"] = preferredLocation_;
    if (!notes_.empty()) j["notes"] = notes_;
    return j;
}

InstructorAvailability InstructorAvailability::fromJson(const nlohmann::json& json) {
    InstructorAvailability avail;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    if (json.contains("id")) {
        if (json["id"].is_string()) {
            avail.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            avail.id_ = std::to_string(json["id"].get<int>());
        }
    }

    if (attrs.contains("instructor_id") && !attrs["instructor_id"].is_null()) {
        if (attrs["instructor_id"].is_number())
            avail.instructorId_ = attrs["instructor_id"].get<int>();
        else if (attrs["instructor_id"].is_string())
            avail.instructorId_ = std::stoi(attrs["instructor_id"].get<std::string>());
    }

    if (attrs.contains("day_of_week") && !attrs["day_of_week"].is_null())
        avail.dayOfWeek_ = attrs["day_of_week"].get<int>();
    if (attrs.contains("specific_date") && !attrs["specific_date"].is_null())
        avail.specificDate_ = attrs["specific_date"].get<std::string>();
    if (attrs.contains("start_time") && !attrs["start_time"].is_null())
        avail.startTime_ = attrs["start_time"].get<std::string>();
    if (attrs.contains("end_time") && !attrs["end_time"].is_null())
        avail.endTime_ = attrs["end_time"].get<std::string>();
    if (attrs.contains("availability_type") && !attrs["availability_type"].is_null())
        avail.availabilityType_ = attrs["availability_type"].get<std::string>();
    if (attrs.contains("preferred_location") && !attrs["preferred_location"].is_null())
        avail.preferredLocation_ = attrs["preferred_location"].get<std::string>();
    if (attrs.contains("notes") && !attrs["notes"].is_null())
        avail.notes_ = attrs["notes"].get<std::string>();

    return avail;
}

} // namespace Models
} // namespace StudentIntake
