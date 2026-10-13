#include "Rite.hpp"

Rite::Rite() {}

Rite::Rite(const std::string& name, const sf::Color& color) :
    m_Name(name),
    m_Color(color)
{}

//////////////////////////////////////////////////////

std::string Rite::GetName() const {
    return m_Name;
}

sf::Color Rite::GetColor() const {
    return m_Color;
}

//////////////////////////////////////////////////////

void Rite::SetName(const std::string& name) {
    m_Name = name;
}

void Rite::SetColor(const sf::Color& color) {
    m_Color = color;
}

//////////////////////////////////////////////////////