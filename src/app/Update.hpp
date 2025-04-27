#pragma once

namespace Update {
    struct Details {
        bool shouldUpdate;
        std::string lastBuildVersion;
        std::string lastBuildURL;
        std::string error;
    };

    size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    size_t WriteFileCallback(void* contents, size_t size, size_t nmemb, FILE* file);

    Details QueryDetails();
    std::string Update(const Details& details);
}