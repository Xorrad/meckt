#pragma once

#include "Menu.hpp"

class EditingMenu : public Menu {
public:
    EditingMenu(App* app);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Draw(sf::RenderWindow& window);

private:
};