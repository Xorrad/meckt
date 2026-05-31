#include "TerrainType.hpp"

TerrainType::TerrainType()
    : m_Name(""), m_Color(sf::Color::Black)
{}

TerrainType::TerrainType(const std::string& name, const sf::Color& color)
    : m_Name(name), m_Color(color)
{}

std::string TerrainType::GetName() const {
    return m_Name;
}

void TerrainType::SetName(const std::string& name) {
    m_Name = name;
}

sf::Color TerrainType::GetColor() const {
    return m_Color;
}

void TerrainType::SetColor(const sf::Color& color) {
    m_Color = color;
}