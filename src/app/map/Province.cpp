#include "Province.hpp"

Province::Province(int id, sf::Color color, std::string name)
: m_Id(id), m_Color(color), m_Name(name), m_Terrain(TerrainType::PLAINS), m_IsSea(true) {}

int& Province::GetId() {
    return m_Id;
}

sf::Color& Province::GetColor() {
    return m_Color;
}
 
uint32_t Province::GetColorId() const {
    return (m_Color.r << 16) + (m_Color.g << 8) + m_Color.b;
}

std::string& Province::GetName() {
    return m_Name;
}

TerrainType& Province::GetTerrain() {
    return m_Terrain;
}

bool Province::IsSea() const {
    return m_IsSea;
}

void Province::SetColor(sf::Color color) {
    m_Color = color;
}

void Province::SetTerrain(TerrainType terrain) {
    m_Terrain = terrain;
}

void Province::SetIsSea(bool isSea) {
    m_IsSea = isSea;
}