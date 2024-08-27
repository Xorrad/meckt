#pragma once

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

enum class ProvinceHolding {
    NONE,
    TRIBAL,
    CASTLE,
    CITY,
    CHURCH,
    COUNT,
};

const std::vector<const char*> ProvinceHoldingLabels = {
    "none",
    "tribal_holding",
    "castle_holding",
    "city_holding",
    "church_holding",
};

ProvinceHolding ProvinceHoldingFromString(const std::string& str);

class Province {
friend PropertiesTab;
public:
    Province(int id, sf::Color color, std::string name);

    int GetId() const;
    sf::Color GetColor() const;
    uint32_t GetColorId() const;
    std::string GetName() const;
    ProvinceFlags GetFlags() const;
    bool HasFlag(ProvinceFlags flag) const;
    TerrainType GetTerrain() const;
    std::string GetCulture() const;
    std::string GetReligion() const;
    ProvinceHolding GetHolding() const;

    void SetName(std::string name);
    void SetColor(sf::Color color);
    void SetFlags(ProvinceFlags flags);
    void SetFlag(ProvinceFlags flag, bool enabled);
    void SetTerrain(TerrainType terrain);
    void SetCulture(std::string culture);
    void SetReligion(std::string religion);
    void SetHolding(ProvinceHolding holding);
    
    std::string GetOriginalFilePath() const;
    SharedPtr<Parser::Node> GetOriginalData() const;
    void SetOriginalFilePath(const std::string& filePath);
    void SetOriginalData(const Parser::Node& data);

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    ProvinceFlags m_Flags;
    TerrainType m_Terrain;

    std::string m_Culture;
    std::string m_Religion;
    ProvinceHolding m_Holding;

    std::string m_OriginalFilePath;
    SharedPtr<Parser::Node> m_OriginalData;

    // Sea-zone for port
    // Terrain
    // History (modifiers with date, buildings, owners...)
};