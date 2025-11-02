#include "Region.hpp"
#include "Title.hpp"

Region::Region(std::string name) :
    m_Name(name),
    m_Kingdoms({}),
    m_Duchies({}),
    m_Counties({}),
    m_Provinces({}),
    m_Regions({}),
    m_GenerateModifiers(false)
{}

std::string Region::GetName() const {
    return m_Name;
}

std::vector<SharedPtr<KingdomTitle>>& Region::GetKingdoms() {
    return m_Kingdoms;
}

std::vector<SharedPtr<DuchyTitle>>& Region::GetDuchies() {
    return m_Duchies;
}

std::vector<SharedPtr<CountyTitle>>& Region::GetCounties() {
    return m_Counties;
}

std::vector<SharedPtr<Province>>& Region::GetProvinces() {
    return m_Provinces;
}

std::vector<SharedPtr<Region>>& Region::GetRegions() {
    return m_Regions;
}

bool Region::HasTitle(SharedPtr<Title> title) const {
    if (title->Is(TitleType::EMPIRE) || title->Is(TitleType::HEGEMONY))
        return false;
    if (title->Is(TitleType::KINGDOM))
        return std::find(m_Kingdoms.begin(), m_Kingdoms.end(), CastSharedPtr<KingdomTitle>(title)) != m_Kingdoms.end();
    if (title->Is(TitleType::DUCHY))
        return std::find(m_Duchies.begin(), m_Duchies.end(), CastSharedPtr<DuchyTitle>(title)) != m_Duchies.end();
    if (title->Is(TitleType::COUNTY))
        return std::find(m_Counties.begin(), m_Counties.end(), CastSharedPtr<CountyTitle>(title)) != m_Counties.end();
    return false;
}

bool Region::HasProvince(SharedPtr<Province> province) const {
    return std::find(m_Provinces.begin(), m_Provinces.end(), province) != m_Provinces.end();
}

bool Region::HasRegion(SharedPtr<Region> region) const {
    return std::find(m_Regions.begin(), m_Regions.end(), region) != m_Regions.end();
}

bool Region::DoesGenerateModifiers() const {
    return m_GenerateModifiers;
}

void Region::SetName(std::string name) {
    m_Name = name;
}

void Region::AddTitle(SharedPtr<Title> title) {
    switch (title->GetType()) {
        case TitleType::KINGDOM: this->AddKingdom(CastSharedPtr<KingdomTitle>(title)); break;
        case TitleType::DUCHY: this->AddDuchy(CastSharedPtr<DuchyTitle>(title)); break;
        case TitleType::COUNTY: this->AddCounty(CastSharedPtr<CountyTitle>(title)); break;
        default: break;
    }
}

void Region::RemoveTitle(SharedPtr<Title> title) {
    switch (title->GetType()) {
        case TitleType::KINGDOM: this->RemoveKingdom(CastSharedPtr<KingdomTitle>(title)); break;
        case TitleType::DUCHY: this->RemoveDuchy(CastSharedPtr<DuchyTitle>(title)); break;
        case TitleType::COUNTY: this->RemoveCounty(CastSharedPtr<CountyTitle>(title)); break;
        default: break;
    }
}

void Region::AddKingdom(SharedPtr<KingdomTitle> title) {
    if (title == nullptr || this->HasTitle(title))
        return;
    m_Kingdoms.emplace_back(title);
}

void Region::RemoveKingdom(SharedPtr<KingdomTitle> title) {
    if(title == nullptr)
        return;
    m_Kingdoms.erase(std::remove(m_Kingdoms.begin(), m_Kingdoms.end(), title), m_Kingdoms.end());
}

void Region::AddDuchy(SharedPtr<DuchyTitle> title) {
    if (title == nullptr || this->HasTitle(title))
        return;
    m_Duchies.emplace_back(title);
}

void Region::RemoveDuchy(SharedPtr<DuchyTitle> title) {
    if(title == nullptr)
        return;
    m_Duchies.erase(std::remove(m_Duchies.begin(), m_Duchies.end(), title), m_Duchies.end());
}

void Region::AddCounty(SharedPtr<CountyTitle> title) {
    if (title == nullptr || this->HasTitle(title))
        return;
    m_Counties.emplace_back(title);
}

void Region::RemoveCounty(SharedPtr<CountyTitle> title) {
    if(title == nullptr)
        return;
    m_Counties.erase(std::remove(m_Counties.begin(), m_Counties.end(), title), m_Counties.end());
}

void Region::AddProvince(SharedPtr<Province> province) {
    if (province == nullptr || this->HasProvince(province))
        return;
    m_Provinces.emplace_back(province);
}

void Region::RemoveProvince(SharedPtr<Province> province) {
    if(province == nullptr)
        return;
    m_Provinces.erase(std::remove(m_Provinces.begin(), m_Provinces.end(), province), m_Provinces.end());
}

void Region::AddRegion(SharedPtr<Region> region) {
    if (region == nullptr || this->HasRegion(region))
        return;
    m_Regions.emplace_back(region);
}

void Region::RemoveRegion(SharedPtr<Region> region) {
    if(region == nullptr)
        return;
    m_Regions.erase(std::remove(m_Regions.begin(), m_Regions.end(), region), m_Regions.end());
}

void Region::SetGenerateModifiers(bool generateModifiers) {
    m_GenerateModifiers = generateModifiers;
}