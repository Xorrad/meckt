#pragma once

#include <optional>

namespace Update {
    struct Details {
        bool shouldUpdate;
        std::string lastBuildVersion;
        std::string lastBuildDownloadURL;
        std::string lastBuildURL;
        std::string error;
    };

    size_t WriteFileCallback(void* contents, size_t size, size_t nmemb, FILE* file);
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

    std::string HttpGet(const std::string& url, const std::string& filePath = "");

    Details QueryDetails();

}