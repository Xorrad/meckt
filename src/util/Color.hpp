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

inline sf::Color brightenColor(const sf::Color& color) {
	if((color.r + color.g + color.b) >= (255*3/2))
		return sf::Color::Black;
	return sf::Color::White;
}