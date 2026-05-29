#include "Faith.hpp"

Faith::Faith() {}

Faith::Faith(const std::string& name, const sf::Color& color)
: m_Name(name), m_Color(color)
{}

std::string Faith::GetName() const {
    return m_Name;
}

sf::Color Faith::GetColor() const {
    return m_Color;
}

void Faith::SetName(const std::string& name) {
    m_Name = name;
}

void Faith::SetColor(const sf::Color& color) {
    m_Color = color;
}