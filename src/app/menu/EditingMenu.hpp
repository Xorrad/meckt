#pragma once

#include "Menu.hpp"

class EditingMenu : public Menu {
public:
    EditingMenu(App* app);

    SharedPtr<Province> GetHoveredProvince();
    std::vector<sf::Color> GetSelectedProvincesColor() const;

    void ToggleCamera(bool enabled);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Draw();

private:
    MapMode m_MapMode;

    sf::View m_Camera;
    sf::Clock m_Clock;
    sf::Texture m_MapTexture;
    sf::Sprite m_MapSprite;

    bool m_Dragging;
    sf::Vector2i m_LastMousePosition;
    sf::Vector2i m_LastClickMousePosition;
    float m_Zoom;
    float m_TotalZoom;

    sf::Text m_HoverText;

    std::vector<SharedPtr<Province>> m_SelectedProvinces;
};