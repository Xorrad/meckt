#pragma once

class Culture {
public:
    /**
     * @brief Constructs a new culture with default values.
     */
    Culture();

    /**
     * @brief Constructs a new culture with the specified name and color.
     * @param name The name of the culture.
     * @param color The color of the culture.
     */
    Culture(const std::string& name, const sf::Color& color);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the culture.
     * @return The name of the culture.
     */
    std::string GetName() const;

    /**
     * @brief Gets the color of the culture.
     * @return The color of the culture.
     */
    sf::Color GetColor() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the culture.
     * @param name The new name of the culture.
     */
    void SetName(const std::string& name);

    /**
     * @brief Sets the color of the culture.
     * @param color The new color of the culture.
     */
    void SetColor(const sf::Color& color);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
    sf::Color m_Color;
};