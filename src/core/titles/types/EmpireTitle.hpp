#pragma once

class EmpireTitle : public HighTitle {
public:
    /**
     * @brief Construct a new Empire Title object
     */
    EmpireTitle();

    /**
     * @brief Construct a new Empire Title object
     * @param name The name of the empire
     * @param color The color of the empire
     * @param landless Whether the empire is landless (default: false)
     */
    EmpireTitle(std::string name, sf::Color color, bool landless = false);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;

    //////////////////////////////////////////////////////
};