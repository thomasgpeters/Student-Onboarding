#include "AcademicHistory.h"
#include <iostream>

namespace StudentIntake {
namespace Models {

AcademicHistory::AcademicHistory()
    : gpa_(0.0)
    , gpaScale_(4.0)
    , isCurrentlyAttending_(false)
    , transcriptReceived_(false) {
}

AcademicHistory::AcademicHistory(const std::string& studentId)
    : studentId_(studentId)
    , gpa_(0.0)
    , gpaScale_(4.0)
    , isCurrentlyAttending_(false)
    , transcriptReceived_(false) {
}

nlohmann::json AcademicHistory::toJson() const {
    nlohmann::json json;

    // Compound primary key fields
    // student_id as integer for database
    if (!studentId_.empty()) {
        try {
            json["student_id"] = std::stoi(studentId_);
        } catch (const std::exception&) {
            json["student_id"] = studentId_;
        }
    }

    json["institution_name"] = institutionName_;
    json["institution_type"] = institutionType_;

    // Other fields
    json["institution_city"] = institutionCity_;
    json["institution_state"] = institutionState_;
    json["institution_country"] = institutionCountry_;
    json["degree_earned"] = degreeEarned_;
    json["major"] = major_;
    json["minor"] = minor_;

    if (gpa_ > 0) {
        json["gpa"] = gpa_;
    }
    json["gpa_scale"] = gpaScale_;

    if (!startDate_.empty()) {
        json["start_date"] = startDate_;
    }
    if (!endDate_.empty()) {
        json["end_date"] = endDate_;
    }
    if (!graduationDate_.empty()) {
        json["graduation_date"] = graduationDate_;
    }

    json["is_currently_attending"] = isCurrentlyAttending_;
    json["transcript_received"] = transcriptReceived_;

    return json;
}

AcademicHistory AcademicHistory::fromJson(const nlohmann::json& json) {
    AcademicHistory history;

    // Handle JSON:API format (with attributes wrapper) or flat JSON
    const nlohmann::json& data = json.contains("attributes") ? json["attributes"] : json;

    // Get student_id (part of compound key)
    if (data.contains("student_id")) {
        if (data["student_id"].is_string()) {
            history.studentId_ = data["student_id"].get<std::string>();
        } else if (data["student_id"].is_number()) {
            history.studentId_ = std::to_string(data["student_id"].get<int>());
        }
    }

    // Helper lambda to safely get string value (handles null)
    auto safeGetString = [&data](const std::string& key) -> std::string {
        if (data.contains(key) && data[key].is_string()) {
            return data[key].get<std::string>();
        }
        return "";
    };

    // Compound key fields
    history.institutionName_ = safeGetString("institution_name");
    history.institutionType_ = safeGetString("institution_type");

    // Other fields
    history.institutionCity_ = safeGetString("institution_city");
    history.institutionState_ = safeGetString("institution_state");
    history.institutionCountry_ = safeGetString("institution_country");
    history.degreeEarned_ = safeGetString("degree_earned");
    history.major_ = safeGetString("major");
    history.minor_ = safeGetString("minor");
    history.startDate_ = safeGetString("start_date");
    history.endDate_ = safeGetString("end_date");
    history.graduationDate_ = safeGetString("graduation_date");

    if (data.contains("gpa") && !data["gpa"].is_null()) {
        if (data["gpa"].is_number()) {
            history.gpa_ = data["gpa"].get<double>();
        } else if (data["gpa"].is_string()) {
            try {
                history.gpa_ = std::stod(data["gpa"].get<std::string>());
            } catch (...) {
                history.gpa_ = 0.0;
            }
        }
    }

    if (data.contains("gpa_scale") && !data["gpa_scale"].is_null()) {
        if (data["gpa_scale"].is_number()) {
            history.gpaScale_ = data["gpa_scale"].get<double>();
        }
    }

    if (data.contains("is_currently_attending") && data["is_currently_attending"].is_boolean()) {
        history.isCurrentlyAttending_ = data["is_currently_attending"].get<bool>();
    }

    if (data.contains("transcript_received") && data["transcript_received"].is_boolean()) {
        history.transcriptReceived_ = data["transcript_received"].get<bool>();
    }

    return history;
}

bool AcademicHistory::isEmpty() const {
    return institutionName_.empty();
}

bool AcademicHistory::hasValidKey() const {
    return !studentId_.empty() && !institutionName_.empty() && !institutionType_.empty();
}

std::string AcademicHistory::getCompoundKey() const {
    return studentId_ + "|" + institutionName_ + "|" + institutionType_;
}

} // namespace Models
} // namespace StudentIntake
