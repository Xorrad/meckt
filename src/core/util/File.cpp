#include "File.hpp"
#include "Http.hpp"
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

std::vector<std::vector<std::string>> File::ReadCSV(const std::string& filePath, const std::string& delimiter) {
    std::ifstream file(filePath);
    std::vector<std::vector<std::string>> lines;

    if (!file.is_open()) {
        LOG_ERROR("Could not open and read csv file {}", filePath);
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Remove comments.
        if (size_t comment_pos = line.find('#'); comment_pos != std::string::npos)
            line.erase(comment_pos);

        // Skip empty/whitespace-only lines.
        if (line.find_first_not_of(" \t") == std::string::npos)
            continue;

        lines.push_back(String::Split(line, ";"));
    }

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

bool File::DownloadFile(const std::string& url, const std::string& dest) {
    try {
        Http::Get(url, dest);
    }
    catch (std::exception& e) {
        return false;
    }
    return true;
}

bool File::UnzipFile(const std::string& src, const std::string& dest) {
#ifdef _WIN32
    return (bool) std::system(std::string("powershell -Command \"Expand-Archive -Force '" + src + "' '" + dest + "'\"").c_str());
#else
    return (bool) std::system(std::string("unzip -o " + src + " -d " + dest).c_str());
#endif
}