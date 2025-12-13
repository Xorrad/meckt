#pragma once

enum class ClimateType {
    NONE,
    MILD_WINTER,
    NORMAL_WINTER,
    SEVERE_WINTER,
    COUNT
};
const std::unordered_map<ClimateType, const char*> ClimateTypeLabels = {
    { ClimateType::NONE, "None" },
    { ClimateType::MILD_WINTER, "Mild Winter" },
    { ClimateType::NORMAL_WINTER, "Normal Winter" },
    { ClimateType::SEVERE_WINTER, "Severe Winter" },
    { ClimateType::COUNT, "******" }
};

class HoldingType {
public:
    HoldingType();
    HoldingType(const std::string& name);

    std::string GetName() const;
    void SetName(const std::string& name);

private:
    std::string m_Name;
};

class TerrainType {
public:
    TerrainType();
    TerrainType(const std::string& name, const sf::Color& color);

    std::string GetName() const;
    void SetName(const std::string& name);
    
    sf::Color GetColor() const;
    void SetColor(const sf::Color& color);

private:
    std::string m_Name;
    sf::Color m_Color;
};

enum class ProvinceFlags {
    NONE       = 0,
    COASTAL    = 1 << 0,
    LAKE       = 1 << 1,
    ISLAND     = 1 << 2,
    LAND       = 1 << 3,
    SEA        = 1 << 4,
    RIVER      = 1 << 5,
    IMPASSABLE = 1 << 6,
};

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator~(ProvinceFlags a);
ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b);
ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b);

class Province {
friend PropertiesTab;
public:
    Province(int id, sf::Color color, std::string name);

    int GetId() const;
    
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
    uint GetImagePixelsCount() const;
    void SetImagePosition(sf::Vector2i pos);
    void SetImagePixelsCount(uint count);
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
    uint m_ImagePixelsCount;

    ClimateType m_ClimateType;
    std::string m_WinterSeverityBias;
    std::string m_MildWinterFactorOverride;
    std::string m_NormalWinterFactorOverride;
    std::string m_HarshWinterFactorOverride;

    // TODO: Sea-zone for port
};