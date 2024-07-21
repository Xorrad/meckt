#include "EditingMenu.hpp"
#include "HomeMenu.hpp"
#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "imgui/imgui.hpp"

EditingMenu::EditingMenu(App* app)
: Menu(app, "Editor"), m_MapMode(MapMode::PROVINCES) {
    m_App->GetMod()->LoadMapModeTexture(m_MapTexture, m_MapMode);
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

void EditingMenu::ToggleCamera(bool enabled) {
    static sf::View previousView;
    sf::RenderWindow& window = m_App->GetWindow();
    if(enabled) {
        previousView = window.getView();
        window.setView(m_Camera);
    }
    else {
        window.setView(previousView);
    }
}

void EditingMenu::Update(sf::Time delta) {
    ToggleCamera(true);

    if(m_Dragging) {
        sf::RenderWindow& window = m_App->GetWindow();

        sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(m_LastMousePosition) - window.mapPixelToCoords(currentMousePosition);

        m_Camera.move(delta);
        m_LastMousePosition = currentMousePosition;
    }

    ToggleCamera(false);
}

void EditingMenu::Event(const sf::Event& event) {
    ToggleCamera(true);
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
            m_LastClickMousePosition = sf::Mouse::getPosition(window);
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased) {
        if(event.mouseButton.button == sf::Mouse::Button::Left) {
            m_Dragging = false;
            
            sf::Vector2i mouseScreenPosition = sf::Mouse::getPosition(window);
            int dx = (mouseScreenPosition.x - m_LastClickMousePosition.x);
            int dy = (mouseScreenPosition.y - m_LastClickMousePosition.y);
            int d = sqrt(dx*dx + dy*dy);

            if(d < 2) {
                sf::Vector2f mousePosition = window.mapPixelToCoords(mouseScreenPosition);
                if(m_MapSprite.getGlobalBounds().contains(mousePosition)) {
                    SharedPtr<Mod> mod = m_App->GetMod();
                    sf::Vector2f mapMousePosition = mousePosition - m_MapSprite.getPosition();

                    if(m_MapMode == MapMode::PROVINCES) {
                        sf::Color color = mod->getProvinceImage().getPixel(mapMousePosition.x, mapMousePosition.y);
                        uint32_t colorId = (color.r << 16) + (color.g << 8) + color.b;

                        if(mod->GetProvinces().count(colorId) > 0) {
                            SharedPtr<Province> province = mod->GetProvinces()[colorId];
                            fmt::println("Clicked on province {} named {}", province->GetId(), province->GetName());

                            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                                m_SelectedProvinces.clear();
                            if(std::find(m_SelectedProvinces.begin(), m_SelectedProvinces.end(), province) == m_SelectedProvinces.end())
                                m_SelectedProvinces.push_back(province);
                        }
                    }
                }
            }
        }
    }
    ToggleCamera(false);
}

void EditingMenu::Draw() {
    sf::RenderWindow& window = m_App->GetWindow();
    ImVec2 menuBarSize;

    // Main Menu Bar (File, View...)
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
                    m_App->GetMod()->LoadMapModeTexture(m_MapTexture, m_MapMode);
                    m_MapSprite.setTexture(m_MapTexture);
                }    
            }
            ImGui::EndMenu();
        }
        menuBarSize = ImGui::GetWindowSize();
        ImGui::EndMainMenuBar();
    }

    // Right Sidebar (Province, Operations...)
    ImGui::SetNextWindowPos(ImVec2(Configuration::windowResolution.x - 400, menuBarSize.y));
    ImGui::SetNextWindowSize(ImVec2(400, Configuration::windowResolution.y - menuBarSize.y), ImGuiCond_Always);
    if(ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
        if(ImGui::BeginTabBar("MyTabBar")) {
            if(ImGui::BeginTabItem("Province")) {
                
                for(auto& province : m_SelectedProvinces) {
                    
                    if(ImGui::CollapsingHeader(fmt::format("#{} ({})", province->GetId(), province->GetName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                        ImGui::PushID(province->GetId());                        

                        ImGui::BeginDisabled();
                        std::string id = std::to_string(province->GetId());
                        ImGui::InputText("id", &id);
                        ImGui::EndDisabled();

                        ImGui::InputText("name", &province->GetName());

                        float color[4] = {
                            province->GetColor().r/255.f,
                            province->GetColor().g/255.f,
                            province->GetColor().b/255.f,
                            province->GetColor().a/255.f
                        };
                        ImGui::BeginDisabled();
                        if(ImGui::ColorEdit3("color", color)) {
                            province->SetColor(sf::Color(color[0]*255, color[1]*255, color[2]*255, color[3]*255));
                        }
                        ImGui::EndDisabled();
                        
                        ImGui::PopID();
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

    ImGui::End();

    ToggleCamera(true);

    window.draw(m_MapSprite);

    ToggleCamera(false);

    window.draw(m_SelectionText);
}