#include "ProvinceManager.hpp"

#include "mod/Mod.hpp"
#include "titles/TitleManager.hpp"

#include <fmt/ostream.h>

ProvinceManager::ProvinceManager(Mod& mod) :
    m_Mod(mod),
    m_ProvincesImage(sf::Image()),
    m_HeightmapImage(sf::Image()),
    m_RiversImage(sf::Image()),
    m_ProvinceTerrainFileName("00_province_terrain.txt"),
    m_ProvinceTerrainPropertiesFileName("01_province_properties.txt"),
    m_ProvincesHistoryVariables({}),
    m_ProvinceTerrainPropertiesVariables(MakeShared<Jomini::Object>(Jomini::Type::OBJECT)),
    m_TerrainTypesVariables(MakeShared<Jomini::Object>(Jomini::Type::OBJECT))
{
}

size_t ProvinceManager::CountProvinces() const {
    return m_ProvincesByColors.size();
}

bool ProvinceManager::HasProvinceByColor(uint32_t color) const {
    return m_ProvincesByColors.contains(color);
}

bool ProvinceManager::HasProvinceById(int id) const {
    return m_ProvincesByIds.contains(id);
}

bool ProvinceManager::HasHoldingType(const std::string& name) const {
    return m_HoldingTypes.contains(name);
}

bool ProvinceManager::HasTerrainType(const std::string& name) const {
    return m_TerrainTypes.contains(name);
}

//////////////////////////////////////////////////////

Mod& ProvinceManager::GetMod() {
    return m_Mod;
}

sf::Image& ProvinceManager::GetProvincesImage() {
    return m_ProvincesImage;
}

const sf::Image& ProvinceManager::GetProvincesImage() const {
    return m_ProvincesImage;
}

sf::Image& ProvinceManager::GetHeightmapImage() {
    return m_HeightmapImage;
}

const sf::Image& ProvinceManager::GetHeightmapImage() const {
    return m_HeightmapImage;
}

sf::Image& ProvinceManager::GetRiversImage() {
    return m_RiversImage;
}

const sf::Image& ProvinceManager::GetRiversImage() const {
    return m_RiversImage;
}

sf::Image ProvinceManager::GetTerrainImage() const {
    // - Map provinces colors to their terrain color.
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    sf::Color defaultColor = sf::Color(0, 0, 0);

    sf::Image image = Image::MapPixels(
        m_ProvincesImage,
        [&](auto& mappedColors){
            for(const auto& [provinceColorId, province] : m_ProvincesByColors) {
                std::string terrain = province->GetTerrain();
                sf::Color color = defaultColor;

                if(m_TerrainTypes.contains(terrain)) {
                    const TerrainType& terrainType = m_TerrainTypes.at(terrain);
                    color = terrainType.GetColor();
                }

                mappedColors[province->GetColor().toInteger()] = color.toInteger();
            }    
        }
    );
    return image;
}

sf::Image ProvinceManager::GetWinterSeverityBiasImage() const {
    // - Map provinces colors to their winter severity bias.
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    sf::Color defaultColor = sf::Color(255, 0, 0);

    sf::Image image = Image::MapPixels(
        m_ProvincesImage, 
        [&](auto& mappedColors){
            for(const auto& [provinceColorId, province] : m_ProvincesByColors) {
                sf::Color color = defaultColor;

                if (!province->GetWinterSeverityBias().empty()) {
                    try {
                        double winterSeverity = std::clamp(
                            province->GetWinterSeverityBias().starts_with("@")
                                ? String::ParseDouble(m_ProvinceTerrainPropertiesVariables->Get(province->GetWinterSeverityBias())->As<std::string>("0.0"))
                                : String::ParseDouble(province->GetWinterSeverityBias()),
                            0.0,
                            1.0
                        );
                        color = sf::Color(winterSeverity * 255, winterSeverity * 255, winterSeverity * 255, 255);
                    }
                    catch(std::exception&){}
                }

                mappedColors[province->GetColor().toInteger()] = color.toInteger();
            }    
        }
    );
    return image;
}

Province* ProvinceManager::GetProvinceByColor(uint32_t color) {
    auto it = m_ProvincesByColors.find(color);
    return (it != m_ProvincesByColors.end()) ? it->second.get() : nullptr;
}

const Province* ProvinceManager::GetProvinceByColor(uint32_t color) const {
    auto it = m_ProvincesByColors.find(color);
    return (it != m_ProvincesByColors.end()) ? it->second.get() : nullptr;
}
    
Province* ProvinceManager::GetProvinceById(int id) {
    auto it = m_ProvincesByIds.find(id);
    return (it != m_ProvincesByIds.end()) ? it->second : nullptr;
}

const Province* ProvinceManager::GetProvinceById(int id) const {
    auto it = m_ProvincesByIds.find(id);
    return (it != m_ProvincesByIds.end()) ? it->second : nullptr;
}

Province* ProvinceManager::GetProvinceByPixel(int x, int y) {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_ProvincesImage.getSize().x) || y >= static_cast<int>(m_ProvincesImage.getSize().y))
        return nullptr;

    sf::Color color = m_ProvincesImage.getPixel(sf::Vector2u(x, y));
    uint32_t colorId = color.toInteger();

    return this->GetProvinceByColor(colorId);
}

const Province* ProvinceManager::GetProvinceByPixel(int x, int y) const {
    if (x < 0 || y < 0 || x >= static_cast<int>(m_ProvincesImage.getSize().x) || y >= static_cast<int>(m_ProvincesImage.getSize().y))
        return nullptr;

    sf::Color color = m_ProvincesImage.getPixel(sf::Vector2u(x, y));
    uint32_t colorId = color.toInteger();

    return this->GetProvinceByColor(colorId);
}

std::optional<int> ProvinceManager::GetMaxProvinceId() const {
    return m_ProvincesByIds.empty() ? std::nullopt : std::optional<int>(m_ProvincesByIds.rbegin()->first);
}

std::unordered_map<uint32_t, UniquePtr<Province>>& ProvinceManager::GetProvincesByColors() {
    return m_ProvincesByColors;
}

const std::unordered_map<uint32_t, UniquePtr<Province>>& ProvinceManager::GetProvincesByColors() const {
    return m_ProvincesByColors;
}

std::map<int, Province*>& ProvinceManager::GetProvincesByIds() {
    return m_ProvincesByIds;
}

const std::map<int, Province*>& ProvinceManager::GetProvincesByIds() const {
    return m_ProvincesByIds;
}

OrderedMap<std::string, HoldingType>& ProvinceManager::GetHoldingTypes() {
    return m_HoldingTypes;
}

const OrderedMap<std::string, HoldingType>& ProvinceManager::GetHoldingTypes() const {
    return m_HoldingTypes;
}

OrderedMap<std::string, TerrainType>& ProvinceManager::GetTerrainTypes() {
    return m_TerrainTypes;
}

const OrderedMap<std::string, TerrainType>& ProvinceManager::GetTerrainTypes() const {
    return m_TerrainTypes;
}

const std::string& ProvinceManager::GetDefaultLandTerrain() const {
    return m_DefaultLandTerrain;
}

const std::string& ProvinceManager::GetDefaultSeaTerrain() const {
    return m_DefaultSeaTerrain;
}   

const std::string& ProvinceManager::GetDefaultCoastalSeaTerrain() const {
    return m_DefaultCoastalSeaTerrain;
}

const std::string& ProvinceManager::GetProvinceTerrainFileName() const {
    return m_ProvinceTerrainFileName;
}

const std::string& ProvinceManager::GetProvinceTerrainPropertiesFileName() const {
    return m_ProvinceTerrainPropertiesFileName;
}

const std::map<std::string, SharedPtr<Jomini::Object>>& ProvinceManager::GetProvincesHistoryVariables() const {
    return m_ProvincesHistoryVariables;
}

const SharedPtr<Jomini::Object>& ProvinceManager::GetProvinceTerrainPropertiesVariables() const {
    return m_ProvinceTerrainPropertiesVariables;
}

const SharedPtr<Jomini::Object>& ProvinceManager::GetTerrainTypesVariables() const {
    return m_TerrainTypesVariables;
}

const std::map<std::string, std::string>& ProvinceManager::GetVanillaOverrideProvinceTerrainFiles() const {
    return m_VanillaOverrideProvinceTerrainFiles;
}

//////////////////////////////////////////////////////

void ProvinceManager::AddProvince(UniquePtr<Province> province) {
    if (province == nullptr)
        return;

    int id = province->GetId();
    uint32_t colorId = province->GetColorId();

    // If a province with the same color or id already exists, remove them first.
    // Removing both by color and by id is necessary in case the color and the id are not used by the same province.
    this->RemoveProvinceByColor(colorId);
    this->RemoveProvinceById(id);

    // Add the new province to both maps.
    m_ProvincesByIds[id] = province.get();
    m_ProvincesByColors[colorId] = std::move(province);
}

void ProvinceManager::RemoveProvinceByColor(uint32_t color) {
    auto it = m_ProvincesByColors.find(color);
    if (it == m_ProvincesByColors.end())
        return;

    const Province* province = it->second.get();
    int id = province->GetId();

    // TODO: check if the province has a barony title assigned to it.

    m_ProvincesByIds.erase(id);
    m_ProvincesByColors.erase(it);
}

void ProvinceManager::RemoveProvinceById(int id) {
    auto it = m_ProvincesByIds.find(id);
    if (it == m_ProvincesByIds.end())
        return;
    this->RemoveProvinceByColor(it->second->GetColorId());
}

void ProvinceManager::RemoveProvince(const Province *province) {
    if (province == nullptr)
        return;
    this->RemoveProvinceByColor(province->GetColorId());
}

void ProvinceManager::RenameProvinceColor(uint32_t formerColor, uint32_t newColor) {
    if (formerColor == newColor)
        return;

    auto it = m_ProvincesByColors.find(formerColor);
    if (it == m_ProvincesByColors.end())
        return;

    // If a province with the new color already exists, remove it first.
    this->RemoveProvinceByColor(newColor);

    UniquePtr<Province> province = std::move(it->second);
    province->SetColor(sf::Color(newColor));
    m_ProvincesByColors[newColor] = std::move(province);
    m_ProvincesByColors.erase(it);
}

void ProvinceManager::RenameProvinceId(int formerId, int newId) {
    if (newId < 1 || formerId == newId)
        return;

    auto it = m_ProvincesByIds.find(formerId);
    if (it == m_ProvincesByIds.end())
        return;
    
    // If a province with the new id already exists, remove it first.
    this->RemoveProvinceById(newId);

    Province* province = it->second;
    province->SetId(newId);
    m_ProvincesByIds[newId] = province;
    m_ProvincesByIds.erase(it);
}

//////////////////////////////////////////////////////

void ProvinceManager::LoadHoldingTypes() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_HOLDINGS));

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
        HoldingType("herder_holding"),
        HoldingType("nomad_holding")
    };

    for(auto type : vanillaTypes) {
        if(!m_HoldingTypes.contains(type.GetName()))
            m_HoldingTypes.insert(type.GetName(), type);
    }
}

void ProvinceManager::LoadTerrainTypes() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_TERRAIN_TYPES));

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
        TerrainType("sea", sf::Color(0, 0, 255)),
        TerrainType("coastal_sea", sf::Color(255, 255, 230)),
        TerrainType("farmlands", sf::Color(255, 50, 50)),
        TerrainType("hills", sf::Color(200, 200, 200)),
        TerrainType("mountains", sf::Color(255, 255, 255)),
        TerrainType("desert", sf::Color(255, 255, 0)),
        TerrainType("desert_mountains", sf::Color(100, 100, 0)),
        TerrainType("oasis", sf::Color(100, 100, 255)),
        TerrainType("jungle", sf::Color(10, 60, 35)),
        TerrainType("forest", sf::Color(50, 255, 25)),
        TerrainType("taiga", sf::Color(0, 90, 0)),
        TerrainType("wetlands", sf::Color(75, 200, 200)),
        TerrainType("steppe", sf::Color(200, 100, 25)),
        TerrainType("floodplains", sf::Color(200, 50, 100)),
        TerrainType("drylands", sf::Color(220, 45, 120)),
        TerrainType("terraced_hills", sf::Color(225, 35, 40))
    };

    for(auto type : vanillaTypes) {
        if(!m_TerrainTypes.contains(type.GetName()))
            m_TerrainTypes.insert(type.GetName(), type);
    }
}

void ProvinceManager::LoadProvincesDefinition() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_DEFINITIONS);
    
    // Create an empty definition file if it does not exist.
    if (!std::filesystem::exists(filePath)) {
        std::ofstream file(filePath);
        /*if (!file)
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesDefinition: Failed to create an empty definition file at \"{}\"", filePath));*/
        return;
    }
    
    std::vector<std::vector<std::string>> lines = File::ReadCSV(filePath);

    // Skip the first line.
    if(!lines.empty())
        lines.erase(lines.begin());

    int lastId = 0;

    for(const auto& line : lines) {
        try {
            int id = std::stoi(line[0]);
            int r = std::stoi(line[1]);
            int g = std::stoi(line[2]);
            int b = std::stoi(line[3]);
            std::string name = line[4];

            // TODO: Should the alpha channel be included in the color id?
            uint32_t colorId = static_cast<uint32_t>((r << 24) | (g << 16) | (b << 8) | 255);

            if(m_ProvincesByIds.count(id) > 0)
                LOG_ERROR("Several provinces with same id: {}", id);
            if(m_ProvincesByColors.count(colorId) > 0)
                LOG_ERROR("Several provinces with same color: {},{}", id, m_ProvincesByColors.at(colorId)->GetId());
            if(id != lastId+1)
                LOG_ERROR("Ids in definitions.csv are not sequential: {} to {}", lastId, id);

            UniquePtr<Province> province = MakeUnique<Province>(id, sf::Color(r, g, b), name);
            this->AddProvince(std::move(province));
            lastId = id;
        }
        catch (std::exception& e) {
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesDefinition: Failed to parse definitions.csv at \"{}\"\n{}", String::Join(line, ";"), e.what()));
        }
    }
}

void ProvinceManager::LoadProvincesImage() {
    // Load the provinces image from the mod files.
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_PROVINCES);
    if (!std::filesystem::exists(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadProvinceImage: Failed to load provinces image at '{}': file does not exist", filePath));
    if(!m_ProvincesImage.loadFromFile(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadProvinceImage: Failed to load provinces image at '{}': file couldn't be opened", filePath));

    const auto& pixels = m_ProvincesImage.getPixelsPtr();
    std::map<uint32_t, bool> colors;

    size_t width = m_ProvincesImage.getSize().x;
    size_t height = m_ProvincesImage.getSize().y;
    size_t totalPixels = width * height;

    const auto& GetIndexPosition = [&](size_t index) {
        index = index - 4;
        return sf::Vector2i((index / 4) % width, floor(index / (4*width)));
    };

    // Split the image vertically between all the threads.
    const int threadsCount = 4;
    std::vector<UniquePtr<std::thread>> threads;
    const size_t threadRange = totalPixels / threadsCount;

    for(size_t i = 0; i < threadsCount; i++) {

        threads.push_back(MakeUnique<std::thread>([&, i](){
            size_t startIndex = i * threadRange*4;
            size_t endIndex = (i == threadsCount-1) ? totalPixels*4 : (i+1) * threadRange*4;
            size_t index = startIndex;

            uint32_t color = 0x000000FF;
            uint32_t previousColor = 0x00000000;
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

                const auto& province = m_ProvincesByColors.find(color);
                hasProvince = (previousColor == color && hasProvince) || (province != m_ProvincesByColors.end());
                bool alreadySeen = (previousColor == color || colors.count(color) > 0);

                previousColor = color;

                if(!alreadySeen) {
                    colors[color] = true;
                }

                if(!alreadySeen && !hasProvince) {
                    sf::Vector2i pos = GetIndexPosition(index-4);
                    LOG_ERROR("Color found in image but missing province from definition.csv: ({},{},{},{}) at ({},{})", pixels[index-4], pixels[index-3], pixels[index-2], pixels[index-1], pos.x, pos.y);
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
    }

    for (auto& thread : threads) {
		if (thread->joinable())
            thread->join();
    }
}

void ProvinceManager::LoadHeightmapImage() {
    // Load the heightmap image from the mod files.
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_HEIGHTMAP);
    if (!std::filesystem::exists(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadHeightmapImage: Failed to load heightmap image at '{}': file does not exist", filePath));
    if(!m_HeightmapImage.loadFromFile(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadHeightmapImage: Failed to load heightmap image at '{}': file couldn't be opened", filePath));
}

void ProvinceManager::LoadRiversImage() {
    // Load the rivers image from the mod files.
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_RIVERS);
    if (!std::filesystem::exists(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadRiversImage: Failed to load rivers image at '{}': file does not exist", filePath));
    if(!m_RiversImage.loadFromFile(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadRiversImage: Failed to load rivers image at '{}': file couldn't be opened", filePath));
}

void ProvinceManager::LoadDefaultMapFile() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_DEFAULT_MAP);
    
    // Create an empty definition file if it does not exist.
    if (!std::filesystem::exists(filePath))
        throw std::runtime_error(fmt::format("ProvinceManager::LoadDefaultMapFile: Failed to load provinces image at '{}': file does not exist", filePath));

    SharedPtr<Jomini::Object> result = nullptr;
    try {
        result = Jomini::ParseFile(filePath);
    }
    catch (std::exception& e) {
        throw std::runtime_error(fmt::format("ProvinceManager::LoadDefaultMapFile: Failed to parse default map file at '{}': {}", filePath, e.what()));
    }

    // TODO: Investigate how coastal, islands and land provinces work.

    const std::vector<double>& seaZones = result->Get("sea_zones")->AsArray<double>(std::vector<double>{});
    for(double provinceId : seaZones) {
        if (!m_ProvincesByIds.contains(provinceId)) {
            LOG_ERROR("Unknown province '{}' defined as a sea zone in default.map", provinceId);
            continue;
        }
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
    }

    const std::vector<double>& rivers = result->Get("river_provinces")->AsArray<double>(std::vector<double>{});
    for(double provinceId : rivers) {
        if (!m_ProvincesByIds.contains(provinceId)) {
            LOG_ERROR("Unknown province '{}' defined as a river in default.map", provinceId);
            continue;
        }
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::RIVER, true);
    }

    const std::vector<double>& lakes = result->Get("lakes")->AsArray<double>(std::vector<double>{});
    for(double provinceId : lakes) {
        if (!m_ProvincesByIds.contains(provinceId)) {
            LOG_ERROR("Unknown province '{}' defined as a lake in default.map", provinceId);
            continue;
        }
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAKE, true);
    }
    
    const std::vector<double>& impassableMountains = result->Get("impassable_mountains")->AsArray<double>(std::vector<double>{});
    for(double provinceId : impassableMountains) {
        if (!m_ProvincesByIds.contains(provinceId)) {
            LOG_ERROR("Unknown province '{}' defined as an impassable mountain in default.map", provinceId);
            continue;
        }
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }

    const std::vector<double>& impassableSeas = result->Get("impassable_seas")->AsArray<double>(std::vector<double>{});
    for(double provinceId : impassableSeas) {
        if (!m_ProvincesByIds.contains(provinceId)) {
            LOG_ERROR("Unknown province '{}' defined as an impassable sea in default.map", provinceId);
            continue;
        }
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::SEA, true);
        m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::IMPASSABLE, true);
    }

    // Every province that are not definied as a sea, river or lake will be auto-assigned as a LAND province.
    for (auto& [_, province] : m_ProvincesByColors) {
        if (province->HasFlag(ProvinceFlags::SEA)
            || province->HasFlag(ProvinceFlags::RIVER)
            || province->HasFlag(ProvinceFlags::LAKE
        )) {
            province->SetFlag(ProvinceFlags::LAND, false);
        }
        else {
            province->SetFlag(ProvinceFlags::LAND, true);
        }
    }
}

void ProvinceManager::LoadProvincesTerrain() {
    m_VanillaOverrideProvinceTerrainFiles.clear();

    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::COMMON_PROVINCE_TERRAIN) );

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::COMMON_PROVINCE_TERRAIN, filePath);

            if (data->GetMap().empty()) {
                std::ifstream overrideFile = std::ifstream(filePath, std::ios::binary);
                m_VanillaOverrideProvinceTerrainFiles[fileName] = File::ReadString(overrideFile);
                continue;
            }

            // Check if that file is a province terrain type file, and not a climate one.
            if (!data->Contains("default_land"))
                return;

            this->LoadProvincesTerrainFile(fileName, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("ProvinceManager::LoadProvincesTerrain: Failed to parse province terrain file '{}': {}", filePath, e.what());
        }
    }
}

void ProvinceManager::LoadProvincesTerrainFile(const std::string& fileName, SharedPtr<Jomini::Object> data) {
    m_ProvinceTerrainFileName = fileName;
    m_DefaultLandTerrain = data->Get("default_land")->As<std::string>("plains");
    m_DefaultSeaTerrain = data->Get("default_sea")->As<std::string>("sea");
    m_DefaultCoastalSeaTerrain = data->Get("default_coastal_sea")->As<std::string>("sea");

    data->Remove("default_land");
    data->Remove("default_sea");
    data->Remove("default_coastal_sea");

    // Set default terrain for all provinces (especially for those without any in files).
    for(const auto& [colorId, province] : m_ProvincesByColors) {
        std::string defaultTerrain = m_DefaultLandTerrain;
        if(province->HasFlag(ProvinceFlags::SEA))
            defaultTerrain = (province->HasFlag(ProvinceFlags::COASTAL) ? m_DefaultCoastalSeaTerrain : m_DefaultSeaTerrain);
        province->SetTerrain(defaultTerrain);
    }

    for(const auto& [key, pair] : data->GetMap()) {
        const auto& [op, value] = pair;
        int provinceId = 0;
        std::string terrain = "";

        try {
            provinceId = String::ParseInt(key);
        }
        catch (const std::exception& e) {
            LOG_WARNING("Invalid province id '{}' in '{}': {}", key, fileName, e.what());
            continue;
        }

        // If the province id has been assigned several terrain type then we only pick the first one.
        if(!value->Is(Jomini::Type::SCALAR)) {
            LOG_WARNING("Province assigned several terrain types: '{}'", provinceId);
            terrain = value->AsArray<std::string>().front();
        }
        else {
            terrain = value->As<std::string>();
        }

        if(!m_ProvincesByIds.contains(provinceId)) {
            LOG_WARNING("Terrain type assigned to undefined province: '{}'", provinceId);
            continue;
        }

        if(!m_TerrainTypes.contains(terrain)) {
            LOG_WARNING("Undefined terrain type '{}' assigned to province '{}'", terrain, provinceId);
        }

        m_ProvincesByIds[provinceId]->SetTerrain(terrain);
    }
}

void ProvinceManager::LoadProvincesClimate() {
    // Load the province climate type: mild, normal, severe.
    std::string climateFile = m_Mod.GetAbsolutePath(Paths::MAP_DATA_CLIMATE);
    
    if (std::filesystem::exists(climateFile)) {
        SharedPtr<Jomini::Object> result = nullptr;
        try {
            result = Jomini::ParseFile(climateFile);
        }
        catch(const std::exception& e) {
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesClimate: Failed to parse climate file '{}':\n{}", climateFile, e.what()));
        }

        const auto LoadClimateProvinces = [&](ClimateType type, std::string_view name) {
            if (!result->Contains(name))
                return;
            
            auto provinces = result->Get(name);
            if (!provinces->Is(Jomini::Type::ARRAY)) {
                LOG_WARNING("Invalid value for '{}' in '{}'", name, climateFile);
                return;
            }

            std::vector<int> ids = provinces->AsArray<int>();
            for (int id : ids) {
                if (m_ProvincesByIds.count(id) == 0) {
                    LOG_WARNING("Undefined province '{}' for '{}' in '{}'", id, name, climateFile);
                    continue;
                }
                m_ProvincesByIds[id]->SetClimateType(type);
            }
        };
        LoadClimateProvinces(ClimateType::MILD_WINTER, "mild_winter");
        LoadClimateProvinces(ClimateType::NORMAL_WINTER, "normal_winter");
        LoadClimateProvinces(ClimateType::SEVERE_WINTER, "severe_winter");
    }

    /////////////////////////////////////////////////////////////////////////////////

    // Load province winter properties such as severity or factor override.
     std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::COMMON_PROVINCE_TERRAIN) );

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::COMMON_PROVINCE_TERRAIN, filePath);

            if (data->GetMap().empty()) {
                std::ifstream overrideFile = std::ifstream(filePath, std::ios::binary);
                m_VanillaOverrideProvinceTerrainFiles[fileName] = File::ReadString(overrideFile);
                continue;
            }

            // Check if that file is a province terrain type file, and not a climate one.
            if (data->Contains("default_land"))
                continue;

            this->LoadProvincesTerrainPropertiesFile(fileName, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("ProvinceManager::LoadProvincesClimate: Failed to parse properties file '{}':\n{}", filePath, e.what());
        }
    }   
}

void ProvinceManager::LoadProvincesTerrainPropertiesFile(const std::string& fileName, SharedPtr<Jomini::Object> data) {
    m_ProvinceTerrainPropertiesFileName = fileName;

    for(const auto& [key, pair] : data->GetMap()) {
        // Save and ignore variables to only keep province ids.
        if (key.starts_with("@")) {
            m_ProvinceTerrainPropertiesVariables->Put(key, pair.second);
            continue;
        }

        const auto& [op, value] = pair;
        int provinceId = String::ParseInt(key);

        if (m_ProvincesByIds.count(provinceId) == 0) {
            LOG_WARNING("Climate properties assigned to undefined province '{}' in '{}'", provinceId, fileName);
            continue;
        }

        if (!value->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Invalid climate properties for province '{}' in '{}'", provinceId, fileName);
            continue;
        }

        if (value->Contains("winter_severity_bias")) {
            const auto winterSeverityBias = value->Get("winter_severity_bias");
            if (!winterSeverityBias->Is(Jomini::Type::SCALAR)) {
                LOG_ERROR("Invalid 'winter_severity_bias' value for province '{}' in '{}'", provinceId, fileName);
                continue;
            }
            m_ProvincesByIds[provinceId]->SetWinterSeverityBias(winterSeverityBias->As<std::string>());
        }
        
        if (value->Contains("mild_winter_factor_override")) {
            const auto factor = value->Get("mild_winter_factor_override");
            if (!factor->Is(Jomini::Type::SCALAR)) {
                LOG_ERROR("Invalid 'mild_winter_factor_override' value for province '{}' in '{}'", provinceId, fileName);
                continue;
            }
            m_ProvincesByIds[provinceId]->SetMildWinterFactorOverride(factor->As<std::string>());
        }
        if (value->Contains("normal_winter_factor_override")) {
            const auto factor = value->Get("normal_winter_factor_override");
            if (!factor->Is(Jomini::Type::SCALAR)) {
                LOG_ERROR("Invalid 'normal_winter_factor_override' value for province '{}' in '{}'", provinceId, fileName);
                continue;
            }
            m_ProvincesByIds[provinceId]->SetNormalWinterFactorOverride(factor->As<std::string>());
        }
        if (value->Contains("harsh_winter_factor_override")) {
            const auto factor = value->Get("harsh_winter_factor_override");
            if (!factor->Is(Jomini::Type::SCALAR)) {
                LOG_ERROR("Invalid 'harsh_winter_factor_override' value for province '{}' in '{}'", provinceId, fileName);
                continue;
            }
            m_ProvincesByIds[provinceId]->SetHarshWinterFactorOverride(factor->As<std::string>());
        }
    }
}

void ProvinceManager::LoadProvincesHistory() {
    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::HISTORY_PROVINCES) );

    for(const auto& filePath : filesPath) {
        if (!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::HISTORY_PROVINCES, filePath);
            this->LoadProvincesHistoryFile(fileName, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("Failed to parse provinces history file '{}': {}", filePath, e.what());
        }
    }
}

void ProvinceManager::LoadProvincesHistoryFile(const std::string& fileName, SharedPtr<Jomini::Object> data) {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::HISTORY_PROVINCES, fileName);

    for(auto& [key, provincePair] : data->GetMap()) {
        auto& [_, provinceValue] = provincePair;

        // Handle variables that might be in the file and store them for export.
        if (key.starts_with("@")) {
            if (!m_ProvincesHistoryVariables.contains(fileName))
                m_ProvincesHistoryVariables[fileName] = MakeShared<Jomini::Object>();
            if (!m_ProvincesHistoryVariables[fileName]->Contains(key))
                m_ProvincesHistoryVariables[fileName]->Put(key, provinceValue);
            continue;
        }

        int provinceId = 0;
        try {
            provinceId = String::ParseInt(key);
        }
        catch (std::exception& e) {
            LOG_ERROR("Province '{}' has invalid id in {}", key, filePath);
            continue;
        }

        // Ignore undefined provinces.
        if(!m_ProvincesByIds.contains(provinceId)) {
            LOG_WARNING("Unknown province '{}' is defined in '{}'", provinceId, filePath);
            continue;
        }

        // Merge all objects into a single one when there are duplicate definitions for the same province.
        if (provinceValue->Is(Jomini::Type::ARRAY))
            provinceValue = provinceValue->Flatten(false);

        // Assert that the value is a correct object where a key is a date.
        if (!provinceValue->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Province '{}' has invalid history entry in '{}'", provinceId, filePath);
            continue;
        }

        // 1. Extract global values such as culture, religion or holding.
        if(provinceValue->Contains("culture"))
            m_ProvincesByIds[provinceId]->SetCulture(provinceValue->GetFirst("culture")->As<std::string>(""));
        if(provinceValue->Contains("religion"))
            m_ProvincesByIds[provinceId]->SetFaith(provinceValue->GetFirst("religion")->As<std::string>(""));
        if(provinceValue->Contains("holding")) {
            m_ProvincesByIds[provinceId]->SetHolding(provinceValue->GetFirst("holding")->As<std::string>(""));
        }

        if(!m_HoldingTypes.contains(m_ProvincesByIds[provinceId]->GetHolding())) {
            LOG_WARNING("Province '{}' has undefined holding type '{}' in '{}'", provinceId, m_ProvincesByIds[provinceId]->GetHolding(), filePath);
        }

        // Remove those attributes to avoid duplicates when exporting and to reduce memory usage a bit.
        provinceValue->Remove("culture");
        provinceValue->Remove("religion");
        provinceValue->Remove("holding");

        // 2. Loop over dates in the province history and process them.
        
        // Some titles might have other attributes not assigned to a date in their history (e.g special_building_slot).
        SharedPtr<Jomini::Object> extraHistoryData = MakeShared<Jomini::Object>();

        for(const auto& [dateString, datePair] : provinceValue->GetMap()) {
            auto [__, history] = datePair;

            Jomini::Date date;
            try {
                date = Date::ParseDate(dateString);
            }
            catch (std::exception& e) {
                if (dateString.find(".") != std::string::npos) {
                    LOG_ERROR("Province '{}' has invalid date syntax '{}' in '{}'", provinceId, dateString, filePath);
                }
                else {
                    extraHistoryData->Put(dateString, history, __);
                }
                continue;
            }

            // Merge all objects into a single one when there are duplicate definitions for the same date.
            if (history->Is(Jomini::Type::ARRAY))
                history = history->Flatten(true);

            // Make sure that the history entry is correct.
            if (!history->Is(Jomini::Type::OBJECT)) {
                LOG_ERROR("Province '{}' has invalid history entry for '{}' in '{}'", provinceId, dateString, filePath);
                continue;
            }
            
            m_ProvincesByIds[provinceId]->AddHistory(date, history);
        }

        m_ProvincesByIds[provinceId]->SetOriginalHistoryFileName(fileName);
        m_ProvincesByIds[provinceId]->SetExtraHistoryData(extraHistoryData);
    }
}

void ProvinceManager::ExportProvincesDefinition() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_DEFINITIONS);
    
    // Create the directory if it does not exist.
    std::filesystem::path fileDir = std::filesystem::path(filePath).parent_path();
    if (!std::filesystem::exists(fileDir)) std::filesystem::create_directories(fileDir);

    std::ofstream file(filePath, std::ios::out);
    if (!file) throw std::runtime_error(fmt::format("ProvinceManager::ExportProvincesDefinition: Failed to open file for writing at '{}'", filePath));

    // The format of definition.csv is as following:
    // [ID];[RED];[GREEN];[BLUE];[Barony Name];x;
    fmt::println(file, "0;0;0;0;x;x;\n");

    // IDs must be sequential, or the game will crash. That's why m_ProvincesByIds is a map and not an unordered map.
    for(const auto& [id, province] : m_ProvincesByIds) {
        fmt::println(file,
            "{};{};{};{};{};x;",
            province->GetId(),
            province->GetColor().r,
            province->GetColor().g,
            province->GetColor().b,
            province->GetName()
        );
    }
}

void ProvinceManager::ExportDefaultMapFile() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_DEFAULT_MAP);
    
    // Create the directory if it does not exist.
    std::filesystem::path fileDir = std::filesystem::path(filePath).parent_path();
    if (!std::filesystem::exists(fileDir)) std::filesystem::create_directories(fileDir);

    // Read the file and keep all values except for the terrain flags
    // such as: sea_zones, impassable_seas, lakes, impassable_mountains, river_provinces
    SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

    SharedPtr<Jomini::Object> zonesData = MakeShared<Jomini::Object>(Jomini::ObjectMap{});

    zonesData->Put("sea_zones", std::vector<double>());
    zonesData->Put("river_provinces", std::vector<double>());
    zonesData->Put("lakes", std::vector<double>());
    zonesData->Put("impassable_mountains", std::vector<double>());
    zonesData->Put("impassable_seas", std::vector<double>());

    for (auto [key, object] : zonesData->GetMap())
        object.second->SetFlag(Jomini::Flags::RANGE, true);

    // Remove those keys since they are printed seperately.
    data->Remove("sea_zones");
    data->Remove("river_provinces");
    data->Remove("lakes");
    data->Remove("impassable_mountains");
    data->Remove("impassable_seas");

    for(const auto& [id, province] : m_ProvincesByIds) {
        if(province->HasFlag(ProvinceFlags::RIVER))
            zonesData->Get("river_provinces")->Push<int>(id);
        
        if(province->HasFlag(ProvinceFlags::LAKE))
            zonesData->Get("lakes")->Push<int>(id);

        if(province->HasFlag(ProvinceFlags::SEA)) {
            zonesData->Get("sea_zones")->Push<int>(id);
            if(province->HasFlag(ProvinceFlags::IMPASSABLE))
                zonesData->Get("impassable_seas")->Push<int>(id);
        }
        else if(province->HasFlag(ProvinceFlags::IMPASSABLE)) {
            zonesData->Get("impassable_mountains")->Push<int>(id);
        }
    }

    std::ofstream file(filePath, std::ios::out);
    if (!file) throw std::runtime_error(fmt::format("ProvinceManager::ExportDefaultMapFile: Failed to open file for writing at '{}'", filePath));

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
        "\n{}\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}\n\n{}",
        data->Serialize(),
        "#############\n# SEA ZONES\n#############",
        FORMAT_LIST("sea_zones"),
        "#############\n# MAJOR RIVERS\n#############",
        FORMAT_LIST("river_provinces"),
        "#############\n# LAKES\n#############",
        FORMAT_LIST("lakes"),
        "#############\n# IMPASSABLE TERRAIN\n#############",
        FORMAT_LIST("impassable_mountains"),
        "#############\n# IMPASSABLE SEA ZONES\n#############",
        FORMAT_LIST("impassable_seas")
    );
}

void ProvinceManager::ExportProvincesTerrain() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::COMMON_PROVINCE_TERRAIN, m_ProvinceTerrainFileName);
    
    // Create the directory if it does not exist.
    std::filesystem::path fileDir = std::filesystem::path(filePath).parent_path();
    if (!std::filesystem::exists(fileDir)) std::filesystem::create_directories(fileDir);

    std::ofstream file(filePath, std::ios::out);
    if (!file) throw std::runtime_error(fmt::format("ProvinceManager::ExportProvincesTerrain: Failed to open file for writing at '{}'", filePath));
    File::EncodeToUTF8BOM(file);

    fmt::println(file, "default_land={}", m_DefaultLandTerrain);
    fmt::println(file, "default_sea={}", m_DefaultSeaTerrain);
    fmt::println(file, "default_coastal_sea={}\n", m_DefaultCoastalSeaTerrain);

    for(auto& [id, province] : m_ProvincesByIds) {
        if(!province->HasFlag(ProvinceFlags::LAND) || province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        if (province->GetTerrain().empty()) {
            LOG_ERROR("Land province '{}' does not have any specified terrain.", id);
            continue;
        }
        fmt::println(file,
            "{}={}",
            province->GetId(),
            province->GetTerrain()
        );
    }

    // Create empty files for the vanilla overrides.
    for (const auto& [fileName, fileContent] : m_VanillaOverrideProvinceTerrainFiles) {
        std::ofstream file = std::ofstream(m_Mod.GetAbsolutePath(Paths::COMMON_PROVINCE_TERRAIN, fileName), std::ios::out);
        // File::EncodeToUTF8BOM(file); // Encoding bytes are present alongside the file content.
        fmt::print(file, "{}", fileContent);
    }
}

void ProvinceManager::ExportProvincesClimate() {
    std::string climateFilePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_CLIMATE);
    std::string propertiesFilePath = m_Mod.GetAbsolutePath(Paths::COMMON_PROVINCE_TERRAIN, m_ProvinceTerrainPropertiesFileName);
    
    // Create the directories if they do not exist.
    std::filesystem::path climateFileDir = std::filesystem::path(climateFilePath).parent_path();
    if (!std::filesystem::exists(climateFileDir)) std::filesystem::create_directories(climateFileDir);
    std::filesystem::path propertiesFileDir = std::filesystem::path(propertiesFilePath).parent_path();
    if (!std::filesystem::exists(propertiesFileDir)) std::filesystem::create_directories(propertiesFileDir);

    std::ofstream climateFile(climateFilePath, std::ios::out);
    if (!climateFile) throw std::runtime_error(fmt::format("ProvinceManager::ExportProvincesClimate: Failed to open file for writing at '{}'", climateFilePath));
    
    std::ofstream propertiesFile(propertiesFilePath, std::ios::out);
    if (!propertiesFile) throw std::runtime_error(fmt::format("ProvinceManager::ExportProvincesClimate: Failed to open file for writing at '{}'", propertiesFilePath));
    File::EncodeToUTF8BOM(propertiesFile);

    SharedPtr<Jomini::Object> climateObject = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
    climateObject->Put("mild_winter", std::vector<int>());
    climateObject->Put("normal_winter", std::vector<int>());
    climateObject->Put("severe_winter", std::vector<int>());

    SharedPtr<Jomini::Object> mildWinterObject = climateObject->Get("mild_winter");
    SharedPtr<Jomini::Object> normalWinterObject = climateObject->Get("normal_winter");
    SharedPtr<Jomini::Object> severeWinterObject = climateObject->Get("severe_winter");

    // Write variables that were in the properties file before loading the mod.
    fmt::println(propertiesFile, "{}\n", m_ProvinceTerrainPropertiesVariables->Serialize());

    for (auto& [id, province] : m_ProvincesByIds) {
        // Insert the province id to its corresponding climate type.
        if (province->GetClimateType() != ClimateType::NONE) {
            switch (province->GetClimateType()) {
                case ClimateType::MILD_WINTER: mildWinterObject->Push(id); break;
                case ClimateType::NORMAL_WINTER: normalWinterObject->Push(id); break;
                case ClimateType::SEVERE_WINTER: severeWinterObject->Push(id); break;
                default: break;
            }
        }

        // Export individual provinces climate properties to 'common/province_terrain/01_province_properties.txt'.
        bool hasProperty = !province->GetWinterSeverityBias().empty()
            || !province->GetMildWinterFactorOverride().empty()
            || province->GetNormalWinterFactorOverride().empty()
            || province->GetHarshWinterFactorOverride().empty();
        
        // Ignore provinces without any specified properties.
        if (hasProperty) {
            SharedPtr<Jomini::Object> propertiesObject = MakeShared<Jomini::Object>(Jomini::ObjectMap{});

            if (!province->GetWinterSeverityBias().empty()) propertiesObject->Put("winter_severity_bias", province->GetWinterSeverityBias());
            if (!province->GetMildWinterFactorOverride().empty()) propertiesObject->Put("mild_winter_factor_override", province->GetMildWinterFactorOverride());
            if (!province->GetNormalWinterFactorOverride().empty()) propertiesObject->Put("normal_winter_factor_override", province->GetNormalWinterFactorOverride());
            if (!province->GetHarshWinterFactorOverride().empty()) propertiesObject->Put("harsh_winter_factor_override", province->GetHarshWinterFactorOverride());

            fmt::println(propertiesFile, "{} = {}", id, propertiesObject->Serialize(1, false, false));
        }
    }

    // Export provinces climate type in 'map_data/climate.txt'
    fmt::println(climateFile, "{}", climateObject->Serialize());
}

void ProvinceManager::ExportProvincesHistory(TitleManager& titleManager) {
    // Create the directories if they do not exist.
    std::string dir = m_Mod.GetDirectory(Paths::HISTORY_PROVINCES);
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    struct FileData {
        std::ofstream file;
        bool kingdomComment = false;
        bool duchyComment = false;
        bool countyComment = false;
    };
    std::map<std::string, FileData> files;

    std::vector<Province*> provinces(m_ProvincesByIds.size());
    std::transform(m_ProvincesByIds.begin(), m_ProvincesByIds.end(), provinces.begin(), [](const auto& pair) { return pair.second; });

    // In order to be able to add comments for the kingdom, duchy and county tiers, we need to have provinces grouped by their liege titles.
    std::sort(provinces.begin(), provinces.end(), [&](const Province* a, const Province* b) {
        BaronyTitle* aBaronyTitle = titleManager.GetBaronyByProvinceId(a->GetId());
        BaronyTitle* bBaronyTitle = titleManager.GetBaronyByProvinceId(b->GetId());

        // Provinces without barony title are sorted at the end.
        if (!aBaronyTitle || !bBaronyTitle)
            return aBaronyTitle != nullptr;

        HighTitle* aCountyTitle = aBaronyTitle->GetLiegeTitle(TitleType::COUNTY);
        HighTitle* bCountyTitle = bBaronyTitle->GetLiegeTitle(TitleType::COUNTY);

        // Provinces without county title are sorted at the end.
        if (!aCountyTitle || !bCountyTitle)
            return aCountyTitle != nullptr;

        HighTitle* aDuchyTitle = aCountyTitle->GetLiegeTitle(TitleType::DUCHY);
        HighTitle* bDuchyTitle = bCountyTitle->GetLiegeTitle(TitleType::DUCHY);

        if (!aDuchyTitle || !bDuchyTitle)
            return aCountyTitle->GetName() < bCountyTitle->GetName();

        HighTitle* aKingdomTitle = aDuchyTitle->GetLiegeTitle(TitleType::KINGDOM);
        HighTitle* bKingdomTitle = bDuchyTitle->GetLiegeTitle(TitleType::KINGDOM);

        if (!aKingdomTitle || !bKingdomTitle)
            return aDuchyTitle->GetName() < bDuchyTitle->GetName();

        return aKingdomTitle->GetName() < bKingdomTitle->GetName();
    });

    HighTitle* lastKingdomTitle = nullptr;
    HighTitle* lastDuchyTitle = nullptr;
    HighTitle* lastCountyTitle = nullptr;

    for(const auto& province : provinces) {
        int provinceId = province->GetId();

        // Determine the file name for the province history.
        // The original file name is kept if the province history was loaded from an existing file,
        // otherwise we generate a new one based on the kingdom tier title of the province,
        // and if the province doesn't have any kingdom tier liege then we assign it to a default file.
        std::string fileName = province->GetOriginalHistoryFileName();
        if(fileName.empty()) {
            fileName = "00_temp_prov.txt";
            if (BaronyTitle* baronyTitle = titleManager.GetBaronyByProvinceId(provinceId)) {
                if (HighTitle* kingdomTitle = baronyTitle->GetLiegeTitle(TitleType::KINGDOM)) {
                    fileName = "00_" + kingdomTitle->GetName() + "_prov.txt";
                }
            }
        }
        std::string filePath = dir + "/" + fileName;

        // Open the file if it is not already open, and write the original script variables if there are any.
        if(!files.contains(fileName)) {
            files[fileName] = FileData{};
            files[fileName].file = std::ofstream(filePath, std::ios::out);
            if (!files[fileName].file) {
                LOG_ERROR("Failed to open file for writing '{}' for province '{}'", filePath, provinceId);
                continue;
            }
            File::EncodeToUTF8BOM(files[fileName].file);

            // Export original script variables.
            auto it = m_ProvincesHistoryVariables.find(fileName);
            if (it != m_ProvincesHistoryVariables.end() && !it->second->GetMap().empty()) {
                fmt::println(files[fileName].file, "{}\n", it->second->Serialize(0, true));
            }
        }

        std::ofstream& file = files[fileName].file;
        
        BaronyTitle* baronyTitle = titleManager.GetBaronyByProvinceId(provinceId);
        HighTitle* countyTitle = (baronyTitle != nullptr) ? baronyTitle->GetLiegeTitle(TitleType::COUNTY) : nullptr;
        HighTitle* duchyTitle = (countyTitle != nullptr) ? countyTitle->GetLiegeTitle(TitleType::DUCHY) : nullptr;
        HighTitle* kingdomTitle = (duchyTitle != nullptr) ? duchyTitle->GetLiegeTitle(TitleType::KINGDOM) : nullptr;

        // Comments about the kingdoms, duchies and counties should be written only once per file, with all their provinces grouped together.
        if (lastKingdomTitle != kingdomTitle) {
            lastKingdomTitle = kingdomTitle;
            lastDuchyTitle = nullptr;
            lastCountyTitle = nullptr;
            for (auto& [_, fileData] : files)
                fileData.kingdomComment = false;
        }
        if (lastDuchyTitle != duchyTitle) {
            lastDuchyTitle = duchyTitle;
            lastCountyTitle = nullptr;
            for (auto& [_, fileData] : files)
                fileData.duchyComment = false;
        }
        if (lastCountyTitle != countyTitle) {
            lastCountyTitle = countyTitle;
            for (auto& [_, fileData] : files)
                fileData.countyComment = false;
        }
        if (kingdomTitle != nullptr && !files[fileName].kingdomComment) {
            fmt::println(file, "##### {} ############################\n", kingdomTitle->GetName());
            files[fileName].kingdomComment = true;
        }
        if (duchyTitle != nullptr && !files[fileName].duchyComment) {
            fmt::println(file, "### {}", duchyTitle->GetName());
            files[fileName].duchyComment = true;
        }
        if (countyTitle != nullptr && !files[fileName].countyComment) {
            fmt::println(file, "## {}", countyTitle->GetName());
            files[fileName].countyComment = true;
        }
                    
        SharedPtr<Jomini::Object> data = province->GetExtraHistoryData()->Copy();
        if(!province->GetCulture().empty()) data->Put("culture", province->GetCulture());
        if(!province->GetFaith().empty()) data->Put("religion", province->GetFaith());
        data->Put("holding", province->GetHolding().empty() ? "none" : province->GetHolding());
        for(const auto& [date, historyData] : province->GetHistory()) data->Put((std::string) date, historyData);

        SharedPtr<Jomini::Object> object = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
        object->Put(std::to_string(province->GetId()), data);

        fmt::println(file, "# {}", province->GetName());
        fmt::println(file, "{}", object->Serialize());
    }
}

////////////////////////////////////////////////////

void ProvinceManager::GenerateMissingProvinces() {
    // Loop through the province image and generate provinces for any color
    // that does not already have one.

    int count = 0;
    int nextId = 1;

    size_t width = m_ProvincesImage.getSize().x;
    size_t height = m_ProvincesImage.getSize().y;
    size_t pixels = width * height * 4;
    const uint8_t* provincesPixels = m_ProvincesImage.getPixelsPtr();

    size_t index = 0;
    uint32_t provinceColor = 0x000000FF;
    uint32_t previousProvinceColor = 0x00000000;

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
            if(!m_ProvincesByColors.contains(provinceColor)) {
                // Skip ids that are already taken by another province.
                while(m_ProvincesByIds.count(nextId) != 0)
                    nextId++;

                this->AddProvince(
                    MakeUnique<Province>(nextId, sf::Color(provinceColor), fmt::format("province_{}", nextId))
                );
                count++;
                nextId++;
            }
        }
        previousProvinceColor = provinceColor;
    }
    LOG_INFO("Generated {} new provinces based on the province image.", count);
}

void ProvinceManager::GenerateProvincesClimate(bool override, float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor, float mildWinterThreshold, float normalWinterThreshold, float severeWinterThreshold) {
    size_t countProvinces = 0;
    for(const auto& [provinceColorId, province] : m_ProvincesByIds) {
        float winterSeverityBias = province->CalculateWinterSeverityBias(*this, override, elevationOffset, elevationStrength, elevationFactor, hemisphereOffset, hemisphereSize, hemisphereStrength, hemisphereFactor);
        bool hasChanged = false;

        if (province->GetClimateType() == ClimateType::NONE || override) {
            if (winterSeverityBias >= severeWinterThreshold) province->SetClimateType(ClimateType::SEVERE_WINTER);
            else if (winterSeverityBias >= normalWinterThreshold) province->SetClimateType(ClimateType::NORMAL_WINTER);
            else if (winterSeverityBias >= mildWinterThreshold) province->SetClimateType(ClimateType::MILD_WINTER);
            else province->SetClimateType(ClimateType::NONE);
            hasChanged = true;
        }

        if (province->GetWinterSeverityBias().empty() || override) {
            province->SetWinterSeverityBias(fmt::format("{:.2f}", winterSeverityBias));
            hasChanged = true;
        }

        countProvinces += hasChanged;
    }

    LOG_INFO("Generated climate for {} provinces.", countProvinces);
}

void ProvinceManager::GenerateProvincesFlags(float waterLevel) {
    // Loop through the province image to count the number of
    // pixels that are below water level, in order to determine
    // if that province is a sea or land.

    // First size_t is the total number of pixels.
    // Second size_t is the number of pixels below water level.
    std::unordered_map<uint32_t, std::pair<size_t, size_t>> count;
    count.reserve(m_ProvincesByColors.size());

    size_t pixels = m_ProvincesImage.getSize().x * m_ProvincesImage.getSize().y * 4;
    const uint8_t* provincesPixels = m_ProvincesImage.getPixelsPtr();
    const uint8_t* heightmapPixels = m_HeightmapImage.getPixelsPtr();

    size_t index = 0;
    uint32_t provinceColor = 0x000000FF;
    uint32_t previousProvinceColor = 0x00000000;

    // Keep track of the last province iterator to avoid searching
    // for it on successive colors of the same province.
    auto it = count.begin();

    // Cast to edit directly the bytes of the color and pixels.
    // - colorPtr is used to read the color from the provinces map image.
    char* colorPtr = static_cast<char*>((void*) &provinceColor);

    while (index < pixels) {
        // Copy the four bytes corresponding to RGB from the provinces image pixels
        // to the array for the titles image.
        // The bytes need to be flipped, otherwise provinceColor would
        // be ABGR and we couldn't find the associated title color in the map.
        float elevation = ((float) heightmapPixels[index] / 255.f)*100.f;
        colorPtr[3] = provincesPixels[index++]; // R
        colorPtr[2] = provincesPixels[index++]; // G
        colorPtr[1] = provincesPixels[index++]; // B
        index++;
        
        if (previousProvinceColor != provinceColor) {
            it = count.find(provinceColor);
            if (it == count.end())
                it = count.insert({provinceColor, std::pair<size_t, size_t>(0, 0)}).first;
        }

        it->second.first++;
        it->second.second += (elevation >= waterLevel);

        previousProvinceColor = provinceColor;
    }

    // Assign the province flags depending on the ratio
    // of pixels below water level.
    for (const auto& [color, pair] : count) {
        auto it = m_ProvincesByColors.find(color);
        if (it == m_ProvincesByColors.end())
            continue;
        bool isLand = (pair.first <= 2*pair.second);
        it->second->SetFlags(ProvinceFlags::NONE);
        it->second->SetFlag(ProvinceFlags::LAND, isLand);
        it->second->SetFlag(ProvinceFlags::SEA, !isLand);
        it->second->SetTerrain(isLand ? m_DefaultLandTerrain : m_DefaultSeaTerrain);
    }
}

void ProvinceManager::GenerateRivers() {
    // - Map provinces colors to pink if sea, white otherwise.
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    constexpr uint32_t seaColor = sf::Color(255, 0, 128).toInteger();
    constexpr uint32_t landColor = sf::Color(255, 255, 255).toInteger();

    m_RiversImage = Image::MapPixels(m_ProvincesImage, [&](auto& mappedColors){
        for(const auto& [provinceColorId, province] : m_ProvincesByIds) {
            mappedColors[province->GetColor().toInteger()] = (province->HasFlag(ProvinceFlags::SEA)) ? seaColor : landColor;
        }    
    });

    std::string filePath = m_Mod.GetAbsolutePath(Paths::MAP_DATA_RIVERS);
    std::ignore = m_RiversImage.saveToFile(filePath);
    Image::IndexImage(filePath, Image::RIVERS_PALETTE);
}