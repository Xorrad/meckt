#pragma once

class Menu {
public:
    /**
     * @brief Constructs a new Menu instance.
     * @param app The application instance.
     * @param name The name of the menu.
     */
    Menu(App& app, const std::string& name);
    virtual ~Menu() = default;

    /**
     * @brief Gets the application instance.
     * @return A reference to the application instance.
     */
    App& GetApp() const;

    /**
     * @brief Gets the name of the menu.
     * @return The name of the menu.
     */
    std::string GetName() const;

    /**
     * @brief Updates the menu.
     * @param delta The time elapsed since the last time update has been called.
     */
    virtual void Update(sf::Time delta);

    /**
     * @brief Handles SFML events for the menu.
     * @param event The event to handle.
     */
    virtual void Event(const sf::Event& event);

    /**
     * @brief Renders the menu.
     */
    virtual void Render();

protected:
    App& m_App;
    std::string m_Name;
};