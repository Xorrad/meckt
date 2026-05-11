#include "ProvinceManager.hpp"

#include "mod/Mod.hpp"

#include <fmt/ostream.h>

ProvinceManager::ProvinceManager(Mod& mod) :
    m_Mod(mod),
    m_ProvincesImage(sf::Image()),
    m_ProvincesHistoryVariables({}),
    m_TerrainPropertiesVariables(MakeShared<Jomini::Object>(Jomini::Type::OBJECT)),
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

const std::map<std::string, SharedPtr<Jomini::Object>>& ProvinceManager::GetProvincesHistoryVariables() const {
    return m_ProvincesHistoryVariables;
}

const SharedPtr<Jomini::Object>& ProvinceManager::GetTerrainPropertiesVariables() const {
    return m_TerrainPropertiesVariables;
}

const SharedPtr<Jomini::Object>& ProvinceManager::GetTerrainTypesVariables() const {
    return m_TerrainTypesVariables;
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
        TerrainType("drylands", sf::Color(220, 45, 120))
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
        if (!file)
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesDefinition: Failed to create an empty definition file at \"{}\"", filePath));
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
}

void ProvinceManager::LoadProvincesTerrain() {
    std::string filePath = m_Mod.GetAbsolutePath(Paths::COMMON_PROVINCE_TERRAIN);

    if (!std::filesystem::exists(filePath))
        return;

    SharedPtr<Jomini::Object> result = nullptr;
    try {
        result = Jomini::ParseFile(filePath);
    }
    catch (std::exception& e) {
        throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesTerrain: Failed to parse province terrain file at '{}': {}", filePath, e.what()));
    }

    m_DefaultLandTerrain = result->Get("default_land")->As<std::string>("plains");
    m_DefaultSeaTerrain = result->Get("default_sea")->As<std::string>("sea");
    m_DefaultCoastalSeaTerrain = result->Get("default_coastal_sea")->As<std::string>("sea");

    // Set default terrain for all provinces (especially for those without any in files).
    for(const auto& [colorId, province] : m_ProvincesByColors) {
        std::string defaultTerrain = m_DefaultLandTerrain;
        if(province->HasFlag(ProvinceFlags::SEA))
            defaultTerrain = (province->HasFlag(ProvinceFlags::COASTAL) ? m_DefaultCoastalSeaTerrain : m_DefaultSeaTerrain);
        province->SetTerrain(defaultTerrain);
    }

    for(const auto& [key, pair] : result->GetMap()) {
        const auto& [op, value] = pair;
        int provinceId = 0;
        std::string terrain = "";

        try {
            provinceId = String::ParseInt(key);
        }
        catch (const std::exception& e) {
            LOG_WARNING("Invalid province id '{}' in '{}': {}", key, filePath, e.what());
            continue;
        }

        // If the province id has been assigned several terrain type then we only pick the first one.
        if(!value->Is(Jomini::Type::SCALAR)) {
            terrain = value->AsArray<std::string>().front();
            LOG_WARNING("Province assigned several terrain types: '{}'", provinceId);
        }
        else {
            terrain = value->As<std::string>();
        }

        if(m_ProvincesByIds.count(provinceId) == 0) {
            LOG_WARNING("Terrain type assigned to undefined province: '{}'", provinceId);
            continue;
        }

        if(!m_TerrainTypes.contains(terrain)) {
            LOG_WARNING("Undefined terrain type '{}' assigned to province '{}'", terrain, provinceId);
            continue;
        }

        m_ProvincesByIds[provinceId]->SetTerrain(terrain);

        if(!m_ProvincesByIds[provinceId]->HasFlag(ProvinceFlags::SEA))
            m_ProvincesByIds[provinceId]->SetFlag(ProvinceFlags::LAND, true);
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
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesClimate: Failed to parse climate file at '{}':\n{}", climateFile, e.what()));
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
    std::string propertiesFile = m_Mod.GetAbsolutePath(Paths::COMMON_PROVINCE_PROPERTIES);

    if (std::filesystem::exists(propertiesFile)) {
        m_TerrainPropertiesVariables = MakeShared<Jomini::Object>(Jomini::Type::OBJECT);

        SharedPtr<Jomini::Object> result = nullptr;
        try {
            result = Jomini::ParseFile(propertiesFile);
        }
        catch(const std::exception& e) {
            throw std::runtime_error(fmt::format("ProvinceManager::LoadProvincesClimate: Failed to parse properties file at '{}':\n{}", propertiesFile, e.what()));
        }

        for(const auto& [key, pair] : result->GetMap()) {
            // Save and ignore variables to only keep province ids.
            if (key.starts_with("@")) {
                m_TerrainPropertiesVariables->Put(key, pair.second);
                continue;
            }

            const auto& [op, value] = pair;
            int provinceId = String::ParseInt(key);

            if (m_ProvincesByIds.count(provinceId) == 0) {
                LOG_WARNING("Climate properties assigned to undefined province '{}' in '{}'", provinceId, propertiesFile);
                continue;
            }

            if (!value->Is(Jomini::Type::OBJECT)) {
                LOG_ERROR("Invalid climate properties for province '{}' in '{}'", provinceId, propertiesFile);
                continue;
            }

            if (value->Contains("winter_severity_bias")) {
                const auto winterSeverityBias = value->Get("winter_severity_bias");
                if (!winterSeverityBias->Is(Jomini::Type::SCALAR)) {
                    LOG_ERROR("Invalid 'winter_severity_bias' value for province '{}'", provinceId);
                    continue;
                }
                m_ProvincesByIds[provinceId]->SetWinterSeverityBias(winterSeverityBias->As<std::string>());
            }
            
            if (value->Contains("mild_winter_factor_override")) {
                const auto factor = value->Get("mild_winter_factor_override");
                if (!factor->Is(Jomini::Type::SCALAR)) {
                    LOG_ERROR("Invalid 'mild_winter_factor_override' value for province '{}'", provinceId);
                    continue;
                }
                m_ProvincesByIds[provinceId]->SetMildWinterFactorOverride(factor->As<std::string>());
            }
            if (value->Contains("normal_winter_factor_override")) {
                const auto factor = value->Get("normal_winter_factor_override");
                if (!factor->Is(Jomini::Type::SCALAR)) {
                    LOG_ERROR("Invalid 'normal_winter_factor_override' value for province '{}'", provinceId);
                    continue;
                }
                m_ProvincesByIds[provinceId]->SetNormalWinterFactorOverride(factor->As<std::string>());
            }
            if (value->Contains("harsh_winter_factor_override")) {
                const auto factor = value->Get("harsh_winter_factor_override");
                if (!factor->Is(Jomini::Type::SCALAR)) {
                    LOG_ERROR("Invalid 'harsh_winter_factor_override' value for province '{}'", provinceId);
                    continue;
                }
                m_ProvincesByIds[provinceId]->SetHarshWinterFactorOverride(factor->As<std::string>());
            }
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
            m_ProvincesByIds[provinceId]->SetReligion(provinceValue->GetFirst("religion")->As<std::string>(""));
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

        m_ProvincesByIds[provinceId]->SetOriginalHistoryFilePath(fileName);
        m_ProvincesByIds[provinceId]->SetExtraHistoryData(extraHistoryData);
    }
}