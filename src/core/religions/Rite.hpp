#pragma once

class Rite {
public:
    /**
     * @brief Constructs a new rite.
     */
    Rite();

    /**
     * @brief Constructs a new rite with the given name and color.
     * @param name The name of the rite.
     * @param color The color of the rite. 
     */
    Rite(const std::string& name, const sf::Color& color);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the rite.
     * @return The name of the rite.
     */
    std::string GetName() const;

    /**
     * @brief Gets the color of the rite.
     * @return The color of the rite.
     */
    sf::Color GetColor() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the rite.
     * @param name The name to set.
     */
    void SetName(const std::string& name);

    /**
     * @brief Sets the color of the rite.
     * @param color The color to set.
     */
    void SetColor(const sf::Color& color);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
    sf::Color m_Color;
};