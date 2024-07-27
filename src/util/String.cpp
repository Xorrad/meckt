#include "String.hpp"

std::string String::Strip(std::string str, std::string toReplace) {
    size_t i;
    while((i = str.find(toReplace)) != std::string::npos) {
        str.erase(i, toReplace.size());
    }
    return str;
}

std::string String::ToLowercase(std::string str) {
    for(int i = 0; i < str.size(); i++) {
        str[i] = tolower(str[i]);
    }
    return str;
}

std::string String::StripNonPrintable(const std::string& str) {
    std::string s = "";
    for(int i = 0; i < str.size(); i++) {
        if(isprint(str[i]))
            s.push_back(str[i]);
    }
    return s;
}

int UTF8CharLength(char ch) {
    if((ch & 0x80) == 0)
        return 1;
    if((ch & 0xE0) == 0xC0)
        return 2;
    if((ch & 0xF0) == 0xE0)
        return 3;
    if((ch & 0xF8) == 0xF0)
        return 4;
    throw std::runtime_error("invalid UTF-8");
}

std::string String::FileSizeFormat(uint size) {
    if(size < 1000)
        return std::to_string(size) + " Bytes";
    if(size < 1000000)
        return std::to_string(size / 1000.f) + " KB";
    if(size < 1000000000)
        return std::to_string(size / 1000000.f) + " MB";
    return std::to_string(size / 1000000000.f) + " GB";
}

std::string String::DurationFormat(const sf::Time& time) {
    if(time.asSeconds() > 1.f)
        return std::to_string(time.asSeconds()) + "s";
    if(time.asMilliseconds() > 1.f)
        return std::to_string(time.asMilliseconds()) + "ms";
    return std::to_string(time.asMicroseconds()) + "μs";
}

bool String::IsDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool String::IsAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z')
        || (ch >= 'A' && ch <= 'Z')
        || (ch == '_');
}

bool String::IsAlphaNumeric(char ch) {
    return IsDigit(ch) || IsAlpha(ch);
}