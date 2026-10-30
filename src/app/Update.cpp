#include "Update.hpp"
#include "core/util/Http.hpp"
#include "util/String.hpp"
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
            if (std::string(asset["name"]).starts_with("ubuntu")) {
                result.lastBuildDownloadURL = asset["browser_download_url"];
                break;
            }
#endif
        }

        if (result.lastBuildDownloadURL.empty())
            result.error = fmt::format("Failed to find download URL.");

        try {
            if (!result.lastBuildVersion.empty() && IsVersionNewer(Configuration::buildVersion, result.lastBuildVersion)) {
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

bool Update::IsVersionNewer(const std::string& currentVersion, const std::string& newVersion) {
    // Compare the current and latest version by comparing the build numbers between each dots.

    std::vector<std::string> currentNumbers = String::Split(currentVersion, ".");
    std::vector<std::string> newNumbers = String::Split(newVersion, ".");

    for (size_t i = 0; i < std::max(currentNumbers.size(), newNumbers.size()); i++) {
        // Exceptions will be catched by the caller.
        int currentNum = (i < currentNumbers.size()) ? std::stoi(currentNumbers[i]) : 0;
        int newNum = (i < newNumbers.size()) ? std::stoi(newNumbers[i]) : 0;

        if (newNum > currentNum) {
            return true;
        }
        else if (newNum < currentNum) {
            return false;
        }
    }

    return false;
}