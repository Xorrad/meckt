#pragma once

#include <fmt/format.h>

struct Date {
    int year;
    int month;
    int day;

    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    Date(const std::string& str) {
        std::vector<std::string> result = String::Split(str, ".");
        year = std::stoi(result[0]);
        month = std::stoi(result[1]);
        day = std::stoi(result[2]);
    }

    operator std::string() const {
        return fmt::format("{}.{}.{}", year, month, day);
    }
    
    bool operator ==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
    
    bool operator <=(const Date& other) const {
        return !(*this > other);
    }
    
    bool operator >=(const Date& other) const {
        return !(*this < other);
    }

    bool operator <(const Date& other) const {
        return year < other.year
            || (year == other.year && month < other.month)
            || (year == other.year && month == other.month && day < other.day);
    }
    
    bool operator >(const Date& other) const {
        return !(*this < other) && !(*this == other);
    }
};

template <>
class fmt::formatter<Date> {
public:
    constexpr auto parse(format_parse_context& ctx) {
       return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Date& date, Context& ctx) const {
        return format_to(ctx.out(), "{}.{}.{}", date.year, date.month, date.day);
    }
};