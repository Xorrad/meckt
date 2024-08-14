#pragma once

template <>
struct std::hash<sf::Color> {
  std::size_t operator()(const sf::Color& c) const {
        std::size_t res = 17;
        res = res * 31 + hash<char>()(c.r);
        res = res * 31 + hash<char>()(c.g);
        res = res * 31 + hash<char>()(c.b);
        res = res * 31 + hash<char>()(c.a);
        return res;
    }
};