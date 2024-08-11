#pragma once

namespace File {
    std::string ReadString(std::ifstream& file);
    std::vector<std::vector<std::string>> ReadCSV(const std::string& filePath);
}