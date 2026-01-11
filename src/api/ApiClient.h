#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <string>
#include <functional>
#include <memory>
#include <map>
#include <nlohmann/json.hpp>

namespace StudentIntake {
namespace Api {

/**
 * @brief HTTP response structure
 */
struct ApiResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    bool success;
    std::string errorMessage;

    nlohmann::json getJson() const;
    bool isSuccess() const { return success && statusCode >= 200 && statusCode < 300; }
};

/**
 * @brief Callback type for async API calls
 */
using ApiCallback = std::function<void(const ApiResponse&)>;

/**
 * @brief HTTP client for communicating with ApiLogicServer backend
 */
class ApiClient {
public:
    ApiClient();
    explicit ApiClient(const std::string& baseUrl);
    ~ApiClient();

    // Configuration
    void setBaseUrl(const std::string& baseUrl) { baseUrl_ = baseUrl; }
    std::string getBaseUrl() const { return baseUrl_; }

    void setAuthToken(const std::string& token) { authToken_ = token; }
    std::string getAuthToken() const { return authToken_; }

    void setTimeout(int seconds) { timeoutSeconds_ = seconds; }
    int getTimeout() const { return timeoutSeconds_; }

    // Synchronous HTTP methods
    ApiResponse get(const std::string& endpoint);
    ApiResponse post(const std::string& endpoint, const nlohmann::json& data);
    ApiResponse put(const std::string& endpoint, const nlohmann::json& data);
    ApiResponse patch(const std::string& endpoint, const nlohmann::json& data);
    ApiResponse del(const std::string& endpoint);

    // File upload
    ApiResponse uploadFile(const std::string& endpoint,
                           const std::string& fieldName,
                           const std::string& filePath,
                           const std::map<std::string, std::string>& additionalFields = {});

    // Async HTTP methods
    void getAsync(const std::string& endpoint, ApiCallback callback);
    void postAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback);
    void putAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback);
    void patchAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback);
    void deleteAsync(const std::string& endpoint, ApiCallback callback);

    // Health check
    bool isServerAvailable();

    // Default headers
    void setHeader(const std::string& key, const std::string& value);
    void removeHeader(const std::string& key);
    void clearHeaders();

private:
    std::string baseUrl_;
    std::string authToken_;
    int timeoutSeconds_;
    std::map<std::string, std::string> defaultHeaders_;

    // Internal HTTP implementation
    ApiResponse performRequest(const std::string& method,
                               const std::string& endpoint,
                               const std::string& body = "");
    std::string buildFullUrl(const std::string& endpoint) const;
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    static size_t headerCallback(char* buffer, size_t size, size_t nitems,
                                 std::map<std::string, std::string>* headers);
};

} // namespace Api
} // namespace StudentIntake

#endif // API_CLIENT_H
