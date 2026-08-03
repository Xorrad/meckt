#pragma once

class TerrainType {
public:
    /**
     * @brief Constructs a new terrain type with default values.
     */
    TerrainType();

    /**
     * @brief Constructs a new terrain type with the specified name and color.
     * @param name The name of the terrain type.
     * @param color The color of the terrain type.
     */
    TerrainType(const std::string& name, const sf::Color& color);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the terrain type.
     * @return The name of the terrain type.
     */
    std::string GetName() const;

    /**
     * @brief Gets the color of the terrain type.
     * @return The color of the terrain type.
     */
    sf::Color GetColor() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the terrain type.
     * @param name The name of the terrain type.
     */
    void SetName(const std::string& name);

    /**
     * @brief Sets the color of the terrain type.
     * @param color The new color of the terrain type.
     */
    void SetColor(const sf::Color& color);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
    sf::Color m_Color;
};