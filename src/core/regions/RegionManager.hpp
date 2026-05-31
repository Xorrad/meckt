#pragma once

#include "regions/Region.hpp"

class RegionManager {
public:
    RegionManager(Mod& mod);
    RegionManager(const RegionManager&) = delete;
    RegionManager& operator=(const RegionManager&) = delete;

    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of regions.
     * @return The total number of regions.
     */
    size_t CountRegions() const;

    /**
     * @brief Checks if a region exists.
     * @param name The name of the region to search for.
     * @return True if the region exists, false otherwise.
     */
    bool HasRegion(const std::string& name) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Gets a region by its name.
     * @param name The name of the region.
     * @return The pointer to the region if it exists, nullptr otherwise.
     */
    Region* GetRegion(const std::string& name);

    /**
     * @brief Retrieves all regions.
     * @return A reference to the map of regions.
     */
    std::map<std::string, UniquePtr<Region>>& GetRegions();

    /**
     * @brief Retrieves all regions.
     * @return A const reference to the map of regions.
     */
    const std::map<std::string, UniquePtr<Region>>& GetRegions() const;

    //////////////////////////////////////////////////////
    
    /**
     * @brief Adds a new region.
     * @param region The unique pointer of the region.
     * @note If a region with the same name already exists, it will be overwritten with the new one.
     */
    void AddRegion(UniquePtr<Region> region);
    
    /**
     * @brief Removes a region.
     * @param region The region to remove.
     */
    void RemoveRegion(Region* region);
    
    /**
     * @brief Removes a region.
     * @param name The name of the region to remove.
     */
    void RemoveRegion(const std::string& name);
    
    /**
     * @brief Renames a region.
     * @param formerName The former and current name of the region.
     * @param newName The name the region will be renamed to.
     */
    void RenameRegion(const std::string& formerName, const std::string& newName);
    
    //////////////////////////////////////////////////////
    
    /**
     * @brief Loads all geographical regions.
     * @param provinceManager The province manager to get provinces from.
     * @param titleManager The title manager to get titles from.
     */
    void LoadGeographicalRegions(ProvinceManager* provinceManager, TitleManager* titleManager);

    /**
     * @brief Loads a geographical region file.
     * @param fileName The relative path to the region definition file.
     * @param data The parsed data from the file.
     * @param provinceManager The province manager to get provinces from.
     * @param titleManager The title manager to get titles from.
     */
    void LoadGeographicalRegionFile(const std::string& fileName, SharedPtr<Jomini::Object> data, ProvinceManager* provinceManager, TitleManager* titleManager);
    
    //////////////////////////////////////////////////////

    /**
     * @brief Exports all geographical regions.
     * @throws std::runtime_error if the file cannot be opened for writing.
     */
    void ExportGeographicalRegions();
    
private:
    Mod& m_Mod;

    std::map<std::string, UniquePtr<Region>> m_Regions;
};