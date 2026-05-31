#pragma once

#include "cultures/Culture.hpp"

class CultureManager {
public:
    CultureManager(Mod& mod);
    CultureManager(const CultureManager&) = delete;
    CultureManager& operator=(const CultureManager&) = delete;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of cultures.
     * @return The total number of cultures.
     */
    size_t CountCultures() const;

    /**
     * @brief Checks if a culture exists.
     * @param name The name of the culture to search for.
     * @return True if the culture exists, false otherwise.
     */
    bool HasCulture(const std::string& name) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves a culture by its name.
     * @param name The name of the culture to retrieve.
     * @return The pointer to the culture if it exists, nullptr otherwise.
     */
    Culture* GetCulture(const std::string& name);
    
    /**
     * @brief Retrieves a culture by its name.
     * @param name The name of the culture to retrieve.
     * @return The pointer to the culture if it exists, nullptr otherwise.
     */
    const Culture* GetCulture(const std::string& name) const;

    /**
     * @brief Generates the cultures image.
     *        Each province's color is replaced by its culture color.
     * @note  This function can be expensive: to be used with sparingly.
     * @param provinceManager The province manager.
     * @param titleManager The title manager.
     * @return The generated cultures image.
     */
    sf::Image GetCultureImage(ProvinceManager* provinceManager, TitleManager* titleManager) const;

    /**
     * @brief Retrieves a map of cultures.
     * @return The map of cultures.
     */
    std::unordered_map<std::string, UniquePtr<Culture>>& GetCultures();
    
    /**
     * @brief Retrieves a map of cultures.
     * @return The map of cultures.
     */
    const std::unordered_map<std::string, UniquePtr<Culture>>& GetCultures() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new culture.
     * @param culture The unique pointer of the culture.
     * @note If a culture with the same name already exists, it will be overwritten with the new one.
     */
    void AddCulture(UniquePtr<Culture> culture);

    /**
     * @brief Removes a culture.
     * @param name The name of the culture to remove.
     */
    void RemoveCulture(const std::string& name);

    //////////////////////////////////////////////////////

    /**
     * @brief Loads the cultures.
     */
    void LoadCultures();

    ////////////////////////////////////////////////////

private:
    Mod& m_Mod;
    std::unordered_map<std::string, UniquePtr<Culture>> m_Cultures;
};