#include "RegionManager.hpp"

#include "mod/Mod.hpp"
#include "map/provinces/ProvinceManager.hpp"
#include "map/titles/TitleManager.hpp"

RegionManager::RegionManager(Mod& mod) : m_Mod(mod) {}

//////////////////////////////////////////////////////

bool RegionManager::HasRegion(const std::string& name) const {
    return m_Regions.find(name) != m_Regions.end();
}

//////////////////////////////////////////////////////

Mod& RegionManager::GetMod() {
    return m_Mod;
}

Region* RegionManager::GetRegion(const std::string& name) {
    auto it = m_Regions.find(name);
    if (it == m_Regions.end())
        return nullptr;
    return it->second.get();
}

std::map<std::string, UniquePtr<Region>>& RegionManager::GetRegions() {
    return m_Regions;
}

const std::map<std::string, UniquePtr<Region>>& RegionManager::GetRegions() const {
    return m_Regions;
}

//////////////////////////////////////////////////////

void RegionManager::AddRegion(UniquePtr<Region> region) {
    m_Regions[region->GetName()] = std::move(region);
}

void RegionManager::RemoveRegion(Region* region) {
    if (region == nullptr)
        return;
    this->RemoveRegion(region->GetName());
}

void RegionManager::RemoveRegion(const std::string& name) {
    m_Regions.erase(name);
}

void RegionManager::RenameRegion(const std::string& formerName, const std::string& newName) {
    auto it = m_Regions.find(formerName);
    if (it == m_Regions.end())
		return;
    if (m_Regions.contains(newName))
        throw std::invalid_argument(std::format("RegionManager::RenameRegion: couldn't rename region '{}' to '{}' because it is already used by another region", formerName, newName));
    
    Region* region = it->second.get();

    // Change the current name of the region.
    region->SetName(newName);

    // Change the key in the regions hashmap.
    auto nodeHandler = m_Regions.extract(it);
    nodeHandler.key() = newName;
    m_Regions.insert(std::move(nodeHandler));
}

void RegionManager::LoadGeographicalRegions(ProvinceManager& provinceManager, TitleManager& titleManager) {
    m_Regions.clear();

    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS) );

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, filePath);
            this->LoadGeographicalRegionFile(fileName, data, provinceManager, titleManager);
        }
        catch (std::exception& e) {
            LOG_ERROR("Failed to parse geographical region file '{}': {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} geographical regions from {} files", m_Regions.size(), filesPath.size());
}

void RegionManager::LoadGeographicalRegionFile(const std::string& fileName, SharedPtr<Jomini::Object> data, ProvinceManager& provinceManager, TitleManager& titleManager) {
    for(auto& [regionName, regionPair] : data->GetMap()) {
        auto& [_, regionData] = regionPair;

        // Merge all objects into a single one when there are duplicate definitions for the same region.
        if (regionData->Is(Jomini::Type::ARRAY))
            regionData = regionData->Flatten(false);

        if (!regionData->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Failed to load geographical region '{}' in {}: expected an object, got {}", regionName, fileName, regionData->Serialize(0, true, true));
            continue;
        }

        std::vector<std::string> kingdoms = regionData->GetFirst("kingdoms")->AsArray<std::string>({});
        std::vector<std::string> duchies = regionData->GetFirst("duchies")->AsArray<std::string>({});
        std::vector<std::string> counties = regionData->GetFirst("counties")->AsArray<std::string>({});
        std::vector<std::string> provinces = regionData->GetFirst("provinces")->AsArray<std::string>({});
        std::vector<std::string> regions = regionData->GetFirst("regions")->AsArray<std::string>({});
        bool generateModifiers = regionData->GetFirst("generate_modifiers")->As<bool>(false);
        bool shouldRememberCountiesOrder = regionData->GetFirst("should_remember_counties_order")->As<bool>(false);
        
        if (!m_Regions.contains(regionName)) {
            this->AddRegion(MakeUnique<Region>(regionName));
        }

        Region* region = m_Regions[regionName].get();
        region->SetGenerateModifiers(generateModifiers);
        region->SetShouldRememberCountiesOrder(shouldRememberCountiesOrder);

        // Add kingdom, duchy and county titles.
        const auto AddTitles = [&](auto titles) {
            for (auto& title : titles) {
                if (!titleManager.HasTitle(title)) {
                    LOG_ERROR("Geographical region '{}' has unknown title '{}'", regionName, title);
                    continue;
                }
                region->AddTitle(titleManager.GetTitle(title));
            }
        };
        AddTitles(kingdoms);
        AddTitles(duchies);
        AddTitles(counties);

        // Add provinces.
        for (std::string province : provinces) {
            if (!Math::IsInt(province)) {
                LOG_ERROR("Geographical region '{}' has invalid province id '{}'", regionName, province);
                continue;
            }
            int provinceId = String::ParseInt(province);
            if (!provinceManager.HasProvinceById(provinceId)) {
                LOG_ERROR("Geographical region '{}' has unknown province '{}'", regionName, provinceId);
                continue;
            }
            region->AddProvince(provinceManager.GetProvinceById(provinceId));
        }
        
        // Add regions.
        for (std::string otherRegionName : regions) {
            if (!m_Regions.contains(otherRegionName)) {
                LOG_WARNING("Geographical region '{}' has unknown region '{}'. This is probably due to a wrong definition order and SHOULD be fixed automatically.", regionName, otherRegionName);
                this->AddRegion(MakeUnique<Region>(otherRegionName));
            }
            region->AddRegion(m_Regions[otherRegionName].get());
        }
    }
}