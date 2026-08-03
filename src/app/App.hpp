#pragma once

#include "menu/Menu.hpp"
#include "core/mod/Mod.hpp"
#include "Update.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    sf::RenderWindow& GetWindow();
    Mod& GetMod();
    Update::Details& GetUpdateDetails();

    void DebugSettings();
    void OpenMenu(UniquePtr<Menu> menu);
    void OpenMod(UniquePtr<Mod> mod);
    void CloseMod();

    void Init();
    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;

    UniquePtr<Menu> m_ActiveMenu;
    UniquePtr<Mod> m_ActiveMod;

    Update::Details m_UpdateDetails;
};