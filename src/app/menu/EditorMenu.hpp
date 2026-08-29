#pragma once

#include "Menu.hpp"
#include "selection/SelectionHandler.hpp"
#include "ui/components/modals/ScriptModal.hpp"

typedef unsigned int ImGuiID;

class EditorMenu : public Menu {
friend SelectionHandler;
public:
    EditorMenu(App& app);

    //////////////////////////////////////////////////////

    sf::Vector2f GetHoveredPosition();
    Province* GetHoveredProvince();
    MapMode GetMapMode() const;
    SelectionHandler& GetSelectionHandler();
    sf::View& GetCamera();
    ImGuiID GetDockspaceID() const;

    /**
     * @brief Retrieves the overall bounds of the map (in world/image space),
     *        independent of the camera or which chunks are currently drawn.
     */
    sf::FloatRect GetMapBounds() const;

    /**
     * @brief Retrieves the world-space rect currently visible to the camera.
     */
    sf::FloatRect GetVisibleWorldRect() const;

    //////////////////////////////////////////////////////

    void UpdateHoveringText();
    void ToggleCamera(bool enabled);

    //////////////////////////////////////////////////////

    void SwitchMapMode(MapMode mode, bool clearSelection = false);
    void RefreshMapMode(MapMode mode, bool clearSelection = false, bool resetFocus = false);
    void RefreshCurrentMapMode(bool clearSelection = false, bool resetFocus = true);
    void UpdateTexture(MapMode mode, bool resetFocus = true);
    void UpdateTextures();

    //////////////////////////////////////////////////////

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

    //////////////////////////////////////////////////////
    
    void InitSelectionCallbacks();
    void InitTabs();

    //////////////////////////////////////////////////////

    void SetupDockspace();
    void RenderMenuBar();
    void RenderMenuBarSelection();
    void RenderMenuBarTools();
    void RenderModals();

private:
    /**
     * @brief Binds every palette uniform (identity + 6 tier palettes, always
     *        relevant regardless of mode, plus the current mode's content
     *        palette) onto the provinces shader.
     */
    void BindPaletteUniforms(sf::Shader& shader);

    /**
     * @brief Draws the map (province/title modes via the index+palette shader,
     *        heightmap/rivers as plain chunked images) onto the given target,
     *        limited to the chunks intersecting `GetVisibleWorldRect()`.
     * @note  Assumes the target's view is already set to `m_Camera`.
     */
    void DrawMap(sf::RenderTarget& target);

    /**
     * @brief Draws the provinces adjacencies as solid colored lines between
     *        the start and stop positions.
     * @note  Assumes the target's view is already set to `m_Camera`.
     */
    void DrawAdjacencies(sf::RenderTarget& target);

private:
    Mod& m_Mod;

    MapMode m_MapMode;
    SelectionHandler m_SelectionHandler;
    ScriptModal m_ScriptModal;

    sf::View m_Camera;
    sf::Clock m_Clock;

    // Static geometry: which province owns each pixel, chunked so only
    // camera-visible tiles are drawn. Rebuilt only when province geometry changes.
    TextureChunkGrid m_ProvinceIndexGrid;
    // Raw source images (no per-province semantics), also chunked.
    TextureChunkGrid m_HeightmapGrid;
    TextureChunkGrid m_RiversGrid;

    // Dynamic appearance: small per-province color lookup tables, cheap to
    // rebuild on every edit (unlike the full-resolution images they replace).
    sf::Texture m_ProvinceIdentityPalette; // palette[i] = province i's own color
    std::map<TitleType, sf::Texture> m_TierPalettes; // palette[i] = province i's governing title's color, per tier
    std::map<MapMode, sf::Texture> m_ContentPalettes; // one per province-content mode (PROVINCES/FLAGS/TERRAIN/CLIMATE/WINTER_SEVERITY/CULTURE/FAITH)

    bool m_Dragging;
    sf::Vector2i m_LastMousePosition;
    sf::Vector2i m_LastClickMousePosition;
    float m_Zoom;
    float m_TotalZoom;

    sf::Text m_HoverText;
    sf::Text m_HoverTitleText;
    sf::RectangleShape m_HoverShape;

    std::map<Tabs, UniquePtr<Tab>> m_Tabs;
    bool m_DisplayBorders;
    std::string m_ModalName;

    bool m_ExitToMainMenu;

    ImGuiID m_DockspaceID;
};