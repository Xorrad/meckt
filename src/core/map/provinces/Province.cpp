#include "Province.hpp"

Province::Province(int id, sf::Color color, std::string name) :
    m_Id(id),
    m_Name(name),
    m_Color(color),
    m_Flags(ProvinceFlags::NONE),
    m_Holding("none"),
    m_Terrain(""),
    m_Culture(""),
    m_Religion(""),
    m_ExtraHistoryData(MakeShared<Jomini::Object>(Jomini::ObjectMap{})),
    m_History({}),
    m_ImagePosition(sf::Vector2i(0, 0)),
    m_ImagePixelsCount(0),
    m_ClimateType(ClimateType::NONE),
    m_WinterSeverityBias(""),
    m_MildWinterFactorOverride(""),
    m_NormalWinterFactorOverride(""),
    m_HarshWinterFactorOverride("")
{}

int Province::GetId() const {
    return m_Id;
}

sf::Color Province::GetColor() const {
    return m_Color;
}
 
uint32_t Province::GetColorId() const {
    return m_Color.toInteger();
}

void Province::SetColor(sf::Color color) {
    m_Color = color;
}

std::string Province::GetName() const {
    return m_Name;
}

void Province::SetName(std::string name) {
    m_Name = name;
}

ProvinceFlags Province::GetFlags() const {
    return m_Flags;
}

bool Province::HasFlag(ProvinceFlags flag) const {
    return (bool) (m_Flags & flag);
}

void Province::SetFlags(ProvinceFlags flags) {
    m_Flags = flags;
}

void Province::SetFlag(ProvinceFlags flag, bool enabled) {
    if(enabled) m_Flags |= flag;
    else m_Flags &= (~flag);
}

std::string Province::GetHolding() const {
    return m_Holding;
}

void Province::SetHolding(std::string holding) {
    m_Holding = holding;
}

std::string Province::GetTerrain() const {
    return m_Terrain;
}

void Province::SetTerrain(std::string terrain) {
    m_Terrain = terrain;
}

std::string Province::GetCulture() const {
    return m_Culture;
}

void Province::SetCulture(std::string culture) {
    m_Culture = culture;
}

std::string Province::GetReligion() const {
    return m_Religion;
}

void Province::SetReligion(std::string religion) {
    m_Religion = religion;
}

ClimateType Province::GetClimateType() const {
    return m_ClimateType;
}

std::string Province::GetWinterSeverityBias() const {
    return m_WinterSeverityBias;
}

std::string Province::GetMildWinterFactorOverride() const {
    return m_MildWinterFactorOverride;
}

std::string Province::GetNormalWinterFactorOverride() const {
    return m_NormalWinterFactorOverride;
}

std::string Province::GetHarshWinterFactorOverride() const {
    return m_HarshWinterFactorOverride;
}

void Province::SetClimateType(ClimateType type) {
    m_ClimateType = type;
}

void Province::SetWinterSeverityBias(std::string bias) {
    m_WinterSeverityBias = bias;
}

void Province::SetMildWinterFactorOverride(std::string factor) {
    m_MildWinterFactorOverride = factor;
}

void Province::SetNormalWinterFactorOverride(std::string factor) {
    m_NormalWinterFactorOverride = factor;
}

void Province::SetHarshWinterFactorOverride(std::string factor) {
    m_HarshWinterFactorOverride = factor;
}

std::string Province::GetOriginalHistoryFilePath() const {
    return m_OriginalHistoryFilePath;
}

void Province::SetOriginalHistoryFilePath(const std::string& filePath) {
    m_OriginalHistoryFilePath = filePath;
}

SharedPtr<Jomini::Object> Province::GetExtraHistoryData() const {
    return m_ExtraHistoryData;
}

void Province::SetExtraHistoryData(SharedPtr<Jomini::Object> data) {
    m_ExtraHistoryData = data;
}

std::map<Jomini::Date, SharedPtr<Jomini::Object>>& Province::GetHistory() {
    return m_History;
}

void Province::AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data) {
    m_History[date] = data;
}

void Province::RemoveHistory(Jomini::Date date) {
    m_History.erase(date);
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