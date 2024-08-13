#pragma once

#include <fmt/format.h>

struct ScopedString {
    std::string scope;
    std::string value;

    operator std::string() const {
        return fmt::format("{}:{}", scope, value);
    }

    bool operator ==(const ScopedString& other) const {
        return scope == other.scope && value == other.value;
    }

    bool operator <=(const ScopedString& other) const {
        return !(*this > other);
    }
    
    bool operator >=(const ScopedString& other) const {
        return !(*this < other);
    }

    bool operator <(const ScopedString& other) const {
        return (value == other.value) ? scope < other.scope : value < other.value;
    }
    
    bool operator >(const ScopedString& other) const {
        return !(*this < other) && !(*this == other);
    }
};

template <>
class fmt::formatter<ScopedString> {
public:
    constexpr auto parse(format_parse_context& ctx) {
       return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const ScopedString& str, Context& ctx) const {
        return format_to(ctx.out(), "{}:{}", str.scope, str.value);
    }
};