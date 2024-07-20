#pragma once

#include "util/SignalHandler.hpp"

#include "menu/Menu.hpp"
#include "mod/Mod.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void OpenMenu(UniquePtr<Menu> menu);
    void OpenMod(UniquePtr<Mod> mod);
    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;
    SignalHandler m_SignalHandler;

    UniquePtr<Menu> m_ActiveMenu;
    UniquePtr<Mod> m_ActiveMod;
};