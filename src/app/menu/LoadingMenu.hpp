#pragma once

#include "Menu.hpp"
#include <thread>

class LoadingMenu : public Menu {
public:
    LoadingMenu(App& app, std::function<void()> completeCallback, std::function<void(const std::string&)> errorCallback);
    ~LoadingMenu() = default;

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

    void SetState(LoadingState state);
    void Start();

private:
    LoadingState m_State;
    std::string m_LoadingError;
    SharedPtr<sf::Thread> m_Thread;

    std::function<void()> m_CompleteCallback;
    std::function<void(const std::string&)> m_ErrorCallback;
};