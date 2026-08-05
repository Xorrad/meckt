#pragma once

namespace Components {
    /**
     * @brief Renders a few widgets to select a province.
     *   - A text input for the province id.  
     *   - A picker button to pick a province on the map.  
     *   - An alert if the selected province doesn't exist.
     * @param label The name of the input.
     * @param menu A reference to the editor menu required to switch map mode.
     * @param provinceManager The province manager owning the selected province.
     * @param currentProvinceId The id of the province currently selected.
     * @param onChange A callback function that is called when a new province is selected.
     * @return `True` if the value was changed, `false` otherwise.
     */
    bool ProvinceInput(std::string_view label, EditorMenu& menu, ProvinceManager& provinceManager, int currentProvinceId, std::function<void(int)> onChange);
}