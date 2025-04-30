#pragma once

#include "Menu.hpp"

class HomeMenu : public Menu {
public:
    HomeMenu(App* app);
    HomeMenu(App* app, std::string loadingError);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

private:
    std::string m_LoadingError;
};