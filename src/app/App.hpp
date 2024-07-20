#pragma once

#include "util/SignalHandler.hpp"

class App {
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void Run();

private:
    sf::RenderWindow m_Window;
    sf::Clock m_DeltaClock;
    SignalHandler m_SignalHandler;
};