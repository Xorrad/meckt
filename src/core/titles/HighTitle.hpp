#pragma once

class HighTitle : public Title {
public:

    /**
     * @brief Construct a new High Title object
     */
    HighTitle();
    
    /**
     * @brief Construct a new High Title object
     * @param name The name of the high title
     * @param color The color of the high title
     * @param landless Whether the high title is landless (default: false)
     */
    HighTitle(std::string name, sf::Color color, bool landless = false);
    
    virtual ~HighTitle() = default;

    //////////////////////////////////////////////////////
    
    /** @brief Checks if the high title has a de jure vassal title.
     * @param title The title to check.
     * @return True if the high title has the de jure vassal title, false otherwise.
     */
    bool HasDejureTitle(const Title* title) const;
    
    //////////////////////////////////////////////////////

    /**
     * @brief Gets the type of the title.
     * @return The type of the title.
     */
    virtual TitleType GetType() const = 0;

    /**
     * @brief Gets the de jure vassal titles.
     * @return A reference to the vector of de jure vassal titles.
     */
    std::vector<Title*>& GetDejureTitles();

    /**
     * @brief Gets the de jure vassal titles.
     * @return A reference to the vector of de jure vassal titles.
     */
    const std::vector<Title*>& GetDejureTitles() const;

    /**
     * @brief Gets the capital title.
     * @return A pointer to the capital title.
     */
    CountyTitle* GetCapitalTitle();

    /**
     * @brief Gets the title's position in the provinces image.
     * @param provinceManager The province manager.
     * @return The title position.
     */
    virtual sf::Vector2i GetImagePosition(const ProvinceManager& provinceManager) const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the capital title.
     * @param title The capital title.
     */
    void SetCapitalTitle(CountyTitle* title);

    /**
     * @brief Sets the selection focus for this title, and its dejure vassals.
     * @param focus The selection focus.
     */
    virtual void SetSelectionFocus(bool focus) override;

    //////////////////////////////////////////////////////

    /**
     * @brief Adds a de jure vassal title.
     * @param title The title to add.
     */
    void AddDejureTitle(Title* title);

    /**
     * @brief Removes a de jure vassal title.
     * @param title The title to remove.
     */
    void RemoveDejureTitle(Title* title);

    /**
     * @brief Clears all de jure vassal titles.
     */
    void ClearDejureTitles();

    //////////////////////////////////////////////////////

protected:
    std::vector<Title*> m_DejureTitles;
    CountyTitle* m_CapitalTitle;
};