#pragma once

namespace File {
    std::vector<std::string> ListFiles(const std::string& dirPath);
    
    std::string ReadString(std::ifstream& file);
    std::vector<std::vector<std::string>> ReadCSV(const std::string& filePath);
}