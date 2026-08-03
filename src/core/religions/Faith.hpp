#pragma once

class Faith {
public:
    /**
     * @brief Constructs a new faith.
     */
    Faith();

    /**
     * @brief Constructs a new faith with the given name and color.
     * @param name The name of the faith.
     * @param color The color of the faith. 
     */
    Faith(const std::string& name, const sf::Color& color);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the name of the faith.
     * @return The name of the faith.
     */
    std::string GetName() const;

    /**
     * @brief Gets the color of the faith.
     * @return The color of the faith.
     */
    sf::Color GetColor() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the name of the faith.
     * @param name The name to set.
     */
    void SetName(const std::string& name);

    /**
     * @brief Sets the color of the faith.
     * @param color The color to set.
     */
    void SetColor(const sf::Color& color);

    //////////////////////////////////////////////////////

private:
    std::string m_Name;
    sf::Color m_Color;
};