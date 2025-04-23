#include "Province.hpp"

HoldingType::HoldingType() : m_Name("") {}

HoldingType::HoldingType(const std::string& name) : m_Name(name) {}

std::string HoldingType::GetName() const {
    return m_Name;
}

void HoldingType::SetName(const std::string& name) {
    m_Name = name;
}

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

Province::Province(int id, sf::Color color, std::string name) {
    m_Id = id;
    m_Color = color;
    m_Name = name;
    m_Flags = ProvinceFlags::NONE;
    m_Holding = "none";
    m_Terrain = "";
    m_OriginalData = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
    m_ImagePosition = sf::Vector2i(0, 0);
    m_ImagePixelsCount = 0;
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

std::string Province::GetHolding() const {
    return m_Holding;
}

std::string Province::GetTerrain() const {
    return m_Terrain;
}

std::string Province::GetCulture() const {
    return m_Culture;
}

std::string Province::GetReligion() const {
    return m_Religion;
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

void Province::SetHolding(std::string holding) {
    m_Holding = holding;
}

void Province::SetTerrain(std::string terrain) {
    m_Terrain = terrain;
}

void Province::SetCulture(std::string culture) {
    m_Culture = culture;
}

void Province::SetReligion(std::string religion) {
    m_Religion = religion;
}

std::string Province::GetOriginalFilePath() const {
    return m_OriginalFilePath;
}

SharedPtr<Jomini::Object> Province::GetOriginalData() const {
    return m_OriginalData;
}

void Province::SetOriginalFilePath(const std::string& filePath) {
    m_OriginalFilePath = filePath;
}

void Province::SetOriginalData(SharedPtr<Jomini::Object> data) {
    m_OriginalData = data;
}

sf::Vector2i Province::GetImagePosition() const {
    return m_ImagePosition;
}

uint Province::GetImagePixelsCount() const {
    return m_ImagePixelsCount;
}

void Province::SetImagePosition(sf::Vector2i pos) {
    m_ImagePosition = pos;
}

void Province::SetImagePixelsCount(uint count) {
    m_ImagePixelsCount = count;
}

void Province::IncrementImagePixelsCount() {
    m_ImagePixelsCount++;
}