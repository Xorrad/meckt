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

std::span<KingdomTitle*> Region::GetKingdoms() {
    return m_Kingdoms;
}

std::span<DuchyTitle*> Region::GetDuchies() {
    return m_Duchies;
}

std::span<CountyTitle*> Region::GetCounties() {
    return m_Counties;
}

std::span<Province*> Region::GetProvinces() {
    return m_Provinces;
}

std::span<Region*> Region::GetRegions() {
    return m_Regions;
}

bool Region::HasTitle(Title* title) const {
    if (title->Is(TitleType::EMPIRE) || title->Is(TitleType::HEGEMONY))
        return false;

    if (title->Is(TitleType::KINGDOM)) {
        return std::find(m_Kingdoms.begin(), m_Kingdoms.end(), title) != m_Kingdoms.end();
    }

    if (title->Is(TitleType::DUCHY)) {
        return std::find(m_Duchies.begin(), m_Duchies.end(), title) != m_Duchies.end();
    }

    if (title->Is(TitleType::COUNTY)) {
        return std::find(m_Counties.begin(), m_Counties.end(), title) != m_Counties.end();
    }

    return false;
}

bool Region::HasProvince(Province* province) const {
    return std::find(m_Provinces.begin(), m_Provinces.end(), province) != m_Provinces.end();
}

bool Region::HasRegion(Region* region) const {
    return std::find(m_Regions.begin(), m_Regions.end(), region) != m_Regions.end();
}

bool Region::DoesGenerateModifiers() const {
    return m_GenerateModifiers;
}

void Region::SetName(std::string name) {
    m_Name = name;
}

void Region::AddTitle(Title* title) {
    switch (title->GetType()) {
        case TitleType::KINGDOM: this->AddKingdom(static_cast<KingdomTitle*>(title)); break;
        case TitleType::DUCHY: this->AddDuchy(static_cast<DuchyTitle*>(title)); break;
        case TitleType::COUNTY: this->AddCounty(static_cast<CountyTitle*>(title)); break;
        default: break;
    }
}

void Region::RemoveTitle(Title* title) {
    switch (title->GetType()) {
        case TitleType::KINGDOM: this->RemoveKingdom(static_cast<KingdomTitle*>(title)); break;
        case TitleType::DUCHY: this->RemoveDuchy(static_cast<DuchyTitle*>(title)); break;
        case TitleType::COUNTY: this->RemoveCounty(static_cast<CountyTitle*>(title)); break;
        default: break;
    }
}

void Region::AddKingdom(KingdomTitle* title) {
    if (this->HasTitle(title))
        return;
    m_Kingdoms.emplace_back(title);
}

void Region::RemoveKingdom(KingdomTitle* title) {
    m_Kingdoms.erase(
        std::remove(m_Kingdoms.begin(), m_Kingdoms.end(), title),
        m_Kingdoms.end()
    );
}

void Region::AddDuchy(DuchyTitle* title) {
    if (this->HasTitle(title))
        return;
    m_Duchies.emplace_back(title);
}

void Region::RemoveDuchy(DuchyTitle* title) {
    m_Duchies.erase(
        std::remove(m_Duchies.begin(), m_Duchies.end(), title),
        m_Duchies.end()
    );
}

void Region::AddCounty(CountyTitle* title) {
    if (this->HasTitle(title))
        return;
    m_Counties.emplace_back(title);
}

void Region::RemoveCounty(CountyTitle* title) {
    m_Counties.erase(
        std::remove(m_Counties.begin(), m_Counties.end(), title),
        m_Counties.end()
    );
}

void Region::AddProvince(Province* province) {
    if (this->HasProvince(province))
        return;
    m_Provinces.emplace_back(std::ref(province));
}

void Region::RemoveProvince(Province* province) {
    m_Provinces.erase(
        std::remove(m_Provinces.begin(), m_Provinces.end(), province),
        m_Provinces.end()
    );
}

void Region::AddRegion(Region* region) {
    if (this->HasRegion(region))
        return;
    m_Regions.emplace_back(std::ref(region));
}

void Region::RemoveRegion(Region* region) {
    m_Regions.erase(
        std::remove(m_Regions.begin(), m_Regions.end(), region),
        m_Regions.end()
    );
}

void Region::SetGenerateModifiers(bool generateModifiers) {
    m_GenerateModifiers = generateModifiers;
}