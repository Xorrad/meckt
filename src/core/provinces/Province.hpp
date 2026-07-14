#pragma once

class Province {
public:
    /**
     * @brief Constructs a new province with the specified ID, color, and name.
     * @param id The ID of the province.
     * @param color The color of the province.
     * @param name The name of the province.
     */
    Province(int id, sf::Color color, std::string name);

    //////////////////////////////////////////////////////
    
    /**
     * @brief Checks if the province has a specific flag.
     * @param flag The flag to check for.
     * @return True if the province has the flag, false otherwise.
     */
    bool HasFlag(ProvinceFlags flag) const;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Retrieves the ID of the province.
     * @return The ID of the province.
     */
    int GetId() const;

    /**
     * @brief Retrieves the color of the province.
     * @return The color of the province.
     */
    sf::Color GetColor() const;


    /**
     * @brief Retrieves the color of the province.
     * @return The color of the province as an 32-bit integer.
     */
    uint32_t GetColorId() const;

    /**
     * @brief Retrieves the name of the province.
     * @return The name of the province.
     */
    std::string GetName() const;

    /**
     * @brief Retrieves the flags of the province.
     * @return The flags of the province.
     */
    ProvinceFlags GetFlags() const;

    /**
     * @brief Retrieves the holding of the province.
     * @return The holding of the province.
     */
    std::string GetHolding() const;

    /**
     * @brief Retrieves the terrain of the province.
     * @return The terrain of the province as a string.
     */
    std::string GetTerrain() const;

    /**
     * @brief Retrieves the culture of the province.
     * @return The culture of the province as a string.
     */
    std::string GetCulture() const;

    /**
     * @brief Retrieves the faith of the province.
     * @return The faith of the province as a string.
     */
    std::string GetFaith() const;

    /**
     * @brief Retrieves the climate type of the province.
     * @return The climate type of the province.
     */
    ClimateType GetClimateType() const;

    /**
     * @brief Retrieves the winter severity bias of the province.
     * @return The winter severity bias of the province as a string.
     */
    std::string GetWinterSeverityBias() const;

    /**
     * @brief Retrieves the mild winter factor override of the province.
     * @return The mild winter factor override of the province as a string.
     */
    std::string GetMildWinterFactorOverride() const;

    /**
     * @brief Retrieves the normal winter factor override of the province.
     * @return The normal winter factor override of the province as a string.
     */
    std::string GetNormalWinterFactorOverride() const;

    /**
     * @brief Retrieves the harsh winter factor override of the province.
     * @return The harsh winter factor override of the province as a string.
     */
    std::string GetHarshWinterFactorOverride() const;

    /**
     * @brief Retrieves the original history file name of the province.
     * @note It excludes "my_mod/history/provinces/" from the file name.
     * @return The original history file name of the province as a string.
     */
    std::string GetOriginalHistoryFileName() const;

    /**
     * @brief Retrieves the extra history data for the province.
     * @return A shared pointer to the extra history data.
     */
    SharedPtr<Jomini::Object> GetExtraHistoryData() const;

    /**
     * @brief Retrieves the history data for the province.
     * @return A reference to the map containing the history data.
     */
    std::map<Jomini::Date, SharedPtr<Jomini::Object>>& GetHistory();

    /**
     * @brief Retrieves the image position of the province.
     * @return The image position as a vector.
     */
    sf::Vector2i GetImagePosition() const;
    
    /**
     * @brief Retrieves the number of pixels the province has in the province's image.
     * @return The number of pixels in the province's image.
     */
    size_t GetImagePixelsCount() const;

    /**
     * @brief Retrieves the liege title of the province for a given title type.
     * @param titleManager The title manager to use for retrieving titles.
     * @param type The title type to retrieve the liege title for.
     * @return The pointer to the liege title if it exists, nullptr otherwise.
     */
    Title* GetProvinceLiegeTitle(TitleManager& titleManager, TitleType type) const;

    /**
     * @brief Retrieves the focused title of the province for a given title type.
     *        The focused title change when the user wrap/unwrap titles.
     * @param titleManager The title manager to use for retrieving titles.
     * @param type The title type to retrieve the focused title for.
     * @return The pointer to the focused title if it exists, nullptr otherwise.
     */
    Title* GetProvinceFocusedTitle(TitleManager& titleManager, TitleType type) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the ID of the province.
     * @param id The ID to set.
     */
    void SetId(int id);

    /**
     * @brief Sets the color of the province.
     * @param color The color to set.
     */
    void SetColor(sf::Color color);

    /**
     * @brief Sets the name of the province.
     * @param name The name to set.
     */
    void SetName(std::string name);

    /**
     * @brief Sets the flags of the province.
     * @param flags The flags to set.
     */
    void SetFlags(ProvinceFlags flags);

    /**
     * @brief Sets a specific flag of the province.
     * @param flag The flag to set.
     * @param enabled Whether the flag should be enabled.
     */
    void SetFlag(ProvinceFlags flag, bool enabled);
    
    /**
     * @brief Sets the holding of the province.
     * @param holding The holding to set.
     */
    void SetHolding(std::string holding);

    /**
     * @brief Sets the terrain of the province.
     * @param terrain The terrain to set.
     */
    void SetTerrain(std::string terrain);
    
    /**
     * @brief Sets the culture of the province.
     * @param culture The culture to set.
     */
    void SetCulture(std::string culture);

    /**
     * @brief Sets the faith of the province.
     * @param faith The faith to set.
     */
    void SetFaith(std::string faith);

    /**
     * @brief Sets the climate type of the province.
     * @param type The climate type to set.
     */
    void SetClimateType(ClimateType type);

    /**
     * @brief Sets the winter severity bias of the province.
     * @param bias The winter severity bias to set.
     */
    void SetWinterSeverityBias(std::string bias);

    /**
     * @brief Sets the mild winter factor override of the province.
     * @param factor The mild winter factor to set.
     */
    void SetMildWinterFactorOverride(std::string factor);

    /**
     * @brief Sets the normal winter factor override of the province.
     * @param factor The normal winter factor to set.
     */
    void SetNormalWinterFactorOverride(std::string factor);

    /**
     * @brief Sets the harsh winter factor override of the province.
     * @param factor The harsh winter factor to set.
     */
    void SetHarshWinterFactorOverride(std::string factor);

    /**
     * @brief Sets the original history file name of the province.
     * @param fileName The original history file name to set.
     */
    void SetOriginalHistoryFileName(const std::string& fileName);

    /**
     * @brief Sets the extra history data of the province.
     * @param data The extra history data to set.
     */
    void SetExtraHistoryData(SharedPtr<Jomini::Object> data);

    /**
     * @brief Sets the position of the province's image.
     * @param pos The position to set.
     */
    void SetImagePosition(sf::Vector2i pos);

    /**
     * @brief Sets the number of pixels in the province's image.
     * @param count The number of pixels to set.
     */
    void SetImagePixelsCount(size_t count);

    /**
     * @brief Increments the number of pixels in the province's image.
     */
    void IncrementImagePixelsCount();

    //////////////////////////////////////////////////////

    /**
     * @brief Adds history data for the province.
     * @param date The date of the history entry.
     * @param data The history data to add.
     */
    void AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data);

    /**
     * @brief Removes history data for the province.
     * @param date The date of the history entry to remove.
     */
    void RemoveHistory(Jomini::Date date);

    //////////////////////////////////////////////////////

    /**
     * @brief Calculates the winter severity bias for the province based on several parameters.
     * @param provinceManager The province manager.
     * @param override Whether to override the existing bias or not.
     * @param elevationOffset The offset to apply to the province's elevation when calculating the bias.
     * @param elevationStrength The strength of the elevation's influence on the bias.
     * @param elevationFactor The factor to apply to the elevation when calculating the bias.
     * @param hemisphereOffset The offset to apply to the province's latitude when calculating the bias.
     * @param hemisphereSize The size of the hemisphere in degrees (e.g. 90 for northern/southern hemisphere, 180 for eastern/western hemisphere).
     * @param hemisphereStrength The strength of the hemisphere's influence on the bias.
     * @param hemisphereFactor The factor to apply to the hemisphere when calculating the bias.
     * @return The calculated winter severity bias in [0.f, 1.f].
     */
    float CalculateWinterSeverityBias(ProvinceManager& provinceManager, bool override, float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor) const;

    //////////////////////////////////////////////////////

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    ProvinceFlags m_Flags;

    std::string m_Holding;
    std::string m_Terrain;
    std::string m_Culture;
    std::string m_Faith;

    ClimateType m_ClimateType;
    std::string m_WinterSeverityBias;
    std::string m_MildWinterFactorOverride;
    std::string m_NormalWinterFactorOverride;
    std::string m_HarshWinterFactorOverride;

    std::string m_OriginalHistoryFileName;
    SharedPtr<Jomini::Object> m_ExtraHistoryData;
    std::map<Jomini::Date, SharedPtr<Jomini::Object>> m_History;

    sf::Vector2i m_ImagePosition;
    size_t m_ImagePixelsCount;

    // TODO: Sea-zone for port
};