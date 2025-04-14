#pragma once

namespace File {
    std::set<std::string> ListFiles(const std::string& dirPath, bool recursive = true);
    
    std::string ReadString(std::ifstream& file);
    std::vector<std::vector<std::string>> ReadCSV(const std::string& filePath);

    void EncodeToUTF8BOM(std::ofstream& file);
}