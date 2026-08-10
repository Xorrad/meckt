#pragma once

enum class SelectionType {
    NONE      = 0,
    TITLE     = 1,
    PROVINCE  = 2,
    POSITION  = 3
};

class SelectionHandler {
public:
    SelectionHandler(EditorMenu& menu);

    //////////////////////////////////////////////////////

    bool IsSelected(Province* province) const;
    bool IsSelected(const Title* title) const;
    bool IsSelected(const Region* region) const;
    bool IsSelected(const Adjacency* adjacency) const;

    /**
     * @brief Checks whether a province currently has an individual highlight color override.
     */
    bool HasProvinceHighlightColorOverride(Province* province) const;

    /**
     * @brief Checks whether any pick is currently in progress.
     */
    bool IsPicking() const;

    /**
     * @brief Checks whether a pick of the specified kind is currently in progress.
     */
    bool IsPicking(SelectionType type) const;

    //////////////////////////////////////////////////////

    std::span<Province*> GetProvinces();
    std::span<Title*> GetTitles();
    std::span<Region*> GetRegions();
    Adjacency* GetAdjacency();

    /**
     * @brief Retrieves the selection highlight palette texture (see
     *        BuildSelectionPalette()), to be bound to the shader every frame
     *        alongside the other palettes (EditorMenu::BindPaletteUniforms()).
     */
    const sf::Texture& GetSelectionPaletteTexture() const;

    /**
     * @brief Retrieves the shared color used to highlight the current selection on the map.
     */
    sf::Color GetHighlightColor() const;

    SelectionType GetSelectionType() const;

    //////////////////////////////////////////////////////

    /**
     * @brief Sets the shared color used to highlight the current selection on
     *        the map (individual provinces can override it, see
     *        SetProvinceHighlightColor()).
     * @param color The highlight color.
     */
    void SetHighlightColor(sf::Color color);

    /**
     * @brief Highlights a single province with its own color, independent of
     *        whether it's part of the current selection. Takes priority over
     *        the shared highlight color for that province.
     * @param province The province to highlight.
     * @param color The color to highlight it with.
     */
    void SetProvinceHighlightColor(Province* province, sf::Color color);

    //////////////////////////////////////////////////////

    /**
     * @brief Clears a province's individual highlight color override (see
     *        SetProvinceHighlightColor()). It reverts to the shared highlight
     *        color if still selected, or no highlight otherwise.
     */
    void ResetProvinceHighlightColor(Province* province);

    //////////////////////////////////////////////////////

    /**
     * @brief Prompts the user to click a province on the map.
     * @param onPick Called with the clicked province.
     * @param repeatWhileShiftHeld If true and LSHIFT is held on click, picking continues instead of stopping.
     * @param mapModeWhilePicking If set, the map switches to this mode while picking.
     * @param mapModeAfterPicking If set, the map switches to this mode once picking stops (success or cancel).
     */
    void PickProvince(
        std::function<void(Province*)> onPick,
        bool repeatWhileShiftHeld = false,
        std::optional<MapMode> mapModeWhilePicking = std::nullopt,
        std::optional<MapMode> mapModeAfterPicking = std::nullopt
    );

    /**
     * @brief Prompts the user to click a title on the map.
     * @param onPick Called with the clicked title; return false to reject the click and keep picking
     *               (e.g. the clicked title isn't of the expected tier), true to accept it.
     * @param repeatWhileShiftHeld If true and LSHIFT is held on an accepted click, picking continues instead of stopping.
     * @param mapModeWhilePicking If set, the map switches to this mode while picking.
     * @param mapModeAfterPicking If set, the map switches to this mode once picking stops (success or cancel).
     * @param cancelOnNonLeftClick If true (default), any click other than a plain left click (no LCtrl) cancels
     *                             picking. If false, such clicks are ignored by the pick and allowed to reach
     *                             other click handlers (e.g. so the normal RMB-wrap/LCtrl+LMB-unwrap title
     *                             gestures keep working while picking).
     */
    void PickTitle(
        std::function<bool(Title*)> onPick,
        bool repeatWhileShiftHeld = false,
        std::optional<MapMode> mapModeWhilePicking = std::nullopt,
        std::optional<MapMode> mapModeAfterPicking = std::nullopt,
        bool cancelOnNonLeftClick = true
    );

    /**
     * @brief Prompts the user to click an arbitrary position on the map.
     * @param onPick Called with the clicked world position.
     * @param mapModeWhilePicking If set, the map switches to this mode while picking.
     * @param mapModeAfterPicking If set, the map switches to this mode once picking stops (success or cancel).
     */
    void PickPosition(
        std::function<void(sf::Vector2f)> onPick,
        std::optional<MapMode> mapModeWhilePicking = std::nullopt,
        std::optional<MapMode> mapModeAfterPicking = std::nullopt
    );

    /**
     * @brief Cancels whichever pick (if any) is currently in progress. Safe to call when not picking.
     */
    void CancelPick();

    //////////////////////////////////////////////////////

    void Select(Province* province, bool update = true);
    void Select(Title* title);
    void Select(Region* region);
    void Select(Adjacency* adjacency);
    void Deselect(Province* province, bool update = true);
    void Deselect(Title* title);
    void Deselect(Region* region);
    void Deselect(Adjacency* adjacency);
    void ClearSelection();

    //////////////////////////////////////////////////////

    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback);

    void OnClick(sf::Mouse::Button button, sf::Vector2f position);
    void OnClick(sf::Mouse::Button button, Province* province);
    void OnClick(sf::Mouse::Button button, Province* province, Title* title);

    //////////////////////////////////////////////////////

    /**
     * @brief Rebuilds the selection highlight palette.
     * @note  - Called automatically by Select()/Deselect()/ClearSelection().
     *        - Called manually after editing a selected entity (e.g. adding/removing a dejure title).
     */
    void Update();

    /**
     * @brief Per-frame housekeeping (currently: Escape-to-cancel a pick).
     */
    void Tick();

    /**
     * @brief Draws the picking-in-progress UI.
     * @param target The render target to draw the prompt text onto.
     */
    void Render(sf::RenderTarget& target);

    //////////////////////////////////////////////////////

private:
    void BuildSelectionPalette();

private:
    EditorMenu& m_Menu;

    std::vector<Province*> m_Provinces;
    std::unordered_map<Province*, bool> m_ProvincesLookup;
    std::vector<Title*> m_Titles;
    std::vector<Region*> m_Regions;
    Adjacency* m_Adjacency;

    // Persistent callbacks for regular click listeners.
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)>> m_PositionCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)>> m_ProvinceCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)>> m_TitleCallbacks;

    // Callbacks for whichever pick is currently in progress.
    std::optional<std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)>> m_PositionPickCallback;
    std::optional<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)>> m_ProvincePickCallback;
    std::optional<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)>> m_TitlePickCallback;

    // Map mode to restore once the current pick stops.
    std::optional<MapMode> m_MapModeAfterPicking;

    // Default color used to highlight every currently-selected province/title/region/adjacency on the map.
    sf::Color m_HighlightColor;

    // Per-province highlight color overrides.
    // Provinces here are highlighted with their own color regardless of selection.
    std::unordered_map<Province*, sf::Color> m_ProvinceHighlightColorOverrides;

    // Small per-province lookup texture: alpha > 0 for provinces that should be
    // highlighted, sampled by the shader via the shared province index texture.
    sf::Texture m_SelectionPalette;

    sf::Text m_SelectionText;
    SelectionType m_SelectionType;
};
