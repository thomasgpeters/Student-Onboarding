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

    // Get ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            history.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            history.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Get student_id
    if (data.contains("student_id")) {
        if (data["student_id"].is_string()) {
            history.studentId_ = data["student_id"].get<std::string>();
        } else if (data["student_id"].is_number()) {
            history.studentId_ = std::to_string(data["student_id"].get<int>());
        }
    }

    history.institutionName_ = data.value("institution_name", "");
    history.institutionType_ = data.value("institution_type", "");
    history.institutionCity_ = data.value("institution_city", "");
    history.institutionState_ = data.value("institution_state", "");
    history.institutionCountry_ = data.value("institution_country", "");
    history.degreeEarned_ = data.value("degree_earned", "");
    history.major_ = data.value("major", "");
    history.minor_ = data.value("minor", "");

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

    history.startDate_ = data.value("start_date", "");
    history.endDate_ = data.value("end_date", "");
    history.graduationDate_ = data.value("graduation_date", "");
    history.isCurrentlyAttending_ = data.value("is_currently_attending", false);
    history.transcriptReceived_ = data.value("transcript_received", false);

    return history;
}

bool AcademicHistory::isEmpty() const {
    return institutionName_.empty();
}

} // namespace Models
} // namespace StudentIntake
