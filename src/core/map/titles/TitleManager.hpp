#pragma once

#include "map/titles/Title.hpp"

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

    const std::map<std::string, SharedPtr<Jomini::Object>>& GetTitlesVariables() const;
    const std::map<std::string, SharedPtr<Jomini::Object>>& GetTitlesHistoryVariables() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new title.
     * @param title The unique pointer of the title.
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

    //////////////////////////////////////////////////////

    /**
     * @brief Loads all the titles defined in the mod's common/landed_titles directory.
     */
    void LoadTitles();


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
     * @param filePath The path to the history file.
     * @param data The parsed data of the history file.
     */
    void LoadTitlesHistoryFile(const std::string& filePath, SharedPtr<Jomini::Object> data);

    /**
     * @brief Recursively parses titles from a jomini data object.
     * @param filePath The path to the original file.
     * @param data The data of the titles to parse.
     * @return A list of non-owning pointers to the titles defined in the data.
     */
    std::vector<Title*> LoadTitlesFile(const std::string& filePath, SharedPtr<Jomini::Object> data);

    /**
     * @brief Parses a title from a jomini data object.
     * @param filePath The path to the title definition file.
     * @param name The name of the title to parse.
     * @param data The data of the title to parse.
     * @return A non-owning pointer to the parsed title.
     */
    Title* ParseTitle(const std::string& filePath, const std::string& name, SharedPtr<Jomini::Object> data);

    /////////////////////////////////////////////////////

    void Export();

private:
    Mod& m_Mod;

    std::map<std::string, UniquePtr<Title>> m_Titles;
    std::map<TitleType, std::vector<Title*>> m_TitlesByType;
    std::map<int, BaronyTitle*> m_BaroniesByProvinceId;

    // Map variables in common/landed_titles & history/titles with their respective filename.
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesVariables;
    std::map<std::string, SharedPtr<Jomini::Object>> m_TitlesHistoryVariables;
};