#pragma once

namespace Http {
    namespace Impl {
        size_t WriteFileCallback(void* contents, size_t size, size_t nmemb, FILE* file);
        size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
    }

    std::string Get(const std::string& url, const std::string& filePath = "");
}