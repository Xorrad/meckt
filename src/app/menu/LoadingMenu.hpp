#pragma once

#include "Menu.hpp"
#include <thread>

class LoadingMenu : public Menu {
public:
    LoadingMenu(App& app, std::function<void()> completeCallback, std::function<void(const std::string&)> errorCallback);
    ~LoadingMenu();

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

    void SetState(LoadingState state);
    void Start(UniquePtr<Mod> mod);

private:
    std::atomic<LoadingState> m_State;
    std::atomic<bool> m_Cancelled;
    std::string m_LoadingError;
    UniquePtr<std::thread> m_Thread;

    std::function<void()> m_CompleteCallback;
    std::function<void(const std::string&)> m_ErrorCallback;
};