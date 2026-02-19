#include "App.hpp"
#include "menu/HomeMenu.hpp"
#include "menu/LoadingMenu.hpp"
#include "menu/EditorMenu.hpp"
#include "menu/ImGuiStyle.hpp"

#include <imgui/imgui.hpp>

#if _WIN32
#include <windows.h>
#include <winuser.h>
#endif

App::App() :
    m_ActiveMenu(MakeUnique<HomeMenu>(*this)),
    m_ActiveMod(nullptr)
{}

sf::RenderWindow& App::GetWindow() {
    return m_Window;
}

Mod& App::GetMod() {
    return *m_ActiveMod;
}

Update::Details& App::GetUpdateDetails() {
    return m_UpdateDetails;
}

void App::DebugSettings() {
    // this->OpenMod(MakeShared<Mod>("tests/mods/test_hae/"));
}

void App::OpenMenu(UniquePtr<Menu> menu) {
    m_ActiveMenu = std::move(menu);
}

void App::OpenMod(UniquePtr<Mod> mod) {
    if (mod == nullptr)
        return;

    // Remove the mod from the recent mods list
    // and add it back at the top of the list.
    Configuration::recentMods.erase(std::remove(Configuration::recentMods.begin(), Configuration::recentMods.end(), mod->GetDir()), Configuration::recentMods.end());
    Configuration::recentMods.push_front(mod->GetDir());

    m_ActiveMod = std::move(mod);
    Logger::Get()->Clear();

    UniquePtr<LoadingMenu> menu = MakeUnique<LoadingMenu>(
        *this,
        [&](){ this->OpenMenu(MakeUnique<EditorMenu>(*this)); },
        [&](const std::string& error){ this->OpenMenu(MakeUnique<HomeMenu>(*this, error)); }
    );
    menu->Start();
    m_ActiveMenu = std::move(menu);
}

void App::CloseMod() {
    m_ActiveMod.reset();
	m_ActiveMenu = MakeUnique<HomeMenu>(*this);
}

void App::Init() {
    // Initialize random seed.
    srand(time(NULL));

    // Initialize app-related functionalities.
    Configuration::Initialize();
    m_DeltaClock.restart();

    // Initialize SFML.
    m_Window.create(sf::VideoMode({ Configuration::windowResolution.x, Configuration::windowResolution.y }), fmt::format("meckt - v{}", Configuration::buildVersion));
    m_Window.setVerticalSyncEnabled(true);
#if _WIN32
    ShowWindow(m_Window.getNativeHandle(), SW_MAXIMIZE);
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
        while (const std::optional<sf::Event> event = m_Window.pollEvent()) {
            ImGui::SFML::ProcessEvent(m_Window, *event);

            if (event->is<sf::Event::Closed>()) {
                m_Window.close();
                break;
            }

            else if(const auto* resize = event->getIf<sf::Event::Resized>()) {
                sf::FloatRect visibleArea(sf::Vector2f(0, 0), sf::Vector2f(resize->size.x, resize->size.y));
                m_Window.setView(sf::View(visibleArea));
                Configuration::windowResolution = m_Window.getSize();
            }

            else if((event->is<sf::Event::MouseButtonPressed>()
                || event->is<sf::Event::MouseButtonReleased>()
                || event->is<sf::Event::MouseEntered>()
                || event->is<sf::Event::MouseLeft>()
                || event->is<sf::Event::MouseMoved>()
                || event->is<sf::Event::MouseWheelScrolled>())
                && io.WantCaptureMouse)
                continue;
                
            else if((event->is<sf::Event::KeyPressed>()
                || event->is<sf::Event::KeyReleased>())
                && io.WantCaptureKeyboard)
                continue;

            if (event.has_value())
                m_ActiveMenu->Event(*event);
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
    Configuration::Deinitialize();
}