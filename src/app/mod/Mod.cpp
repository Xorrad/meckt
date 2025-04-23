#include "Mod.hpp"
#include "app/mod/Culture.hpp"
#include "app/mod/Religion.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"
#include "util/Yaml.hpp"

#include <filesystem>
#include <fmt/ostream.h>

Mod::Mod(const std::string& dir) :
    m_Dir(dir),
    m_TitlesLocalizationFilePath(dir + "/localization/english/00_titles_l_english.yml"),
    m_CulturalNamesLocalizationFilePath(dir + "/localization/english/00_cultural_titles_l_english.yml")
{}

std::string Mod::GetDir() const {
    return m_Dir;
}

sf::Image& Mod::GetHeightmapImage() {
    return m_HeightmapImage;
}

sf::Image& Mod::GetProvinceImage() {
    return m_ProvinceImage;
}

sf::Image& Mod::GetRiversImage() {
    return m_RiversImage;
}

sf::Image Mod::GetTerrainImage() {
    // - Map provinces colors to their terrain color.
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    sf::Color defaultColor = sf::Color(0, 0, 0);

    sf::Image image = Image::MapPixels(m_ProvinceImage, [&](auto& mappedColors){
        for(const auto& [provinceColorId, province] : m_Provinces) {
            std::string terrain = province->GetTerrain();
            sf::Color color = defaultColor;

            if(m_TerrainTypes.contains(terrain)) {
                TerrainType terrainType = m_TerrainTypes.at(terrain);
                color = terrainType.GetColor();
            }

            mappedColors[province->GetColor().toInteger()] = color.toInteger();
        }    
    });
    return image;
}

sf::Image Mod::GetCultureImage() {
    // - Map provinces colors to their culture color (province -> county -> county capital -> province).
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    // - Provinces with an explicit culture assigned will have alpha=0
    //   in order to inform the shader.
    sf::Color defaultColor = sf::Color(127, 127, 127);

    sf::Image image = Image::MapPixels(m_ProvinceImage, [&](auto& mappedColors){
        for(const auto& [provinceColorId, province] : m_Provinces) {
            std::string cultureName = province->GetCulture();
            sf::Color color = defaultColor;
            sf::Uint8 alpha = cultureName.empty() ? 255 : 0;

            if(cultureName.empty()) {
                const SharedPtr<CountyTitle>& liege = CastSharedPtr<CountyTitle>(this->GetProvinceLiegeTitle(province, TitleType::COUNTY));

                if(liege == nullptr) {
                    goto End;
                }

                for(const auto& dejureTitle : liege->GetDejureTitles()) {
                    const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(dejureTitle);
                    const SharedPtr<Province>& baronyProvince = m_ProvincesByIds[barony->GetProvinceId()];

                    if(baronyProvince != nullptr && !baronyProvince->GetCulture().empty()) {
                        cultureName = baronyProvince->GetCulture();
                        break;
                    }
                }
            }

            if(cultureName.empty() || m_Cultures.count(cultureName) == 0) {
                color = sf::Color(cultureName[0], cultureName[1], cultureName[2]);
            } 
            else {
                SharedPtr<Culture> culture = m_Cultures[cultureName];
                color = culture->GetColor();
            }

            End:
            color.a = alpha;
            mappedColors[province->GetColor().toInteger()] = color.toInteger();
        }    
    });
    return image;
}

sf::Image Mod::GetReligionImage() {
    // - Map provinces colors to their religion color (province -> county -> county capital -> province).
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    // - Provinces with an explicit religion assigned will have alpha=0
    //   in order to inform the shader.
    sf::Color defaultColor = sf::Color(127, 127, 127);

    sf::Image image = Image::MapPixels(m_ProvinceImage, [&](auto& mappedColors){
        for(const auto& [provinceColorId, province] : m_Provinces) {
            std::string religionName = province->GetReligion();
            sf::Color color = defaultColor;
            sf::Uint8 alpha = religionName.empty() ? 255 : 0;

            if(religionName.empty()) {
                const SharedPtr<CountyTitle>& liege = CastSharedPtr<CountyTitle>(this->GetProvinceLiegeTitle(province, TitleType::COUNTY));

                if(liege == nullptr) {
                    goto End;
                }

                for(const auto& dejureTitle : liege->GetDejureTitles()) {
                    const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(dejureTitle);
                    const SharedPtr<Province>& baronyProvince = m_ProvincesByIds[barony->GetProvinceId()];

                    if(baronyProvince != nullptr && !baronyProvince->GetReligion().empty()) {
                        religionName = baronyProvince->GetReligion();
                        break;
                    }
                }
            }

            if(m_Religions.count(religionName) == 0 || m_Religions.count(religionName) == 0) {
                color = sf::Color(religionName[0], religionName[1], religionName[2]);
            } 
            else {
                SharedPtr<Religion> religion = m_Religions[religionName];
                color = religion->GetColor();
            }

            End:
            color.a = alpha;
            mappedColors[province->GetColor().toInteger()] = color.toInteger();
        }    
    });
    return image;
}

sf::Image Mod::GetTitleImage(TitleType type) {
    sf::Image image = Image::MapPixels(m_ProvinceImage, [&](auto& mappedColors){
        for(const auto& [provinceColorId, province] : m_Provinces) {
            const SharedPtr<Title>& liege = this->GetProvinceFocusedTitle(province, type);

            if(liege == nullptr)
                continue;

            mappedColors[province->GetColor().toInteger()] = liege->GetColor().toInteger();
        }    
    });
    return image;
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

SharedPtr<Title> Mod::GetProvinceLiegeTitle(const SharedPtr<Province>& province, TitleType type) {
    if(m_BaroniesByProvinceIds.count(province->GetId()) == 0)
        return nullptr;

    const SharedPtr<Title>& barony = m_BaroniesByProvinceIds[province->GetId()];

    #define RETURN_IF_NULL(v) if(v == nullptr) return nullptr;

    // Determine which liege title to choose depending on type.
    switch(type) {
        case TitleType::BARONY:  return barony;
        case TitleType::COUNTY:  return barony->GetLiegeTitle();
        case TitleType::DUCHY:
            RETURN_IF_NULL(barony->GetLiegeTitle());
            return barony->GetLiegeTitle()->GetLiegeTitle();
        case TitleType::KINGDOM:
            RETURN_IF_NULL(barony->GetLiegeTitle());
            RETURN_IF_NULL(barony->GetLiegeTitle()->GetLiegeTitle());
            return barony->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle();
        case TitleType::EMPIRE:
            RETURN_IF_NULL(barony->GetLiegeTitle());
            RETURN_IF_NULL(barony->GetLiegeTitle()->GetLiegeTitle());
            RETURN_IF_NULL(barony->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle());
            return barony->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle()->GetLiegeTitle();
        default: return nullptr;
    }
}

SharedPtr<Title> Mod::GetProvinceFocusedTitle(const SharedPtr<Province>& province, TitleType type) {
    if(m_BaroniesByProvinceIds.count(province->GetId()) == 0)
        return nullptr;

    SharedPtr<Title> barony = m_BaroniesByProvinceIds[province->GetId()];
    SharedPtr<Title> title = barony;

    while(title->GetLiegeTitle() != nullptr && (int) title->GetType() < (int) type && title->GetLiegeTitle()->HasSelectionFocus()) {
        title = title->GetLiegeTitle();
    }

    return title;
}

int Mod::GetMaxProvinceId() const {
    return m_ProvincesByIds.empty() ? -1 : m_ProvincesByIds.rbegin()->first;
}

std::map<std::string, SharedPtr<Title>>& Mod::GetTitles() {
    return m_Titles;
}

std::map<TitleType, std::vector<SharedPtr<Title>>>& Mod::GetTitlesByType() {
    return m_TitlesByType;
}

std::map<int, SharedPtr<BaronyTitle>>& Mod::GetBaroniesByProvinceIds() {
    return m_BaroniesByProvinceIds;
}

const OrderedMap<std::string, HoldingType>& Mod::GetHoldingTypes() const {
    return m_HoldingTypes;
}

const OrderedMap<std::string, TerrainType>& Mod::GetTerrainTypes() const {
    return m_TerrainTypes;
}

std::map<std::string, std::map<std::string, std::string>>& Mod::GetLocCulturalNames() {
    return m_LocCulturalNames;
}

std::map<std::string, std::string>& Mod::GetLocCulturalNames(const std::string& lang) {
    return m_LocCulturalNames[lang];
}

std::string& Mod::GetLocCulturalName(const std::string& lang, const std::string& key) {
    return m_LocCulturalNames[lang][key];
}

std::string Mod::GetLocCulturalName(const std::string& lang, const std::string& key) const {
    if(!m_LocCulturalNames.contains(lang) || !m_LocCulturalNames.at(lang).contains(key))
        return "";
    return m_LocCulturalNames.at(lang).at(key);
}

void Mod::SetLocCulturalName(const std::string& lang, const std::string& key, std::string name) {
    m_LocCulturalNames[lang][key] = name;
}

void Mod::AddTitle(SharedPtr<Title> title) {
    // Add title to global titles map.
    m_Titles[title->GetName()] = title;

    // Remove any titles with the same name from the lists before adding it.
    m_TitlesByType[title->GetType()].erase(
        std::remove_if(m_TitlesByType[title->GetType()].begin(), m_TitlesByType[title->GetType()].end(), [title](SharedPtr<Title> t) {
            return t->GetName() == title->GetName();
    }), m_TitlesByType[title->GetType()].end());
    m_TitlesByType[title->GetType()].push_back(title);

    // Add barony title to province-barony map.
    if(title->Is(TitleType::BARONY)) {
        const SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
        m_BaroniesByProvinceIds[baronyTitle->GetProvinceId()] = baronyTitle;
    }
}

void Mod::RemoveTitle(SharedPtr<Title> title) {
    // Remove title from global titles map.
    m_Titles.erase(title->GetName());

    // Remove any titles with the same name from the lists.
    m_TitlesByType[title->GetType()].erase(
        std::remove_if(m_TitlesByType[title->GetType()].begin(), m_TitlesByType[title->GetType()].end(), [title](SharedPtr<Title> t) {
            return t->GetName() == title->GetName();
    }), m_TitlesByType[title->GetType()].end());

    // Add barony title to province-barony map.
    if(title->Is(TitleType::BARONY)) {
        const SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
        m_BaroniesByProvinceIds.erase(baronyTitle->GetProvinceId());
    }
}

void Mod::RenameTitle(SharedPtr<Title> title, std::string formerName) {
    for(auto [n, t] : m_Titles) {
        for(auto [date, history] : t->GetHistory()) {
            if(history->Contains("liege") && history->Get("liege")->As<std::string>() == formerName) {
                history->Put("liege", title->GetName());
            }
        }
    }
}

void Mod::HarmonizeTitlesColors(const std::vector<SharedPtr<Title>>& titles, sf::Color rgb, float hue, float saturation) {
    // Generate a list of colors with uniformly spaced saturations around
    // the saturation of the original color while picking a random hue.
    sf::HSVColor defaultColor = rgb;
    std::vector<sf::HSVColor> colors;

    // Define the range of values for hue and saturation.
    float hues[] = { std::max(0.f, defaultColor.h - hue), std::min(360.f, defaultColor.h + hue) };
    float saturations[] = { std::max(0.f, defaultColor.s - saturation), std::min(1.f, defaultColor.s + saturation) };

    // Initialize first saturation value and the speed/step at which to increment it.
    float s = saturations[0];
    float saturationStep = (saturations[1] - saturations[0]) / (float) titles.size();

    while(colors.size() < titles.size()) {
        // TODO: check if the color isn't already used by another title when generating one.

        float h = Math::RandomFloat(hues[0], hues[1]);
        sf::HSVColor color = sf::HSVColor(h, s, defaultColor.v);
        colors.push_back(color);
        s += saturationStep;
    }

    // Shuffle the colors not to have a gradient but random
    // distribution which may make it easier to discern titles.
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(colors.begin(), colors.end(), g);

    // Apply those colors to the titles.
    for(int i = 0; i < titles.size(); i++) {
        titles[i]->SetColor(colors[i]);
    }
}

void Mod::GenerateMissingProvinces() {
    // Loop through the province image and generate provinces for any color
    // that does not already have one.

    int count = 0;
    int nextId = 1;

    uint width = m_ProvinceImage.getSize().x;
    uint height = m_ProvinceImage.getSize().y;
    uint pixels = width * height * 4;
    const sf::Uint8* provincesPixels = m_ProvinceImage.getPixelsPtr();

    uint index = 0;
    sf::Uint32 provinceColor = 0x000000FF;
    sf::Uint32 previousProvinceColor = 0x00000000;

    // Cast to edit directly the bytes of the color and pixels.
    // - colorPtr is used to read the color from the provinces map image.
    char* colorPtr = static_cast<char*>((void*) &provinceColor);

    while(index < pixels) {
        // Copy the four bytes corresponding to RGB from the provinces image pixels
        // to the array for the titles image.
        // The bytes need to be flipped, otherwise provinceColor would
        // be ABGR and we couldn't find the associated title color in the map.
        colorPtr[3] = provincesPixels[index++]; // R
        colorPtr[2] = provincesPixels[index++]; // G
        colorPtr[1] = provincesPixels[index++]; // B
        index++;
        if(previousProvinceColor != provinceColor) {
            if(m_Provinces.count(provinceColor) == 0) {
                // Skip ids that are already taken by another province.
                while(m_ProvincesByIds.count(nextId) != 0)
                    nextId++;

                SharedPtr<Province> province = MakeShared<Province>(nextId, sf::Color(provinceColor), fmt::format("province_{}", nextId));
                m_Provinces[province->GetColorId()] = province;
                m_ProvincesByIds[province->GetId()] = province;
                count++;
                nextId++;
            }
        }
        previousProvinceColor = provinceColor;
    }
    LOG_INFO("Generated {} new provinces based on the province image", count);
}

void Mod::GenerateMissingBaronies() {
    int count = 0;
    for(auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND))
            continue;
        if(province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        if(m_BaroniesByProvinceIds.count(id) > 0)
            continue;
        
        // Make sure to use a title name that isn't already taken.
        std::string baronyName = "b_" + String::ToLowercase(province->GetName());
        int i = 1;
        while(m_Titles.count(baronyName) > 0) {
            baronyName = "b_" + String::ToLowercase(province->GetName()) + std::to_string(i);
            i++;
        }

        // Create a new barony title for that land province.
        SharedPtr<Title> title = MakeTitle(TitleType::BARONY, baronyName, province->GetColor(), false);
        SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
        baronyTitle->SetProvinceId(province->GetId());

        // Add the barony title.
        this->AddTitle(title);
        count++;
    }
    LOG_INFO("Generated {} new barony titles for passable land provinces without any", count);
}

void Mod::Load() {
    if(!this->HasMap())
        return;

    if(!m_HeightmapImage.loadFromFile(m_Dir + "/map_data/heightmap.png")) {
        LOG_ERROR("Failed to load heightmap image at ", m_Dir + "/map_data/heightmap.png");
    }
    if(!m_ProvinceImage.loadFromFile(m_Dir + "/map_data/provinces.png")) {
        FATAL("Failed to load provinces image at ", m_Dir + "/map_data/provinces.png");
    }
    if(!m_RiversImage.loadFromFile(m_Dir + "/map_data/rivers.png")) {
        LOG_ERROR("Failed to load rivers image at ", m_Dir + "/map_data/rivers.png");
    }

    this->LoadHoldingTypes();
    this->LoadTerrainTypes();
    this->LoadProvincesDefinition();
    this->LoadProvinceImage();
    this->LoadDefaultMapFile();
    this->LoadProvincesTerrain();
    this->LoadProvincesHistory();
    this->LoadTitles();
    this->LoadTitlesHistory();
    this->LoadCultures();
    this->LoadReligions();
    this->LoadLocalization();
}

void Mod::LoadHoldingTypes() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/holdings/");

    // Manually define this type as it's not really type.
    m_HoldingTypes = OrderedMap<std::string, HoldingType>();

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                m_HoldingTypes.insert(key, HoldingType(key));
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse file {} : {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} holding types from {} files", m_HoldingTypes.size(), filesPath.size());

    // Insert vanilla holdings if they are not already added.
    // NB: holding type 'none' is added by default.
    auto vanillaTypes = {
        HoldingType("none"),
        HoldingType("tribal_holding"),
        HoldingType("castle_holding"),
        HoldingType("city_holding"),
        HoldingType("church_holding"),
    };

    for(auto type : vanillaTypes) {
        if(!m_HoldingTypes.contains(type.GetName()))
            m_HoldingTypes.insert(type.GetName(), type);
    }
}

void Mod::LoadTerrainTypes() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/terrain_types/");

    m_TerrainTypes = OrderedMap<std::string, TerrainType>();

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                auto [op, value] = pair;
                if(!value->Is(Jomini::Type::OBJECT))
                    continue;
                sf::Color color = value->Get("color")->As<sf::Color>(sf::Color::Black);
                m_TerrainTypes.insert(key, TerrainType(key, color));
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse file {} : {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} terrain types from {} files", m_TerrainTypes.size(), filesPath.size());

    // Insert vanilla terrains that are not already loaded.
    auto vanillaTypes = {
        TerrainType("plains", sf::Color(204, 163, 102)),
        TerrainType("farmlands", sf::Color(255, 50, 50)),
        TerrainType("hills", sf::Color(200, 200, 200)),
        TerrainType("mountains", sf::Color(255, 255, 255)),
        TerrainType("desert", sf::Color(255, 255, 0)),
        TerrainType("desert Mountains", sf::Color(100, 100, 0)),
        TerrainType("oasis", sf::Color(100, 100, 255)),
        TerrainType("jungle", sf::Color(100, 0, 0)),
        TerrainType("forest", sf::Color(255, 0, 0)),
        TerrainType("wetlands", sf::Color(100, 20, 20)),
        TerrainType("steppe", sf::Color(200, 100, 200)),
        TerrainType("floodplains", sf::Color(50, 50, 255)),
        TerrainType("drylands", sf::Color(200, 200, 0)),
        TerrainType("sea", sf::Color(0, 0, 255))
    };

    for(auto type : vanillaTypes) {
        if(!m_TerrainTypes.contains(type.GetName()))
            m_TerrainTypes.insert(type.GetName(), type);
    }
}

void Mod::LoadDefaultMapFile() {
    SharedPtr<Jomini::Object> result = Jomini::ParseFile(m_Dir + "/map_data/default.map");

    // TODO: Coastal provinces??
    
    const std::vector<double>& lakes = result->Get("lakes")->AsArray<double>(std::vector<double>{});
    for(double provinceId : lakes) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAKE, true);
    }
    
    // TODO: Islands provinces??
    // TODO: Land provinces??

    const std::vector<double>& seaZones = result->Get("sea_zones")->AsArray<double>(std::vector<double>{});
    for(double provinceId : seaZones) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
    }

    const std::vector<double>& rivers = result->Get("river_provinces")->AsArray<double>(std::vector<double>{});
    for(double provinceId : rivers) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::RIVER, true);
    }
    
    const std::vector<double>& impassableSeas = result->Get("impassable_seas")->AsArray<double>(std::vector<double>{});
    for(double provinceId : impassableSeas) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
    
    const std::vector<double>& impassableMountains = result->Get("impassable_mountains")->AsArray<double>(std::vector<double>{});
    for(double provinceId : impassableMountains) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
}

void Mod::LoadProvinceImage() {
    const auto& pixels = m_ProvinceImage.getPixelsPtr();
    std::map<uint32_t, bool> colors;

    uint width = m_ProvinceImage.getSize().x;
    uint height = m_ProvinceImage.getSize().y;
    uint totalPixels = width * height;

    const auto& GetIndexPosition = [&](uint index) {
        index = index - 4;
        return sf::Vector2i((index / 4) % width, floor(index / (4*width)));
    };

    // Split the image vertically between all the threads.
    const int threadsCount = 4;
    std::vector<UniquePtr<sf::Thread>> threads;
    const uint threadRange = totalPixels / threadsCount;

    for(uint i = 0; i < threadsCount; i++) {

        threads.push_back(MakeUnique<sf::Thread>([&, i](){
            uint startIndex = i * threadRange*4;
            uint endIndex = (i == threadsCount-1) ? totalPixels*4 : (i+1) * threadRange*4;
            uint index = startIndex;

            sf::Uint32 color = 0x000000FF;
            sf::Uint32 previousColor = 0x00000000;
            bool hasProvince = false;

            // Cast to edit directly the bytes of the color and pixels.
            char* colorPtr = static_cast<char*>((void*) &color);

            while(index < endIndex) {
                // Copy the four bytes corresponding to RGBA from the provinces image pixels
                // to the array for the titles image.
                // The bytes need to be flipped, otherwise provinceColor would
                // be ABGR and we couldn't find the associated title color in the map.
                colorPtr[3] = pixels[index++]; // R
                colorPtr[2] = pixels[index++]; // G
                colorPtr[1] = pixels[index++]; // B
                colorPtr[0] = pixels[index++]; // A

                if((color & 0xFF) != 0xFF) {
                    sf::Vector2i pos = GetIndexPosition(index-4);
                    LOG_ERROR("Transparent pixel in province image at coordinates ({},{})", pos.x, pos.y);
                    continue;
                }

                const auto& province = m_Provinces.find(color);
                hasProvince = (previousColor == color && hasProvince) || (province != m_Provinces.end());
                bool alreadySeen = (previousColor == color || colors.count(color) > 0);

                previousColor = color;

                if(!alreadySeen) {
                    colors[color] = true;
                }

                if(!alreadySeen && !hasProvince) {
                    LOG_ERROR("Color found in image but missing province from definition.csv: ({},{},{},{})", pixels[index-4], pixels[index-3], pixels[index-2], pixels[index-1]);
                    continue;
                }

                if(hasProvince) {
                    if(!alreadySeen) {
                        sf::Vector2i pos = GetIndexPosition(index-4);
                        province->second->SetImagePosition(pos);
                    }
                    province->second->IncrementImagePixelsCount();
                }
            }
        }));
        threads[threads.size()-1]->launch();
    }

    for(auto& thread : threads)
        thread->wait();
}

void Mod::LoadProvincesDefinition() {
    std::string filePath = m_Dir + "/map_data/definition.csv";
    
    // Create an empty definition file if it does not exist.
    if(!std::filesystem::exists(filePath))
        std::ofstream {filePath};
    
    std::vector<std::vector<std::string>> lines = File::ReadCSV(filePath);

    // Skip the first line.
    if(!lines.empty())
        lines.erase(lines.begin());

    int lastId = 0;

    for(const auto& line : lines) {
        int id = std::stoi(line[0]);
        int r = std::stoi(line[1]);
        int g = std::stoi(line[2]);
        int b = std::stoi(line[3]);
        std::string name = line[4];

        SharedPtr<Province> province = MakeShared<Province>(id, sf::Color(r, g, b), name);

        if(m_ProvincesByIds.count(id) > 0)
            LOG_ERROR("Several provinces with same id: {}", id);
        if(m_Provinces.count(province->GetColorId()) > 0)
            LOG_ERROR("Several provinces with same color: {},{}", id, m_Provinces.at(province->GetColorId())->GetId());
        if(id != lastId+1)
            LOG_ERROR("Ids in definitions.csv are not sequential: {} to {}", lastId, id);

        m_Provinces[province->GetColorId()] = province;
        m_ProvincesByIds[province->GetId()] = province;
        lastId = id;
    }
}

void Mod::LoadProvincesTerrain() {
    SharedPtr<Jomini::Object> result = Jomini::ParseFile(m_Dir + "/common/province_terrain/00_province_terrain.txt");

    m_DefaultLandTerrain = result->Get("default_land")->As<std::string>("plains");
    m_DefaultSeaTerrain = result->Get("default_sea")->As<std::string>("sea");
    m_DefaultCoastalSeaTerrain = result->Get("default_coastal_sea")->As<std::string>("sea");

    // Set default terrain for all provinces (especially for those without any in files).
    for(const auto& [colorId, province] : m_Provinces) {
        std::string defaultTerrain = m_DefaultLandTerrain;
        if(province->HasFlag(ProvinceFlags::SEA))
            defaultTerrain = (province->HasFlag(ProvinceFlags::COASTAL) ? m_DefaultCoastalSeaTerrain : m_DefaultSeaTerrain);
        province->SetTerrain(defaultTerrain);
    }

    for(const auto& [key, pair] : result->GetMap()) {
        // TODO: check if key is an integer.
        const auto& [op, value] = pair;
        int provinceId = String::ParseInt(key);
        std::string terrain = "";

        // If the province id has been assigned several terrain type
        // then we only pick the first one, and send a warning to the user.
        if(!value->Is(Jomini::Type::SCALAR)) {
            terrain = value->AsArray<std::string>().front();
            LOG_WARNING("Province assigned several terrain types: {}", provinceId);
        }
        else {
            terrain = value->As<std::string>();
        }

        if(m_ProvincesByIds.count(provinceId) == 0) {
            LOG_WARNING("Terrain type assigned to undefined province: {}", provinceId);
            continue;
        }

        if(!m_TerrainTypes.contains(terrain)) {
            LOG_WARNING("Undefined terrain type '{}' assigned to province {}", terrain, provinceId);
            continue;
        }

        m_ProvincesByIds[provinceId]->SetTerrain(terrain);

        if(!m_ProvincesByIds[provinceId]->HasFlag(ProvinceFlags::SEA))
            m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
    }
}

void Mod::LoadProvincesHistory() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/history/provinces/");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
        
        for(auto& [key, pair] : data->GetMap()) {
            // TODO: check if the key is an integer.
            auto& [op, value] = pair;
            int provinceId = String::ParseInt(key);

            const auto GetStringOrFirstElement = [&](std::string key) {
                if(!value->Get(key)->Is(Jomini::Type::SCALAR))
                    return value->Get(key)->AsArray<std::string>().front();
                return value->Get(key)->As<std::string>();
            };

            if(value->Contains("culture"))
                m_ProvincesByIds[provinceId]->SetCulture(GetStringOrFirstElement("culture"));
            if(value->Contains("religion"))
                m_ProvincesByIds[provinceId]->SetReligion(GetStringOrFirstElement("religion"));
            if(value->Contains("holding")) {
                m_ProvincesByIds[provinceId]->SetHolding(GetStringOrFirstElement("holding"));
            }

            if(!m_HoldingTypes.contains(m_ProvincesByIds[provinceId]->GetHolding())) {
                LOG_WARNING("Undefined holding type '{}' assigned to province {}", m_ProvincesByIds[provinceId]->GetHolding(), provinceId);
                continue;
            }

            // Remove those attributes to avoid duplicates when exporting
            // and to reduce memory usage a bit.
            value->Remove("culture");
            value->Remove("religion");
            value->Remove("holding");

            m_ProvincesByIds[provinceId]->SetOriginalFilePath(filePath);
            m_ProvincesByIds[provinceId]->SetOriginalData(value);
        }
    }
}

void Mod::LoadTitlesHistory() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/history/titles/");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
        
        // 1. Loop over titles key in the file.
        for(auto& [key, pair] : data->GetMap()) {
            auto& [op, value] = pair;

            if(m_Titles.count(key) == 0) {
                LOG_WARNING("Undefined title {} found in {}", key, filePath);
                continue;
            }

            m_Titles[key]->SetOriginalHistoryFilePath(filePath);

            // 2. Loop over dates in the title history.
            for(auto& [date, pair2] : value->GetMap()) {
                // TODO: check if key is a correct date.
                auto& [op2, history] = pair2;
                m_Titles[key]->AddHistory(Jomini::Date(date), history);
            }
        }
    }
}

void Mod::LoadCultures() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/culture/cultures/");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                auto& [op, value] = pair;

                sf::Color color = value->Get("color")->As<sf::Color>(sf::Color::White);
                SharedPtr<Culture> culture = MakeShared<Culture>(key, color);
                m_Cultures[culture->GetName()] = culture;
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse file {} : {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} cultures from {} files", m_Cultures.size(), filesPath.size());
}

void Mod::LoadReligions() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/religion/religions/");

    // Faiths are defined within a religion, so we have to loop over every religion
    // keys, and then over every keys inside "faiths".
    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                auto& [op, value] = pair;
                if(!value->Contains("faiths"))
                    continue;

                for(auto& [faithKey, faithPair] : value->Get("faiths")->GetMap()) {
                    auto& [op2, faithValue] = faithPair;

                    sf::Color color = faithValue->Get("color")->As<sf::Color>(sf::Color::White);
                    SharedPtr<Religion> religion = MakeShared<Religion>(faithKey, color);
                    m_Religions[religion->GetName()] = religion;
                }
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse file {} : {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} faiths from {} files", m_Religions.size(), filesPath.size());
}

void Mod::LoadLocalization() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/localization/english/");
    std::set<std::string> filesPath2 = File::ListFiles(m_Dir + "/localization/replace/english/");
    filesPath.insert(filesPath2.begin(), filesPath2.end());

    uint countNames = 0;
    uint countAdjectives = 0;
    uint countArticles = 0;
    uint countCulturalNamesTotal = 0;

    uint maxCount = 0;
    uint maxCountCulturalNames = 0;

    if(filesPath.empty())
        LOG_WARNING("No localization files have been found in /localization/english/, nor /localization/replace/english/");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".yml"))
            continue;
        if(filePath.find("titles") == std::string::npos && filePath.find("cultural") == std::string::npos)
            continue;

        std::map<std::string, std::string> loc = Yaml::ParseFile(filePath);

        // fmt::println("{}\t{}", filePath, loc.size());

        // Count the number of localization for this file.
        uint count = countNames + countAdjectives;
        uint countCulturalNames = 0;

        for(auto [key, value] : loc) {
            // TODO: handle cultural names.

            // Parse a cultural name.
            if(key.starts_with("cn_")) {
                this->SetLocCulturalName("english", key, value);
                countCulturalNames++;
                continue;
            }

            // Skip localization that are not related to titles.
            if(!key.starts_with("b_")
                && !key.starts_with("c_")
                && !key.starts_with("d_")
                && !key.starts_with("k_")
                && !key.starts_with("e_"))
                continue;
            
            std::string titleId = key;
            enum LocType { NAME, ADJECTIVE, ARTICLE };
            LocType locType = NAME;

            // Titles have names, adjectives, articles and cultural names.
            if(key.ends_with("_adj")) {
                titleId = key.substr(0, key.size()-4);
                locType = ADJECTIVE;
            }
            else if(key.ends_with("_article")) {
                titleId = key.substr(0, key.size()-8);
                locType = ARTICLE;
            }

            auto it = m_Titles.find(titleId);
            if(it == m_Titles.end())
                continue;

            switch(locType) {
                case NAME:
                    it->second->SetLocName("english", value);
                    countNames++;
                    break;
                case ADJECTIVE:
                    it->second->SetLocAdjective("english", value);
                    countAdjectives++;
                    break;
                case ARTICLE:
                    it->second->SetLocArticle("english", value);
                    countArticles++;
                    break;
            }
        }

        count = countNames + countAdjectives + countArticles - count;
        countCulturalNamesTotal += countCulturalNames;

        // Use the most used localization file for titles
        // as the main file for export.
        if(count > maxCount) {
            maxCount = count;
            m_TitlesLocalizationFilePath = filePath;
        }
        
        // Same for cultural names.
        if(countCulturalNames > maxCountCulturalNames) {
            maxCountCulturalNames = countCulturalNames;
            m_CulturalNamesLocalizationFilePath = filePath;
        }
    }

    LOG_INFO("Default titles localization file will be {}", m_TitlesLocalizationFilePath);
    LOG_INFO("Loaded {} titles names, {} adjectives and {} articles from {} localization files", countNames, countAdjectives, countArticles, filesPath.size());
    LOG_INFO("Default cultural names localization file will be {}", m_CulturalNamesLocalizationFilePath);
    LOG_INFO("Loaded {} cultural names from {} localization files", countCulturalNamesTotal, filesPath.size());
}

void Mod::LoadTitles() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/common/landed_titles/");

    for(int i = 0; i < (int) TitleType::COUNT; i++)
        m_TitlesByType[(TitleType) i] = std::vector<SharedPtr<Title>>();

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        // fmt::println("loading titles from {}", filePath);
        SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
        std::vector<SharedPtr<Title>> titles = ParseTitles(filePath, data);
    }

    LOG_INFO("Loaded {} titles from {} files", m_Titles.size(), filesPath.size());
    
    for(int i = 0; i < (int) TitleType::COUNT; i++)
        LOG_INFO("Loaded {} {} titles", m_TitlesByType[(TitleType) i].size(), TitleTypeLabels[i]);
}

std::vector<SharedPtr<Title>> Mod::ParseTitles(const std::string& filePath, SharedPtr<Jomini::Object> data) {
    std::vector<SharedPtr<Title>> titles;

    for(auto& [key, pair] : data->GetMap()) {
        auto& [op, value] = pair;

        // Need to check if the key is a title (starts with e_, k_, d_, c_ or b_)
        // because it could be attributes such as color, capital, can_create...

        try {
            // This function throws an exception if key does not
            // correspond to a title type.
            TitleType type = GetTitleTypeByName(key);

            sf::Color color = value->Get("color")->As<sf::Color>(sf::Color::Black);
            bool landless = value->Get("landless")->As<bool>(false);

            // Need to use a custom function to create a SharedPtr<Title>
            // to get the right derived class such as BaronyTitle, CountyTitle...
            SharedPtr<Title> title = MakeTitle(type, key, color, landless);

            if(!value->Contains("color"))
                LOG_WARNING("Title missing color in definition: {}", key);

            if(type == TitleType::BARONY) {
                SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
                baronyTitle->SetProvinceId(value->Get("province")->As<int>(0));
                
                if(!value->Contains("province"))
                    LOG_ERROR("Barony title missing province id in definition: {}", key);
                if(m_ProvincesByIds.count(baronyTitle->GetProvinceId()) == 0)
                    LOG_ERROR("Barony title with undefined province id in definition: {},{}", key, baronyTitle->GetProvinceId());
                if(m_BaroniesByProvinceIds.count(baronyTitle->GetProvinceId()) > 0) {
                    LOG_ERROR("Province {} has several assigned barony titles (e.g {})", baronyTitle->GetProvinceId(), key);
                    continue;
                }

                m_BaroniesByProvinceIds[baronyTitle->GetProvinceId()] = baronyTitle;
                value->Remove("province");
            }
            else {
                SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);
                std::vector<SharedPtr<Title>> dejureTitles = ParseTitles(filePath, value);

                if(landless && !dejureTitles.empty())
                    LOG_WARNING("Landless title has dejure vassals in definition: {}", key);
                else if(!landless && dejureTitles.empty())
                    LOG_WARNING("Title does not have any dejure vassals in definition: {}", key);

                for(const auto& dejureTitle : dejureTitles) {
                    highTitle->AddDejureTitle(dejureTitle);

                    // Remove that title from the node tree to keep only
                    // the title own attributes for its original data.
                    value->Remove(dejureTitle->GetName());
                }

                if(type != TitleType::COUNTY) {
                    if(value->Contains("capital")) {
                        std::string capitalName = value->Get("capital")->As<std::string>();
                        if(m_Titles.count(capitalName) > 0 && IsInstance<CountyTitle>(m_Titles[capitalName])) {
                            highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(m_Titles[capitalName]));
                        }
                        value->Remove("capital");
                    }
                    else {
                        LOG_ERROR("Title missing county capital in definition: {}", key);
                    }
                }

                if(value->Contains("cultural_names")) {
                    SharedPtr<Jomini::Object> culturalNames = value->Get("cultural_names");
                    if(culturalNames->Is(Jomini::Type::OBJECT)) {
                        // TODO: Rewrite this whole chunk of code correctly.
                        for(auto [culture, p] : culturalNames->GetMap()) {
                            auto [op, o] = p;
                            std::string name = "";
                            if(o->Is(Jomini::Type::ARRAY)) {
                                name = o->AsArray<std::string>().front();
                            }
                            else if(o->Is(Jomini::Type::SCALAR)) {
                                name = o->As<std::string>();
                            }
                            if(!name.empty()) {
                                title->AddCulturalName(culture, name);
                            }
                        }
                    }
                }
            }

            value->Remove("color");
            value->Remove("landless");
            value->Remove("cultural_names");
            title->SetOriginalFilePath(filePath);
            title->SetOriginalData(value);

            m_Titles[key] = title;
            m_TitlesByType[type].push_back(title);
            titles.push_back(title);
        }
        catch(const std::runtime_error& e) {
            // fmt::println("error: {}", e.what());
        }
    }

    return titles;
}

void Mod::Export() {
    this->ExportDefaultMapFile();
    this->ExportProvincesDefinition();
    this->ExportProvincesTerrain();
    this->ExportProvincesHistory();

    this->ExportTitles();
    this->ExportTitlesHistory();

    this->ExportLocalization();
}

void Mod::ExportDefaultMapFile() {
    // Read the file and keep all values except for the terrain flags
    // such as: sea_zones, impassable_seas, lakes, impassable_mountains, river_provinces
    SharedPtr<Jomini::Object> data = Jomini::ParseFile(m_Dir + "/map_data/default.map");

    SharedPtr<Jomini::Object> zonesData = MakeShared<Jomini::Object>(Jomini::ObjectMap{});

    zonesData->Put("sea_zones", std::vector<double>());
    zonesData->Put("impassable_seas", std::vector<double>());
    zonesData->Put("river_provinces", std::vector<double>());
    zonesData->Put("lakes", std::vector<double>());
    zonesData->Put("impassable_mountains", std::vector<double>());

    for (auto [key, object] : zonesData->GetMap())
        object.second->SetFlag(Jomini::Flags::RANGE, true);

    // Remove those keys since they are printed seperately.
    data->Remove("sea_zones");
    data->Remove("impassable_seas");
    data->Remove("river_provinces");
    data->Remove("lakes");
    data->Remove("impassable_mountains");

    for(const auto& [id, province] : m_ProvincesByIds) {
        if(province->HasFlag(ProvinceFlags::SEA)) {
            zonesData->Get("sea_zones")->Push<int>(id);
            if(province->HasFlag(ProvinceFlags::IMPASSABLE))
                zonesData->Get("impassable_seas")->Push<int>(id);
        }
        else if(province->HasFlag(ProvinceFlags::IMPASSABLE)) {
            zonesData->Get("impassable_mountains")->Push<int>(id);
        }

        if(province->HasFlag(ProvinceFlags::LAKE))
            zonesData->Get("lakes")->Push<int>(id);
            
        if(province->HasFlag(ProvinceFlags::RIVER))
            zonesData->Get("river_provinces")->Push<int>(id);
    }

    // TODO: add error log if file can't be opened.

    std::ofstream file(m_Dir + "/map_data/default.map", std::ios::out);

    #define PRINT_DATA(key, def) fmt::println(file, "{} = {}", key, data->Get(key)->As<std::string>(def)); data->Remove(key)
    #define FORMAT_LIST(key) zonesData->Get(key)->SerializeArrayRange(key, Jomini::Operator::EQUAL, 0)

    PRINT_DATA("definitions", "\"definition.csv\"");
    PRINT_DATA("provinces", "\"provinces.png\"");
    PRINT_DATA("rivers", "\"rivers.png\"");
    PRINT_DATA("topology", "\"heightmap.heightmap\"");
    PRINT_DATA("continent", "\"continent.txt\"");
    PRINT_DATA("adjacencies", "\"adjacencies.csv\"");
    PRINT_DATA("island_region", "\"island_region.txt\"");
    PRINT_DATA("geographical_region", "\"geographical_region.txt\"");
    PRINT_DATA("seasons", "\"seasons.txt\"");

    fmt::println(
        file, 
        "\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}",
        data->Serialize(),
        FORMAT_LIST("sea_zones"),
        FORMAT_LIST("impassable_seas"),
        FORMAT_LIST("river_provinces"),
        FORMAT_LIST("lakes"),
        FORMAT_LIST("impassable_mountains")
    );

    file.close();
}

void Mod::ExportProvincesDefinition() {
    std::ofstream file(m_Dir + "/map_data/definition.csv", std::ios::out);

    // The format of definition.csv is as following:
    // [ID];[RED];[GREEN];[BLUE];[Barony Name];x;
    file << "0;0;0;0;x;x;\n";

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
            "{};{};{};{};{};x;",
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
    std::filesystem::create_directories(m_Dir + "/common/province_terrain/");
    std::ofstream file(m_Dir + "/common/province_terrain/00_province_terrain.txt", std::ios::out);

    fmt::println(file, "default_land={}", m_DefaultLandTerrain);
    fmt::println(file, "default_sea={}", m_DefaultSeaTerrain);
    fmt::println(file, "default_coastal_sea={}", m_DefaultCoastalSeaTerrain);
    fmt::println(file, "\n");

    for(auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND) || province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        fmt::println(file,
            "{}={}",
            province->GetId(),
            province->GetTerrain()
        );
    }

    file.close();
}

void Mod::ExportProvincesHistory() {
    // Provinces history are written in a file named after the kingdom tier title
    // such as: history/provinces/00_k_dorne_prov.txt

    // TODO: it may be a better approach to loop through
    // the list of kingdoms and write their dejure provinces
    // instead of looping the provinces and having the file/kingdom
    // in disorder.

    std::string dir = m_Dir + "/history/provinces";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    for(const auto& kingdomTitle : m_TitlesByType[TitleType::KINGDOM]) {
        SharedPtr<HighTitle> kingdomHighTitle = CastSharedPtr<HighTitle>(kingdomTitle);

        // Provinces history are grouped by kingdoms.
        std::string filePath = fmt::format("{}/00_{}_prov.txt", dir, kingdomTitle->GetName());
        std::ofstream file = std::ofstream(filePath, std::ios::out);

        for(const auto& duchyTitle : kingdomHighTitle->GetDejureTitles()) {
            SharedPtr<HighTitle> duchyHighTitle = CastSharedPtr<HighTitle>(duchyTitle);

            fmt::println(file, "##### {} ############################\n", duchyTitle->GetName());

            for(const auto& countyTitle : duchyHighTitle->GetDejureTitles()) {
                SharedPtr<HighTitle> countyHighTitle = CastSharedPtr<HighTitle>(countyTitle);
                fmt::println(file, "### {}", countyTitle->GetName());

                for(const auto& baronyTitle : countyHighTitle->GetDejureTitles()) {
                    SharedPtr<BaronyTitle> baronyBaronyTitle = CastSharedPtr<BaronyTitle>(baronyTitle);
                    SharedPtr<Province> province = m_ProvincesByIds[baronyBaronyTitle->GetProvinceId()];
        
                    if(!province->HasFlag(ProvinceFlags::LAND) || province->HasFlag(ProvinceFlags::IMPASSABLE))
                        continue;
                    
                    SharedPtr<Jomini::Object> data = province->GetOriginalData();
                    if(!province->GetCulture().empty()) data->Put("culture", province->GetCulture());
                    if(!province->GetReligion().empty()) data->Put("religion", province->GetReligion());
                    data->Put("holding", province->GetHolding().empty() ? "none" : province->GetHolding());

                    SharedPtr<Jomini::Object> object = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
                    object->Put(std::to_string(province->GetId()), data);

                    fmt::println(file, "# {}", province->GetName());
                    fmt::println(file, "{}", object->Serialize());
                }

                fmt::println(file, "\n");
            }
        }
    }

    // Check if there are any provinces that couldn't be saved.
    for(const auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND) || province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        SharedPtr<Title> kingdomTitle = this->GetProvinceLiegeTitle(province, TitleType::KINGDOM);
        if(kingdomTitle == nullptr) {
            LOG_ERROR("Province cannot be saved because missing dejure kingdom tier liege: {}", id);
            continue;
        }
    }
}

void Mod::ExportTitles() {
    std::string dir = m_Dir + "/common/landed_titles";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::map<std::string, std::ofstream> files;

    for(const auto& [name, title] : m_Titles) {
        if(title->GetLiegeTitle() != nullptr)
            continue;
        std::string filePath = title->GetOriginalFilePath();
        if(filePath.empty())
            filePath = dir + "/01_landed_titles.txt";
        if(files.count(filePath) == 0) {
            files[filePath] = std::ofstream(filePath, std::ios::out);
            File::EncodeToUTF8BOM(files[filePath]);
        }
        std::ofstream& file = files[filePath];
        fmt::println(file, "{} = {{", title->GetName());
        this->ExportTitle(title, file, 1);
        fmt::println(file, "}}\n");
    }

    for(auto& [key, file] : files)
        file.close();
}

void Mod::ExportTitlesHistory() {
    std::string dir = m_Dir + "/history/titles";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::map<std::string, std::ofstream> files;

    // 1. Use original history file if the title has one.
    // 2. Use empire_titles.txt for empire tier titles.
    // 3. Use kingdom tier liege for other titles (i.e k_the_wall).
    // 4. Use "landless_titles.txt" for landless titles.
    // 5. Use "special_titles.txt" for everything else.
    const std::function<std::string(SharedPtr<Title>)> GetTitleFileName = [&](SharedPtr<Title> liege) {
        if(liege->Is(TitleType::EMPIRE))
            return std::string("empire_titles");
        if(liege->Is(TitleType::KINGDOM))
            return liege->GetName();
        if(liege->IsLandless()) 
            return std::string("landless_titles");
        if(liege->GetLiegeTitle() == nullptr)
            return std::string("special_titles");
        return GetTitleFileName(liege->GetLiegeTitle());
    };

    for(const auto& [name, title] : m_Titles) {
        if(title->GetHistory().size() == 0)
            continue;
        std::string filePath = title->GetOriginalHistoryFilePath();
        if(filePath.empty())
            filePath = dir + "/" + GetTitleFileName(title) + ".txt";
        if(files.count(filePath) == 0)
            files[filePath] = std::ofstream(filePath, std::ios::out);
        std::ofstream& file = files[filePath];
        
        SharedPtr<Jomini::Object> history = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
        for(auto const& [date, data] : title->GetHistory()) {
            history->Put((std::string) date, data);
        }

        // Because of indentation and curly brackets, we
        // need to put the history inside an object and
        // print that object.
        SharedPtr<Jomini::Object> object = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
        object->Put(title->GetName(), history);

        fmt::println(file, "{}", object->Serialize());
    }

    for(auto& [key, file] : files)
        file.close();
}

void Mod::ExportTitle(const SharedPtr<Title>& title, std::ofstream& file, int depth) {
    std::string indent = std::string(depth, '\t');
    SharedPtr<Jomini::Object> data = (title->GetOriginalData() == nullptr) ? MakeShared<Jomini::Object>(Jomini::ObjectMap{}) : title->GetOriginalData();

    #define EXPORT_PROPERTIES(key, value) fmt::println(file, "{}{} = {}", indent, key, value)

    const auto ExportCulturalNames = [&]() {
        if(!title->GetCulturalNames().empty()) {
            fmt::println(file, "\n{}cultural_names = {{", indent);
            for(auto [culture, name] : title->GetCulturalNames()) {
                fmt::println(file, "{}\t{} = {}", indent, culture, name);
            }
            fmt::println(file, "{}}}", indent);
        }
    };

    EXPORT_PROPERTIES("color", fmt::format("{{ {} {} {} }}", title->GetColor().r, title->GetColor().g, title->GetColor().b));
        
    if(title->Is(TitleType::BARONY)) {
        SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
        EXPORT_PROPERTIES("province", baronyTitle->GetProvinceId());
        // TODO: warning if there is no province with this id.

        ExportCulturalNames();

        if(!data->GetMap().empty())
            fmt::println(file, "{}", data->Serialize(depth, true));
    }
    else {
        SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);

        // Raise an error if the main barony of a county does
        // not have any holding type.
        if(title->Is(TitleType::COUNTY) && !highTitle->GetDejureTitles().empty()) {
            SharedPtr<BaronyTitle> vassalTitle = CastSharedPtr<BaronyTitle>(highTitle->GetDejureTitles().front());
            if(m_ProvincesByIds.count(vassalTitle->GetProvinceId()) > 0) {
                SharedPtr<Province> province = m_ProvincesByIds[vassalTitle->GetProvinceId()];
                if(province->GetHolding() == "none") {
                    LOG_ERROR("Capital barony {} of county {} does not have any holding", vassalTitle->GetName(), title->GetName());
                }
            }
        }

        if(!title->Is(TitleType::COUNTY) && highTitle->GetCapitalTitle() != nullptr)
            EXPORT_PROPERTIES("capital", highTitle->GetCapitalTitle()->GetName());
            
        if(title->IsLandless())
            EXPORT_PROPERTIES("landless", "yes");

        ExportCulturalNames();

        if(!data->GetMap().empty())
            fmt::println(file, "\n{}", data->Serialize(depth, true));

        for(const auto& dejureTitle : highTitle->GetDejureTitles()) {
            fmt::println(file, "\n{}{} = {{", indent, dejureTitle->GetName());
            this->ExportTitle(dejureTitle, file, depth+1);
            fmt::println(file, "{}}}", indent);
        }
    }
}

void Mod::ExportLocalization() {
    // Remove all localization related to titles from the files
    // in order to centralize the loc and avoid duplicates.
    this->DeleteTitlesLocalization();

    this->ExportTitlesLocalization();
    this->ExportCulturalNamesLocalization();
}

void Mod::ExportTitlesLocalization() {
    std::filesystem::create_directories(std::filesystem::path(m_TitlesLocalizationFilePath).parent_path());
    std::ofstream file(m_TitlesLocalizationFilePath);

    fmt::println(file, "l_english:");
    for(auto [type, titles] : m_TitlesByType) {
        for(auto title : titles) {
            std::string name = title->GetLocName("english");
            std::string adjective = title->GetLocAdjective("english");
            std::string article = title->GetLocArticle("english");

            if(!name.empty()) fmt::println(file, " {}: \"{}\"", title->GetName(), name);
            if(!adjective.empty()) fmt::println(file, " {}_adj: \"{}\"", title->GetName(), adjective);
            if(!article.empty()) fmt::println(file, " {}_article: \"{}\"", title->GetName(), article);
        }
    }

    file.close();
}

void Mod::ExportCulturalNamesLocalization() {
    std::filesystem::create_directories(std::filesystem::path(m_CulturalNamesLocalizationFilePath).parent_path());
    std::ofstream file(m_CulturalNamesLocalizationFilePath);

    fmt::println(file, "l_english:");
    for(auto [key, name] : m_LocCulturalNames["english"]) {
        if(!name.empty()) fmt::println(file, " {}: \"{}\"", key, name);
    }

    file.close();
}

void Mod::DeleteTitlesLocalization() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/localization/english/");
    std::set<std::string> filesPath2 = File::ListFiles(m_Dir + "/localization/replace/english/");
    filesPath.insert(filesPath2.begin(), filesPath2.end());

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".yml"))
            continue;
        if(filePath.find("titles") == std::string::npos && filePath.find("cultural") == std::string::npos)
            continue;
        if(filePath == m_TitlesLocalizationFilePath)
            continue;

        std::ifstream file(filePath);
        std::ofstream tmpFile(filePath + ".tmp");
        std::map<std::string, std::string> loc = Yaml::ParseFile(filePath);

        // Rewrite the localization line by line while
        // omitting titles and adjectives localization.
        std::string line;
        while(std::getline(file, line)) {
            // Remove all blanks from the string until reading ':'
            // in order to extract the key.
            std::string key = "";
            for(int i = 0; i < line.size(); i++) {
                if(line[i] == ' ' || line[i] == '\t' || line[i] == '\r')
                    continue;
                if(line[i] == ':' || line[i] == '#')
                    break;
                key.push_back(line[i]);
            }

            // If the line is empty or contains a comment, then we keep it.
            if(key.empty()) {
                fmt::println(tmpFile, "{}", line);
                continue;
            }

            // Ignore lines with a cultural name.
            if(key.starts_with("cn_")) {
                continue;
            }

            // If the line is not related to titles, then we keep it.
            if(!key.starts_with("b_")
            && !key.starts_with("c_")
            && !key.starts_with("d_")
            && !key.starts_with("k_")
            && !key.starts_with("e_")) {
                fmt::println(tmpFile, "{}", line);
                continue;
            }

            // Determine the title name/id from the key.
            std::string titleId = key;
            if(key.ends_with("_adj")) {
                titleId = key.substr(0, key.size()-4);
            }
            else if(key.ends_with("_article")) {
                titleId = key.substr(0, key.size()-8);
            }
            
            // If there isn't any titles by that name, then we keep it.
            auto it = m_Titles.find(titleId);
            if(it == m_Titles.end()) {
                fmt::println(tmpFile, "{}", line);
                continue;
            }
        }

        file.close();
        tmpFile.close();
        std::remove(filePath.c_str());
        std::rename((filePath + ".tmp").c_str(), filePath.c_str());
    }
}