#include "Religion.hpp"

Religion::Religion() {}

Religion::Religion(const std::string& name, const sf::Color& color)
: m_Name(name), m_Color(color)
{}

std::string Religion::GetName() const {
    return m_Name;
}

sf::Color Religion::GetColor() const {
    return m_Color;
}

void Religion::SetName(const std::string& name) {
    m_Name = name;
}

void Religion::SetColor(const sf::Color& color) {
    m_Color = color;
}