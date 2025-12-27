#pragma once

// #ifdef __linux__
// #include "util/SignalHandler.hpp"
// #endif

#include "menu/Menu.hpp"
#include "mod/Mod.hpp"
#include "Update.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    sf::RenderWindow& GetWindow();
    SharedPtr<Mod> GetMod();
    Update::Details& GetUpdateDetails();

    void DebugSettings();
    void OpenMod(SharedPtr<Mod> mod);
    void OpenMenu(SharedPtr<Menu> menu);

    void Init();
    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;
    
    // #ifdef __linux__
    // SignalHandler m_SignalHandler;
    // #endif

    SharedPtr<Menu> m_ActiveMenu;
    SharedPtr<Mod> m_ActiveMod;

    Update::Details m_UpdateDetails;
};