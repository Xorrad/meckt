#pragma once

namespace Update {
    struct Details {
        bool shouldUpdate;
        std::string lastBuildVersion;
        std::string lastBuildDownloadURL;
        std::string lastBuildURL;
        std::string error;
    };

    Details QueryDetails();
}