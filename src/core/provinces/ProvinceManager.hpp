#pragma once

#include "provinces/Province.hpp"

class ProvinceManager {
public:
    ProvinceManager(Mod& mod);
    ProvinceManager(const ProvinceManager&) = delete;
    ProvinceManager& operator=(const ProvinceManager&) = delete;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of provinces.
     * @return The total number of provinces.
     */
    size_t CountProvinces() const;

    /**
     * @brief Checks if a province exists.
     * @param color The color of the province to search for.
     * @return True if the province exists, false otherwise.
     */
    bool HasProvinceByColor(uint32_t color) const;
    
    /**
     * @brief Checks if a province exists.
     * @param id The id of the province to search for.
     * @return True if the province exists, false otherwise.
     */
    bool HasProvinceById(int id) const;

    /**
     * @brief Checks if a holding type exists.
     * @param name The name of the holding type to search for.
     * @return True if the holding type exists, false otherwise.
     */
    bool HasHoldingType(const std::string& name) const;

    /**
     * @brief Checks if a terrain type exists.
     * @param name The name of the terrain type to search for.
     * @return True if the terrain type exists, false otherwise.
     */
    bool HasTerrainType(const std::string& name) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves the provinces image.
     * @return The reference to the provinces image.
     */
    sf::Image& GetProvincesImage();

    /**
     * @brief Retrieves the provinces image.
     * @return The reference to the provinces image.
     */
    const sf::Image& GetProvincesImage() const;
    
    /**
     * @brief Retrieves the heightmap image.
     * @return The reference to the heightmap image.
     */
    sf::Image& GetHeightmapImage();

    /**
     * @brief Retrieves the heightmap image.
     * @return The reference to the heightmap image.
     */
    const sf::Image& GetHeightmapImage() const;
    
    /**
     * @brief Retrieves the rivers image.
     * @return The reference to the rivers image.
     */
    sf::Image& GetRiversImage();

    /**
     * @brief Retrieves the rivers image.
     * @return The reference to the rivers image.
     */
    const sf::Image& GetRiversImage() const;

    /**
     * @brief Generates the province flags image.
     *        SEA provinces are colored in blue, LAND provinces are colored in green, and RIVERS/LAKES are colored in shades of blue.
     * @note  This function can be expensive: to be used with sparingly.
     * @return The generated flags image.
     */
    sf::Image GetFlagsImage() const;
    
    /**
     * @brief Generates the terrain image.
     *        Each province's color is replaced by its terrain color.
     * @note  This function can be expensive: to be used with sparingly.
     * @return The generated terrain image.
     */
    sf::Image GetTerrainImage() const;

    /**
     * @brief Generates the climate image.
     *        Each province's color is replaced by a shade of gray corresponding to its climate type.
     *        The whiter, the more severe the winter.
     * @note  This function can be expensive: to be used with sparingly.
     * @return The generated climate image.
     */
    sf::Image GetClimateImage() const;

    /**
     * @brief Generates the winter severity bias image.
     *        Each province's color is replaced by a shade of gray corresponding to its winter severity.
     * @note  This function can be expensive: to be used with sparingly.
     * @return The generated winter severity bias image.
     */
    sf::Image GetWinterSeverityBiasImage() const;

    /**
     * @brief Retrieves a province by its color.
     * @param color The color of the province to retrieve.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    Province* GetProvinceByColor(uint32_t color);

    /**
     * @brief Retrieves a province by its color.
     * @param color The color of the province to retrieve.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    const Province* GetProvinceByColor(uint32_t color) const;

    /**
     * @brief Retrieves a province by its id.
     * @param id The id of the province to retrieve.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    Province* GetProvinceById(int id);
    
    /**
     * @brief Retrieves a province by its id.
     * @param id The id of the province to retrieve.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    const Province* GetProvinceById(int id) const;

    /**
     * @brief Retrieves a province by its pixel position on the map.
     * @param x The x-coordinate of the pixel.
     * @param y The y-coordinate of the pixel.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    Province* GetProvinceByPixel(int x, int y);
    
    /**
     * @brief Retrieves a province by its pixel position on the map.
     * @param x The x-coordinate of the pixel.
     * @param y The y-coordinate of the pixel.
     * @return The pointer to the province if it exists, nullptr otherwise.
     */
    const Province* GetProvinceByPixel(int x, int y) const;

    /**
     * @brief Retrieves the maximum province id.
     * @return The maximum province id if there are provinces, std::nullopt otherwise.
     */
    std::optional<int> GetMaxProvinceId() const;

    /**
     * @brief Retrieves a map of provinces paired by their colors.
     * @return The map of provinces paired by their colors.
     */
    std::unordered_map<uint32_t, UniquePtr<Province>>& GetProvincesByColors();
    
    /**
     * @brief Retrieves a map of provinces paired by their colors.
     * @return The map of provinces paired by their colors.
     */
    const std::unordered_map<uint32_t, UniquePtr<Province>>& GetProvincesByColors() const;

    /**
     * @brief Retrieves a map of provinces paired by their ids.
     * @return The map of provinces paired by their ids.
     */
    std::map<int, Province*>& GetProvincesByIds();
    
    /**
     * @brief Retrieves a map of provinces paired by their ids.
     * @return The map of provinces paired by their ids.
     */
    const std::map<int, Province*>& GetProvincesByIds() const;

    /**
     * @brief Retrieves the list of holding types.
     * @return The holding types paired by their name.
     */
    OrderedMap<std::string, HoldingType>& GetHoldingTypes();

    /**
     * @brief Retrieves the list of holding types.
     * @return The holding types paired by their name.
     */
    const OrderedMap<std::string, HoldingType>& GetHoldingTypes() const;

    /**
     * @brief Retrieves the list of terrain types.
     * @return The terrain types paired by their name.
     */
    OrderedMap<std::string, TerrainType>& GetTerrainTypes();

    /**
     * @brief Retrieves the list of terrain types.
     * @return The terrain types paired by their name.
     */
    const OrderedMap<std::string, TerrainType>& GetTerrainTypes() const;

    /**
     * @brief Retrieves the default land terrain.
     * @return The default land terrain.
     */
    const std::string& GetDefaultLandTerrain() const;

    /**
    * @brief Retrieves the default sea terrain.
    * @return The default sea terrain.
    */
    const std::string& GetDefaultSeaTerrain() const;

    /**
     * @brief Retrieves the default coastal sea terrain.
     * @return The default coastal sea terrain.
     */
    const std::string& GetDefaultCoastalSeaTerrain() const;

    /**
     * @brief Retrieves the file name of the province terrain types (e.g common/province_terrain/00_province_terrain.txt)
     * @return The file name.
     */
    const std::string& GetProvinceTerrainFileName() const;
    
    /**
     * @brief Retrieves the file name of the province terrain types (e.g common/provinces_terrain/01_province_properties.txt)
     * @return The file name.
     */
    const std::string& GetProvinceTerrainPropertiesFileName() const;

    /**
     * @brief Retrieves the constants that were in the original provinces history files.
     * @return A map of constants.
     */
    const std::map<std::string, SharedPtr<Jomini::Object>>& GetProvincesHistoryVariables() const;

    /**
     * @brief Retrieves the constants defined in the terrain properties file (common/provinces_terrain/01_province_properties.txt).
     * @return The terrain properties variables.
     */
    const SharedPtr<Jomini::Object>& GetProvinceTerrainPropertiesVariables() const;
    
    /**
     * @brief Retrieves the constants defined in the terrain types file (common/terrain_types/00_terrains.txt).
     * @return The terrain types variables.
     */
    const SharedPtr<Jomini::Object>& GetTerrainTypesVariables() const;

    /**
     * @brief Retrieves the list of vanilla override files in common/province_terrain
     * @return A list of relative file paths.
     */
    const std::map<std::string, std::string>& GetVanillaOverrideProvinceTerrainFiles() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new province.
     * @param province The unique pointer of the province.
     * @note If a province with the same id or color already exists, it will be overwritten with the new one.
     */
    void AddProvince(UniquePtr<Province> province);

    /**
     * @brief Removes a province.
     * @param color The color of the province to remove.
     */
    void RemoveProvinceByColor(uint32_t color);
    
    /**
     * @brief Removes a province.
     * @param id The id of the province to remove.
     * @note `RemoveProvinceByColor()` is slightly faster since it only requires one lookup.
     */
    void RemoveProvinceById(int id);

    /**
     * @brief Removes a province.
     * @param province The province to remove.
     */
    void RemoveProvince(const Province* province);

    /**
     * @brief Renames a province's color.
     * @param formerColor The former and current color of the province.
     * @param newColor The color the province will be renamed to.
     */
    void RenameProvinceColor(uint32_t formerColor, uint32_t newColor);
    
    /**
     * @brief Renames a province's id.
     * @param formerId The former and current id of the province.
     * @param newId The id the province will be renamed to.
     */
    void RenameProvinceId(int formerId, int newId);

    //////////////////////////////////////////////////////

    /**
     * @brief Loads the holding types from the definition files.
     */
    void LoadHoldingTypes();

    /**
     * @brief Loads the terrain types from the definition files.
     */
    void LoadTerrainTypes();

    /**
     * @brief Loads the provinces from the definition.csv file.
     * @throws std::runtime_error if the file cannot be read or if the data is invalid.
     */
    void LoadProvincesDefinition();

    /**
     * @brief Loads the provinces image from the mod files and checks if the colors
     *        in the image match the provinces loaded from the definition file.
     * @throws std::runtime_error if the file doesn't exist or cannot be opened as an image.
     */
    void LoadProvincesImage();
    
    /**
     * @brief Loads the heightmap image from the mod files.
     * @throws std::runtime_error if the file doesn't exist or cannot be opened as an image.
     */
    void LoadHeightmapImage();
    
    /**
     * @brief Loads the rivers image from the mod files.
     * @throws std::runtime_error if the file doesn't exist or cannot be opened as an image.
     */
    void LoadRiversImage();

    /**
     * @brief Loads provinces data from the default map file (map_data/default.map).
     * @throws std::runtime_error if the default map file cannot be read or if the data is invalid.
     */
    void LoadDefaultMapFile();

    /**
     * @brief Loads provinces terrain from all the files in common/province_terrain/.
     */
    void LoadProvincesTerrain();
    
    /**
     * @brief Loads provinces terrain from the a specified file (e.g common/province_terrain/00_province_terrain.txt).
     */
    void LoadProvincesTerrainFile(const std::string& fileName, SharedPtr<Jomini::Object> data);

    /**
     * @brief Loads provinces climate from the province properties file (common/province_terrain/01_province_properties.txt)
     *        and from the climate file (map_data/climate.txt).
     */
    void LoadProvincesClimate();

    /**
     * @brief Loads provinces terrain from the a specified file (e.g common/province_terrain/01_province_properties.txt).
     */
    void LoadProvincesTerrainPropertiesFile(const std::string& fileName, SharedPtr<Jomini::Object> data);

    /**
     * @brief Loads provinces history from the history files.
     */
    void LoadProvincesHistory();

    /**
     * @brief Load the provinces history of a specific file.
     * @param fileName The relative path to the history file.
     * @param data The parsed data of the history file.
     */
    void LoadProvincesHistoryFile(const std::string& fileName, SharedPtr<Jomini::Object> data);

    ////////////////////////////////////////////////////
    
    /**
     * @brief Exports the provinces definition to the definition.csv file.
     * @throws std::runtime_error if the file cannot be written.
     */
    void ExportProvincesDefinition();

    /**
     * @brief Exports the provinces flags to the default.map file.
     * @throws std::runtime_error if the file cannot be written.
	 */
    void ExportDefaultMapFile();

    /**
     * @brief Exports the provinces terrain to the province terrain file (common/terrain_types/00_province_terrain.txt).
     * @throws std::runtime_error if the file cannot be written.
     */
    void ExportProvincesTerrain();

    /**
     * @brief Exports the provinces climate to the province properties file (common/terrain_types/01_province_properties.txt).
     * @throws std::runtime_error if the file cannot be written.
     */
    void ExportProvincesClimate();

    /**
     * @brief Exports the provinces history to the history files.
     * @param titleManager A reference to the mod's title manager.
     * @throws std::runtime_error if any of the files cannot be written.
     */
    void ExportProvincesHistory(TitleManager& titleManager);

    ////////////////////////////////////////////////////

    /**
     * @brief Generates missing provinces for passable land provinces that don't have a province yet.
     */
    void GenerateMissingProvinces();

    /**
     * @brief Generates the climate of the provinces based on their latitude and elevation.
     */
    void GenerateProvincesClimate(bool override, float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor, float mildWinterThreshold, float normalWinterThreshold, float severeWinterThreshold);

    /**
     * @brief Determines flags (land or sea) for the provinces depending on their elevation on the heightmap and the water level.
     * @param waterLevel The water level.
     */
    void GenerateProvincesFlags(float waterLevel);

    /**
     * @brief Generates a blank rivers image using the landmass of the provinces.
     * @note This create an image file, and any existing rivers image will be overwritten.
     */
    void GenerateRivers();

private:
    Mod& m_Mod;

    sf::Image m_ProvincesImage; // map_data/provinces.png
    sf::Image m_HeightmapImage; // map_data/heightmap.png
    sf::Image m_RiversImage; // map_data/rivers.png

    std::unordered_map<uint32_t, UniquePtr<Province>> m_ProvincesByColors;
    std::map<int, Province*> m_ProvincesByIds;

    OrderedMap<std::string, HoldingType> m_HoldingTypes; // common/holdings/
    OrderedMap<std::string, TerrainType> m_TerrainTypes; // common/terrain_types/

    std::string m_DefaultLandTerrain;
    std::string m_DefaultSeaTerrain;
    std::string m_DefaultCoastalSeaTerrain;
    std::string m_ProvinceTerrainFileName; // common/provinces_terrain/00_province_terrain.txt
    std::string m_ProvinceTerrainPropertiesFileName; // common/provinces_terrain/01_province_properties.txt

    // Map variables with their respective filename.
    std::map<std::string, SharedPtr<Jomini::Object>> m_ProvincesHistoryVariables; // history/provinces/
    SharedPtr<Jomini::Object> m_ProvinceTerrainPropertiesVariables; // common/provinces_terrain/01_province_properties.txt
    SharedPtr<Jomini::Object> m_TerrainTypesVariables; // common/terrain_types/00_terrains.txt

    // List of empty files in common/province_terrain that override vanilla files.
    std::map<std::string, std::string> m_VanillaOverrideProvinceTerrainFiles;
};