#pragma once

#include "Menu.hpp"
#include "selection/SelectionHandler.hpp"

class EditorMenu : public Menu {
friend SelectionHandler;
public:
    EditorMenu(App* app);

    SharedPtr<Province> GetHoveredProvince();
    MapMode GetMapMode() const;
    SelectionHandler& GetSelectionHandler();

    void UpdateHoveringText();
    void ToggleCamera(bool enabled);

    void SwitchMapMode(MapMode mode, bool clearSelection = false);
    void RefreshMapMode(bool clearSelection = false, bool resetFocus = true);
    void UpdateTexture(MapMode mode, bool resetFocus = true);
    void UpdateTextures();

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();
    
    void InitSelectionCallbacks();
    void InitTabs();

    void SetupDockspace();
    void RenderMenuBar();

private:
    MapMode m_MapMode;
    SelectionHandler m_SelectionHandler;

    sf::View m_Camera;
    sf::Clock m_Clock;

    std::map<MapMode, sf::Texture> m_MapTextures;
    sf::Sprite m_MapSprite;

    bool m_Dragging;
    sf::Vector2i m_LastMousePosition;
    sf::Vector2i m_LastClickMousePosition;
    float m_Zoom;
    float m_TotalZoom;

    sf::Text m_HoverText;

    std::map<Tabs, SharedPtr<Tab>> m_Tabs;
    bool m_DisplayBorders;

    bool m_ExitToMainMenu;
};