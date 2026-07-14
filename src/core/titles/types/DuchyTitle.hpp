#pragma once

class DuchyTitle : public HighTitle {
public:
    /**
     * @brief Construct a new Duchy Title object
     */
    DuchyTitle();

    /**
     * @brief Construct a new Duchy Title object
     * @param name The name of the duchy
     * @param color The color of the duchy
     * @param landless Whether the duchy is landless (default: false)
     */
    DuchyTitle(std::string name, sf::Color color, bool landless = false);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;

    //////////////////////////////////////////////////////
};