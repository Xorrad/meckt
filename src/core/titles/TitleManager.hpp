#pragma once

#include "titles/Title.hpp"

class TitleManager {
public:
    TitleManager(Mod& mod);
    TitleManager(const TitleManager&) = delete;
    TitleManager& operator=(const TitleManager&) = delete;

    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of titles.
     * @return The total number of titles.
     */
    size_t CountTitles() const;

    /**
     * @brief Counts the number of titles of a specific type.
     * @param type The title type.
     * @return The number of titles of the specified type.
     */
    size_t CountTitles(TitleType type) const;

    /**
     * @brief Checks if a title exists.
     * @param name The name of the title to search for.
     * @return True if the title exists, false otherwise.
     */
    bool HasTitle(const std::string& name) const;
    
    /**
     * @brief Checks if a cultural name localization exists.
     * @param lang The localization language (e.g `english`).
     * @param key The cultural name key (e.g `cn_naoned`).
     * @return True if the key exists, false otherwise.
     */
    bool HasLocCulturalName(const std::string& lang, const std::string& key) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves a title by its name.
     * @param name The name of the title to retrieve.
     * @return The pointer to the title if it exists, nullptr otherwise.
     */
    Title* GetTitle(const std::string& name);

    /**
     * @brief Retrieves a title by its name.
     * @param name The name of the title to retrieve.
     * @return The pointer to the title if it exists, nullptr otherwise.
     */
    const Title* GetTitle(const std::string& name) const;

    /**
     * @brief Retrieves a title by its name and type.
     * @tparam T The derived class to cast to.
     * @param name The name of the title to retrieve.
     * @return The pointer to the title if it exists and is the right type, nullptr otherwise.
     */
    template <typename T> T* GetTitleAs(const std::string& name);

    /**
     * @brief Retrieves a title by its name and type.
     * @tparam T The derived class to cast to.
     * @param name The name of the title to retrieve.
     * @return The pointer to the title if it exists and is the right type, nullptr otherwise.
     */
    template <typename T> const T* GetTitleAs(const std::string& name) const;

    /**
     * @brief Retrieves a barony title by its province id.
     * @param provinceId The province id of the barony title to retrieve.
     * @return The pointer to the barony title if it exists, nullptr otherwise.
     */
    BaronyTitle* GetBaronyByProvinceId(int provinceId);

    /**
     * @brief Retrieves a barony title by its province id.
     * @param provinceId The province id of the barony title to retrieve.
     * @return The pointer to the barony title if it exists, nullptr otherwise.
     */
    const BaronyTitle* GetBaronyByProvinceId(int provinceId) const;

        /**
     * @brief Generates the titles image.
     *        Each province is mapped to the focused highest title of the specified type.
     * @note  This function can be expensive: to be used with sparingly.
     * @param provinceManager The province manager.
     * @param type The highest title type.
     * @return The generated titles image.
     */
    sf::Image GetTitleImage(ProvinceManager& provinceManager, TitleType type);

    /**
     * @brief Retrieves a map of titles.
     * @return The map of titles paired by their name.
     */
    std::map<std::string, UniquePtr<Title>>& GetTitles();

    /**
     * @brief Retrieves a map of titles.
     * @return The map of titles paired by their name.
     */
    const std::map<std::string, UniquePtr<Title>>& GetTitles() const;

    /**
     * @brief Retrieves a map of list of titles.
     * @return The map of lists of titles paired by their type.
     */
    std::map<TitleType, std::vector<Title*>>& GetTitlesByType();

    /**
     * @brief Retrieves a map of list of titles.
     * @return The map of lists of titles paired by their type.
     */
    const std::map<TitleType, std::vector<Title*>>& GetTitlesByType() const;

    /**
     * @brief Retrieves a map of baronies.
     * @return The map of baronies paired by their province id.
     */
    std::map<int, BaronyTitle*>& GetBaroniesByProvinceId();

    /**
     * @brief Retrieves a map of baronies.
     * @return The map of baronies paired by their province id.
     */
    const std::map<int, BaronyTitle*>& GetBaroniesByProvinceId() const;

    /**
     * @brief Retrieves the constants that were in the original landed_titles definition files.
     * @return A map of constants.
     */
    const std::map<std::string, SharedPtr<Jomini::Object>>& GetTitlesVariables() const;

    /**
     * @brief Retrieves the constants that were in the original titles history files.
     * @return A map of constants.
     */
    const std::map<std::string, SharedPtr<Jomini::Object>>& GetTitlesHistoryVariables() const;

    /**
     * @brief Retrieves the map of cultural names indexed by their localization language.
     * @return A map of all cultural names indexed by language.
     */
    std::map<std::string, std::map<std::string, std::string>>& GetLocCulturalNames();

    /**
     * @brief Retrieves the map of cultural names indexed by their localization language.
     * @return A map of all cultural names indexed by language.
     */
    const std::map<std::string, std::map<std::string, std::string>>& GetLocCulturalNames() const;

    /**
     * @brief Retrieves the cultural names localization for a specific language.
     * @param lang The localization language.
     * @return A map of cultural names localization indexed by their cultural name key.
     */
    std::map<std::string, std::string>& GetLocCulturalNames(const std::string& lang);
    
    /**
     * @brief Retrieves the cultural names localization for a specific language.
     * @param lang The localization language.
     * @return A map of cultural names localization indexed by their cultural name key.
     */
    const std::map<std::string, std::string>& GetLocCulturalNames(const std::string& lang) const;

    /**
     * @brief Retrieves a cultural name localization by language and cultural name key.
     * @param lang The localization language.
     * @param key The cultural name key.
     * @return The cultural name localization.
     */
    std::string& GetLocCulturalName(const std::string& lang, const std::string& key);

    /**
     * @brief Retrieves a cultural name localization by language and cultural name key.
     * @param lang The localization language.
     * @param key The cultural name key.
     * @return The cultural name localization.
     */
    std::string GetLocCulturalName(const std::string& lang, const std::string& key) const;

    /**
     * @brief Retrieves the relative file path of the titles localization file.
     * @return The relative file path of the titles localization file.
     */
    std::string GetTitlesLocalizationFileName() const;
    
    /**
     * @brief Retrieves the relative file path of the cultural names localization file.
     * @return The relative file path of the cultural names localization file.
     */
    std::string GetCulturalNamesLocalizationFileName() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new title.
     * @param title The unique pointer of the title.
     * @note If a title with the same name already exists, it will be overwritten with the new one.
     */
    void AddTitle(UniquePtr<Title> title);

    /**
     * @brief Removes a title.
     * @param name The name of the title to remove.
     */
    void RemoveTitle(const std::string& name);

    /**
     * @brief Removes a title.
     * @param title The title to remove.
     */
    void RemoveTitle(const Title* title);

    /**
     * @brief Renames a title.
     * @param formerName The former and current name of the title.
     * @param newName The name the title will be renamed to.
     */
    void RenameTitle(const std::string& formerName, const std::string& newName);

    /**
     * @brief Changes the province ID of a barony title.
     * @param barony The barony title whose province ID is to be changed.
     * @param newProvinceId The new province ID.
     */
    void ChangeBaronyProvinceId(BaronyTitle* barony, int newProvinceId);

    /**
     * @brief Adds a new cultural name localization.
     * @param lang The localization language (e.g `english`).
     * @param key The cultural name key (e.g `cn_naoned`).
     * @param name The cultural name localization (e.g `Naoned`)
     * @note If the key already exists, it will be overwritten with the new name.
     */
    void AddLocCulturalName(const std::string& lang, const std::string& key, const std::string& name);

    //////////////////////////////////////////////////////

    /**
     * @brief Loads all the titles defined in the mod's common/landed_titles directory.
     */
    void LoadTitles();

    /**
     * @brief Recursively parses titles from a jomini data object.
     * @param fileName The relative path to the original file.
     * @param data The data of the titles to parse.
     * @return A list of non-owning pointers to the titles defined in the data.
     */
    std::vector<Title*> LoadTitlesFile(const std::string& fileName, SharedPtr<Jomini::Object> data);

    /**
     * @brief Parses a title from a jomini data object.
     * @param fileName The relativepath to the title definition file.
     * @param name The name of the title to parse.
     * @param data The data of the title to parse.
     * @return A non-owning pointer to the parsed title.
     */
    UniquePtr<Title> ParseTitle(const std::string& fileName, const std::string& name, SharedPtr<Jomini::Object> data);

    /**
     * @brief Initializes the capital title class members for high titles (duchy, kingdom, empire, hegemony).
     *        This is automatically called when loading titles, therefore this shouldn't be called manually.
     */
    void LoadTitlesCapitals();


    /**
     * @brief Loads and initializes the history of the titles from the mod's history/titles directory.
     *        This works on already loaded titles, so it has to be called after TitleManager::LoadTitles.
     */
    void LoadTitlesHistory();

    /**
     * @brief Load the titles history of a specific file.
     * @param fileName The relative path to the history file.
     * @param data The parsed data of the history file.
     */
    void LoadTitlesHistoryFile(const std::string& fileName, SharedPtr<Jomini::Object> data);

    /**
     * @brief Load the titles localization.
     *        This includes names, adjectives, articles and cultural names.
     */
    void LoadLocalization();

    ////////////////////////////////////////////////////

    /**
     * @brief Exports the titles definitions.
     */
    void ExportTitles();

    /**
     * @brief Exports a title definition to a specific file.
     * @param title The title to export.
     * @param file The file stream to export the title to.
     * @param depth The depth of the title in the hierarchy, used for indentation.
     */
    void ExportTitle(Title* title, std::ofstream& file, int depth);
    
    /**
    * @brief Exports the titles history.
    */
    void ExportTitlesHistory();

    /**
     * @brief Exports the titles and cultural names localization files.
     *        It also removes former localization keys from the original mod files.
     * @param exportTitlesLocalization If true, exports the titles localization (names, adjectives and articles).
     * @param exportCulturalNamesLocalization If true, exports the cultural names localization.
     */
    void ExportLocalization(bool exportTitlesLocalization, bool exportCulturalNamesLocalization);

    /**
     * @brief Exports the titles localization.
     *        This includes names, adjectives, articles.
     */
    void ExportTitlesLocalization();

    /**
     * @brief Exports the cultural names localization.
     */
    void ExportCulturalNamesLocalization();

    ////////////////////////////////////////////////////
    
    /**
     * @brief Removes the former titles and cultural names localization
     *        from the original mod files in order to avoid duplicates.
     * @param exportTitlesLocalization If true, removes lines related to title localization (names, adjectives and articles).
     * @param exportCulturalNamesLocalization If true, removes lines related to cultural names localization
     */
    void DeleteLocalization(bool exportTitlesLocalization, bool exportCulturalNamesLocalization);
    
    ////////////////////////////////////////////////////

    /**
     * @brief Generates barony titles for provinces that don't have one.
     *        The generated baronies will be named after their province (e.g. `b_naoned` for the province of Naoned) and colored with the color of their province.
     */
    void GenerateMissingBaronies(ProvinceManager& provinceManager);

    /**
     * @brief Generates localization for titles that don't have one.
     *       The generated localization will be based on the title name (e.g. `Naoned` for the title c_naoned).
     */
    void GenerateTitlesLocalization(const std::string& lang, bool names, bool adjectives, bool articles);

    /**
     * @brief Harmonizes the colors of a list of titles by applying a hue and saturation shift to a base color.
     * @param titles The list of titles to harmonize the colors of.
     * @param color The base color to use for the harmonization.
     * @param hue The maximum hue shift to apply to the base color, in [0.f, 360.f].
     * @param saturation The maximum saturation shift to apply to the base color, in [0.f, 1.f].
     */
    void HarmonizeTitlesColors(std::span<Title*> titles, sf::Color color, float hue, float saturation);

private:
    Mod& m_Mod;

    std::map<std::string, UniquePtr<Title>> m_Titles;
    std::map<TitleType, std::vector<Title*>> m_TitlesByType;
    std::map<int, BaronyTitle*> m_BaroniesByProvinceId;

    // Map variables in common/landed_titles & history/titles with their respective filename.
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesVariables;
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesHistoryVariables;

    // Map of cultural names by languages, including names, articles and adjectives.
    std::map<std::string, std::map<std::string, std::string>> m_LocCulturalNames;

    // Relative file paths of the localization file that will be used for export.
    std::string m_TitlesLocalizationFileName;
    std::string m_CulturalNamesLocalizationFileName;
};