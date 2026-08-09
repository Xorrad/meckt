#include "RegionManager.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"

#include <fmt/ostream.h>

RegionManager::RegionManager(Mod& mod) : m_Mod(mod) {}

//////////////////////////////////////////////////////

size_t RegionManager::CountRegions() const {
    return m_Regions.size();
}

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

const std::map<std::string, std::string>& RegionManager::GetVanillaOverrideFiles() const {
    return m_VanillaOverrideFiles;
}

//////////////////////////////////////////////////////

void RegionManager::AddRegion(UniquePtr<Region> region) {
    m_Regions[region->GetName()] = std::move(region);
}

void RegionManager::RemoveRegion(Region* region) {
    if (region == nullptr)
        return;

    // Remove the region from all regions that contain it, otherwise there will be dangling pointers.
    for (auto& [_, otherRegion] : m_Regions) {
        otherRegion->RemoveRegion(region);
    }

    m_Regions.erase(region->GetName());
}

void RegionManager::RemoveRegion(const std::string& name) {
    auto it = m_Regions.find(name);
    if (it == m_Regions.end())
        return;
    this->RemoveRegion(it->second.get());
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

            if (data->GetMap().empty()) {
                std::ifstream overrideFile = std::ifstream(filePath, std::ios::binary);
                m_VanillaOverrideFiles[fileName] = File::ReadString(overrideFile);
                continue;
            }

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

        std::vector<std::string> kingdoms = regionData->Get("kingdoms")->AsArray<std::string>({});
        std::vector<std::string> duchies = regionData->Get("duchies")->AsArray<std::string>({});
        std::vector<std::string> counties = regionData->Get("counties")->AsArray<std::string>({});
        std::vector<std::string> provinces = regionData->Get("provinces")->AsArray<std::string>({});
        std::vector<std::string> regions = regionData->Get("regions")->AsArray<std::string>({});
        bool generateModifiers = regionData->GetFirst("generate_modifiers")->As<bool>(false);
        bool shouldRememberCountiesOrder = regionData->GetFirst("should_remember_counties_order")->As<bool>(false);
        
        if (!m_Regions.contains(regionName)) {
            this->AddRegion(MakeUnique<Region>(regionName, ""));
        }

        Region* region = m_Regions[regionName].get();
        region->SetGenerateModifiers(generateModifiers);
        region->SetShouldRememberCountiesOrder(shouldRememberCountiesOrder);
        region->SetFileName(fileName);

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

void RegionManager::ExportGeographicalRegions() {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Determine geographical regions order based on their dependencies.
    // We use a topological sort to solve that problem.
    std::vector<Region*> regions;
    for (const auto& [_, region] : m_Regions)
        regions.push_back(region.get());

    std::unordered_map<Region*, int> indegree;
    std::unordered_map<Region*, std::vector<Region*>> graph;

    // Build a graph of the regions.
    for (const auto& [_, a] : m_Regions) {
        indegree[a.get()] = 0;
        for (const auto& [_, b] : m_Regions) {
            // If region A contains region B, then A is dependant on B, and B must come first.
            if (a != b && a->HasRegion(b.get())) {
                graph[b.get()].push_back(a.get());
                indegree[a.get()]++;
            }
        }
    }

    // Queue of regions with no dependencies.
    std::queue<Region*> queue;
    for (const auto& [node, deg] : indegree) {
        if (deg == 0) queue.push(node);
    }

    std::vector<Region*> sortedRegions;
    std::unordered_set<Region*> visited;
    while (!queue.empty()) {
        auto region = queue.front();
        queue.pop();
        visited.insert(region);
        sortedRegions.push_back(region);
        for (auto next : graph[region]) {
            if (--indegree[next] == 0) {
                queue.push(next);
            }
        }
    }

    // Check if there are cycles and add any remaining regions to the end of the list.
    for (const auto& [_, region] : m_Regions) {
        if (!visited.count(region.get())) {
            LOG_ERROR("Geographical region '{}' has a cycle", region->GetName());
            sortedRegions.push_back(region.get());
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    
    // Create the directories and files.
    std::string dir = m_Mod.GetDirectory( Paths::MAP_DATA_GEOGRAPHICAL_REGIONS );
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::map<std::string, std::ofstream> files;

    for(Region* region : sortedRegions) {
        // Determine the region's file name.
        if(region->GetFileName().empty())
            region->ResetFileName();
        std::string fileName = region->GetFileName();
        
        // Initialize the region's file.
        if(files.count(fileName) == 0) {
            files[fileName] = std::ofstream(m_Mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, fileName), std::ios::binary);
            if (!files[fileName].is_open()) {
                LOG_ERROR("RegionManager::ExportGeographicalRegions: Failed to open file '{}' for writing", dir + "/geographical_region.txt");
                continue;
            }
            File::EncodeToUTF8BOM(files[fileName]);
        }

        std::ofstream& file = files[fileName];
        this->ExportGeographicalRegion(region, file);
    }

    for(auto& [key, file] : files)
        file.close();

    // Create empty files for the vanilla overrides.
    for (const auto& [fileName, fileContent] : m_VanillaOverrideFiles) {
        std::ofstream file = std::ofstream(m_Mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, fileName), std::ios::binary);
        // File::EncodeToUTF8BOM(file); // Encoding bytes are present alongside the file content.
        fmt::print(file, "{}", fileContent);
    }
}

void RegionManager::ExportGeographicalRegion(Region* region, std::ofstream& file) {
    SharedPtr<Jomini::Object> regionObject = MakeShared<Jomini::Object>(Jomini::ObjectMap{});

    if (region->DoesGenerateModifiers())
        regionObject->Put("generate_modifiers", region->DoesGenerateModifiers());
        
    if (region->ShouldRememberCountiesOrder())
        regionObject->Put("should_remember_counties_order", region->ShouldRememberCountiesOrder());

    if (!region->GetKingdoms().empty()) {
        std::vector<std::string> titles;
        titles.reserve(region->GetKingdoms().size());
        for (auto title : region->GetKingdoms())
            titles.push_back(title->GetName());
        regionObject->Put("kingdoms", titles);
    }

    if (!region->GetDuchies().empty()) {
        std::vector<std::string> titles;
        titles.reserve(region->GetDuchies().size());
        for (auto title : region->GetDuchies())
            titles.push_back(title->GetName());
        regionObject->Put("duchies", titles);
    }

    if (!region->GetCounties().empty()) {
        std::vector<std::string> titles;
        titles.reserve(region->GetCounties().size());
        for (auto title : region->GetCounties())
            titles.push_back(title->GetName());
        regionObject->Put("counties", titles);
    }

    if (!region->GetProvinces().empty()) {
        std::vector<std::string> provinces;
        provinces.reserve(region->GetProvinces().size());
        for (auto province : region->GetProvinces())
            provinces.push_back(std::to_string(province->GetId()));
        regionObject->Put("provinces", provinces);
    }

    if (!region->GetRegions().empty()) {
        std::vector<std::string> subRegions;
        subRegions.reserve(region->GetRegions().size());
        for (auto subRegion : region->GetRegions())
            subRegions.push_back(subRegion->GetName());
        regionObject->Put("regions", subRegions);
    }

    SharedPtr<Jomini::Object> object = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
    object->Put(region->GetName(), regionObject);

    fmt::println(file, "{}\n", object->Serialize());
}