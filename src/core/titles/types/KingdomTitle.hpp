#pragma once

class KingdomTitle : public HighTitle {
public:
    /**
     * @brief Construct a new Kingdom Title object
     */
    KingdomTitle();

    /**
     * @brief Construct a new Kingdom Title object
     * @param name The name of the kingdom
     * @param color The color of the kingdom
     * @param landless Whether the kingdom is landless (default: false)
     */
    KingdomTitle(std::string name, sf::Color color, bool landless = false);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;

    //////////////////////////////////////////////////////
};