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

class Province {
public:
    Province(int id, sf::Color color, std::string name);

    int& GetId();
    sf::Color& GetColor();
    uint32_t GetColorId() const;
    std::string& GetName();
    ProvinceFlags& GetFlags();
    bool HasFlag(ProvinceFlags flag) const;
    TerrainType& GetTerrain();

    void SetColor(sf::Color color);
    void SetFlags(ProvinceFlags flags);
    void SetFlag(ProvinceFlags flag, bool enabled);
    void SetTerrain(TerrainType terrain);

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    ProvinceFlags m_Flags;
    TerrainType m_Terrain;

    // Sea-zone for port
    // Terrain
    // Culture
    // Religion
};