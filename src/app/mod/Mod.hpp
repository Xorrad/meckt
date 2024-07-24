#pragma once

class Mod {
public:
    Mod(const std::string& dir);

    std::string GetDir() const;
    sf::Image& getHeightmapImage();
    sf::Image& getProvinceImage();
    sf::Image& getRiversImage();
    bool HasMap() const;

    std::map<uint32_t, SharedPtr<Province>>& GetProvinces();
    std::map<int, SharedPtr<Province>>& GetProvincesByIds();

    void LoadMapModeTexture(sf::Texture& texture, MapMode mode);
    
    void Load();
    void LoadProvincesDefinition();
    void LoadProvincesTerrain();
    void LoadTitlesHistory();
    void LoadTitles();

    void Export();
    void ExportProvincesDefinition();
    void ExportProvincesTerrain();

private:
    std::string m_Dir;
    sf::Image m_HeightmapImage;
    sf::Image m_ProvinceImage;
    sf::Image m_RiversImage;

    std::map<uint32_t, SharedPtr<Province>> m_Provinces;
    std::map<int, SharedPtr<Province>> m_ProvincesByIds;

    TerrainType m_DefaultLandTerrain;
    TerrainType m_DefaultSeaTerrain;
    TerrainType m_DefaultCoastalSeaTerrain;
};