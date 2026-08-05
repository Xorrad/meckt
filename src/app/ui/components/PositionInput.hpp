#pragma once

namespace Components {
    /**
     * @brief Renders a few widgets to pick a position on the map.
     *   - A text input for the (x, y) coordinates.  
     *   - A picker button to pick a position on the map.  
     * @param label The name of the input.
     * @param menu A reference to the editor menu required for the selection handler.
     * @param provinceManager The province manager.
     * @param currentPosition The current position vector.
     * @param onChange A callback function that is called when a new position is chosen.
     * @return `True` if the value was changed, `false` otherwise.
     */
    bool PositionInput(std::string_view label, EditorMenu& menu, ProvinceManager& provinceManager, sf::Vector2u currentPosition, std::function<void(sf::Vector2u)> onChange);
}