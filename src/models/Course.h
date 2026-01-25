#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Models {

/**
 * @brief Represents an online course associated with a curriculum
 *
 * Courses contain modules with reading assignments, videos, and quizzes.
 * Used for programs with online study like the ELD prep-course.
 */
class Course {
public:
    Course();
    Course(const std::string& id, const std::string& name);

    // Getters
    std::string getId() const { return id_; }
    int getCurriculumId() const { return curriculumId_; }
    std::string getCode() const { return code_; }
    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }
    int getTotalModules() const { return totalModules_; }
    int getEstimatedHours() const { return estimatedHours_; }
    int getPassingScore() const { return passingScore_; }
    bool requiresSequentialCompletion() const { return requiresSequentialCompletion_; }
    bool requiresTimeTracking() const { return requiresTimeTracking_; }
    int getMinimumTimePerModule() const { return minimumTimePerModule_; }
    bool isActive() const { return isActive_; }
    std::string getVersion() const { return version_; }
    std::string getPublishedAt() const { return publishedAt_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setCurriculumId(int id) { curriculumId_ = id; }
    void setCode(const std::string& code) { code_ = code; }
    void setName(const std::string& name) { name_ = name; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setTotalModules(int count) { totalModules_ = count; }
    void setEstimatedHours(int hours) { estimatedHours_ = hours; }
    void setPassingScore(int score) { passingScore_ = score; }
    void setRequiresSequentialCompletion(bool required) { requiresSequentialCompletion_ = required; }
    void setRequiresTimeTracking(bool required) { requiresTimeTracking_ = required; }
    void setMinimumTimePerModule(int minutes) { minimumTimePerModule_ = minutes; }
    void setActive(bool active) { isActive_ = active; }
    void setVersion(const std::string& version) { version_ = version; }
    void setPublishedAt(const std::string& timestamp) { publishedAt_ = timestamp; }

    // Serialization
    nlohmann::json toJson() const;
    static Course fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int curriculumId_;
    std::string code_;
    std::string name_;
    std::string description_;
    int totalModules_;
    int estimatedHours_;
    int passingScore_;
    bool requiresSequentialCompletion_;
    bool requiresTimeTracking_;
    int minimumTimePerModule_;
    bool isActive_;
    std::string version_;
    std::string publishedAt_;
};

/**
 * @brief Represents a module within a course
 *
 * Modules contain content items (reading, video, quiz) and track student progress.
 * A typical course like ELD has approximately 32 modules.
 */
class CourseModule {
public:
    CourseModule();
    CourseModule(const std::string& id, const std::string& title);

    // Getters
    std::string getId() const { return id_; }
    int getCourseId() const { return courseId_; }
    int getModuleNumber() const { return moduleNumber_; }
    std::string getCode() const { return code_; }
    std::string getTitle() const { return title_; }
    std::string getDescription() const { return description_; }
    int getEstimatedMinutes() const { return estimatedMinutes_; }
    int getMinimumTimeRequired() const { return minimumTimeRequired_; }
    int getPassingScore() const { return passingScore_; }
    int getPrerequisiteModuleId() const { return prerequisiteModuleId_; }
    bool isActive() const { return isActive_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setCourseId(int id) { courseId_ = id; }
    void setModuleNumber(int num) { moduleNumber_ = num; }
    void setCode(const std::string& code) { code_ = code; }
    void setTitle(const std::string& title) { title_ = title; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setEstimatedMinutes(int minutes) { estimatedMinutes_ = minutes; }
    void setMinimumTimeRequired(int minutes) { minimumTimeRequired_ = minutes; }
    void setPassingScore(int score) { passingScore_ = score; }
    void setPrerequisiteModuleId(int id) { prerequisiteModuleId_ = id; }
    void setActive(bool active) { isActive_ = active; }

    // Serialization
    nlohmann::json toJson() const;
    static CourseModule fromJson(const nlohmann::json& json);

private:
    std::string id_;
    int courseId_;
    int moduleNumber_;
    std::string code_;
    std::string title_;
    std::string description_;
    int estimatedMinutes_;
    int minimumTimeRequired_;
    int passingScore_;
    int prerequisiteModuleId_;
    bool isActive_;
};

/**
 * @brief Content type enumeration for module content
 */
enum class ContentType {
    Reading,
    Video,
    Quiz,
    Interactive,
    Document
};

/**
 * @brief Represents a content item within a module
 *
 * Content can be reading assignments, videos, quizzes, or interactive elements.
 */
class ModuleContent {
public:
    ModuleContent();
    ModuleContent(const std::string& id, const std::string& title, ContentType type);

    // Getters
    std::string getId() const { return id_; }
    int getModuleId() const { return moduleId_; }
    int getContentOrder() const { return contentOrder_; }
    ContentType getContentType() const { return contentType_; }
    std::string getContentTypeString() const;
    std::string getTitle() const { return title_; }
    std::string getDescription() const { return description_; }
    std::string getContentUrl() const { return contentUrl_; }
    std::string getContentText() const { return contentText_; }
    int getContentDuration() const { return contentDuration_; }
    bool isRequired() const { return isRequired_; }
    int getMinimumViewTime() const { return minimumViewTime_; }
    int getAssessmentId() const { return assessmentId_; }
    bool isActive() const { return isActive_; }

    // Setters
    void setId(const std::string& id) { id_ = id; }
    void setModuleId(int id) { moduleId_ = id; }
    void setContentOrder(int order) { contentOrder_ = order; }
    void setContentType(ContentType type) { contentType_ = type; }
    void setContentTypeFromString(const std::string& type);
    void setTitle(const std::string& title) { title_ = title; }
    void setDescription(const std::string& desc) { description_ = desc; }
    void setContentUrl(const std::string& url) { contentUrl_ = url; }
    void setContentText(const std::string& text) { contentText_ = text; }
    void setContentDuration(int seconds) { contentDuration_ = seconds; }
    void setRequired(bool required) { isRequired_ = required; }
    void setMinimumViewTime(int seconds) { minimumViewTime_ = seconds; }
    void setAssessmentId(int id) { assessmentId_ = id; }
    void setActive(bool active) { isActive_ = active; }

    // Serialization
    nlohmann::json toJson() const;
    static ModuleContent fromJson(const nlohmann::json& json);

    // Helper methods
    static ContentType stringToContentType(const std::string& type);
    static std::string contentTypeToString(ContentType type);

private:
    std::string id_;
    int moduleId_;
    int contentOrder_;
    ContentType contentType_;
    std::string title_;
    std::string description_;
    std::string contentUrl_;
    std::string contentText_;
    int contentDuration_;
    bool isRequired_;
    int minimumViewTime_;
    int assessmentId_;
    bool isActive_;
};

} // namespace Models
} // namespace StudentIntake

#endif // COURSE_H
