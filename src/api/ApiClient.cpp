#include "ApiClient.h"
#include "utils/Logger.h"
#include <curl/curl.h>
#include <thread>
#include <sstream>
#include <fstream>

namespace StudentIntake {
namespace Api {

nlohmann::json ApiResponse::getJson() const {
    try {
        if (!body.empty()) {
            return nlohmann::json::parse(body);
        }
    } catch (const nlohmann::json::parse_error&) {
        // Return empty object if parsing fails
    }
    return nlohmann::json::object();
}

ApiClient::ApiClient()
    : baseUrl_("http://localhost:5656/api")
    , authToken_("")
    , timeoutSeconds_(30) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    defaultHeaders_["Content-Type"] = "application/json";
    defaultHeaders_["Accept"] = "application/json";
}

ApiClient::ApiClient(const std::string& baseUrl)
    : baseUrl_(baseUrl)
    , authToken_("")
    , timeoutSeconds_(30) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    defaultHeaders_["Content-Type"] = "application/json";
    defaultHeaders_["Accept"] = "application/json";
}

ApiClient::~ApiClient() {
    curl_global_cleanup();
}

size_t ApiClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t ApiClient::headerCallback(char* buffer, size_t size, size_t nitems,
                                  std::map<std::string, std::string>* headers) {
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);

    size_t colonPos = header.find(':');
    if (colonPos != std::string::npos) {
        std::string key = header.substr(0, colonPos);
        std::string value = header.substr(colonPos + 1);

        // Trim whitespace
        auto trimStart = value.find_first_not_of(" \t\r\n");
        auto trimEnd = value.find_last_not_of(" \t\r\n");
        if (trimStart != std::string::npos && trimEnd != std::string::npos) {
            value = value.substr(trimStart, trimEnd - trimStart + 1);
        }

        (*headers)[key] = value;
    }

    return totalSize;
}

std::string ApiClient::buildFullUrl(const std::string& endpoint) const {
    std::string url = baseUrl_;
    if (!url.empty() && url.back() != '/' && !endpoint.empty() && endpoint.front() != '/') {
        url += '/';
    }
    url += endpoint;
    return url;
}

ApiResponse ApiClient::performRequest(const std::string& method,
                                       const std::string& endpoint,
                                       const std::string& body) {
    ApiResponse response;
    response.statusCode = 0;
    response.success = false;

    std::string fullUrl = buildFullUrl(endpoint);

    // Log request details
    LOG_DEBUG("ApiClient", method << " " << fullUrl);
    if (!body.empty()) {
        LOG_DEBUG("ApiClient", "Request body: " << body);
    }
    if (Logger::shouldLog(LogLevel::DEBUG)) {
        for (const auto& pair : defaultHeaders_) {
            LOG_DEBUG("ApiClient", "  Header: " << pair.first << ": " << pair.second);
        }
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        response.errorMessage = "Failed to initialize CURL";
        LOG_ERROR("ApiClient", "Failed to initialize CURL");
        return response;
    }

    std::string responseBody;

    curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds_);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // Set HTTP method
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    } else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    } else if (method == "PATCH") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    // Set headers
    struct curl_slist* headers = nullptr;
    for (const auto& pair : defaultHeaders_) {
        std::string header = pair.first + ": " + pair.second;
        headers = curl_slist_append(headers, header.c_str());
    }

    if (!authToken_.empty()) {
        std::string authHeader = "Authorization: Bearer " + authToken_;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Perform request
    LOG_DEBUG("ApiClient", "Sending request...");
    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        long httpCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        response.statusCode = static_cast<int>(httpCode);
        response.body = responseBody;
        response.success = (httpCode >= 200 && httpCode < 300);

        LOG_DEBUG("ApiClient", "Response status: " << httpCode);
        LOG_DEBUG("ApiClient", "Response body: " << responseBody);
        if (!response.success) {
            LOG_WARN("ApiClient", "Request failed with status " << httpCode);
        }
    } else {
        response.errorMessage = curl_easy_strerror(res);
        LOG_ERROR("ApiClient", "CURL error: " << response.errorMessage);
    }

    // Cleanup
    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);

    return response;
}

ApiResponse ApiClient::get(const std::string& endpoint) {
    return performRequest("GET", endpoint);
}

ApiResponse ApiClient::post(const std::string& endpoint, const nlohmann::json& data) {
    return performRequest("POST", endpoint, data.dump());
}

ApiResponse ApiClient::put(const std::string& endpoint, const nlohmann::json& data) {
    return performRequest("PUT", endpoint, data.dump());
}

ApiResponse ApiClient::patch(const std::string& endpoint, const nlohmann::json& data) {
    return performRequest("PATCH", endpoint, data.dump());
}

ApiResponse ApiClient::del(const std::string& endpoint) {
    return performRequest("DELETE", endpoint);
}

ApiResponse ApiClient::uploadFile(const std::string& endpoint,
                                   const std::string& fieldName,
                                   const std::string& filePath,
                                   const std::map<std::string, std::string>& additionalFields) {
    ApiResponse response;
    response.statusCode = 0;
    response.success = false;

    CURL* curl = curl_easy_init();
    if (!curl) {
        response.errorMessage = "Failed to initialize CURL";
        return response;
    }

    std::string responseBody;
    std::string fullUrl = buildFullUrl(endpoint);

    // Create multipart form
    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part;

    // Add file
    part = curl_mime_addpart(mime);
    curl_mime_name(part, fieldName.c_str());
    curl_mime_filedata(part, filePath.c_str());

    // Add additional fields
    for (const auto& field : additionalFields) {
        part = curl_mime_addpart(mime);
        curl_mime_name(part, field.first.c_str());
        curl_mime_data(part, field.second.c_str(), CURL_ZERO_TERMINATED);
    }

    curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeoutSeconds_);

    // Set auth header
    struct curl_slist* headers = nullptr;
    if (!authToken_.empty()) {
        std::string authHeader = "Authorization: Bearer " + authToken_;
        headers = curl_slist_append(headers, authHeader.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        long httpCode;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        response.statusCode = static_cast<int>(httpCode);
        response.body = responseBody;
        response.success = true;
    } else {
        response.errorMessage = curl_easy_strerror(res);
    }

    curl_mime_free(mime);
    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);

    return response;
}

void ApiClient::getAsync(const std::string& endpoint, ApiCallback callback) {
    std::thread([this, endpoint, callback]() {
        ApiResponse response = get(endpoint);
        if (callback) {
            callback(response);
        }
    }).detach();
}

void ApiClient::postAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback) {
    std::thread([this, endpoint, data, callback]() {
        ApiResponse response = post(endpoint, data);
        if (callback) {
            callback(response);
        }
    }).detach();
}

void ApiClient::putAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback) {
    std::thread([this, endpoint, data, callback]() {
        ApiResponse response = put(endpoint, data);
        if (callback) {
            callback(response);
        }
    }).detach();
}

void ApiClient::patchAsync(const std::string& endpoint, const nlohmann::json& data, ApiCallback callback) {
    std::thread([this, endpoint, data, callback]() {
        ApiResponse response = patch(endpoint, data);
        if (callback) {
            callback(response);
        }
    }).detach();
}

void ApiClient::deleteAsync(const std::string& endpoint, ApiCallback callback) {
    std::thread([this, endpoint, callback]() {
        ApiResponse response = del(endpoint);
        if (callback) {
            callback(response);
        }
    }).detach();
}

bool ApiClient::isServerAvailable() {
    ApiResponse response = get("/health");
    return response.isSuccess();
}

void ApiClient::setHeader(const std::string& key, const std::string& value) {
    defaultHeaders_[key] = value;
}

void ApiClient::removeHeader(const std::string& key) {
    defaultHeaders_.erase(key);
}

void ApiClient::clearHeaders() {
    defaultHeaders_.clear();
    defaultHeaders_["Content-Type"] = "application/json";
    defaultHeaders_["Accept"] = "application/json";
}

} // namespace Api
} // namespace StudentIntake
