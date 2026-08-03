#include "Date.hpp"

Jomini::Date Date::ParseDate(std::string_view sv) {
    // This function is used to allow/auto-complete incomplete date in script, such as missing day or month.
    int year;
    int month = 1;
    int day = 1;

    size_t dot1 = sv.find('.');
    size_t dot2 = (dot1 == std::string::npos) ? std::string::npos : sv.find('.', dot1 + 1);

    // Parse the year.
    {
        const char* first = sv.data();
        const char* last = (dot1 == std::string::npos) ? sv.data() + sv.length()
                                                       : sv.data() + dot1;

        auto res = std::from_chars(first, last, year);
        if (res.ec != std::errc{})
            throw std::invalid_argument("Date::constructor: invalid year number format.");
    }

    // Parse the month.
    if (dot1 != std::string::npos) {
        const char* first = sv.data() + dot1 + 1;
        const char* last = (dot2 == std::string::npos) ? sv.data() + sv.length()
                                                       : sv.data() + dot2;

        auto res = std::from_chars(first, last, month);
        if (res.ec != std::errc{} || month < 1 || month > 12)
            throw std::invalid_argument("Date::constructor: invalid month number format.");
    }

    // Parse the day.
    if (dot2 != std::string::npos && dot2 != dot1 + 1 && dot2 != sv.length() - 1) {
        const char* first = sv.data() + dot2 + 1;
        const char* last = sv.data() + sv.length();

        auto res = std::from_chars(first, last, day);
        if (res.ec != std::errc{} || day < 1 || day > 31)
            throw std::invalid_argument("Date::constructor: invalid day number format.");
    }

    return Jomini::Date(year, month, day);
}