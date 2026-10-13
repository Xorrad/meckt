#pragma once

#include "religions/Rite.hpp"

class ReligionManager {
public:
    ReligionManager(Mod& mod);
    ReligionManager(const ReligionManager&) = delete;
    ReligionManager& operator=(const ReligionManager&) = delete;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of rites.
     * @return The total number of rites.
     */
    size_t CountRites() const;

    /**
     * @brief Checks if a rite exists.
     * @param name The name of the rite to search for.
     * @return True if the rite exists, false otherwise.
     */
    bool HasRite(const std::string& name) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves a rite by its name.
     * @param name The name of the rite to retrieve.
     * @return The pointer to the rite if it exists, nullptr otherwise.
     */
    Rite* GetRite(const std::string& name);
    
    /**
     * @brief Retrieves a rite by its name.
     * @param name The name of the rite to retrieve.
     * @return The pointer to the rite if it exists, nullptr otherwise.
     */
    const Rite* GetRite(const std::string& name) const;

    /**
     * @brief Generates the rites image.
     *        Each province's color is replaced by its rite color.
     * @note  This function can be expensive: to be used with sparingly.
     * @param provinceManager The province manager.
     * @param titleManager The title manager.
     * @return The generated rites image.
     */
    sf::Image GetRiteImage(ProvinceManager& provinceManager, TitleManager& titleManager) const;

    /**
     * @brief Builds the rite palette: palette[renderIndex] = that province's rite color.
     * @note  Requires `provinceManager.BuildProvinceIndex()` to have been called beforehand.
     * @param provinceManager The province manager.
     * @param titleManager The title manager.
     * @return The palette, indexed by the province manager's render index.
     */
    std::vector<sf::Color> GetRitePalette(ProvinceManager& provinceManager, TitleManager& titleManager) const;

    /**
     * @brief Retrieves a map of rites.
     * @return The map of rites.
     */
    std::unordered_map<std::string, UniquePtr<Rite>>& GetRites();
    
    /**
     * @brief Retrieves a map of rites.
     * @return The map of rites.
     */
    const std::unordered_map<std::string, UniquePtr<Rite>>& GetRites() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new rite.
     * @param rite The unique pointer of the rite.
     * @note If a rite with the same name already exists, it will be overwritten with the new one.
     */
    void AddRite(UniquePtr<Rite> rite);

    /**
     * @brief Removes a rite.
     * @param name The name of the rite to remove.
     */
    void RemoveRite(const std::string& name);

    //////////////////////////////////////////////////////

    /**
     * @brief Loads the rites (and not religion types).
     */
    void LoadRites();

    ////////////////////////////////////////////////////

private:
    Mod& m_Mod;
    std::unordered_map<std::string, UniquePtr<Rite>> m_Rites;
};