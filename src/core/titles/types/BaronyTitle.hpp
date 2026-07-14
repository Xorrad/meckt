#pragma once

class BaronyTitle : public Title {
public:
    /**
     * @brief Construct a new Barony Title object
    */
    BaronyTitle();

    /**
     * @brief Construct a new Barony Title object
     * @param name The name of the barony
     * @param color The color of the barony
     * @param landless Whether the barony is landless (default: false)
     */
    BaronyTitle(std::string name, sf::Color color, bool landless = false);

    /**
     * @brief Construct a new Barony Title object
     * @param name The name of the barony
     * @param color The color of the barony
     * @param landless Whether the barony is landless
     * @param provinceId The ID of the province to which the barony belongs
     */
    BaronyTitle(std::string name, sf::Color color, bool landless, int provinceId);

    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const;
    
    /**
     * @brief Gets the ID of the province to which the barony belongs.
     * @return The ID of the province.
     */
    int GetProvinceId() const;

    /**
     * @brief Checks if the barony has selection focus.
     * @return True if the barony has selection focus, false otherwise.
     */
    virtual bool HasSelectionFocus() const override;

    /**
     * @brief Gets the position of the barony in the provinces image.
     * @param provinceManager The province manager.
     * @return The position of the barony in the provinces image.
     */
    virtual sf::Vector2i GetImagePosition(const ProvinceManager& provinceManager) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the ID of the province to which the barony belongs.
     * @param id The new ID of the province.
     */
    void SetProvinceId(int id);

    //////////////////////////////////////////////////////

private:
    int m_ProvinceId;
};