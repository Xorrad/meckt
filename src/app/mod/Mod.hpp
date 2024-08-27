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
    SharedPtr<Title> GetProvinceLiegeTitle(const SharedPtr<Province>& province, TitleType type);
    
    std::map<std::string, SharedPtr<Title>>& GetTitles();
    std::map<TitleType, std::vector<SharedPtr<Title>>>& GetTitlesByType();

    void HarmonizeTitlesColors(const std::vector<SharedPtr<Title>>& titles, sf::Color color, float hue, float saturation);

    void LoadMapModeTexture(sf::Texture& texture, MapMode mode);
    void UpdateTitlesImages(sf::Texture& texture, TitleType type);

    void Load();
    void LoadDefaultMapFile();
    void LoadProvincesDefinition();
    void LoadProvincesTerrain();
    void LoadProvincesInfo();
    void LoadTitles();
    void LoadTitlesHistory();

    std::vector<SharedPtr<Title>> ParseTitles(Parser::Node& data);

    void Export();
    void ExportDefaultMapFile();
    void ExportProvincesDefinition();
    void ExportProvincesTerrain();
    void ExportProvincesInfo();

private:
    std::string m_Dir;
    sf::Image m_HeightmapImage;
    sf::Image m_ProvinceImage;
    sf::Image m_RiversImage;

    sf::Image m_TitlesImage;

    std::map<uint32_t, SharedPtr<Province>> m_Provinces;
    std::map<int, SharedPtr<Province>> m_ProvincesByIds;
    
    std::map<std::string, SharedPtr<Title>> m_Titles;
    std::map<TitleType, std::vector<SharedPtr<Title>>> m_TitlesByType;

    TerrainType m_DefaultLandTerrain;
    TerrainType m_DefaultSeaTerrain;
    TerrainType m_DefaultCoastalSeaTerrain;
};