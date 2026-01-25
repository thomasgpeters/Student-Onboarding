#include "Course.h"
#include <algorithm>

namespace StudentIntake {
namespace Models {

// =============================================================================
// Course Implementation
// =============================================================================

Course::Course()
    : id_("")
    , curriculumId_(0)
    , code_("")
    , name_("")
    , description_("")
    , totalModules_(0)
    , estimatedHours_(0)
    , passingScore_(70)
    , requiresSequentialCompletion_(true)
    , requiresTimeTracking_(true)
    , minimumTimePerModule_(0)
    , isActive_(true)
    , version_("1.0")
    , publishedAt_("") {
}

Course::Course(const std::string& id, const std::string& name)
    : id_(id)
    , curriculumId_(0)
    , code_("")
    , name_(name)
    , description_("")
    , totalModules_(0)
    , estimatedHours_(0)
    , passingScore_(70)
    , requiresSequentialCompletion_(true)
    , requiresTimeTracking_(true)
    , minimumTimePerModule_(0)
    , isActive_(true)
    , version_("1.0")
    , publishedAt_("") {
}

nlohmann::json Course::toJson() const {
    nlohmann::json j;
    j["code"] = code_;
    j["name"] = name_;
    j["description"] = description_;
    j["curriculum_id"] = curriculumId_;
    j["total_modules"] = totalModules_;
    j["estimated_hours"] = estimatedHours_;
    j["passing_score"] = passingScore_;
    j["requires_sequential_completion"] = requiresSequentialCompletion_;
    j["requires_time_tracking"] = requiresTimeTracking_;
    j["minimum_time_per_module"] = minimumTimePerModule_;
    j["is_active"] = isActive_;
    j["version"] = version_;
    if (!publishedAt_.empty()) {
        j["published_at"] = publishedAt_;
    }
    return j;
}

Course Course::fromJson(const nlohmann::json& json) {
    Course course;

    // Handle JSON:API format
    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            course.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            course.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Attributes
    if (attrs.contains("code") && !attrs["code"].is_null())
        course.code_ = attrs["code"].get<std::string>();
    if (attrs.contains("name") && !attrs["name"].is_null())
        course.name_ = attrs["name"].get<std::string>();
    if (attrs.contains("description") && !attrs["description"].is_null())
        course.description_ = attrs["description"].get<std::string>();

    // curriculum_id
    if (attrs.contains("curriculum_id") && !attrs["curriculum_id"].is_null()) {
        if (attrs["curriculum_id"].is_number())
            course.curriculumId_ = attrs["curriculum_id"].get<int>();
        else if (attrs["curriculum_id"].is_string())
            course.curriculumId_ = std::stoi(attrs["curriculum_id"].get<std::string>());
    }

    // Numeric fields
    if (attrs.contains("total_modules") && !attrs["total_modules"].is_null())
        course.totalModules_ = attrs["total_modules"].get<int>();
    if (attrs.contains("estimated_hours") && !attrs["estimated_hours"].is_null())
        course.estimatedHours_ = attrs["estimated_hours"].get<int>();
    if (attrs.contains("passing_score") && !attrs["passing_score"].is_null())
        course.passingScore_ = attrs["passing_score"].get<int>();
    if (attrs.contains("minimum_time_per_module") && !attrs["minimum_time_per_module"].is_null())
        course.minimumTimePerModule_ = attrs["minimum_time_per_module"].get<int>();

    // Boolean fields
    if (attrs.contains("requires_sequential_completion") && !attrs["requires_sequential_completion"].is_null())
        course.requiresSequentialCompletion_ = attrs["requires_sequential_completion"].get<bool>();
    if (attrs.contains("requires_time_tracking") && !attrs["requires_time_tracking"].is_null())
        course.requiresTimeTracking_ = attrs["requires_time_tracking"].get<bool>();
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        course.isActive_ = attrs["is_active"].get<bool>();

    // String fields
    if (attrs.contains("version") && !attrs["version"].is_null())
        course.version_ = attrs["version"].get<std::string>();
    if (attrs.contains("published_at") && !attrs["published_at"].is_null())
        course.publishedAt_ = attrs["published_at"].get<std::string>();

    return course;
}

// =============================================================================
// CourseModule Implementation
// =============================================================================

CourseModule::CourseModule()
    : id_("")
    , courseId_(0)
    , moduleNumber_(0)
    , code_("")
    , title_("")
    , description_("")
    , estimatedMinutes_(0)
    , minimumTimeRequired_(0)
    , passingScore_(70)
    , prerequisiteModuleId_(0)
    , isActive_(true) {
}

CourseModule::CourseModule(const std::string& id, const std::string& title)
    : id_(id)
    , courseId_(0)
    , moduleNumber_(0)
    , code_("")
    , title_(title)
    , description_("")
    , estimatedMinutes_(0)
    , minimumTimeRequired_(0)
    , passingScore_(70)
    , prerequisiteModuleId_(0)
    , isActive_(true) {
}

nlohmann::json CourseModule::toJson() const {
    nlohmann::json j;
    j["course_id"] = courseId_;
    j["module_number"] = moduleNumber_;
    j["code"] = code_;
    j["title"] = title_;
    j["description"] = description_;
    j["estimated_minutes"] = estimatedMinutes_;
    j["minimum_time_required"] = minimumTimeRequired_;
    j["passing_score"] = passingScore_;
    if (prerequisiteModuleId_ > 0) {
        j["prerequisite_module_id"] = prerequisiteModuleId_;
    }
    j["is_active"] = isActive_;
    return j;
}

CourseModule CourseModule::fromJson(const nlohmann::json& json) {
    CourseModule module;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            module.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            module.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Attributes
    if (attrs.contains("course_id") && !attrs["course_id"].is_null()) {
        if (attrs["course_id"].is_number())
            module.courseId_ = attrs["course_id"].get<int>();
        else if (attrs["course_id"].is_string())
            module.courseId_ = std::stoi(attrs["course_id"].get<std::string>());
    }

    if (attrs.contains("module_number") && !attrs["module_number"].is_null())
        module.moduleNumber_ = attrs["module_number"].get<int>();
    if (attrs.contains("code") && !attrs["code"].is_null())
        module.code_ = attrs["code"].get<std::string>();
    if (attrs.contains("title") && !attrs["title"].is_null())
        module.title_ = attrs["title"].get<std::string>();
    if (attrs.contains("description") && !attrs["description"].is_null())
        module.description_ = attrs["description"].get<std::string>();
    if (attrs.contains("estimated_minutes") && !attrs["estimated_minutes"].is_null())
        module.estimatedMinutes_ = attrs["estimated_minutes"].get<int>();
    if (attrs.contains("minimum_time_required") && !attrs["minimum_time_required"].is_null())
        module.minimumTimeRequired_ = attrs["minimum_time_required"].get<int>();
    if (attrs.contains("passing_score") && !attrs["passing_score"].is_null())
        module.passingScore_ = attrs["passing_score"].get<int>();
    if (attrs.contains("prerequisite_module_id") && !attrs["prerequisite_module_id"].is_null()) {
        if (attrs["prerequisite_module_id"].is_number())
            module.prerequisiteModuleId_ = attrs["prerequisite_module_id"].get<int>();
        else if (attrs["prerequisite_module_id"].is_string())
            module.prerequisiteModuleId_ = std::stoi(attrs["prerequisite_module_id"].get<std::string>());
    }
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        module.isActive_ = attrs["is_active"].get<bool>();

    return module;
}

// =============================================================================
// ModuleContent Implementation
// =============================================================================

ModuleContent::ModuleContent()
    : id_("")
    , moduleId_(0)
    , contentOrder_(0)
    , contentType_(ContentType::Reading)
    , title_("")
    , description_("")
    , contentUrl_("")
    , contentText_("")
    , contentDuration_(0)
    , isRequired_(true)
    , minimumViewTime_(0)
    , assessmentId_(0)
    , isActive_(true) {
}

ModuleContent::ModuleContent(const std::string& id, const std::string& title, ContentType type)
    : id_(id)
    , moduleId_(0)
    , contentOrder_(0)
    , contentType_(type)
    , title_(title)
    , description_("")
    , contentUrl_("")
    , contentText_("")
    , contentDuration_(0)
    , isRequired_(true)
    , minimumViewTime_(0)
    , assessmentId_(0)
    , isActive_(true) {
}

std::string ModuleContent::getContentTypeString() const {
    return contentTypeToString(contentType_);
}

void ModuleContent::setContentTypeFromString(const std::string& type) {
    contentType_ = stringToContentType(type);
}

ContentType ModuleContent::stringToContentType(const std::string& type) {
    std::string lower = type;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if (lower == "reading") return ContentType::Reading;
    if (lower == "video") return ContentType::Video;
    if (lower == "quiz") return ContentType::Quiz;
    if (lower == "interactive") return ContentType::Interactive;
    if (lower == "document") return ContentType::Document;
    return ContentType::Reading;  // Default
}

std::string ModuleContent::contentTypeToString(ContentType type) {
    switch (type) {
        case ContentType::Reading: return "reading";
        case ContentType::Video: return "video";
        case ContentType::Quiz: return "quiz";
        case ContentType::Interactive: return "interactive";
        case ContentType::Document: return "document";
        default: return "reading";
    }
}

nlohmann::json ModuleContent::toJson() const {
    nlohmann::json j;
    j["module_id"] = moduleId_;
    j["content_order"] = contentOrder_;
    j["content_type"] = contentTypeToString(contentType_);
    j["title"] = title_;
    j["description"] = description_;
    if (!contentUrl_.empty()) {
        j["content_url"] = contentUrl_;
    }
    if (!contentText_.empty()) {
        j["content_text"] = contentText_;
    }
    j["content_duration"] = contentDuration_;
    j["is_required"] = isRequired_;
    j["minimum_view_time"] = minimumViewTime_;
    if (assessmentId_ > 0) {
        j["assessment_id"] = assessmentId_;
    }
    j["is_active"] = isActive_;
    return j;
}

ModuleContent ModuleContent::fromJson(const nlohmann::json& json) {
    ModuleContent content;

    const nlohmann::json& attrs = json.contains("attributes") ? json["attributes"] : json;

    // ID from top level
    if (json.contains("id")) {
        if (json["id"].is_string()) {
            content.id_ = json["id"].get<std::string>();
        } else if (json["id"].is_number()) {
            content.id_ = std::to_string(json["id"].get<int>());
        }
    }

    // Attributes
    if (attrs.contains("module_id") && !attrs["module_id"].is_null()) {
        if (attrs["module_id"].is_number())
            content.moduleId_ = attrs["module_id"].get<int>();
        else if (attrs["module_id"].is_string())
            content.moduleId_ = std::stoi(attrs["module_id"].get<std::string>());
    }

    if (attrs.contains("content_order") && !attrs["content_order"].is_null())
        content.contentOrder_ = attrs["content_order"].get<int>();
    if (attrs.contains("content_type") && !attrs["content_type"].is_null())
        content.contentType_ = stringToContentType(attrs["content_type"].get<std::string>());
    if (attrs.contains("title") && !attrs["title"].is_null())
        content.title_ = attrs["title"].get<std::string>();
    if (attrs.contains("description") && !attrs["description"].is_null())
        content.description_ = attrs["description"].get<std::string>();
    if (attrs.contains("content_url") && !attrs["content_url"].is_null())
        content.contentUrl_ = attrs["content_url"].get<std::string>();
    if (attrs.contains("content_text") && !attrs["content_text"].is_null())
        content.contentText_ = attrs["content_text"].get<std::string>();
    if (attrs.contains("content_duration") && !attrs["content_duration"].is_null())
        content.contentDuration_ = attrs["content_duration"].get<int>();
    if (attrs.contains("is_required") && !attrs["is_required"].is_null())
        content.isRequired_ = attrs["is_required"].get<bool>();
    if (attrs.contains("minimum_view_time") && !attrs["minimum_view_time"].is_null())
        content.minimumViewTime_ = attrs["minimum_view_time"].get<int>();
    if (attrs.contains("assessment_id") && !attrs["assessment_id"].is_null()) {
        if (attrs["assessment_id"].is_number())
            content.assessmentId_ = attrs["assessment_id"].get<int>();
        else if (attrs["assessment_id"].is_string())
            content.assessmentId_ = std::stoi(attrs["assessment_id"].get<std::string>());
    }
    if (attrs.contains("is_active") && !attrs["is_active"].is_null())
        content.isActive_ = attrs["is_active"].get<bool>();

    return content;
}

} // namespace Models
} // namespace StudentIntake
