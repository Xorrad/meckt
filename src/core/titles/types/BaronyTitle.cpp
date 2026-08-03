#include "BaronyTitle.hpp"
#include "provinces/ProvinceManager.hpp"

BaronyTitle::BaronyTitle() :
    Title(),
    m_ProvinceId(0)
{}

BaronyTitle::BaronyTitle(std::string name, sf::Color color, bool landless) :
    Title(name, color, landless),
    m_ProvinceId(0)
{}

BaronyTitle::BaronyTitle(std::string name, sf::Color color, bool landless, int provinceId) :
    Title(name, color, landless),
    m_ProvinceId(provinceId)
{}

//////////////////////////////////////////////////////

TitleType BaronyTitle::GetType() const {
    return TitleType::BARONY;
}

int BaronyTitle::GetProvinceId() const {
    return m_ProvinceId;
}

void BaronyTitle::SetProvinceId(int id) {
    m_ProvinceId = id;
}

sf::Vector2i BaronyTitle::GetImagePosition(const ProvinceManager& provinceManager) const {
    if (const Province* province = provinceManager.GetProvinceById(m_ProvinceId))
        return province->GetImagePosition();
    return sf::Vector2i(0, 0);
}

//////////////////////////////////////////////////////

bool BaronyTitle::HasSelectionFocus() const {
    return true;
}

//////////////////////////////////////////////////////