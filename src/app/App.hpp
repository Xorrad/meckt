#pragma once

#include "menu/Menu.hpp"
#include "core/mod/Mod.hpp"
#include "Update.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    /**
     * @brief Gets the main application window.
     * @return The main application window.
     */
    sf::RenderWindow& GetWindow();

    /**
     * @brief Gets the currently active menu.
     * @return The active menu.
     */
    Menu& GetMenu();

    /**
     * @brief Gets the currently open mod.
     * @return The active mod.
     */
    Mod& GetMod();

    /**
     * @brief Retrieves data about the latest meckt release.
     * @return The update details.
     */
    Update::Details& GetUpdateDetails();

    /**
     * @brief Applies the debug settings.
     */
    void DebugSettings();

    /**
     * @brief Opens a menu and close the current one.
     * @param menu The menu to open.
     */
    void OpenMenu(UniquePtr<Menu> menu);

    /**
     * @brief Opens a mod in the loading menu.
     * @param mod The mod to open.
     */
    void OpenMod(UniquePtr<Mod> mod);

    /**
     * @brief Opens a mod in the loading menu with the specified setup.
     * @param mod The mod to open.
     * @param setup The setup parameters for the editor.
     */
    void OpenMod(UniquePtr<Mod> mod, EditorSetup setup);

    /**
     * @brief Closes the currently open mod.
     */
    void CloseMod();

    /**
     * @brief Initializes the application.
     */
    void Init();

    /**
     * @brief Runs the application with the input, drawing, and update main loop.
     */
    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;

    UniquePtr<Menu> m_ActiveMenu;
    UniquePtr<Mod> m_ActiveMod;

    Update::Details m_UpdateDetails;
};