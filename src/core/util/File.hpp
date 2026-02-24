#pragma once

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <stdlib.h>
#elif __linux__
    #include <stdlib.h>
#endif

namespace File {
    std::set<std::string> ListFiles(const std::string& dirPath, bool recursive = true);
    
    std::string ReadString(std::ifstream& file);
    std::vector<std::vector<std::string>> ReadCSV(const std::string& filePath, const std::string& delimiter = ";");

    void EncodeToUTF8BOM(std::ofstream& file);

    void OpenFile(const std::string& path);
    bool DownloadFile(const std::string& url, const std::string& dest);
    bool UnzipFile(const std::string& src, const std::string& dest);
}