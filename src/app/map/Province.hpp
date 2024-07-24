#pragma once

class Province {
public:
    Province(int id, sf::Color color, std::string name);

    int& GetId();
    sf::Color& GetColor();
    uint32_t GetColorId() const;
    std::string& GetName();
    TerrainType& GetTerrain();
    bool IsSea() const;

    void SetColor(sf::Color color);
    void SetTerrain(TerrainType terrain);
    void SetIsSea(bool isSea);

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    TerrainType m_Terrain;
    bool m_IsSea;
    // Terrain
    // Culture
    // Religion
};