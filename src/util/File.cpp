#include "File.hpp"
#include <filesystem>

std::set<std::string> File::ListFiles(const std::string& dirPath) {
    std::set<std::string> files;
    if(std::filesystem::exists(dirPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
            files.insert(entry.path());
    }
    return files;
}

std::string File::ReadString(std::ifstream& file) {
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::vector<std::vector<std::string>> File::ReadCSV(const std::string& filePath) {
    std::ifstream file(filePath);
    std::vector<std::vector<std::string>> lines;

    if(!file)
        return lines;

    std::string line;

    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::vector<std::string> rows;
        std::string cell;

        while (std::getline(lineStream, cell, ';'))
            rows.push_back(cell);

        lines.push_back(std::move(rows));
    }

    file.close();
    
    return lines;
}