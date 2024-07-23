#include "Mod.hpp"
#include "app/map/Province.hpp"

#include <filesystem>
#include <fmt/ostream.h>

Mod::Mod(const std::string& dir)
: m_Dir(dir) {}

std::string Mod::GetDir() const {
    return m_Dir;
}

sf::Image& Mod::getHeightmapImage() {
    return m_HeightmapImage;
}

sf::Image& Mod::getProvinceImage() {
    return m_ProvinceImage;
}

sf::Image& Mod::getRiversImage() {
    return m_RiversImage;
}

bool Mod::HasMap() const {
    return std::filesystem::exists(m_Dir + "/map_data/provinces.png");
}

std::map<uint32_t, SharedPtr<Province>>& Mod::GetProvinces() {
    return m_Provinces;
}

void Mod::LoadMapModeTexture(sf::Texture& texture, MapMode mode) {
    switch(mode) {
        case MapMode::PROVINCES:
            texture.loadFromImage(m_ProvinceImage);
            return;
        case MapMode::HEIGHTMAP:
            texture.loadFromImage(m_HeightmapImage);
            return;
        case MapMode::RIVERS:
            texture.loadFromImage(m_RiversImage);
            return;
    }
}

void Mod::Load() {
    if(!this->HasMap())
        return;

    // TODO: catch exceptions.
    m_HeightmapImage.loadFromFile(m_Dir + "/map_data/heightmap.png");
    m_ProvinceImage.loadFromFile(m_Dir + "/map_data/provinces.png");
    m_RiversImage.loadFromFile(m_Dir + "/map_data/rivers.png");

    this->LoadProvincesDefinition();
    this->LoadProvincesTerrain();
    this->LoadTitlesHistory();
    this->LoadTitles();
}

void Mod::LoadProvincesDefinition() {
    std::ifstream file(m_Dir + "/map_data/definition.csv");
    std::string line;

    while (std::getline(file, line)) {
        std::stringstream lineStream(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(lineStream, cell, ';')) {
            row.push_back(cell);
        }

        int id = std::stoi(row[0]);
        int r = std::stoi(row[1]);
        int g = std::stoi(row[2]);
        int b = std::stoi(row[3]);
        std::string name = row[4];

        SharedPtr<Province> province = MakeShared<Province>(id, sf::Color(r, g, b), name);
        if(m_Provinces.count(province->GetColorId()) != 0)
            INFO("Several provinces with same color: {}", id);
        m_Provinces[province->GetColorId()] = province;
    }
}

void Mod::LoadProvincesTerrain() {

}

void Mod::LoadTitlesHistory() {

}

void Mod::LoadTitles() {

}

void Mod::Export() {
    this->ExportProvincesDefinition();
}

void Mod::ExportProvincesDefinition() {
    std::ofstream file(m_Dir + "/map_data/definition.csv", std::ios::out);

    // The format of definition.csv is as following:
    // [ID];[RED];[GREEN];[BLUE];[Barony Name];x;

    file << "0;0;0;0;x;x\n";

    // "IDs must be sequential, or your game will crash."
    // That's why it is needed to make a sorted list of the provinces.
    std::vector<SharedPtr<Province>> provincesSorted;
    for(auto&[colorId, province] : m_Provinces) {
        provincesSorted.push_back(province);
    }
    std::sort(provincesSorted.begin(), provincesSorted.end(), [=](SharedPtr<Province>& a, SharedPtr<Province>& b) {
        return a->GetId() < b->GetId();
    });

    for(auto& province : provincesSorted) {
        fmt::println(file, 
            "{};{};{};{};{};x",
            province->GetId(),
            province->GetColor().r,
            province->GetColor().g,
            province->GetColor().b,
            province->GetName()
        );
    }
}