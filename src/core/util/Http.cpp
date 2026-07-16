#include "Http.hpp"

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <curl/curl.h>
#endif

#include <errno.h>

// Write callback for curl.
size_t Http::Impl::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*) contents, totalSize);
    return totalSize;
}

// Write binary callback for downloading.
size_t Http::Impl::WriteFileCallback(void* contents, size_t size, size_t nmemb, FILE* file) {
    return fwrite(contents, size, nmemb, file);
}

std::string Http::Get(const std::string& url, const std::string& filePath) {
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

    if (!WinHttpSetTimeouts(
        hSession,
        250, // Resolve timeout
        250, // Connect timeout
        250, // Send timeout
        250  // Receive timeout
    )) {
        WinHttpCloseHandle(hSession);
        throw std::runtime_error("Failed to set WinHTTP timeouts.");
    }

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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 250L);
    
    if (filePath.empty()) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http::Impl::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    }
    else {
        file = fopen(filePath.c_str(), "wb");
        if (!file)
            throw std::runtime_error(fmt::format("Failed to open file for writing: {}; {}", errno, strerror(errno)));   
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http::Impl::WriteFileCallback);
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