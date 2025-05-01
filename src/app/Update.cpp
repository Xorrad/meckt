#include "Update.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <errno.h>

// Write callback for curl.
size_t Update::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*) contents, totalSize);
    return totalSize;
}

// Write binary callback for downloading.
size_t Update::WriteFileCallback(void* contents, size_t size, size_t nmemb, FILE* file) {
    return fwrite(contents, size, nmemb, file);
}

Update::Details Update::QueryDetails() {
    Details result{false, "", "", ""};

    CURL* curl;
    CURLcode curlRes;
    std::string buffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Get latest build info from GitHub.
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.github.com/repos/Xorrad/meckt/releases/latest");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "meckt");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        
        curlRes = curl_easy_perform(curl);
        if (curlRes != CURLE_OK) {
            result.error = fmt::format("Failed to fetch build info: {}", curl_easy_strerror(curlRes));
            return result;
        }

        try {
            // Parse JSON to get the latest build version and appropriate download url.
            auto json = nlohmann::json::parse(buffer);
            result.lastBuildVersion = std::string(json["tag_name"]).substr(1);
            result.lastBuildURL = std::string(json["html_url"]);

            for (auto asset : json["assets"]) {
    #ifdef _WIN32
                if (asset["name"].starts_with("win")) {
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

        curl_easy_cleanup(curl);
    }
    else {
        result.error = "Failed to initialize curl.";
    }
    curl_global_cleanup();

    return result;
}

std::string Update::Update(const Details& details) {
    CURL* curl;
    CURLcode curlRes;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        FILE* file = fopen("latest_build.zip", "wb");

        if (!file)
            return fmt::format("Failed to open file for writing: {}; {}", errno, strerror(errno));
        
        curl_easy_setopt(curl, CURLOPT_URL, details.lastBuildDownloadURL.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        curlRes = curl_easy_perform(curl);
        fclose(file);

        if (curlRes != CURLE_OK) {
            std::remove("latest_build.zip");
            return fmt::format("Failed to download file: {}", curl_easy_strerror(curlRes));;
        }
        
        // Write a bash script to extract the zip replace
        // the former build files and restart the tool.
#ifdef _WIN32
        std::string script = R"(
            @echo off
            echo Updating...
            timeout /t 2 /nobreak > nul
            powershell -Command "Expand-Archive -Force 'latest_build.zip' '.'"
            del latest_build.zip
            start "" "meckt.exe"
            del "%~f0"
        )";
        std::ofstream scriptFile("update.bat");
        scriptFile << script;
        scriptFile.close();

        std::system("start \"\" update.bat");
#else
        std::string script = R"(
            #!/bin/bash
            echo "Updating..."
            sleep 2
            unzip -o latest_build.zip -d .
            rm latest_build.zip
            ./meckt &
            rm -- "$0"
        )";
        std::ofstream scriptFile("update.sh");
        scriptFile << script;
        scriptFile.close();

        // Make the script executable.
        system("chmod +x update.sh");
        system("./update.sh &");
#endif
        
        curl_easy_cleanup(curl);
    }
    else {
        return "Failed to initialize curl.";
    }
    curl_global_cleanup();

    return "";
}