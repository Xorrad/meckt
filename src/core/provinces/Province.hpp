#pragma once

class Province {
public:
    Province(int id, sf::Color color, std::string name);

    //////////////////////////////////////////////////////

    /**
     * @brief Retrieves the liege title of the province for a given title type.
     * @param titleManager The title manager to use for retrieving titles.
     * @param type The title type to retrieve the liege title for.
     * @return The pointer to the liege title if it exists, nullptr otherwise.
     */
    Title* GetProvinceLiegeTitle(TitleManager* titleManager, TitleType type) const;

    /**
     * @brief Retrieves the focused title of the province for a given title type.
     *        The focused title change when the user wrap/unwrap titles.
     * @param titleManager The title manager to use for retrieving titles.
     * @param type The title type to retrieve the focused title for.
     * @return The pointer to the focused title if it exists, nullptr otherwise.
     */
    Title* GetProvinceFocusedTitle(TitleManager* titleManager, TitleType type) const;

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
    float CalculateWinterSeverityBias(ProvinceManager* provinceManager, bool override, float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor) const;

    //////////////////////////////////////////////////////

    int GetId() const;
    void SetId(int id);
    
    sf::Color GetColor() const;
    uint32_t GetColorId() const;
    void SetColor(sf::Color color);

    std::string GetName() const;
    void SetName(std::string name);

    ProvinceFlags GetFlags() const;
    bool HasFlag(ProvinceFlags flag) const;
    void SetFlags(ProvinceFlags flags);
    void SetFlag(ProvinceFlags flag, bool enabled);
    
    std::string GetHolding() const;
    void SetHolding(std::string holding);

    std::string GetTerrain() const;
    void SetTerrain(std::string terrain);
    
    std::string GetCulture() const;
    void SetCulture(std::string culture);

    std::string GetFaith() const;
    void SetFaith(std::string faith);

    ClimateType GetClimateType() const;
    std::string GetWinterSeverityBias() const;
    std::string GetMildWinterFactorOverride() const;
    std::string GetNormalWinterFactorOverride() const;
    std::string GetHarshWinterFactorOverride() const;
    void SetClimateType(ClimateType type);
    void SetWinterSeverityBias(std::string bias);
    void SetMildWinterFactorOverride(std::string factor);
    void SetNormalWinterFactorOverride(std::string factor);
    void SetHarshWinterFactorOverride(std::string factor);

    std::string GetOriginalHistoryFileName() const;
    void SetOriginalHistoryFileName(const std::string& fileName);

    SharedPtr<Jomini::Object> GetExtraHistoryData() const;
    void SetExtraHistoryData(SharedPtr<Jomini::Object> data);

    std::map<Jomini::Date, SharedPtr<Jomini::Object>>& GetHistory();
    void AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data);
    void RemoveHistory(Jomini::Date date);
    
    sf::Vector2i GetImagePosition() const;
    size_t GetImagePixelsCount() const;
    void SetImagePosition(sf::Vector2i pos);
    void SetImagePixelsCount(size_t count);
    void IncrementImagePixelsCount();

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    ProvinceFlags m_Flags;

    std::string m_Holding;
    std::string m_Terrain;
    std::string m_Culture;
    std::string m_Faith;

    std::string m_OriginalHistoryFileName;
    SharedPtr<Jomini::Object> m_ExtraHistoryData;
    std::map<Jomini::Date, SharedPtr<Jomini::Object>> m_History;

    sf::Vector2i m_ImagePosition;
    size_t m_ImagePixelsCount;

    ClimateType m_ClimateType;
    std::string m_WinterSeverityBias;
    std::string m_MildWinterFactorOverride;
    std::string m_NormalWinterFactorOverride;
    std::string m_HarshWinterFactorOverride;

    // TODO: Sea-zone for port
};