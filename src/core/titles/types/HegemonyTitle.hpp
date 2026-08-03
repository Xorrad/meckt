#pragma once

class HegemonyTitle : public HighTitle {
public:
    /**
     * @brief Construct a new Hegemony Title object
     */
    HegemonyTitle();

    /**
     * @brief Construct a new Hegemony Title object
     * @param name The name of the hegemony
     * @param color The color of the hegemony
     * @param landless Whether the hegemony is landless (default: false)
     */
    HegemonyTitle(std::string name, sf::Color color, bool landless = false);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;

    //////////////////////////////////////////////////////
};