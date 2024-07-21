#include "App.hpp"
#include "imgui/imgui.hpp"
#include "menu/HomeMenu.hpp"
#include "menu/EditingMenu.hpp"
#include "menu/ImGuiStyle.hpp"

App::App()
: m_ActiveMenu(MakeUnique<HomeMenu>(this)) {}

sf::RenderWindow& App::GetWindow() {
    return m_Window;
}

SharedPtr<Mod> App::GetMod() {
    return m_ActiveMod;
}

void App::DebugSettings() {
    this->OpenMod(MakeShared<Mod>("test_mod/"));
}

void App::OpenMenu(UniquePtr<Menu> menu) {
    m_ActiveMenu = std::move(menu);
}

void App::OpenMod(SharedPtr<Mod> mod) {
    m_ActiveMod = mod;
    mod->Load();
    this->OpenMenu(MakeUnique<EditingMenu>(this));
}

void App::Init() {
    // Initialize app-related functionalities.
    Logger::ClearLogs();
    Configuration::Initialize();
    m_DeltaClock.restart();

    // Initialize SFML.
    m_Window.create(sf::VideoMode(Configuration::windowResolution.x, Configuration::windowResolution.y), "Xorrad's Map Editor");
    // m_Window.setFramerateLimit(60);
    // m_Window.setVerticalSyncEnabled(true);
    
    // Initialize ImGui.
    if(!ImGui::SFML::Init(m_Window)) {
        ERROR("Failed to initialize ImGui for SFML.", "");
        exit(EXIT_FAILURE);
    }

    ImGui::SetupSettings();
    ImGui::SetupStyle();
}

void App::Run() {
    // Main application loop.
    while (m_Window.isOpen()) {

        // Handle SFML events.
        sf::Event event;
        while (m_Window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(m_Window, event);

            if (event.type == sf::Event::Closed) {
                m_Window.close();
                break;
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                m_Window.setView(sf::View(visibleArea));
                Configuration::windowResolution = m_Window.getSize();
            }

            if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
                break;

            m_ActiveMenu->Event(event);
        }

        // Update between frames.
        sf::Time delta = m_DeltaClock.restart();
        ImGui::SFML::Update(m_Window, delta);
        m_ActiveMenu->Update(delta);

        // Drawing.
        m_Window.clear();
        m_ActiveMenu->Draw();

        ImGui::ShowDemoWindow();
        ImGui::SFML::Render(m_Window);

        m_Window.display();
    }

    ImGui::SFML::Shutdown();
}