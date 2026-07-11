#pragma once

#include "app/menu/ImGuiStyle.hpp"

namespace Components {

    /**
     * @brief Renders a combo box for selecting a terrain type from a provided map of terrain types
     * @param terrainTypes A reference to an ordered map of terrain types, where the key is the terrain type name and the value is the TerrainType object.
     * @param currentTerrainType A string representing the currently selected terrain type. If empty, no terrain type is selected.
     * @param onSelect A callback function that is called when a terrain type is selected.
     * @return True if the value was changed, false otherwise.
     */
    bool TerrainTypeCombo(const OrderedMap<std::string, TerrainType>& terrainTypes, const std::string& currentTerrainType, std::function<void(const TerrainType*)> onSelect);
    
    /**
     * @brief Renders a combo box for selecting a holding type from a provided map of holding types
     * @param holdingTypes A reference to an ordered map of holding types, where the key is the holding type name and the value is the HoldingType object.
     * @param currentHoldingType A string representing the currently selected holding type. If empty, no holding type is selected.
     * @param onSelect A callback function that is called when a holding type is selected.
     * @return True if the value was changed, false otherwise.
     */
    bool HoldingTypeCombo(const OrderedMap<std::string, HoldingType>& holdingTypes, const std::string& currentHoldingType, std::function<void(const HoldingType*)> onSelect);
    
    /**
     * @brief Renders a combo box for selecting a climate type from a provided map of climate types
     * @param currentClimateType The currently selected climate type.
     * @param onSelect A callback function that is called when a climate type is selected.
     * @return True if the value was changed, false otherwise.
     */
    bool ClimateTypeCombo(ClimateType currentClimateType, std::function<void(ClimateType)> onSelect);

}