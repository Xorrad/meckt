#include "App.hpp"
#include "imgui/imgui.hpp"
#include "menu/HomeMenu.hpp"
#include "menu/LoadingMenu.hpp"
#include "menu/EditorMenu.hpp"
#include "menu/ImGuiStyle.hpp"

#if _WIN32
#include <windows.h>
#include <winuser.h>
#endif

App::App()
: m_ActiveMenu(MakeShared<HomeMenu>(this)) {}

sf::RenderWindow& App::GetWindow() {
    return m_Window;
}

SharedPtr<Mod> App::GetMod() {
    return m_ActiveMod;
}

Update::Details& App::GetUpdateDetails() {
    return m_UpdateDetails;
}

void App::DebugSettings() {
    this->OpenMod(MakeShared<Mod>("tests/mods/test_hae/"));
}

void App::OpenMenu(SharedPtr<Menu> menu) {
    m_ActiveMenu = menu;
}

void App::OpenMod(SharedPtr<Mod> mod) {
    // Remove the mod from the recent mods list
    // and add it back at the top of the list.
    Configuration::recentMods.erase(std::remove(Configuration::recentMods.begin(), Configuration::recentMods.end(), mod->GetDir()), Configuration::recentMods.end());
    Configuration::recentMods.push_front(mod->GetDir());

    m_ActiveMod = mod;
    Logger::Get()->Clear();

    SharedPtr<LoadingMenu> menu = MakeShared<LoadingMenu>(
        this,
        [&](){ this->OpenMenu(MakeShared<EditorMenu>(this)); },
        [&](const std::string& error){ this->OpenMenu(MakeShared<HomeMenu>(this, error)); }
    );
    m_ActiveMenu = menu;
    menu->Start();
}

void App::Init() {
    // Initialize random seed.
    srand(time(NULL));

    // Initialize app-related functionalities.
    Configuration::Initialize();
    m_DeltaClock.restart();

    // Initialize SFML.
    m_Window.create(sf::VideoMode(Configuration::windowResolution.x, Configuration::windowResolution.y), fmt::format("meckt - v{}", Configuration::buildVersion));
    m_Window.setVerticalSyncEnabled(true);
#if _WIN32
    ShowWindow(m_Window.getSystemHandle(), SW_MAXIMIZE);
#endif

    // Initialize ImGui.
    if(!ImGui::SFML::Init(m_Window)) {
        LOG_ERROR("Failed to initialize ImGui for SFML.", "");
        exit(EXIT_FAILURE);
    }

    ImGui::SetupSettings();
    ImGui::SetupFonts();
    ImGui::SetupStyle();

    m_UpdateDetails = Update::QueryDetails();
    if (!m_UpdateDetails.error.empty())
        LOG_ERROR("Failed to check for updates: {}", m_UpdateDetails.error);

#ifdef DEBUG
    LOG_INFO("DEBUG_MODE is enabled", "");
    this->DebugSettings();
#elif _WIN32
    ShowWindow(GetConsoleWindow(), SW_HIDE);
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

    // TODO: Move that else where, i.e make a App::Exit or Stop function
    // that is called whenever the app closes.
    Configuration::Save();
}