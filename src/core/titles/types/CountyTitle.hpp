#pragma once

class CountyTitle : public HighTitle {
public:
    /**
     * @brief Construct a new County Title object
     */
    CountyTitle();
    
    /**
     * @brief Construct a new County Title object
     * @param name The name of the county
     * @param color The color of the county
     * @param landless Whether the county is landless (default: false)
     */
    CountyTitle(std::string name, sf::Color color, bool landless = false);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;

    //////////////////////////////////////////////////////
};