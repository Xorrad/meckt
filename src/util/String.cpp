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