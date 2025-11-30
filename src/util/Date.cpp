#include "Date.hpp"

Jomini::Date Date::ParseDate(std::string_view sv) {
    // This function is used to allow/auto-complete incomplete date in script, such as missing day or month.
    int year;
    int month = 1;
    int day = 1;

    size_t dot1 = sv.find('.');
    size_t dot2 = sv.find('.', (dot1 == std::string::npos) ? std::string::npos : dot1 + 1);

    try {
        std::from_chars(sv.data(), sv.data() + ((dot1 == std::string::npos) ? sv.length() : dot1), year);
    }
    catch (std::exception& e) {
        throw std::invalid_argument("Date::constructor: invalid year number format.");
    }

    if (dot1 != std::string::npos && dot1 != 0) {
        try {
            std::from_chars(sv.data() + dot1 + 1, sv.data() + ((dot2 == std::string::npos) ? sv.length() - dot1 : dot2 - dot1), month);
        }
        catch (std::exception& e) {
            throw std::invalid_argument("Date::constructor: invalid month number format.");
        }
    }
    
    if (dot2 != std::string::npos && dot2 != dot1 + 1 && dot2 != sv.length() - 1) {
        try {
            std::from_chars(sv.data() + dot2 + 1, sv.data() + sv.length() - dot2, day);
        }
        catch (std::exception& e) {
            throw std::invalid_argument("Date::constructor: invalid day number format.");
        }
    }

    return Jomini::Date(year, month, day);
}