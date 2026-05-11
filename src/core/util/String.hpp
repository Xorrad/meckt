#pragma once

namespace String {
    std::string Strip(std::string str, std::string toReplace);
    std::string ToLowercase(std::string str);
    std::string StripNonPrintable(const std::string& str);
    std::vector<std::string> Split(std::string str, const std::string& delimiter);
    std::string Join(std::vector<std::string> list, const std::string& delimiter);
    void ReplaceAll(std::string& str, const std::string& from, const std::string& to);

    std::string FileSizeFormat(size_t size);
    std::string DurationFormat(const sf::Time& time);

    bool IsDigit(char ch);
    bool IsAlpha(char ch);
    bool IsAlphaNumeric(char ch);

    double ParseDouble(const std::string& str);
    int ParseInt(const std::string& str);
}