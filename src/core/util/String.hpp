#pragma once

namespace String {
    std::string Strip(std::string str, std::string toReplace);
    std::string ToLowercase(std::string str);
    std::string StripNonPrintable(const std::string& str);
    std::vector<std::string> Split(std::string str, const std::string& delimiter);
    std::string Join(std::vector<std::string> list, const std::string& delimiter);

    std::string FileSizeFormat(uint size);
    std::string DurationFormat(const sf::Time& time);

    bool IsDigit(char ch);
    bool IsAlpha(char ch);
    bool IsAlphaNumeric(char ch);

    double ParseDouble(const std::string& str);
    int ParseInt(const std::string& str);
}