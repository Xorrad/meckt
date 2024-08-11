#pragma once

namespace String {
    std::string Strip(std::string str, std::string toReplace);
    std::string ToLowercase(std::string str);
    std::string StripNonPrintable(const std::string& str);
    std::vector<std::string> Split(std::string str, const std::string& delimiter);

    int UTF8CharLength(char ch);

    std::string FileSizeFormat(uint size);
    std::string DurationFormat(const sf::Time& time);

    bool IsDigit(char ch);
    bool IsAlpha(char ch);
    bool IsAlphaNumeric(char ch);
}