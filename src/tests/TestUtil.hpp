#pragma once

// Shared helpers used across the meckt-tests suite to avoid duplicating
// common setup/assertion boilerplate across individual test files.
namespace TestUtil {

/**
 * @brief Recursively removes `path` if it exists. Used at the start of export test
 * cases to clear out a leftover "_modified" mod directory from a previous
 * run before writing to it again.
 * @param path The path to the directory.
 */
inline void ResetDirectory(const std::string& path) {
    std::filesystem::remove_all(path);
}

/**
 * @brief Returns true if the file at `path` starts with a UTF-8 byte order mark (EF BB BF).
 * @param path The path to the file to check.
 */
inline bool HasUTF8BOM(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        return false;

    unsigned char bom[3] = {};
    file.read(reinterpret_cast<char*>(bom), sizeof(bom));

    return bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF;
}

inline std::vector<std::string> ReadComments(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    REQUIRE(file.is_open());

    std::vector<std::string> comments;
    std::string line;

    while (std::getline(file, line)) {
        if (line.starts_with("\xEF\xBB\xBF"))
            line = line.substr(3);
        if (line.ends_with("\n"))
            line.pop_back();
        if (line.starts_with("#"))
            comments.push_back(line);
    }
    return comments;
}

}
