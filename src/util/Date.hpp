#pragma once

// #include <fmt/format.h>

struct Date {
    int year;
    int month;
    int day;

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

// template <>
// class fmt::formatter<Date> {
// public:
//     constexpr auto parse (format_parse_context& ctx) {
//        return ctx.begin();
//     }

//     template <typename Context>
//     constexpr auto format (Date const& date, Context& ctx) const {
//         return format_to(ctx.out(), "{}.{}.{}", date.year, date.month, date.day);
//     }
// };