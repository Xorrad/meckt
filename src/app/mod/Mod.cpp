#include "Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"
#include "parser/Parser.hpp"

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

std::map<std::string, SharedPtr<Title>>& Mod::GetTitles() {
    return m_Titles;
}

std::map<TitleType, std::vector<SharedPtr<Title>>>& Mod::GetTitlesByType() {
    return m_TitlesByType;
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
        case MapMode::CULTURE:
            return;
        case MapMode::RELIGION:
            return;
        case MapMode::COUNTY:
            UpdateTitlesImages(TitleType::COUNTY);
            texture.loadFromImage(m_TitlesImage);
            return;
        case MapMode::DUCHY:
            UpdateTitlesImages(TitleType::DUCHY);
            texture.loadFromImage(m_TitlesImage);
            return;
        case MapMode::KINGDOM:
            UpdateTitlesImages(TitleType::KINGDOM);
            texture.loadFromImage(m_TitlesImage);
            return;
        case MapMode::EMPIRE:
            UpdateTitlesImages(TitleType::EMPIRE);
            texture.loadFromImage(m_TitlesImage);
            return;
        default:
            return;
    }
}

void Mod::UpdateTitlesImages(TitleType type) {
    // Map provinces color to the titles color and then when 
    // updating the image, swap the color of each pixel.
    std::map<sf::Uint32, sf::Color> colors;

    std::function<const SharedPtr<HighTitle>(const SharedPtr<BaronyTitle>&)> GetLiegeTitle;

    switch(type) {
        case TitleType::COUNTY:
            GetLiegeTitle = [&](const SharedPtr<BaronyTitle>& barony) { return barony->GetLiegeTitle(); };
            break;
        case TitleType::DUCHY:
            GetLiegeTitle = [&](const SharedPtr<BaronyTitle>& barony) { return barony->GetLiegeTitle()->GetLiegeTitle(); };
            break;
        case TitleType::KINGDOM:
            GetLiegeTitle = [&](const SharedPtr<BaronyTitle>& barony) { return barony->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle(); };
            break;
        case TitleType::EMPIRE:
            GetLiegeTitle = [&](const SharedPtr<BaronyTitle>& barony) { return barony->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle(); };
            break;
        default:
            return;
    }

    for(const auto& [provinceColorId, province] : m_Provinces) {
        if(m_Titles.count(province->GetName()) == 0) {
            // INFO("{}", province->GetName());
            colors[province->GetColor().toInteger()] = province->GetColor();
            continue;
        }

        const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(m_Titles[province->GetName()]);
        if(barony->GetLiegeTitle() == nullptr) {
            colors[province->GetColor().toInteger()] = province->GetColor();
            continue;
        }
        
        const SharedPtr<HighTitle>& liege = GetLiegeTitle(barony);
        colors[province->GetColor().toInteger()] = liege->GetColor();
    }

    //m_TitlesImage.create(m_ProvinceImage.getSize().x, m_ProvinceImage.getSize().y);
    m_TitlesImage = m_ProvinceImage;

    for(uint y = 0; y < m_TitlesImage.getSize().y; y++) {
        for(uint x = 0; x < m_TitlesImage.getSize().x; x++) {
            m_TitlesImage.setPixel(x, y, colors[m_ProvinceImage.getPixel(x, y).toInteger()]);
        }
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
    this->LoadDefaultMapFile();
    this->LoadProvincesTerrain();
    this->LoadProvincesInfo();
    this->LoadTitles();
    this->LoadTitlesHistory();
}

void Mod::LoadDefaultMapFile() {
    Parser::Node result = Parser::Parse(m_Dir + "/map_data/default.map");

    // TODO: Coastal provinces??
    
    std::vector<double> lakes = result.Get("lakes");
    for(double provinceId : lakes) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAKE, true);
    }
    
    // TODO: Islands provinces??
    // TODO: Land provinces??

    const std::vector<double>& seaZones = result.Get("sea_zones");
    for(double provinceId : seaZones) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
    }

    const std::vector<double>& rivers = result.Get("river_provinces");
    for(double provinceId : rivers) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::RIVER, true);
    }
    
    std::vector<double>& impassable = result.Get("impassable_seas");
    for(double provinceId : impassable) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
    
    impassable = result.Get("impassable_mountains");
    for(double provinceId : impassable) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
}

void Mod::LoadProvincesDefinition() {
    std::vector<std::vector<std::string>> lines = File::ReadCSV(m_Dir + "/map_data/definition.csv");
    
    // Skip the first line.
    lines.erase(lines.begin());

    for(const auto& line : lines) {
        int id = std::stoi(line[0]);
        int r = std::stoi(line[1]);
        int g = std::stoi(line[2]);
        int b = std::stoi(line[3]);
        std::string name = line[4];

        SharedPtr<Province> province = MakeShared<Province>(id, sf::Color(r, g, b), name);
        if(m_Provinces.count(province->GetColorId()) > 0)
            INFO("Several provinces with same color: {}", id);
        m_Provinces[province->GetColorId()] = province;
        m_ProvincesByIds[province->GetId()] = province;
    }
}

void Mod::LoadProvincesTerrain() {
    Parser::Node result = Parser::Parse(m_Dir + "/common/province_terrain/00_province_terrain.txt");

    m_DefaultLandTerrain = TerrainTypefromString(result.Get("default_land"));
    m_DefaultSeaTerrain = TerrainTypefromString(result.Get("default_sea"));
    m_DefaultCoastalSeaTerrain = TerrainTypefromString(result.Get("default_coastal_sea"));

    for(const auto& [colorId, province] : m_Provinces) {
        TerrainType defaultTerrain = m_DefaultLandTerrain;

        if(province->HasFlag(ProvinceFlags::SEA))
            defaultTerrain = (province->HasFlag(ProvinceFlags::COASTAL) ? m_DefaultCoastalSeaTerrain : m_DefaultSeaTerrain);

        province->SetTerrain(defaultTerrain);
    }

    for(const auto& [key, value] : result.GetEntries()) {
        if(!std::holds_alternative<double>(key))
            continue;

        int provinceId = std::get<double>(key);
        TerrainType terrain = TerrainTypefromString(value);

        if(m_ProvincesByIds.count(provinceId) == 0) {
            INFO("A province's terrain is defined but the province does not exist: {}", provinceId);
            continue;
        }

        m_ProvincesByIds[provinceId]->SetTerrain(terrain);

        if(!m_ProvincesByIds[provinceId]->HasFlag(ProvinceFlags::SEA))
            m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
    }
}

void Mod::LoadProvincesInfo() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/history/provinces/");

    for(const auto& filePath : filesPath) {
        Parser::Node result = Parser::Parse(filePath);
        
        for(const auto& [key, value] : result.GetEntries()) {
            if(!std::holds_alternative<double>(key))
                continue;
            int provinceId = std::get<double>(key);

            if(value.ContainsKey("culture"))
                m_ProvincesByIds[provinceId]->SetCulture(value.Get("culture"));
            if(value.ContainsKey("religion"))
                m_ProvincesByIds[provinceId]->SetReligion(value.Get("religion"));
            if(value.ContainsKey("holding"))
                m_ProvincesByIds[provinceId]->SetHolding(ProvinceHoldingFromString(value.Get("holding")));
        }
    }
}

void Mod::LoadTitlesHistory() {

}

void Mod::LoadTitles() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/landed_titles/");

    for(int i = 0; i < (int) TitleType::COUNT; i++)
        m_TitlesByType[(TitleType) i] = std::vector<SharedPtr<Title>>();

    for(const auto& filePath : filesPath) {
        // fmt::println("loading titles from {}", filePath);
        Parser::Node data = Parser::Parse(filePath);
        std::vector<SharedPtr<Title>> titles = ParseTitles(data);
    }

    INFO("loaded {} titles from {} files", m_Titles.size(), filesPath.size());
    
    for(int i = 0; i < (int) TitleType::COUNT; i++)
        INFO("loaded {} {} titles", m_TitlesByType[(TitleType) i].size(), TitleTypeLabels[i]);
}

std::vector<SharedPtr<Title>> Mod::ParseTitles(Parser::Node& data) {
    std::vector<SharedPtr<Title>> titles;

    for(auto& [k, value] : data.GetEntries()) {
        if(!std::holds_alternative<std::string>(k))
            continue;
        std::string key = std::get<std::string>(k);

        // Need to check if the key is a title (starts with e_, k_, d_, c_ or b_)
        // because it could be attributes such as color, capital, can_create...

        try {
            // This function throws an exception if key does not
            // correspond to a title type.
            TitleType type = GetTitleTypeByName(key);

            // Need to use a custom function to create a SharedPtr<Title>
            // to get the right derived class such as BaronyTitle, CountyTitle...
            SharedPtr<Title> title = MakeTitle(type, key, value.Get("color"));

            // fmt::println("{} {}", key, TitleTypeLabels[(int) type]);

            if(type == TitleType::BARONY) {
                SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
                baronyTitle->SetProvinceId(value.Get("province"));
            }
            else {
                SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);
                std::vector<SharedPtr<Title>> dejureTitles = ParseTitles(value);

                for(const auto& dejureTitle : dejureTitles) {
                    highTitle->AddDejureTitle(dejureTitle);
                }

                if(type != TitleType::COUNTY && value.ContainsKey("capital")) {
                    std::string capitalName = value.Get("capital");
                    if(m_Titles.count(capitalName) > 0 && IsInstance<CountyTitle>(m_Titles[capitalName])) {
                        highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(m_Titles[capitalName]));
                    }
                }
            }

            m_Titles[key] = title;
            m_TitlesByType[type].push_back(title);
            titles.push_back(title);

            // if(type == TitleType::EMPIRE) break;
        }
        catch(const std::runtime_error& e) {}
    }

    return titles;
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