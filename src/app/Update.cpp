#include "Update.hpp"
#include "core/util/Http.hpp"
#include <nlohmann/json.hpp>

Update::Details Update::QueryDetails() {
    Details result{false, "", "", ""};
    std::string response = "";

    try {
        response = Http::Get("https://api.github.com/repos/Xorrad/meckt/releases/latest");
    }
    catch (std::exception& e) {
        result.error = e.what();
        return result;
    }

    try {
        // Parse JSON to get the latest build version and appropriate download url.
        auto json = nlohmann::json::parse(response);
        result.lastBuildVersion = std::string(json["tag_name"]).substr(1);
        result.lastBuildURL = std::string(json["html_url"]);

        for (auto asset : json["assets"]) {
#ifdef _WIN32
            if (std::string(asset["name"]).starts_with("win")) {
                result.lastBuildDownloadURL = asset["browser_download_url"];
                break;
            }
#elif __linux__
            if (std::string(asset["name"]).starts_with("deb")) {
                result.lastBuildDownloadURL = asset["browser_download_url"];
                break;
            }
#endif
        }

        if (result.lastBuildDownloadURL.empty())
            result.error = fmt::format("Failed to find download URL.");

        try {
            // Compare the current and latest version by stripping
            // the dots in the version and casting them to integers.
            std::string tmp = result.lastBuildVersion;
            tmp.erase(std::remove(tmp.begin(), tmp.end(), '.'), tmp.end());
            int buildNumber = std::stoi(tmp);

            tmp = Configuration::buildVersion;
            tmp.erase(std::remove(tmp.begin(), tmp.end(), '.'), tmp.end());
            int currentBuildNumber = std::stoi(tmp);

            if (!result.lastBuildVersion.empty() && buildNumber > currentBuildNumber) {
                result.shouldUpdate = true;
            }
        }
        catch (std::exception& e) {
            result.error = fmt::format("Failed to process latest build version: {}", e.what());
        }
    }
    catch (std::exception& e) {
        result.error = fmt::format("Failed to parse build info: {}", e.what());
    }

    return result;
}