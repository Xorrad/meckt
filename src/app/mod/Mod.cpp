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

sf::Image& Mod::GetHeightmapImage() {
    return m_HeightmapImage;
}

sf::Image& Mod::GetProvinceImage() {
    return m_ProvinceImage;
}

sf::Image& Mod::GetRiversImage() {
    return m_RiversImage;
}

sf::Image Mod::GetTitleImage(TitleType type) {
    // Used for benchmarking.
    sf::Clock clock;
    
    // Map provinces color to the titles color and then when 
    // updating the image, swap the color of each pixel.
    std::unordered_map<sf::Uint32, sf::Uint32> colors;

    for(const auto& [provinceColorId, province] : m_Provinces) {

        if(m_Titles.count(province->GetName()) == 0)
            continue;

        const SharedPtr<Title>& liege = this->GetProvinceFocusedTitle(province, type);
        // const SharedPtr<Title>& liege = GetProvinceLiegeTitle(province, type);

        if(liege == nullptr)
            continue;

        colors[province->GetColor().toInteger()] = liege->GetColor().toInteger();
    }
    // fmt::println("mapping colors: {}", String::DurationFormat(clock.restart()));
    // fmt::println("mapped: {} colors", colors.size());

    uint width = m_ProvinceImage.getSize().x;
    uint height = m_ProvinceImage.getSize().y;
    uint pixels = width * height * 4;

    // Use vectors to avoid using SFML getters and setters for pixels.
    const sf::Uint8* provincesPixels = m_ProvinceImage.getPixelsPtr();
    std::vector<sf::Uint8> titlesPixels = std::vector<sf::Uint8>();
    titlesPixels.reserve(width * height * 4);
    // fmt::println("image=[{}, {}]\tbytes={}", width, height, titlesPixels.capacity());
    // fmt::println("initializing pixels array: {}", String::DurationFormat(clock.restart()));

    const int threadsCount = 6;
    std::vector<UniquePtr<sf::Thread>> threads;

    // Split the image vertically between all the threads.
    const uint threadRange = pixels / threadsCount;

    for(uint i = 0; i < threadsCount; i++) {

        threads.push_back(MakeUnique<sf::Thread>([&, i](){
            uint startIndex = i * threadRange;
            uint endIndex = (i == threadsCount-1) ? pixels : (i+1) * threadRange;
            uint index = startIndex;
            // // fmt::println("i={}\t[{},{}]", i, startIndex, endIndex);

            sf::Uint32 provinceColor = 0x000000FF;
            sf::Uint32 previousProvinceColor = 0x00000000;

            // Cast to edit directly the bytes of the color and pixels.
            // - colorPtr is used to read the color from the provinces map image.
            // - titlePtr is used to access the color composites (RGBA)
            //   of the pixels from the array we are painting.
            // - replacePtr is the liege color to paint on the new texture.
            char* colorPtr = static_cast<char*>((void*) &provinceColor);
            char* titlePtr = static_cast<char*>((void*) &titlesPixels[startIndex]);
            char* replacePtr = NULL;
            const char alpha = (const char) 0xFF;

            while(index < endIndex) {
                // Copy the four bytes corresponding to RGBA from the provinces image pixels
                // to the array for the titles image.
                // The bytes need to be flipped, otherwise provinceColor would
                // be ABGR and we couldn't find the associated title color in the map.
                colorPtr[3] = provincesPixels[index++]; // R
                colorPtr[2] = provincesPixels[index++]; // G
                colorPtr[1] = provincesPixels[index++]; // B
                index++;

                // Search for the corresponding title color in the map only
                // if it isn't the same color has the previous one.
                if(previousProvinceColor != provinceColor) {
                    const auto& it = colors.find(provinceColor);
                    replacePtr = (it == colors.end()) ? colorPtr : static_cast<char*>((void*) &it->second);
                }
                
                // Replace the bits of the pixel in the titles image
                // where each byte correspond to a color composite (RGBA).
                *titlePtr++ = replacePtr[3]; // R
                *titlePtr++ = replacePtr[2]; // G
                *titlePtr++ = replacePtr[1]; // B
                *titlePtr++ = alpha;

                previousProvinceColor = provinceColor;
            }    
        
        }));
        threads[threads.size()-1]->launch();
    }

    for(auto& thread : threads)
        thread->wait();
    // fmt::println("filling pixels: {}", String::DurationFormat(clock.restart()));

    sf::Image image;
    image.create(width, height, titlesPixels.data());
    // texture.update(titlesPixels.data());
    // fmt::println("initializing image: {}", String::DurationFormat(clock.restart()));

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
    if(!m_Titles.count(province->GetName()))
        return nullptr;

    const SharedPtr<Title>& barony = m_Titles[province->GetName()];

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
    if(!m_Titles.count(province->GetName()))
        return nullptr;

    SharedPtr<Title> barony = m_Titles[province->GetName()];
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
    std::random_shuffle(colors.begin(), colors.end());

    // Apply those colors to the titles.
    for(int i = 0; i < titles.size(); i++) {
        titles[i]->SetColor(colors[i]);
    }
}

void Mod::GenerateMissingProvinces() {
    // Loop through the province image and generate provinces for any color
    // that does not already have one.

    int nextId = this->GetMaxProvinceId() + 1;

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
        // Copy the four bytes corresponding to RGBA from the provinces image pixels
        // to the array for the titles image.
        // The bytes need to be flipped, otherwise provinceColor would
        // be ABGR and we couldn't find the associated title color in the map.
        colorPtr[3] = provincesPixels[index++]; // R
        colorPtr[2] = provincesPixels[index++]; // G
        colorPtr[1] = provincesPixels[index++]; // B
        index++;
        if(previousProvinceColor != provinceColor) {
            if(m_Provinces.count(provinceColor) == 0) {
                SharedPtr<Province> province = MakeShared<Province>(nextId, sf::Color(provinceColor), fmt::format("province_{}", nextId));
                m_Provinces[province->GetColorId()] = province;
                m_ProvincesByIds[province->GetId()] = province;
                nextId++;
            }
        }
        previousProvinceColor = provinceColor;
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
    this->LoadProvincesHistory();
    this->LoadTitles();
    this->LoadTitlesHistory();
}

void Mod::LoadDefaultMapFile() {
    Parser::Node result = Parser::Parse(m_Dir + "/map_data/default.map");

    // TODO: Coastal provinces??
    
    const std::vector<double>& lakes = result.Get("lakes", std::vector<double>{});
    for(double provinceId : lakes) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAKE, true);
    }
    
    // TODO: Islands provinces??
    // TODO: Land provinces??

    const std::vector<double>& seaZones = result.Get("sea_zones", std::vector<double>{});
    for(double provinceId : seaZones) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
    }

    const std::vector<double>& rivers = result.Get("river_provinces", std::vector<double>{});
    for(double provinceId : rivers) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::RIVER, true);
    }
    
    const std::vector<double>& impassableSeas = result.Get("impassable_seas", std::vector<double>{});
    for(double provinceId : impassableSeas) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
    
    const std::vector<double>& impassableMountains = result.Get("impassable_mountains", std::vector<double>{});
    for(double provinceId : impassableMountains) {
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }
}

void Mod::LoadProvincesDefinition() {
    std::string filePath = m_Dir + "/map_data/definition.csv";
    
    // Create an empty definition file if it does not exist.
    if(std::filesystem::exists(filePath))
        std::ofstream {filePath};
    
    std::vector<std::vector<std::string>> lines = File::ReadCSV(filePath);
    
    // Skip the first line.
    if(!lines.empty())
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

    for(const auto& [key, pair] : result.GetEntries()) {
        if(!std::holds_alternative<double>(key))
            continue;
        const auto& [op, value] = pair;
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

void Mod::LoadProvincesHistory() {
    std::set<std::string> filesPath = File::ListFiles(m_Dir + "/history/provinces/");

    for(const auto& filePath : filesPath) {
        Parser::Node data = Parser::Parse(filePath);
        
        for(auto& [key, pair] : data.GetEntries()) {
            if(!std::holds_alternative<double>(key))
                continue;
            auto& [op, value] = pair;
            int provinceId = std::get<double>(key);

            if(value.ContainsKey("culture"))
                m_ProvincesByIds[provinceId]->SetCulture(value.Get("culture"));
            if(value.ContainsKey("religion"))
                m_ProvincesByIds[provinceId]->SetReligion(value.Get("religion"));
            if(value.ContainsKey("holding"))
                m_ProvincesByIds[provinceId]->SetHolding(ProvinceHoldingFromString(value.Get("holding")));

            // Remove those attributes to avoid duplicates when exporting
            // and to reduce memory usage a bit.
            value.Remove("culture");
            value.Remove("religion");
            value.Remove("holding");

            m_ProvincesByIds[provinceId]->SetOriginalFilePath(filePath);
            m_ProvincesByIds[provinceId]->SetOriginalData(value);
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
        std::vector<SharedPtr<Title>> titles = ParseTitles(filePath, data);
    }

    INFO("loaded {} titles from {} files", m_Titles.size(), filesPath.size());
    
    for(int i = 0; i < (int) TitleType::COUNT; i++)
        INFO("loaded {} {} titles", m_TitlesByType[(TitleType) i].size(), TitleTypeLabels[i]);
}

std::vector<SharedPtr<Title>> Mod::ParseTitles(const std::string& filePath, Parser::Node& data) {
    std::vector<SharedPtr<Title>> titles;

    for(auto& [k, pair] : data.GetEntries()) {
        if(!std::holds_alternative<std::string>(k))
            continue;
        std::string key = std::get<std::string>(k);
        auto& [op, value] = pair;

        // Need to check if the key is a title (starts with e_, k_, d_, c_ or b_)
        // because it could be attributes such as color, capital, can_create...

        try {
            // This function throws an exception if key does not
            // correspond to a title type.
            TitleType type = GetTitleTypeByName(key);

            sf::Color color = value.Get("color", sf::Color::Black);
            bool landless = value.Get("landless", false);

            // Need to use a custom function to create a SharedPtr<Title>
            // to get the right derived class such as BaronyTitle, CountyTitle...
            SharedPtr<Title> title = MakeTitle(type, key, color, landless);

            // TODO: add error log if title color is not specified.

            if(type == TitleType::BARONY) {
                SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
                baronyTitle->SetProvinceId(value.Get("province", 0));
                
                // TODO: add error log if province id is not specified.

                value.Remove("province");
            }
            else {
                SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);
                std::vector<SharedPtr<Title>> dejureTitles = ParseTitles(filePath, value);

                for(const auto& dejureTitle : dejureTitles) {
                    highTitle->AddDejureTitle(dejureTitle);

                    // Remove that title from the node tree to keep only
                    // the title own attributes for its original data.
                    value.Remove(dejureTitle->GetName());
                }

                if(type != TitleType::COUNTY && value.ContainsKey("capital")) {
                    std::string capitalName = value.Get("capital");
                    if(m_Titles.count(capitalName) > 0 && IsInstance<CountyTitle>(m_Titles[capitalName])) {
                        highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(m_Titles[capitalName]));
                    }
                    value.Remove("capital");
                }
            }

            value.Remove("color");
            value.Remove("landless");
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
}

void Mod::ExportDefaultMapFile() {
    // Read the file and keep all values except for the terrain flags
    // such as: sea_zones, impassable_seas, lakes, impassable_mountains, river_provinces
    Parser::Node data = Parser::Parse(m_Dir + "/map_data/default.map");

    data.Put("sea_zones", Parser::Node(std::vector<double>()));
    data.Put("impassable_seas", Parser::Node(std::vector<double>()));
    data.Put("lakes", Parser::Node(std::vector<double>()));
    data.Put("impassable_mountains", Parser::Node(std::vector<double>()));
    data.Put("river_provinces", Parser::Node(std::vector<double>()));

    // TODO: improve function to use range instead of giant list of ids.

    for(const auto& [id, province] : m_ProvincesByIds) {
        if(province->HasFlag(ProvinceFlags::SEA)) {
            data.Get("sea_zones").Push((double) id);
            if(province->HasFlag(ProvinceFlags::IMPASSABLE))
                data.Get("impassable_seas").Push((double) id);
        }
        else if(province->HasFlag(ProvinceFlags::IMPASSABLE)) {
                data.Get("impassable_mountains").Push((double) id);
        }

        if(province->HasFlag(ProvinceFlags::LAKE))
            data.Get("lakes").Push((double) id);
            
        if(province->HasFlag(ProvinceFlags::RIVER))
            data.Get("river_provinces").Push((double) id);
    }

    // TODO: add error log if file can't be opened.

    std::ofstream file(m_Dir + "/map_data/default.map", std::ios::out);
    fmt::println(file, "{}", data);
    file.close();
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

    auto TerrainToString = [=](TerrainType terrain) {
        return String::ToLowercase(TerrainTypeLabels[(int) terrain]);
    };

    fmt::println(file, "default_land={}", TerrainToString(m_DefaultLandTerrain));
    fmt::println(file, "default_sea={}", TerrainToString(m_DefaultSeaTerrain));
    fmt::println(file, "default_coastal_sea={}", TerrainToString(m_DefaultCoastalSeaTerrain));

    for(auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND) || province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        fmt::println(file,
            "{}={}",
            province->GetId(),
            TerrainToString(province->GetTerrain())
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

    std::map<std::string, std::ofstream> files;

    for(const auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND))
            continue;
        SharedPtr<Title> kingdomTitle = this->GetProvinceLiegeTitle(province, TitleType::KINGDOM);
        if(kingdomTitle == nullptr) {
            // TODO: add error log
            continue;
        }
        if(files.count(kingdomTitle->GetName()) == 0) {
            std::string filePath = fmt::format("{}/00_{}_prov.txt", dir, kingdomTitle->GetName());
            files[kingdomTitle->GetName()] = std::ofstream(filePath, std::ios::out);
        }
        std::ofstream& file = files[kingdomTitle->GetName()];

        Parser::Node data = *province->GetOriginalData();
        if(!province->GetCulture().empty()) data.Put("culture", province->GetCulture());
        if(!province->GetReligion().empty()) data.Put("religion", province->GetReligion());
        data.Put("holding", ProvinceHoldingLabels[(int) province->GetHolding()]);
        data.SetDepth(1);

        fmt::println(file, "# {}", province->GetName());
        fmt::println(file, "{} = {}", province->GetId(), data);
    }

    for(auto& [key, file] : files)
        file.close();
}

void Mod::ExportTitles() {
    std::string dir = m_Dir + "/common/landed_titles";
    std::map<std::string, std::ofstream> files;
    
    // TODO: handle newly created titles without any original file.

    for(const auto& [name, title] : m_Titles) {
        if(title->GetLiegeTitle() != nullptr)
            continue;
        if(files.count(title->GetOriginalFilePath()) == 0) {
            files[title->GetOriginalFilePath()] = std::ofstream(title->GetOriginalFilePath(), std::ios::out);
        }
        std::ofstream& file = files[title->GetOriginalFilePath()];
        Parser::Node data = this->ExportTitle(title, 1);
        fmt::println(file, "{} = {}", title->GetName(), data);
    }

    for(auto& [key, file] : files)
        file.close();
}

Parser::Node Mod::ExportTitle(const SharedPtr<Title>& title, int depth) {
    Parser::Node data = *title->GetOriginalData();
    
    data.Put("color", title->GetColor());

    if(title->Is(TitleType::BARONY)) {
        SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
        data.Put("province", (double) baronyTitle->GetProvinceId());
    }
    else {
        SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);

        if(!title->Is(TitleType::COUNTY) && highTitle->GetCapitalTitle() != nullptr)
            data.Put("capital", highTitle->GetCapitalTitle()->GetName());

        for(const auto& dejureTitle : highTitle->GetDejureTitles()) {
            data.Put(dejureTitle->GetName(), this->ExportTitle(dejureTitle, depth+1));
        }
    }

    data.SetDepth(depth);
    return data;
}