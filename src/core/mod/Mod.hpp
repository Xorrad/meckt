#pragma once

class Mod {
public:
    Mod(const std::string& rootDirectory);
    Mod(const std::string& rootDirectory, sf::Image heightmapImage, sf::Image provincesImage);
    ~Mod();
    
    //////////////////////////////////////////////////////

    /**
     * @brief Checks if the mod has a map (provinces image).
     * @return True if the mod has a map, false otherwise.
     */
    bool HasMap() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the absolute root directory of the mod.
     * @return The absolute path to the root directory.
     */
    const std::string& GetRootDirectory() const;

    /**
     * @brief Gets the directory path for a specific directory.
     * @param directory The directory.
     * @return The directory path as a string.
     */
    std::string GetDirectory(GamePath directory) const;

    /**
     * @brief Resolves a relative file name to an absolute path based on its directory.
     * @param directory The directory of the file.
     * @param relativeFileName The relative file name (e.g. "subdir/my_file.txt").
     * @return The resolved absolute path as a string.
     */
    std::string GetAbsolutePath(GamePath directory, const std::string& relativeFileName = "") const;

    /**
     * @brief Converts an absolute path to a relative path based on its directory.
     * @param directory The directory of the file.
     * @param absolutePath The absolute path to convert.
     * @return The relative path as a string (e.g. "01_landed_titles.txt" instead of "mod/common/landed_titles/01_landed_titles.txt").
     */
    std::string GetRelativePath(GamePath directory, const std::string& absolutePath) const;

    //////////////////////////////////////////////////////

    #define DECLARE_MANAGER_GETTER(ManagerName) \
        ManagerName##Manager& Get##ManagerName##Manager(); \
        const ManagerName##Manager& Get##ManagerName##Manager() const;

    DECLARE_MANAGER_GETTER(Title);
    DECLARE_MANAGER_GETTER(Province);
    DECLARE_MANAGER_GETTER(Region);
    DECLARE_MANAGER_GETTER(Culture);
    DECLARE_MANAGER_GETTER(Religion);
    DECLARE_MANAGER_GETTER(Define);

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the root directory of the mod.
     * @param dir The absolute path to the root directory.
     */
    void SetRootDirectory(const std::string& rootDirectory);

    //////////////////////////////////////////////////////

    /**
     * @brief Initializes all the data managers and loads the mod data from the root directory.
     * @param completeCallback The callback function to call when the loading is complete.
     * @param changeCallback The callback function to call when the loading state changes, with the new loading state as a parameter.
     * @param errorCallback The callback function to call when an error occurs during loading, with the error message as a parameter.
     */
    void Load(
        std::function<void()> completeCallback,
        std::function<void(LoadingState)> changeCallback,
        std::function<void(const std::string&)> errorCallback
    );

    /**
     * @brief Exports the current mod data to the mod files in the root directory.
     * @param defaultMap If true, exports the provinces flags to the default.map file.
     * @param provincesDefinition If true, exports the provinces definition to the definition.csv file.
     * @param provincesTerrain If true, exports the provinces terrain to the province terrain file.
     * @param provincesClimate If true, exports the provinces climate to the province properties file.
     * @param provincesHistory If true, exports the provinces history to the history files.
     * @param titles If true, exports the titles definitions to the landed_titles files.
     * @param titlesHistory If true, exports the titles history to the history files.
     * @param titlesLocalization If true, exports the titles localization to the localization files.
     * @param culturalNamesLocalization If true, exports the cultural names localization to the localization files.
     * @param geographicalRegions If true, exports the geographical regions to the geographical regions files.
     */
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

private:
    std::string m_RootDirectory;

    UniquePtr<TitleManager> m_TitleManager;
    UniquePtr<ProvinceManager> m_ProvinceManager;
    UniquePtr<RegionManager> m_RegionManager;
    UniquePtr<CultureManager> m_CultureManager;
    UniquePtr<ReligionManager> m_ReligionManager;
    UniquePtr<DefineManager> m_DefineManager;
};