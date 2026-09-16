#pragma once

enum class Tabs {
    TITLES,
    PROPERTIES,
    PROVINCES,
    REGIONS,
    LOG,
    CULTURAL_NAMES,
    ADJACENCIES,
};

class Tab {
public:
    Tab(std::string name, Tabs type, EditorMenu& menu, bool visible = true, bool focusedAtStartup = false);

    std::string GetName() const;
    Tabs GetType() const;
    bool IsVisible() const;
    bool& IsVisible();
    bool IsFocusedAtStartup() const;
    
    void SetVisible(bool visible);
    void SetFocusedAtStartup(bool focused);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

protected:
    App& GetApp();

protected:
    Mod& m_Mod;
    std::string m_Name;
    Tabs m_Type;
    EditorMenu& m_Menu;
    bool m_Visible;
    bool m_FocusedAtStartup;
};

#include "TitlesTab.hpp"
#include "PropertiesTab.hpp"
#include "ProvincesTab.hpp"
#include "RegionsTab.hpp"
#include "LogTab.hpp"
#include "CulturalNamesTab.hpp"
#include "AdjacenciesTab.hpp"