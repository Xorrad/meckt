#include "Update.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <curl/curl.h>
#endif

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

std::string Update::HttpGet(const std::string& url, const std::string& filePath) {
#ifdef _WIN32
    std::string response = "";

    std::wstring host = L"";
    std::wstring path = L"/";
    INTERNET_PORT port = INTERNET_DEFAULT_HTTPS_PORT;

    // Parse URL manually (basic, assumes https)
    std::string prefix = "https://";
    if (url.rfind(prefix, 0) != 0)
        throw std::runtime_error("Only HTTPS URLs are supported.");

    std::string trimmed = url.substr(prefix.size());
    size_t slashPos = trimmed.find('/');
    std::string hostStr = trimmed.substr(0, slashPos);
    std::string pathStr = (slashPos != std::string::npos) ? trimmed.substr(slashPos) : "/";
    host.assign(hostStr.begin(), hostStr.end());
    path.assign(pathStr.begin(), pathStr.end());

    HINTERNET hSession = WinHttpOpen(L"meckt", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession)
        throw std::runtime_error("Failed to open WinHTTP session.");

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) {
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to connect to host.");
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to open HTTP request.");
    }

    WinHttpAddRequestHeaders(hRequest, L"User-Agent: meckt\r\n", -1L, WINHTTP_ADDREQ_FLAG_ADD);

    BOOL result = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
    if (!result || !WinHttpReceiveResponse(hRequest, NULL)) {
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("HTTP request failed.");
    }

    if (!filePath.empty()) {
        std::ofstream file(filePath, std::ios::binary);
        if (!file)
            throw std::runtime_error("Failed to open file for writing.");
        DWORD dwSize = 0;
        do {
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize) || dwSize == 0)
                break;
            std::string buffer(dwSize, 0);
            DWORD dwDownloaded = 0;
            if (WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded)) {
                file.write(buffer.data(), dwDownloaded);
            }
        } while (dwSize > 0);
        file.close();

    }
    else {
        DWORD dwSize = 0;
        do {
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize) || dwSize == 0)
                break;
            std::string buffer(dwSize, 0);
            DWORD dwDownloaded = 0;
            if (WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded))
                response.append(buffer, 0, dwDownloaded);
        } while (dwSize > 0);
    }

    return response;
#else
    CURL* curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Failed to initialize libcurl.");

    std::string response = "";
    FILE* file;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "meckt");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    if (filePath.empty()) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }
    else {
        file = fopen(filePath.c_str(), "wb");
        if (!file)
            throw std::runtime_error(fmt::format("Failed to open file for writing: {}; {}", errno, strerror(errno)));   
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
    }

    CURLcode res = curl_easy_perform(curl);

    if (!filePath.empty())
        fclose(file);

    if (res != CURLE_OK) {
        if (!filePath.empty())
            std::remove(filePath.c_str());
        std::string error = "libcurl error: ";
        error += curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error(error);
    }

    curl_easy_cleanup(curl);
    return response;
#endif
}

Update::Details Update::QueryDetails() {
    Details result{false, "", "", ""};
    std::string response = "";

    try {
        response = HttpGet("https://api.github.com/repos/Xorrad/meckt/releases/latest");
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