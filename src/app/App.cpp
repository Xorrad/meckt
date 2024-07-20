#include "App.hpp"
#include "imgui/imgui.hpp"
#include "menu/HomeMenu.hpp"

App::App()
: m_ActiveMenu(MakeUnique<HomeMenu>(this)) {}

void App::OpenMenu(UniquePtr<Menu> menu) {
    m_ActiveMenu = std::move(menu);
}

void App::Run() {
    // Initialize app-related functionalities.
    Logger::ClearLogs();
    Configuration::Initialize();
    m_DeltaClock.restart();

    // Initialize SFML.
    m_Window.create(sf::VideoMode(Configuration::windowResolution.x, Configuration::windowResolution.y), "CK3 - XME");
    m_Window.setFramerateLimit(60);
    
    // Initialize ImGui.
    if(!ImGui::SFML::Init(m_Window)) {
        ERROR("Failed to initialize ImGui for SFML.", "");
        exit(EXIT_FAILURE);
    }

    // Here temporarily.
    sf::Text text;
    text.setCharacterSize(12);
    text.setString("No map loaded.");
    text.setFillColor(sf::Color::White);
    text.setFont(Configuration::fonts.Get(Fonts::FIGTREE));
    text.setPosition({5, m_Window.getSize().y - text.getGlobalBounds().height - 10});

    // Main application loop.
    while (m_Window.isOpen()) {

        // Handle SFML events.
        sf::Event event;
        while (m_Window.pollEvent(event)) {
            m_ActiveMenu->Event(event);

            if (event.type == sf::Event::Closed) {
                m_Window.close();
                break;
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                m_Window.setView(sf::View(visibleArea));
                Configuration::windowResolution = m_Window.getSize();
            }

            ImGui::SFML::ProcessEvent(m_Window, event);
            if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
                break;

            if(event.type == sf::Event::MouseButtonPressed) {
                if(event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i localPosition = sf::Mouse::getPosition(m_Window);
                }
            }
        }

        // Update between frames.
        sf::Time delta = m_DeltaClock.restart();
        ImGui::SFML::Update(m_Window, delta);
        m_ActiveMenu->Update(delta);

        // Drawing.
        m_Window.clear();
        m_ActiveMenu->Draw(m_Window);

        // ImGui::ShowDemoWindow();
        m_Window.draw(text);
        ImGui::SFML::Render(m_Window);

        m_Window.display();
    }

    ImGui::SFML::Shutdown();
}