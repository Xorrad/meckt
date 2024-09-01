#include "Province.hpp"
#include "parser/Parser.hpp"

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) | static_cast<int>(b));
}


ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) & static_cast<int>(b));
}

ProvinceFlags operator~(ProvinceFlags a) {
    return static_cast<ProvinceFlags>(~static_cast<int>(a));
}

ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a | b;
}

ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a & b;
}

ProvinceHolding ProvinceHoldingFromString(const std::string& str) {
    for(int i = 0; i < ProvinceHoldingLabels.size(); i++) {
        if(String::ToLowercase(ProvinceHoldingLabels[i]) == str)
            return (ProvinceHolding) i;
    }
    return ProvinceHolding::NONE;
}

Province::Province(int id, sf::Color color, std::string name) {
    m_Id = id;
    m_Color = color;
    m_Name = name;
    m_Flags = ProvinceFlags::NONE;
    m_Terrain = TerrainType::PLAINS;
    m_Holding = ProvinceHolding::NONE;
}

int Province::GetId() const {
    return m_Id;
}

sf::Color Province::GetColor() const {
    return m_Color;
}
 
uint32_t Province::GetColorId() const {
    return m_Color.toInteger();
}

std::string Province::GetName() const {
    return m_Name;
}

ProvinceFlags Province::GetFlags() const {
    return m_Flags;
}

bool Province::HasFlag(ProvinceFlags flag) const {
    return (bool) (m_Flags & flag);
}

TerrainType Province::GetTerrain() const {
    return m_Terrain;
}

std::string Province::GetCulture() const {
    return m_Culture;
}

std::string Province::GetReligion() const {
    return m_Religion;
}

ProvinceHolding Province::GetHolding() const {
    return m_Holding;
}

void Province::SetName(std::string name) {
    m_Name = name;
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

void Province::SetCulture(std::string culture) {
    m_Culture = culture;
}

void Province::SetReligion(std::string religion) {
    m_Religion = religion;
}

void Province::SetHolding(ProvinceHolding holding) {
    m_Holding = holding;
}

std::string Province::GetOriginalFilePath() const {
    return m_OriginalFilePath;
}

SharedPtr<Parser::Node> Province::GetOriginalData() const {
    return m_OriginalData;
}

void Province::SetOriginalFilePath(const std::string& filePath) {
    m_OriginalFilePath = filePath;
}

void Province::SetOriginalData(const Parser::Node& data) {
    m_OriginalData = MakeShared<Parser::Node>(data);
}