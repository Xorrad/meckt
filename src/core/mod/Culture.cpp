#include "Culture.hpp"

Culture::Culture() {}

Culture::Culture(const std::string& name, const sf::Color& color)
: m_Name(name), m_Color(color)
{}

std::string Culture::GetName() const {
    return m_Name;
}

sf::Color Culture::GetColor() const {
    return m_Color;
}

void Culture::SetName(const std::string& name) {
    m_Name = name;
}

void Culture::SetColor(const sf::Color& color) {
    m_Color = color;
}