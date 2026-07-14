#pragma once

class Region {
public:
    
    /**
     * @brief Constructs a new Region with the specified name.
     * @param name The name of the region.
     */
    Region(std::string name);

    //////////////////////////////////////////////////////

    /**
     * @brief Checks if the region has the specified title.
     * @param title The title to check.
     * @return True if the region has the title, false otherwise.
     */
    bool HasTitle(Title* title) const;

    /**
     * @brief Checks if the region has the specified province.
     * @param province The province to check.
     * @return True if the region has the province, false otherwise.
     */
    bool HasProvince(Province* province) const;

    /**
     * @brief Checks if the region has the specified sub-region.
     * @param region The sub-region to check.
     * @return True if the region has the sub-region, false otherwise.
     */
    bool HasRegion(Region* region) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the region.
     * @return The name of the region.
     */
    std::string GetName() const;

    /**
     * @brief Gets the kingdoms in the region.
     * @return A span of pointers to the kingdoms.
     */
    std::span<KingdomTitle*> GetKingdoms();

    /**
     * @brief Gets the duchies in the region.
     * @return A span of pointers to the duchies.
     */
    std::span<DuchyTitle*> GetDuchies();

    /**
     * @brief Gets the counties in the region.
     * @return A span of pointers to the counties.
     */
    std::span<CountyTitle*> GetCounties();

    /**
     * @brief Gets the provinces in the region.
     * @return A span of pointers to the provinces.
     */
    std::span<Province*> GetProvinces();

    /**
     * @brief Gets the sub-regions in the region.
     * @return A span of pointers to the sub-regions.
     */
    std::span<Region*> GetRegions();

    /**
     * @brief Checks if the region generates modifiers.
     * @return True if the region generates modifiers, false otherwise.
     */
    bool DoesGenerateModifiers() const;

    /**
     * @brief Checks if the region should remember the order of its counties.
     * @return True if the region should remember the order of its counties, false otherwise.
     */
    bool ShouldRememberCountiesOrder() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the region.
     * @param name The name to set.
     */
    void SetName(std::string name);

    /**
     * @brief Sets whether the region generates modifiers.
     * @param generateModifiers True if the region should generate modifiers, false otherwise.
     */
    void SetGenerateModifiers(bool generateModifiers);

    /**
     * @brief Sets whether the region should remember the order of its counties.
     * @param shouldRememberCountiesOrder True if the region should remember the order of its counties, false otherwise.
     */
    void SetShouldRememberCountiesOrder(bool shouldRememberCountiesOrder);

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a title to the region.
     * @param title The title to add.
     * @note If the title is already present, it will not be added again.
     */
    void AddTitle(Title* title);

    /**
     * @brief Removes a title from the region.
     * @param title The title to remove.
     */
    void RemoveTitle(Title* title);

    /**
     * @brief Adds a kingdom to the region.
     * @param title The kingdom title to add.
     * @note If the kingdom is already present, it will not be added again.
     */
    void AddKingdom(KingdomTitle* title);

    /**
     * @brief Removes a kingdom from the region.
     * @param title The kingdom title to remove.
     */
    void RemoveKingdom(KingdomTitle* title);

    /**
     * @brief Adds a duchy to the region.
     * @param title The duchy title to add.
     * @note If the duchy is already present, it will not be added again.
     */
    void AddDuchy(DuchyTitle* title);

    /**
     * @brief Removes a duchy from the region.
     * @param title The duchy title to remove.
     */
    void RemoveDuchy(DuchyTitle* title);

    /**
     * @brief Adds a county to the region.
     * @param title The county title to add.
     * @note If the county is already present, it will not be added again.
     */
    void AddCounty(CountyTitle* title);

    /**
     * @brief Removes a county from the region.
     * @param title The county title to remove.
     */
    void RemoveCounty(CountyTitle* title);

    /**
     * @brief Adds a province to the region.
     * @param province The province to add.
     * @note If the province is already present, it will not be added again.
     */
    void AddProvince(Province* province);

    /**
     * @brief Removes a province from the region.
     * @param province The province to remove.
     */
    void RemoveProvince(Province* province);

    /**
     * @brief Adds a region to the region.
     * @param region The region to add.
     * @note If the region is already present, it will not be added again.
     */
    void AddRegion(Region* region);

    /**
     * @brief Removes a region from the region.
     * @param region The region to remove.
     */
    void RemoveRegion(Region* region);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
    
    std::vector<KingdomTitle*> m_Kingdoms;
    std::vector<DuchyTitle*> m_Duchies;
    std::vector<CountyTitle*> m_Counties;
    std::vector<Province*> m_Provinces;
    std::vector<Region*> m_Regions;

    bool m_GenerateModifiers;
    bool m_ShouldRememberCountiesOrder;
};