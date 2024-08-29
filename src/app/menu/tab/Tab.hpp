#pragma once

enum class Tabs {
    TITLES,
    PROPERTIES,
};

class Tab {
public:
    Tab(std::string name, Tabs type, EditorMenu* menu, bool visible = true);

    std::string GetName() const;
    Tabs GetType() const;
    bool IsVisible() const;
    bool& IsVisible();

    void SetVisible(bool visible);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

protected:
    App* GetApp();
    SharedPtr<Mod> GetMod();

protected:
    std::string m_Name;
    Tabs m_Type;
    EditorMenu* m_Menu;
    bool m_Visible;
};

#include "TitlesTab.hpp"
#include "PropertiesTab.hpp"