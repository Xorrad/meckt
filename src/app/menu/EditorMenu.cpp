#include "EditorMenu.hpp"
#include "HomeMenu.hpp"
#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "app/menu/tab/Tab.hpp"

#include "imgui/imgui.hpp"

MapSelectionHandler::MapSelectionHandler(EditorMenu* menu) : m_Menu(menu), m_Count(0) {}

void MapSelectionHandler::Select(const SharedPtr<Province>& province) {
    if(m_Menu->m_MapMode == MapMode::PROVINCES) {
        if(this->IsSelected(province))
            return;
        m_Provinces.push_back(province);
    }
    else if(MapModeIsTitle(m_Menu->m_MapMode)) {
        TitleType type = MapModeToTileType(m_Menu->m_MapMode);
        SharedPtr<Title> title = m_Menu->GetApp()->GetMod()->GetProvinceLiegeTitle(province, type);
        if(title == nullptr)
            return;
        if(this->IsSelected(title))
            return;
        m_Titles.push_back(title);
    }

    this->UpdateColors();
}

void MapSelectionHandler::Select(const SharedPtr<Title>& title) {
    if(title == nullptr)
        return;
    if(this->IsSelected(title))
        return;
    m_Titles.push_back(title);

    this->UpdateColors();
}

void MapSelectionHandler::Deselect(const SharedPtr<Province>& province) {
    if(m_Menu->m_MapMode == MapMode::PROVINCES) {
        m_Provinces.erase(std::remove(m_Provinces.begin(), m_Provinces.end(), province));
    }
    else if(MapModeIsTitle(m_Menu->m_MapMode)) {
        TitleType type = MapModeToTileType(m_Menu->m_MapMode);
        SharedPtr<Title> title = m_Menu->GetApp()->GetMod()->GetProvinceLiegeTitle(province, type);
        if(title == nullptr)
            return;
        m_Titles.erase(std::remove(m_Titles.begin(), m_Titles.end(), title));
    }

    this->UpdateColors();
}

void MapSelectionHandler::Deselect(const SharedPtr<Title>& title) {
    if(title == nullptr)
        return;
    m_Titles.erase(std::remove(m_Titles.begin(), m_Titles.end(), title));

    this->UpdateColors();
}

void MapSelectionHandler::ClearSelection() {
    m_Provinces.clear();
    m_Titles.clear();
    m_Colors.clear();

    this->UpdateColors();
}

bool MapSelectionHandler::IsSelected(const SharedPtr<Province>& province) {
    return std::find(m_Provinces.begin(), m_Provinces.end(), province) != m_Provinces.end();
}

bool MapSelectionHandler::IsSelected(const SharedPtr<Title>& title) {
    return std::find(m_Titles.begin(), m_Titles.end(), title) != m_Titles.end();
}

std::vector<SharedPtr<Province>>& MapSelectionHandler::GetProvinces() {
    return m_Provinces;
}

std::vector<SharedPtr<Title>>& MapSelectionHandler::GetTitles() {
    return m_Titles;
}

std::vector<sf::Glsl::Vec4>& MapSelectionHandler::GetColors() {
    return m_Colors;
}

std::size_t MapSelectionHandler::GetCount() const {
    return m_Count;
}

void MapSelectionHandler::UpdateColors() {
    m_Colors.clear();
    m_Count = 0;

    if(m_Menu->m_MapMode == MapMode::PROVINCES) {
        for(const auto& province : m_Provinces) {
            sf::Color c = province->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
        }
        m_Count = m_Provinces.size();
    }
    else if(MapModeIsTitle(m_Menu->m_MapMode)) {
        // The shader need the colors of provinces.
        // Ttherefore, we have to loop recursively through
        // each titles until we reach a barony tier and get the color.
        // std::function<void(const SharedPtr<Title>&)> PushTitleProvincesColor = [&](const SharedPtr<Title>& title) {
        //     if(title->Is(TitleType::BARONY)) {
        //         const SharedPtr<BaronyTitle> barony = CastSharedPtr<BaronyTitle>(title);
        //         const SharedPtr<Province>& province = m_Menu->GetApp()->GetMod()->GetProvincesByIds()[barony->GetProvinceId()];
        //         sf::Color c = province->GetColor();
        //         m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
        //         m_Count++;
        //     }
        //     else {
        //         const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
        //         for(const auto& dejureTitle : highTitle->GetDejureTitles())
        //             PushTitleProvincesColor(dejureTitle);
        //     }
        // };
        // for(const auto& title : m_Titles)
        //     PushTitleProvincesColor(title);

        for(const auto& title : m_Titles) {
            sf::Color c = title->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
        }
        m_Count = m_Titles.size();
    }

    this->UpdateShader();
}

void MapSelectionHandler::UpdateShader() {
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniformArray("selectedProvinces", m_Colors.data(), m_Colors.size());
    provinceShader.setUniform("selectedProvincesCount", (int) m_Count);
}

EditorMenu::EditorMenu(App* app)
: Menu(app, "Editor"), m_MapMode(MapMode::PROVINCES), m_SelectionHandler(MapSelectionHandler(this)), m_ExitToMainMenu(false)
{
    m_App->GetMod()->LoadMapModeTexture(m_ProvinceTexture, MapMode::PROVINCES);
    Configuration::shaders.Get(Shaders::PROVINCES).setUniform("provincesTexture", m_ProvinceTexture);

    m_App->GetMod()->LoadMapModeTexture(m_MapTexture, m_MapMode);
    m_MapSprite.setTexture(m_MapTexture);

    m_Camera = m_App->GetWindow().getDefaultView();

    m_Dragging = false;
    m_LastMousePosition = {0, 0};
    m_Zoom = 1.f;
    m_TotalZoom = 1.f;

    m_HoverText.setCharacterSize(12);
    m_HoverText.setString("#");
    m_HoverText.setFillColor(sf::Color::White);
    m_HoverText.setFont(Configuration::fonts.Get(Fonts::FIGTREE));
    // m_HoverText.setPosition({5, m_App->GetWindow().getSize().y - m_HoverText.getGlobalBounds().height - 10});

    this->InitTabs();
}

SharedPtr<Province> EditorMenu::GetHoveredProvince() {
    sf::Vector2f mousePosition = m_App->GetWindow().mapPixelToCoords(sf::Mouse::getPosition(m_App->GetWindow()));

    if(!m_MapSprite.getGlobalBounds().contains(mousePosition))
        return nullptr;

    SharedPtr<Mod> mod = m_App->GetMod();
    sf::Vector2f mapMousePosition = mousePosition - m_MapSprite.getPosition();

    sf::Color color = mod->getProvinceImage().getPixel(mapMousePosition.x, mapMousePosition.y);
    uint32_t colorId = (color.r << 16) + (color.g << 8) + color.b;

    if(mod->GetProvinces().count(colorId) == 0)
        return nullptr;

    return mod->GetProvinces()[colorId];
}

MapSelectionHandler& EditorMenu::GetSelectionHandler() {
    return m_SelectionHandler;
}

void EditorMenu::UpdateHoveringText() {
    SharedPtr<Province> province = this->GetHoveredProvince();
    sf::Vector2i mousePosition = sf::Mouse::getPosition(m_App->GetWindow());

    if(province == nullptr)
        goto Hide;

    if(m_MapMode == MapMode::PROVINCES) {
        m_HoverText.setString(fmt::format("#{} ({})", province->GetId(), province->GetName()));
        m_HoverText.setPosition({(float) mousePosition.x + 5, (float) mousePosition.y - m_HoverText.getGlobalBounds().height - 10});
        m_HoverText.setFillColor(brightenColor(province->GetColor()));
        return;
    }
    else if(MapModeIsTitle(m_MapMode)) {
        const SharedPtr<Title>& title = m_App->GetMod()->GetProvinceLiegeTitle(province, MapModeToTileType(m_MapMode));
        if(title == nullptr)
            goto Hide;
        m_HoverText.setString(fmt::format("{}", title->GetName()));
        m_HoverText.setPosition({(float) mousePosition.x + 5, (float) mousePosition.y - m_HoverText.getGlobalBounds().height - 10});
        m_HoverText.setFillColor(brightenColor(title->GetColor()));
        return;
    }

    Hide:
    m_HoverText.setString("");
}

void EditorMenu::ToggleCamera(bool enabled) {
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

void EditorMenu::SwitchMapMode(MapMode mode) {
    m_MapMode = mode;
    m_SelectionHandler.ClearSelection();
    
    m_App->GetMod()->LoadMapModeTexture(m_MapTexture, m_MapMode);
    m_MapSprite.setTexture(m_MapTexture);
}

void EditorMenu::Update(sf::Time delta) {
    ToggleCamera(true);

    // Update all currently opened tabs
    for(const auto& [type, tab] : m_Tabs) {
        if(!tab->IsVisible())
            continue;
        tab->Update(delta);
    }

    if(m_Dragging) {
        sf::RenderWindow& window = m_App->GetWindow();

        sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(m_LastMousePosition) - window.mapPixelToCoords(currentMousePosition);

        m_Camera.move(delta);
        m_LastMousePosition = currentMousePosition;
    }

    ToggleCamera(false);
}

void EditorMenu::Event(const sf::Event& event) {
    ToggleCamera(true);
    sf::RenderWindow& window = m_App->GetWindow();

    // Report event to all currently opened tabs
    for(const auto& [type, tab] : m_Tabs) {
        if(!tab->IsVisible())
            continue;
        tab->Event(event);
    }

    if(event.type == sf::Event::MouseMoved) {
        this->UpdateHoveringText();
    }
    else if(event.type == sf::Event::MouseWheelMoved) {
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
            
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            int dx = (mousePosition.x - m_LastClickMousePosition.x);
            int dy = (mousePosition.y - m_LastClickMousePosition.y);
            int d = sqrt(dx*dx + dy*dy);

            if(d < 5) {
                if(m_MapMode == MapMode::PROVINCES || MapModeIsTitle(m_MapMode)) {
                    SharedPtr<Province> province = this->GetHoveredProvince();
                    if(province != nullptr) {
                        bool isSelected = m_SelectionHandler.IsSelected(province);

                        if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                            m_SelectionHandler.Deselect(province);
                        }
                        else if(!isSelected) {
                            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                                m_SelectionHandler.ClearSelection();
                            m_SelectionHandler.Select(province);
                        }
                    }
                }
            }
        }
    }
    ToggleCamera(false);
}

void EditorMenu::Render() {
    sf::RenderWindow& window = m_App->GetWindow();

    // Update provinces shader
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniform("texture", sf::Shader::CurrentTexture);
    provinceShader.setUniform("time", m_Clock.getElapsedTime().asSeconds());
    provinceShader.setUniform("mapMode", (int) m_MapMode);

    this->RenderMenuBar();
    this->SetupDockspace();

    for(const auto& [type, tab] : m_Tabs) {
        if(!tab->IsVisible())
            continue;
        if(ImGui::Begin(tab->GetName().c_str(), &tab->IsVisible())) {
            tab->Render();
        }
        ImGui::End();   
    }

    ToggleCamera(true);

    if(m_MapMode == MapMode::PROVINCES || MapModeIsTitle(m_MapMode))
        window.draw(m_MapSprite, &Configuration::shaders.Get(Shaders::PROVINCES));
    else 
        window.draw(m_MapSprite);

    ToggleCamera(false);

    window.draw(m_HoverText);

    if(m_ExitToMainMenu) {
        m_App->OpenMenu(MakeUnique<HomeMenu>(m_App));
    }
}

void EditorMenu::InitTabs() {
    m_Tabs[Tabs::TITLES] = MakeShared<TitlesTab>(this, true);
    m_Tabs[Tabs::PROPERTIES] = MakeShared<PropertiesTab>(this, true);
}

void EditorMenu::SetupDockspace() {
    // Get the position and size of the "work area", which does not include the menu bar.
    ImVec2 workPos = ImGui::GetMainViewport()->WorkPos;
    ImVec2 workSize = ImGui::GetMainViewport()->WorkSize;

    // Setup a fullscreen window
    ImGui::SetNextWindowPos(workPos);
    ImGui::SetNextWindowSize(workSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::Begin("Global Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground);

    // Get the central dockspace ID and create the central dockspace
    ImGuiID dockspaceID = ImGui::GetID("MainDockspace");
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspaceID, ImVec2(0, 0), dockspaceFlags);

    // Setup docking layout only once
    static bool dockspaceInitialized = false;
    if (!dockspaceInitialized) {
        dockspaceInitialized = true;

        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodePos(dockspaceID, workPos);
		ImGui::DockBuilderSetNodeSize(dockspaceID, workSize);

        // Split the right dockspace into top and bottom
        ImGuiID dockRight = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.25f, nullptr, &dockspaceID);
        ImGuiID dockDown = ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.6f, nullptr, &dockRight);

        // Create docked windows
        ImGui::DockBuilderDockWindow("Titles", dockRight);
        ImGui::DockBuilderDockWindow("Properties", dockDown);

        ImGui::DockBuilderFinish(dockspaceID);
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void EditorMenu::RenderMenuBar() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Export")) {
                SharedPtr<Mod> mod = m_App->GetMod();
                mod->Export();
            }
            if (ImGui::MenuItem("Close")) {
                m_ExitToMainMenu = true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {

            for(const auto& [type, tab] : m_Tabs) {
                ImGui::MenuItem(tab->GetName().c_str(), "", &tab->IsVisible());
            }

            if(ImGui::BeginMenu("Map")) {
                for(int i = 0; i < (int) MapMode::COUNT; i++) {
                    if(ImGui::MenuItem(MapModeLabels[i], "", m_MapMode == (MapMode) i)) {
                        this->SwitchMapMode((MapMode) i);
                    }    
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}