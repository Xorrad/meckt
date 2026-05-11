#pragma once

class Province {
public:
    Province(int id, sf::Color color, std::string name);

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

    std::string GetReligion() const;
    void SetReligion(std::string religion);

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

    std::string GetOriginalHistoryFilePath() const;
    void SetOriginalHistoryFilePath(const std::string& filePath);

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
    std::string m_Religion;

    std::string m_OriginalHistoryFilePath;
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