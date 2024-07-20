#pragma once

class Menu {
public:
    Menu(App* app, const std::string& name);

    App* GetApp() const;
    std::string GetName() const;

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Draw();

protected:
    App* m_App;
    std::string m_Name;
};