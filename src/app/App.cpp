#include "App.hpp"
#include "imgui/imgui.hpp"
#include "menu/HomeMenu.hpp"
#include "menu/EditorMenu.hpp"
#include "menu/ImGuiStyle.hpp"

#if _WIN32
#include <windows.h>
#include <winuser.h>
#endif

App::App()
: m_ActiveMenu(MakeUnique<HomeMenu>(this)) {}

sf::RenderWindow& App::GetWindow() {
    return m_Window;
}

SharedPtr<Mod> App::GetMod() {
    return m_ActiveMod;
}

void App::DebugSettings() {
    this->OpenMod(MakeShared<Mod>("tests/mods/test_hae/"));
}

void App::OpenMenu(UniquePtr<Menu> menu) {
    m_ActiveMenu = std::move(menu);
}

void App::OpenMod(SharedPtr<Mod> mod) {
    m_ActiveMod = mod;
    Logger::Get()->Clear();
    mod->Load();
    this->OpenMenu(MakeUnique<EditorMenu>(this));
}

void App::Init() {
    // Initialize random seed.
    srand(time(NULL));

    // Initialize app-related functionalities.
    Configuration::Initialize();
    m_DeltaClock.restart();

    // Initialize SFML.
    m_Window.create(sf::VideoMode(Configuration::windowResolution.x, Configuration::windowResolution.y), "Meckt");
    // m_Window.setFramerateLimit(60);
    m_Window.setVerticalSyncEnabled(true);
    
    // Initialize ImGui.
    if(!ImGui::SFML::Init(m_Window)) {
        LOG_ERROR("Failed to initialize ImGui for SFML.", "");
        exit(EXIT_FAILURE);
    }

    ImGui::SetupSettings();
    ImGui::SetupFonts();
    ImGui::SetupStyle();

#ifdef DEBUG
    LOG_INFO("DEBUG_MODE is enabled", "");
    this->DebugSettings();
#elif _WIN32
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    ShowWindow(m_Window.getSystemHandle(), SW_MAXIMIZE);
#endif
}

void App::Run() {
    // Main application loop.
    while(m_Window.isOpen()) {

        // Handle SFML events.
        ImGuiIO& io = ImGui::GetIO();
        sf::Event event;
        while(m_Window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_Window, event);

            if(event.type == sf::Event::Closed) {
                m_Window.close();
                break;
            }

            if(event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                m_Window.setView(sf::View(visibleArea));
                Configuration::windowResolution = m_Window.getSize();
            }

            if((event.type == sf::Event::MouseButtonPressed
                || event.type == sf::Event::MouseButtonReleased
                || event.type == sf::Event::MouseEntered
                || event.type == sf::Event::MouseLeft
                || event.type == sf::Event::MouseMoved
                || event.type == sf::Event::MouseWheelMoved
                || event.type == sf::Event::MouseWheelScrolled)
                && io.WantCaptureMouse)
                continue;
                
            if((event.type == sf::Event::KeyPressed
                || event.type == sf::Event::KeyReleased)
                && io.WantCaptureKeyboard)
                continue;

            m_ActiveMenu->Event(event);
        }

        // Update between frames.
        sf::Time delta = m_DeltaClock.restart();
        ImGui::SFML::Update(m_Window, delta);
        m_ActiveMenu->Update(delta);

        // Drawing.
        m_Window.clear();
        
#ifdef DEBUG
        ImGui::ShowDemoWindow();
#endif
        m_ActiveMenu->Render();

        ImGui::SFML::Render(m_Window);
        m_Window.display();
    }

    ImGui::SFML::Shutdown();
}