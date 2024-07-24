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

std::map<int, SharedPtr<Province>>& Mod::GetProvincesByIds() {
    return m_ProvincesByIds;
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
        m_ProvincesByIds[province->GetId()] = province;
    }
}

void Mod::LoadProvincesTerrain() {
    std::ifstream file(m_Dir + "/common/province_terrain/00_province_terrain.txt");

    std::map<std::string, std::string> values;
    std::string line;

    while (std::getline(file, line)) {
        line = String::Strip(line, " ");
        line = String::Strip(line, "\n");
        line = String::Strip(line, "\r");
        line = String::StripNonPrintable(line);

        // Remove everything that is after a comment.
        size_t i = line.find("#");
        if(i != std::string::npos) {
            line = line.substr(0, i);
        }

        std::stringstream lineStream(line);
        std::vector<std::string> row;
        std::string cell;

        while (std::getline(lineStream, cell, '=')) {
            row.push_back(cell);
        }

        if(row.size() < 2)
            continue;

        values[row[0]] = row[1];
    }

    m_DefaultLandTerrain = TerrainTypefromString(values["default_land"]);
    m_DefaultSeaTerrain = TerrainTypefromString(values["default_sea"]);
    m_DefaultCoastalSeaTerrain = TerrainTypefromString(values["default_coastal_sea"]);

    values.erase("default_land");
    values.erase("default_sea");
    values.erase("default_coastal_sea");

    for(const auto& [colorId, province] : m_Provinces) {
        TerrainType defaultTerrain = (province->IsSea() ? m_DefaultSeaTerrain : m_DefaultLandTerrain);
        province->SetTerrain(defaultTerrain);
    }

    for(const auto& [key, value] : values) {
        int id = std::stoi(key);
        TerrainType terrain = TerrainTypefromString(value);

        if(m_ProvincesByIds.count(id) == 0) {
            INFO("A province's terrain is defined but the province does not exist: {}", id);
            continue;
        }

        m_ProvincesByIds[id]->SetTerrain(terrain);
        m_ProvincesByIds[id]->SetIsSea(false);
    }
}

void Mod::LoadTitlesHistory() {

}

void Mod::LoadTitles() {

}

void Mod::Export() {
    this->ExportProvincesDefinition();
    this->ExportProvincesTerrain();
}

void Mod::ExportProvincesDefinition() {
    std::ofstream file(m_Dir + "/map_data/definition.csv", std::ios::out);

    // The format of definition.csv is as following:
    // [ID];[RED];[GREEN];[BLUE];[Barony Name];x;

    file << "0;0;0;0;x;x\n";

    // "IDs must be sequential, or your game will crash."
    // That's why it is needed to make a sorted list of the provinces.
    // std::vector<SharedPtr<Province>> provincesSorted;
    // for(auto&[colorId, province] : m_Provinces) {
    //     provincesSorted.push_back(province);
    // }
    // std::sort(provincesSorted.begin(), provincesSorted.end(), [=](SharedPtr<Province>& a, SharedPtr<Province>& b) {
    //     return a->GetId() < b->GetId();
    // });
    // std::map already sorts key in increasing order
    // so there is no need to sort it again.

    for(auto& [id, province] : m_ProvincesByIds) {
        fmt::println(file, 
            "{};{};{};{};{};x",
            province->GetId(),
            province->GetColor().r,
            province->GetColor().g,
            province->GetColor().b,
            province->GetName()
        );
    }

    file.close();
}

void Mod::ExportProvincesTerrain() {
    std::ofstream file(m_Dir + "/common/province_terrain/00_province_terrain.txt", std::ios::out);

    auto terrainToString = [=](TerrainType terrain) {
        return String::ToLowercase(TerrainTypeLabels[(int) terrain]);
    };

    fmt::println(file, "default_land={}", terrainToString(m_DefaultLandTerrain));
    fmt::println(file, "default_sea={}", terrainToString(m_DefaultSeaTerrain));
    fmt::println(file, "default_coastal_sea={}", terrainToString(m_DefaultCoastalSeaTerrain));

    for(auto& [id, province] : m_ProvincesByIds) {
        fmt::println(file,
            "{}={}",
            province->GetId(),
            terrainToString(province->GetTerrain())
        );
    }

    file.close();
}