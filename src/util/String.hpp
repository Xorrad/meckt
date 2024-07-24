#pragma once

namespace String {
    std::string Strip(std::string str, std::string toReplace);
    std::string ToLowercase(std::string str);
    std::string StripNonPrintable(const std::string& str);
}