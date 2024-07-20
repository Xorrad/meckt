#pragma once

#include "Menu.hpp"

class EditingMenu : public Menu {
public:
    EditingMenu(App* app, UniquePtr<Mod> mod);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Draw();

private:
    UniquePtr<Mod> m_Mod;
    MapMode m_MapMode;

    sf::View m_Camera;
    sf::Texture m_MapTexture;
    sf::Sprite m_MapSprite;

    bool m_Dragging;
    sf::Vector2i m_LastMousePosition;
    float m_Zoom;
    float m_TotalZoom;

    sf::Text m_SelectionText;
};