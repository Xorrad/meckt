#include "EditingMenu.hpp"
#include "HomeMenu.hpp"
#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "imgui/imgui.hpp"

EditingMenu::EditingMenu(App* app, UniquePtr<Mod> mod)
: Menu(app, "Editor"), m_Mod(std::move(mod)), m_MapMode(MapMode::PROVINCES) {
    m_Mod->Load();
    m_Mod->LoadMapModeTexture(m_MapTexture, m_MapMode);
    m_MapSprite.setTexture(m_MapTexture);

    m_Camera = m_App->GetWindow().getDefaultView();

    m_Dragging = false;
    m_LastMousePosition = {0, 0};
    m_Zoom = 1.f;
    m_TotalZoom = 1.f;

    m_SelectionText.setCharacterSize(12);
    m_SelectionText.setString("No province selected.");
    m_SelectionText.setFillColor(sf::Color::White);
    m_SelectionText.setFont(Configuration::fonts.Get(Fonts::FIGTREE));
    m_SelectionText.setPosition({5, m_App->GetWindow().getSize().y - m_SelectionText.getGlobalBounds().height - 10});
}

void EditingMenu::Update(sf::Time delta) {
    if(m_Dragging) {
        sf::RenderWindow& window = m_App->GetWindow();

        sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(m_LastMousePosition) - window.mapPixelToCoords(currentMousePosition);

        m_Camera.move(delta * m_TotalZoom);
        m_LastMousePosition = currentMousePosition;
    }
}

void EditingMenu::Event(const sf::Event& event) {
    sf::RenderWindow& window = m_App->GetWindow();

    if(event.type == sf::Event::MouseWheelMoved) {
        float delta = (-event.mouseWheel.delta)/50.f;

        // Reset zoom if scrolling in reverse.
        if((m_Zoom - 1.f) * (-delta) > 0.f) m_Zoom = 1.f;
        m_Zoom = m_Zoom + delta/2.f;

        float factor = std::max(0.9f, std::min(1.1f, m_Zoom));
        m_TotalZoom *= factor;
        m_Camera.zoom(factor);
    }
    else if(event.type == sf::Event::MouseButtonPressed) {
        if(event.mouseButton.button == sf::Mouse::Button::Left) {
            m_Dragging = true;
            m_LastMousePosition = sf::Mouse::getPosition(window);
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased) {
        if(event.mouseButton.button == sf::Mouse::Button::Left) {
            m_Dragging = false;
        }
    }
}

void EditingMenu::Draw() {
    sf::RenderWindow& window = m_App->GetWindow();

    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Close")) {
                m_App->OpenMenu(MakeUnique<HomeMenu>(m_App));
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {
            for(int i = 0; i < (int) MapMode::COUNT; i++) {
                if(ImGui::MenuItem(MapModeLabels[i], "", m_MapMode == (MapMode) i)) {
                    m_MapMode = (MapMode) i;
                    m_Mod->LoadMapModeTexture(m_MapTexture, m_MapMode);
                    m_MapSprite.setTexture(m_MapTexture);
                }    
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    sf::View previousView = window.getView();
    window.setView(m_Camera);

    window.draw(m_MapSprite);

    window.setView(previousView);

    window.draw(m_SelectionText);
}