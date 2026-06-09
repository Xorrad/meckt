#include "Province.hpp"

#include "titles/TitleManager.hpp"
#include "provinces/ProvinceManager.hpp"

Province::Province(int id, sf::Color color, std::string name) :
    m_Id(id),
    m_Name(name),
    m_Color(color),
    m_Flags(ProvinceFlags::NONE),
    m_Holding("none"),
    m_Terrain(""),
    m_Culture(""),
    m_Faith(""),
    m_OriginalHistoryFileName(""),
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

Title* Province::GetProvinceLiegeTitle(TitleManager& titleManager, TitleType type) const {
    Title* liege = static_cast<Title*>(titleManager.GetBaronyByProvinceId(m_Id));

    while (liege != nullptr) {
        if(liege->Is(type))
            return liege;

        liege = liege->GetLiegeTitle();
    }

    return liege;
}

Title* Province::GetProvinceFocusedTitle(TitleManager& titleManager, TitleType type) const {
    BaronyTitle* baronyTitle = titleManager.GetBaronyByProvinceId(m_Id);
    if (baronyTitle == nullptr)
        return nullptr;
    Title* title = static_cast<Title*>(baronyTitle);

    while(title->GetLiegeTitle() != nullptr && static_cast<int>(title->GetType()) < static_cast<int>(type) && title->GetLiegeTitle()->HasSelectionFocus()) {
        title = title->GetLiegeTitle();
    }

    // Return nullptr if the title hasn't any liege title of the provided type.
    if (static_cast<int>(title->GetType()) < static_cast<int>(type) && title->GetLiegeTitle() == nullptr)
        return nullptr;

    return title;
}

float Province::CalculateWinterSeverityBias(ProvinceManager& provinceManager, bool override,float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor) const {
    // If no overrides and the climate is already initialized, then we use that value for the preview.
    if (!override && (m_ClimateType != ClimateType::NONE || !m_WinterSeverityBias.empty())) {
        if (!m_WinterSeverityBias.empty() && String::IsDigit(m_WinterSeverityBias[0]))
            return static_cast<float>(std::stod(m_WinterSeverityBias));
        if (m_ClimateType != ClimateType::NONE)
            return (m_ClimateType == ClimateType::MILD_WINTER ? 0.f :
                (m_ClimateType == ClimateType::MILD_WINTER ? 0.5f : 1.f)
            );
        return 0.f;
    }

    // Otherwise, if the province is safe to edit, then determine the winter severity
    // using the elevation and hemisphere.
    sf::Color color = (m_ImagePosition.x < 0 || m_ImagePosition.x >= provinceManager.GetHeightmapImage().getSize().x || m_ImagePosition.y < 0 || m_ImagePosition.y >= provinceManager.GetHeightmapImage().getSize().y)
        ? sf::Color::Black
        : provinceManager.GetHeightmapImage().getPixel(sf::Vector2u(m_ImagePosition.x, m_ImagePosition.y));
    float elevation = std::min(1.f, color.r/255.f) * elevationStrength + elevationOffset;

    float hemisphere = std::min(
        1.f, 
        (hemisphereStrength * abs(m_ImagePosition.y - (provinceManager.GetHeightmapImage().getSize().y / 2.f) + hemisphereOffset) - hemisphereSize) / provinceManager.GetHeightmapImage().getSize().y
    );
    
    float winterSeverityBias = elevation * elevationFactor + hemisphere * hemisphereFactor;
    winterSeverityBias = std::max(0.f, std::min(1.f, winterSeverityBias));
    winterSeverityBias = winterSeverityBias;

    return winterSeverityBias;
}

int Province::GetId() const {
    return m_Id;
}

void Province::SetId(int id) {
    m_Id = id;
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

std::string Province::GetFaith() const {
    return m_Faith;
}

void Province::SetFaith(std::string faith) {
    m_Faith = faith;
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

std::string Province::GetOriginalHistoryFileName() const {
    return m_OriginalHistoryFileName;
}

void Province::SetOriginalHistoryFileName(const std::string& fileName) {
    m_OriginalHistoryFileName = fileName;
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

size_t Province::GetImagePixelsCount() const {
    return m_ImagePixelsCount;
}

void Province::SetImagePosition(sf::Vector2i pos) {
    m_ImagePosition = pos;
}

void Province::SetImagePixelsCount(size_t count) {
    m_ImagePixelsCount = count;
}

void Province::IncrementImagePixelsCount() {
    m_ImagePixelsCount++;
}