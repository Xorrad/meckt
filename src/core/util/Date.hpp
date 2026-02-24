#pragma once

#include <fmt/format.h>

namespace Date {
    Jomini::Date ParseDate(std::string_view sv);
}

template <>
class fmt::formatter<Jomini::Date> {
public:
    constexpr auto parse(format_parse_context& ctx) {
       return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Jomini::Date& date, Context& ctx) const {
        return fmt::format_to(ctx.out(), "{}.{}.{}", date.year, date.month, date.day);
    }
};