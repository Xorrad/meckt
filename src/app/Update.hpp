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

    std::string HttpGet(const std::string& url, const std::string& filePath = "");

    Details QueryDetails();

}