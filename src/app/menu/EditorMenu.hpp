#pragma once

#include "Menu.hpp"

class MapSelectionHandler {
public:
    MapSelectionHandler(EditorMenu* menu);
    
    void Select(const SharedPtr<Province>& province);
    void Select(const SharedPtr<Title>& title);
    void Deselect(const SharedPtr<Province>& province);
    void Deselect(const SharedPtr<Title>& title);
    void ClearSelection();

    bool IsSelected(const SharedPtr<Province>& province);
    bool IsSelected(const SharedPtr<Title>& title);

    std::vector<SharedPtr<Province>>& GetProvinces();
    std::vector<SharedPtr<Title>>& GetTitles();
    std::vector<sf::Glsl::Vec4>& GetColors();
    std::size_t GetCount() const;
private:
    void UpdateColors();
    void UpdateShader();

private:
    EditorMenu* m_Menu;

    std::vector<SharedPtr<Province>> m_Provinces;
    std::vector<SharedPtr<Title>> m_Titles;

    // This vector is passed to the fragment shader to change
    // color of pixels in selected provinces.
    std::vector<sf::Glsl::Vec4> m_Colors;

    // Keep track of the number of provinces that are
    // selected, especially for titles. The value is updated
    // in UpdateColors() along side the colors.
    std::size_t m_Count;
};

class EditorMenu : public Menu {
friend MapSelectionHandler;
public:
    EditorMenu(App* app);

    SharedPtr<Province> GetHoveredProvince();
    MapSelectionHandler& GetSelectionHandler();

    void UpdateHoveringText();
    void ToggleCamera(bool enabled);
    void SwitchMapMode(MapMode mode);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();
    
    void InitTabs();
    void SetupDockspace();
    void RenderMenuBar();

private:
    MapMode m_MapMode;
    MapSelectionHandler m_SelectionHandler;

    sf::View m_Camera;
    sf::Clock m_Clock;
    sf::Texture m_ProvinceTexture;
    sf::Texture m_MapTexture;
    sf::Sprite m_MapSprite;

    bool m_Dragging;
    sf::Vector2i m_LastMousePosition;
    sf::Vector2i m_LastClickMousePosition;
    float m_Zoom;
    float m_TotalZoom;

    sf::Text m_HoverText;

    std::map<Tabs, SharedPtr<Tab>> m_Tabs;
    bool m_ExitToMainMenu;
};