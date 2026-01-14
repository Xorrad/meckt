#pragma once

#define ASSERT_IS_OBJECT(property, value, key, filePath) \
    if (!value->Is(Jomini::Type::OBJECT)) { \
        LOG_ERROR("Wrong value for {} '{}' in {}\nThis is probably caused by duplicates definition of key '{}'.", property, key, filePath, key); \
        continue; \
    } \

class Mod {
public:
    Mod(const std::string& dir);
    Mod(const std::string& dir, sf::Image heightmapImage, sf::Image provincesImage, float waterLevel);
    ~Mod();

    std::string GetDir() const;
    sf::Image& GetHeightmapImage();
    sf::Image& GetProvinceImage();
    sf::Image& GetRiversImage();
    sf::Image GetTerrainImage();
    sf::Image GetWinterSeverityImage();
    sf::Image GetCultureImage();
    sf::Image GetReligionImage();
    sf::Image GetTitleImage(TitleType type);
    bool HasMap() const;

    std::map<uint32_t, UniquePtr<Province>>& GetProvinces();
    std::map<int, Province*>& GetProvincesByIds();
    Title* GetProvinceLiegeTitle(Province* province, TitleType type);
    Title* GetProvinceFocusedTitle(Province* province, TitleType type);
    int GetMaxProvinceId() const;
    
    std::map<std::string, UniquePtr<Region>>& GetRegions();

    std::map<std::string, UniquePtr<Title>>& GetTitles();
    std::map<TitleType, std::vector<Title*>>& GetTitlesByType();
    std::map<int, BaronyTitle*>& GetBaroniesByProvinceIds();

    const OrderedMap<std::string, HoldingType>& GetHoldingTypes() const;
    const OrderedMap<std::string, TerrainType>& GetTerrainTypes() const;

    const std::string& GetTitlesLocalizationFilePath() const;
    const std::string& GetCulturalNamesLocalizationFilePath() const;

    std::map<std::string, std::map<std::string, std::string>>& GetLocCulturalNames();
    std::map<std::string, std::string>& GetLocCulturalNames(const std::string& lang);
    std::string& GetLocCulturalName(const std::string& lang, const std::string& key);
    std::string GetLocCulturalName(const std::string& lang, const std::string& key) const;
    void SetLocCulturalName(const std::string& lang, const std::string& key, std::string name);

    void AddCulture(UniquePtr<Culture> culture);
    void AddReligion(UniquePtr<Religion> religion);
    void AddProvince(UniquePtr<Province> province);

    void AddTitle(UniquePtr<Title> title);
    void RemoveTitle(Title* title);
    void RenameTitle(Title* title, std::string formerName);
    
    void AddRegion(UniquePtr<Region> region);
    void RenameRegion(Region* region, std::string formerName);
    void RemoveRegion(Region* region);

    void HarmonizeTitlesColors(std::span<Title*> titles, sf::Color color, float hue, float saturation);
    void GenerateMissingProvinces();
    void GenerateMissingBaronies();
    void GenerateTitlesLocalization(const std::string& lang, bool names, bool adjectives, bool articles);

    float CalculateWinterSeverityBias(Province* province, bool override, float elevationOffset, float elevationStrength, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor) const;
    void GenerateProvincesClimate(bool override, float elevationStrength, float elevationOffset, float elevationFactor, int hemisphereOffset, int hemisphereSize, float hemisphereStrength, float hemisphereFactor, float mildWinterThreshold, float normalWinterThreshold, float severeWinterThreshold);

    void ClearProvinces(); // Remove all current provinces.
    void ClearTitles(); // Remove all current titles.
    void DetermineProvincesFlags(); // Determine province flags (land, sea...) for each province depending on elevation.
    void GenerateRivers(); // Generate a blank rivers image using the landmass.
    void GenerateWorld(); // Generate a provinces image using the heightmap, voronoi and conquests.

    void Load(std::function<void()> completeCallback, std::function<void(LoadingState)> changeCallback, std::function<void(const std::string&)> errorCallback, bool loadImages = true);
    void LoadHoldingTypes();
    void LoadTerrainTypes();
    void LoadProvinceImage();
    void LoadDefaultMapFile();
    void LoadProvincesDefinition();
    void LoadProvincesTerrain();
    void LoadProvincesClimate();
    void LoadProvincesHistory();
    void LoadTitles();
    void LoadTitlesHistory();
    void LoadGeographicalRegions();
    void LoadCultures();
    void LoadReligions();
    void LoadLocalization();

    std::vector<Title*> ParseTitles(const std::string& filePath, SharedPtr<Jomini::Object> data);

    void Export(
        bool defaultMap = true,
        bool provincesDefinition = true,
        bool provincesTerrain = true,
        bool provincesClimate = true,
        bool provincesHistory = true,
        bool titles = true,
        bool titlesHistory = true,
        bool titlesLocalization = true,
        bool culturalNamesLocalization = true,
        bool geographicalRegions = true
    );
    void ExportDefaultMapFile();
    void ExportProvincesDefinition();
    void ExportProvincesTerrain();
    void ExportProvincesClimate();
    void ExportProvincesHistory();
    void ExportTitles();
    void ExportTitlesHistory();
    void ExportTitle(Title* title, std::ofstream& file, int depth);
    void ExportGeographicalRegions();

    void ExportTitlesLocalization();
    void ExportCulturalNamesLocalization();
    void DeleteTitlesLocalization(bool titlesLocalization, bool culturalNamesLocalization);

private:
    std::string m_Dir;
    sf::Image m_HeightmapImage;
    sf::Image m_ProvinceImage;
    sf::Image m_RiversImage;

    float m_WaterLevel;

    std::map<uint32_t, UniquePtr<Province>> m_Provinces;
    std::map<int, Province*> m_ProvincesByIds;
    
    std::map<std::string, UniquePtr<Region>> m_Regions;

    std::map<std::string, UniquePtr<Title>> m_Titles;
    std::map<TitleType, std::vector<Title*>> m_TitlesByType;
    std::map<int, BaronyTitle*> m_BaroniesByProvinceIds;

    // Map variables in common/landed_titles & history/titles & history/provinces with their respective filename.
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesVariables;
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesHistoryVariables;
    std::map<std::string, SharedPtr<Jomini::Object>> m_ProvincesHistoryVariables;

    std::map<std::string, UniquePtr<Culture>> m_Cultures;
    std::map<std::string, UniquePtr<Religion>> m_Religions;

    OrderedMap<std::string, HoldingType> m_HoldingTypes;
    OrderedMap<std::string, TerrainType> m_TerrainTypes;
    SharedPtr<Jomini::Object> m_TerrainPropertiesVariables;

    std::string m_DefaultLandTerrain;
    std::string m_DefaultSeaTerrain;
    std::string m_DefaultCoastalSeaTerrain;

    std::string m_TitlesLocalizationFilePath;
    std::string m_CulturalNamesLocalizationFilePath;

    std::map<std::string, std::map<std::string, std::string>> m_LocCulturalNames;
};