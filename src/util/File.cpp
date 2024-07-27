#include "File.hpp"

std::string File::ReadString(std::ifstream& file) {
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}