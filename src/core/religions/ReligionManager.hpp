#pragma once

#include "religions/Faith.hpp"

class ReligionManager {
public:
    ReligionManager(Mod& mod);
    ReligionManager(const ReligionManager&) = delete;
    ReligionManager& operator=(const ReligionManager&) = delete;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Counts the total number of faiths.
     * @return The total number of faiths.
     */
    size_t CountFaiths() const;

    /**
     * @brief Checks if a faith exists.
     * @param name The name of the faith to search for.
     * @return True if the faith exists, false otherwise.
     */
    bool HasFaith(const std::string& name) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves a faith by its name.
     * @param name The name of the faith to retrieve.
     * @return The pointer to the faith if it exists, nullptr otherwise.
     */
    Faith* GetFaith(const std::string& name);
    
    /**
     * @brief Retrieves a faith by its name.
     * @param name The name of the faith to retrieve.
     * @return The pointer to the faith if it exists, nullptr otherwise.
     */
    const Faith* GetFaith(const std::string& name) const;

    /**
     * @brief Generates the faiths image.
     *        Each province's color is replaced by its faith color.
     * @note  This function can be expensive: to be used with sparingly.
     * @param provinceManager The province manager.
     * @param titleManager The title manager.
     * @return The generated faiths image.
     */
    sf::Image GetFaithImage(ProvinceManager& provinceManager, TitleManager& titleManager) const;

    /**
     * @brief Retrieves a map of faiths.
     * @return The map of faiths.
     */
    std::unordered_map<std::string, UniquePtr<Faith>>& GetFaiths();
    
    /**
     * @brief Retrieves a map of faiths.
     * @return The map of faiths.
     */
    const std::unordered_map<std::string, UniquePtr<Faith>>& GetFaiths() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a new faith.
     * @param faith The unique pointer of the faith.
     * @note If a faith with the same name already exists, it will be overwritten with the new one.
     */
    void AddFaith(UniquePtr<Faith> faith);

    /**
     * @brief Removes a faith.
     * @param name The name of the faith to remove.
     */
    void RemoveFaith(const std::string& name);

    //////////////////////////////////////////////////////

    /**
     * @brief Loads the faiths (and not religion types).
     */
    void LoadFaiths();

    ////////////////////////////////////////////////////

private:
    Mod& m_Mod;
    std::unordered_map<std::string, UniquePtr<Faith>> m_Faiths;
};