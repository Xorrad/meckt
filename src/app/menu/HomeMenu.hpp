#pragma once

#include "Menu.hpp"

class HomeMenu : public Menu {
public:
    static bool s_PromptUpdate;

    HomeMenu(App& app);
    HomeMenu(App& app, std::string loadingError);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

    void RenderErrorModal();
    void RenderUpdateModal();

private:
    std::string m_LoadingError;
};