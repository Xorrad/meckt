#pragma once

#include "provinces/Province.hpp"

class DefineManager {
public:
    DefineManager(Mod& mod);
    DefineManager(const DefineManager&) = delete;
    DefineManager& operator=(const DefineManager&) = delete;

    //////////////////////////////////////////////////////

    /**
     * @brief Returns the internal reference to the mod.
     * @return The reference to the mod.
     */
    Mod& GetMod();

    /**
     * @brief Returns the water level defined in the mod or the vanilla one if not defined.
     * @return A float representing the water level (3.8f by default).
     */
    float GetWaterLevel() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Loads the defines files from the mod.
     */
    void LoadDefines();

    ////////////////////////////////////////////////////



    ////////////////////////////////////////////////////

private:
    Mod& m_Mod;

    float m_WaterLevel; // 3.8f
};