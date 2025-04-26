#include "File.hpp"
#include <filesystem>

std::set<std::string> File::ListFiles(const std::string& dirPath, bool recursive) {
    std::set<std::string> files;
    if(std::filesystem::exists(dirPath)) {
        if(recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
                files.insert(entry.path().string());
        }
        else {
            for (const auto& entry : std::filesystem::directory_iterator(dirPath))
                files.insert(entry.path().string());
        }
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

    // Read lines one by one, and then character by character
    // and push new values for cells when encountering ';'. 
    while (std::getline(file, line)) {
        std::vector<std::string> rows;
        std::string buffer = "";

        for (int i = 0; i < line.size(); i++) {
            // Set buffer as new column value.
            if (line[i] == ';' || line[i] == '#') {
                if (!buffer.empty()) {
                    rows.push_back(buffer);
                    buffer = "";
                }
                // To handle comments, just set the value for the cell before
                // skipping to next line.
                if (line[i] == '#')
                    break;
                continue;
            }
            buffer += line[i];
        }

        if (!rows.empty())
            lines.push_back(std::move(rows));
    }

    file.close();
    
    return lines;
}

void File::EncodeToUTF8BOM(std::ofstream& file) {
    unsigned char bom[] = { 0xEF, 0xBB, 0xBF };
    file.seekp(file.beg);
    file.write(reinterpret_cast<char*>(bom), sizeof(bom));
}

void File::OpenFile(const std::string& path) {
#ifdef _WIN32
    std::string windowsPath = path;
    std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');
    std::string command = "start \"\" \"" + windowsPath + "\"";
    if(!std::system(command.c_str()))
        return;
#elif __linux__
    std::string command = "xdg-open \"" + path + "\"";
    if(!system(command.c_str()))
        return;
#endif
}