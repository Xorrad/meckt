#include "Province.hpp"

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) | static_cast<int>(b));
}


ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) & static_cast<int>(b));
}

ProvinceFlags operator~(ProvinceFlags a) {
    return static_cast<ProvinceFlags>(static_cast<int>(~a));
}

ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a | b;
}

ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a & b;
}

Province::Province(int id, sf::Color color, std::string name)
: m_Id(id), m_Color(color), m_Name(name), m_Flags(ProvinceFlags::NONE), m_Terrain(TerrainType::PLAINS) {}

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

ProvinceFlags& Province::GetFlags() {
    return m_Flags;
}

bool Province::HasFlag(ProvinceFlags flag) const {
    return (bool) (m_Flags & flag);
}

TerrainType& Province::GetTerrain() {
    return m_Terrain;
}

void Province::SetColor(sf::Color color) {
    m_Color = color;
}

void Province::SetFlags(ProvinceFlags flags) {
    m_Flags = flags;
}

void Province::SetFlag(ProvinceFlags flag, bool enabled) {
    if(enabled) m_Flags |= flag;
    else m_Flags &= (~flag);
}

void Province::SetTerrain(TerrainType terrain) {
    m_Terrain = terrain;
}