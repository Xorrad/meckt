#pragma once

#include "util/SignalHandler.hpp"

#include "menu/Menu.hpp"
#include "mod/Mod.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    sf::RenderWindow& GetWindow();
    SharedPtr<Mod> GetMod();

    void DebugSettings();
    void OpenMod(SharedPtr<Mod> mod);
    void OpenMenu(UniquePtr<Menu> menu);

    void Init();
    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;
    SignalHandler m_SignalHandler;

    UniquePtr<Menu> m_ActiveMenu;
    SharedPtr<Mod> m_ActiveMod;
};