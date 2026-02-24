#include "EditorMenu.hpp"
#include "HomeMenu.hpp"
#include "app/menu/tab/Tab.hpp"

#include "app/App.hpp"
#include "core/mod/Mod.hpp"
#include "core/map/provinces/Province.hpp"
#include "core/map/regions/Region.hpp"
#include "core/map/titles/Title.hpp"

#include <imgui/imgui.hpp>
#include "app/menu/ImGuiStyle.hpp"

EditorMenu::EditorMenu(App& app)
: Menu(app, "Editor"),
m_MapMode(MapMode::PROVINCES),
m_SelectionHandler(SelectionHandler(*this)),
m_HoverText(Configuration::fonts.Get(Fonts::FIGTREE)),
m_HoverTitleText(Configuration::fonts.Get(Fonts::FIGTREE)),
m_DisplayBorders(true),
m_ExitToMainMenu(false)
{
    // Update all the textures for the shader and then apply
    // the current map mode texture to the map sprite.
    this->UpdateTextures();
    this->SwitchMapMode(m_MapMode);

    m_Camera = m_App.GetWindow().getDefaultView();

    m_Dragging = false;
    m_LastMousePosition = {0, 0};
    m_Zoom = 1.f;
    m_TotalZoom = 1.f;

    m_HoverText.setCharacterSize(12);
    m_HoverText.setString("");
    m_HoverText.setFillColor(sf::Color::Black);
    m_HoverText.setFont(Configuration::fonts.Get(Fonts::FIGTREE));
    m_HoverTitleText = m_HoverText;
    // m_HoverText.setPosition({5, m_App.GetWindow().getSize().y - m_HoverText.getGlobalBounds().height - 10});
    m_HoverShape.setSize({0, 0});
    m_HoverShape.setOutlineColor(sf::Color::Black);
    m_HoverShape.setOutlineThickness(1.0f);
    m_HoverShape.setFillColor(sf::Color::White);

    this->InitSelectionCallbacks();
    this->InitTabs();
}

Province* EditorMenu::GetHoveredProvince() {
    if (!m_MapSprite.has_value())
        return nullptr;

    ToggleCamera(true);
    sf::Vector2f mousePosition = m_App.GetWindow().mapPixelToCoords(sf::Mouse::getPosition(m_App.GetWindow()));
    ToggleCamera(false);

    if(!m_MapSprite->getGlobalBounds().contains(mousePosition))
        return nullptr;
    
    Mod& mod = m_App.GetMod();
    sf::Vector2u mapMousePosition = sf::Vector2u(mousePosition);

    sf::Color color = mod.GetProvinceImage().getPixel(mapMousePosition);
    uint32_t colorId = color.toInteger();

	auto it = mod.GetProvinces().find(colorId);
	if (it == mod.GetProvinces().end())
        return nullptr;
        
    return it->second.get();
}

MapMode EditorMenu::GetMapMode() const {
    return m_MapMode;
}

SelectionHandler& EditorMenu::GetSelectionHandler() {
    return m_SelectionHandler;
}

sf::View& EditorMenu::GetCamera() {
    return m_Camera;
}

ImGuiID EditorMenu::GetDockspaceID() const {
    return m_DockspaceID;
}

std::optional<sf::Sprite> EditorMenu::GetMapSprite() const {
    return m_MapSprite;
}

void EditorMenu::UpdateHoveringText() {
    Province* province = this->GetHoveredProvince();
    sf::Vector2i mousePosition = sf::Mouse::getPosition(m_App.GetWindow());

    if(province == nullptr)
        goto Hide;

    // Detailed tooltip always showing the province and liege titles info
    // regardless of the current map mode.
    if (!Configuration::compactTooltip) {
        std::string text = fmt::format("#{} - {}", province->GetId(), province->GetName());
        std::string hoveredTitleText = fmt::format("");

        Title* barony = m_App.GetMod().GetProvinceLiegeTitle(province, TitleType::BARONY);
        Title* hoveredTitle = m_App.GetMod().GetProvinceFocusedTitle(province, MapModeToTileType(m_MapMode));
        Title* title = barony;

        while(title != nullptr) {
            bool isMainTitle = (title == hoveredTitle && MapModeIsTitle(m_MapMode));
            hoveredTitleText += fmt::format("\n{}", (isMainTitle ? title->GetName() : ""));
            text += fmt::format("\n{}", (!isMainTitle ? title->GetName() : ""));
            title = title->GetLiegeTitle();
        }

        m_HoverText.setString(text);
        m_HoverText.setPosition({(float) mousePosition.x + 12, (float) mousePosition.y - 8});
        m_HoverText.setFillColor(sf::Color::Black);
        
        m_HoverTitleText.setString(hoveredTitleText);
        m_HoverTitleText.setStyle(sf::Text::Bold);
        m_HoverTitleText.setPosition({(float) mousePosition.x + 12, (float) mousePosition.y - 8});
        m_HoverTitleText.setFillColor(sf::Color::Black);

        m_HoverShape.setPosition({(float) mousePosition.x + 10, (float) mousePosition.y - 10});
        m_HoverShape.setSize({(float) m_HoverText.getGlobalBounds().size.x + 4, (float) m_HoverText.getGlobalBounds().size.y + 8});
        return;
    }

    // Legacy compact tooltip displaying the province or title info
    // depending on the current map mode.
    if(m_MapMode == MapMode::PROVINCES
    || m_MapMode == MapMode::TERRAIN
    || m_MapMode == MapMode::WINTER_SEVERITY
    || m_MapMode == MapMode::CULTURE
    || m_MapMode == MapMode::RELIGION) {
        m_HoverText.setString(fmt::format("#{} ({})", province->GetId(), province->GetName()));
        m_HoverTitleText.setString("");
        m_HoverText.setPosition({(float) mousePosition.x + 5, (float) mousePosition.y - m_HoverText.getGlobalBounds().size.y - 10});
        m_HoverText.setFillColor(brightenColor(province->GetColor()));
        return;
    }
    else if(MapModeIsTitle(m_MapMode)) {
        Title* title = m_App.GetMod().GetProvinceFocusedTitle(province, MapModeToTileType(m_MapMode));
        if(title == nullptr)
            goto Hide;
        m_HoverText.setString(fmt::format("{}", title->GetName()));
        m_HoverTitleText.setString("");
        m_HoverText.setPosition({(float) mousePosition.x + 5, (float) mousePosition.y - m_HoverText.getGlobalBounds().size.y - 10});
        m_HoverText.setFillColor(brightenColor(title->GetColor()));
        return;
    }

    Hide:
    m_HoverText.setString("");
    m_HoverTitleText.setString("");
    m_HoverShape.setSize({0, 0});
}

void EditorMenu::UpdateCameraBounds() {
    if (!m_MapSprite.has_value())
		return;

    sf::Vector2f viewCenter = m_Camera.getCenter();
    sf::Vector2f viewSize = m_Camera.getSize();

    // Calculate the top-left corner of the visible area.
    float left = viewCenter.x - viewSize.x / 2.0f;
    float top = viewCenter.y - viewSize.y / 2.0f;
    float width = viewSize.x;
    float height = viewSize.y;

    // Clamp the rectangle so it doesn't exceed the texture boundaries.
    const sf::Texture& mapTexture = *m_MapTextures.at(m_MapMode);
    float texWidth = static_cast<float>(mapTexture.getSize().x);
    float texHeight = static_cast<float>(mapTexture.getSize().y);

    float clampedLeft = std::max(0.f, std::min(left, texWidth - width));
    float clampedTop = std::max(0.f, std::min(top, texHeight - height));
    float clampedWidth = std::min(width, texWidth - clampedLeft);
    float clampedHeight = std::min(height, texHeight - clampedTop);

    // Apply the visible rectangle to the sprite.
    m_MapSprite->setTextureRect(sf::IntRect(
        { static_cast<int>(clampedLeft), static_cast<int>(clampedTop) },
        { static_cast<int>(clampedWidth), static_cast<int>(clampedHeight) }
    ));

    // Move the sprite to the world position of the top-left corner
    // so it matches the camera physical location in the game world
    m_MapSprite->setPosition({ clampedLeft, clampedTop });
}

void EditorMenu::ToggleCamera(bool enabled) {
    static sf::View previousView;
    sf::RenderWindow& window = m_App.GetWindow();
    if(enabled) {
        previousView = window.getView();
        window.setView(m_Camera);
    }
    else {
        window.setView(previousView);
    }
}

void EditorMenu::SwitchMapMode(MapMode mode, bool clearSelection) {
    // Update the map sprite with the corresponding map mode.
    //
    // This function does not update the base image, EditorMenu::UpdateTexture(mode)
    // needs to be called if any province/title/... has been modified.

    m_MapMode = mode;
    if(clearSelection)
        m_SelectionHandler.ClearSelection();

    if (m_MapSprite.has_value())
        m_MapSprite->setTexture(*m_MapTextures.at(m_MapMode));
    else
		m_MapSprite = sf::Sprite(*m_MapTextures.at(m_MapMode));
}

void EditorMenu::RefreshMapMode(MapMode mode, bool clearSelection, bool resetFocus) {
    this->UpdateTexture(mode, resetFocus);
	if (mode == m_MapMode)
        this->SwitchMapMode(mode, clearSelection);
}

void EditorMenu::RefreshCurrentMapMode(bool clearSelection, bool resetFocus) {
    // Recreate the image for the current map mode, update the shader
    // and update the map sprite on the screen.
    this->UpdateTexture(m_MapMode, resetFocus);
    this->SwitchMapMode(m_MapMode, clearSelection);
}

void EditorMenu::UpdateTexture(MapMode mode, bool resetFocus) {
    // Update the pixels of the specified image (from scratch) and then
    // update the corresponding texture in the shader.
    Mod& mod = m_App.GetMod();
    m_MapTextures.emplace(mode, MakeUnique<sf::Texture>());
    switch(mode) {
        case MapMode::PROVINCES:
            // TODO: update pixel colors in mod.
            m_MapTextures[mode]->loadFromImage(mod.GetProvinceImage());
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform("provincesTexture", *m_MapTextures[mode]);
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform("textureSize", sf::Vector2f(m_MapTextures[mode]->getSize()));
            break;
        case MapMode::HEIGHTMAP:
            m_MapTextures[mode]->loadFromImage(mod.GetHeightmapImage());
            break;
        case MapMode::RIVERS:
            m_MapTextures[mode]->loadFromImage(mod.GetRiversImage());
            break;
        case MapMode::TERRAIN:
            m_MapTextures[mode]->loadFromImage(mod.GetTerrainImage());
            break;
        case MapMode::WINTER_SEVERITY:
            m_MapTextures[mode]->loadFromImage(mod.GetWinterSeverityImage());
            break;
        case MapMode::CULTURE:
            m_MapTextures[mode]->loadFromImage(mod.GetCultureImage());
            break;
        case MapMode::RELIGION:
            m_MapTextures[mode]->loadFromImage(mod.GetReligionImage());
            break;
        case MapMode::BARONY:
        case MapMode::COUNTY:
        case MapMode::DUCHY:
        case MapMode::KINGDOM:
        case MapMode::EMPIRE:
        case MapMode::HEGEMONY: {
            TitleType type = MapModeToTileType(mode);
            m_MapTextures[mode]->loadFromImage(mod.GetTitleImage(type));
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform(
                String::ToLowercase(TitleTypeLabels[(int) type]) + "Texture",
                *m_MapTextures[mode]
            );

            // Reset the selection focus for every titles of that tier or below.
            if(resetFocus) {
                for(const auto& title : mod.GetTitlesByType()[type]) {
                    title->SetSelectionFocus(true);
                }
            }
            break;
        }
        default:
            break;
    }   
}

void EditorMenu::UpdateTextures() {
    // Update the textures for all map modes. This includes:
    // - Redraw titles/provinces image pixels (with colors from Province/Title objects).
    // - Update titles and provinces textures in the shader.

    for (int mode = 0; mode < static_cast<int>(MapMode::COUNT); mode++) {
        this->UpdateTexture(static_cast<MapMode>(mode));
    }
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
        sf::RenderWindow& window = m_App.GetWindow();

        sf::Vector2i currentMousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f delta = window.mapPixelToCoords(m_LastMousePosition) - window.mapPixelToCoords(currentMousePosition);

        m_Camera.move(delta);
        m_LastMousePosition = currentMousePosition;
        UpdateCameraBounds();
    }

    ToggleCamera(false);
}

void EditorMenu::Event(const sf::Event& event) {
    Menu::Event(event);

    sf::RenderWindow& window = m_App.GetWindow();

    // Report event to all currently opened tabs
    for(const auto& [type, tab] : m_Tabs) {
        if(!tab->IsVisible())
            continue;
        tab->Event(event);
    }

    if(event.is<sf::Event::MouseMoved>()) {
        this->UpdateHoveringText();
    }
    else if(const auto* mouseWheel = event.getIf<sf::Event::MouseWheelScrolled>()) {
        ToggleCamera(true);
        float delta = (-mouseWheel->delta)/50.f;

        // Reset zoom if scrolling in reverse.
        if((m_Zoom - 1.f) * (-delta) > 0.f) m_Zoom = 1.f;
        m_Zoom = m_Zoom + delta/2.f;

        float factor = std::max(0.9f, std::min(1.1f, m_Zoom));
        m_TotalZoom *= factor;
        m_Camera.zoom(factor);
        ToggleCamera(false);
        UpdateCameraBounds();
    }
    else if(const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) {
        if(mouseButton->button == sf::Mouse::Button::Left) {
            m_Dragging = true;
            m_LastMousePosition = sf::Mouse::getPosition(window);
            m_LastClickMousePosition = sf::Mouse::getPosition(window);
        }
    }
    else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonReleased>()) {

        int d = 0;
        if(mouseButton->button == sf::Mouse::Button::Left) {
            m_Dragging = false;
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            int dx = (mousePosition.x - m_LastClickMousePosition.x);
            int dy = (mousePosition.y - m_LastClickMousePosition.y);
            d = sqrt(dx*dx + dy*dy);
        }

        if(d < 5) {

            if(m_MapMode == MapMode::PROVINCES
            || m_MapMode == MapMode::TERRAIN
            || m_MapMode == MapMode::WINTER_SEVERITY
            || m_MapMode == MapMode::CULTURE
            || m_MapMode == MapMode::RELIGION
            || MapModeIsTitle(m_MapMode)) {
                Province* province = this->GetHoveredProvince();
                if(province != nullptr) {
                    if(MapModeIsTitle(m_MapMode)) {
                        Title* title = m_App.GetMod().GetProvinceFocusedTitle(province, MapModeToTileType(m_MapMode));
                        if(title == nullptr)
                            return;
                        m_SelectionHandler.OnClick(mouseButton->button, province, title);
                    }
                    m_SelectionHandler.OnClick(mouseButton->button, province);
                }
            }

        }
    }
    else  if (const auto* resize = event.getIf<sf::Event::Resized>()) {
        m_Camera.setSize({
            static_cast<float>(resize->size.x),
            static_cast<float>(resize->size.y)
        });
        UpdateCameraBounds();
    }
}

void EditorMenu::Render() {
    sf::RenderWindow& window = m_App.GetWindow();

    // Update provinces shader
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniform("texture", sf::Shader::CurrentTexture);
    provinceShader.setUniform("time", m_Clock.getElapsedTime().asSeconds());
    provinceShader.setUniform("mapMode", (int) m_MapMode);
    provinceShader.setUniform("displayBorders", m_DisplayBorders);

    ToggleCamera(true);

    if(m_MapMode == MapMode::PROVINCES
    || m_MapMode == MapMode::TERRAIN
    || m_MapMode == MapMode::WINTER_SEVERITY
    || m_MapMode == MapMode::CULTURE
    || m_MapMode == MapMode::RELIGION
    || MapModeIsTitle(m_MapMode))
        window.draw(*m_MapSprite, &Configuration::shaders.Get(Shaders::PROVINCES));
    else 
        window.draw(*m_MapSprite);

    ToggleCamera(false);

    if (!Configuration::compactTooltip) {
        window.draw(m_HoverShape);
        window.draw(m_HoverTitleText);
    }
    window.draw(m_HoverText);

    this->RenderMenuBar();
    this->RenderModals();
    this->SetupDockspace();

    for(const auto& [type, tab] : m_Tabs) {
        if(!tab->IsVisible())
            continue;
        if(ImGui::Begin(tab->GetName().c_str(), &tab->IsVisible())) {
            tab->Render();
        }
        ImGui::End();   
    }

    if(m_ExitToMainMenu) {
        // m_App.OpenMenu(MakeUnique<HomeMenu>(m_App));
        m_App.CloseMod();
    }
}

void EditorMenu::InitSelectionCallbacks() {
    m_SelectionHandler.AddCallback([&](sf::Mouse::Button button, Province* province) {
        if((m_MapMode != MapMode::PROVINCES && m_MapMode != MapMode::TERRAIN && m_MapMode != MapMode::WINTER_SEVERITY && m_MapMode != MapMode::CULTURE && m_MapMode != MapMode::RELIGION)
        || button != sf::Mouse::Button::Left)
            return SelectionCallbackResult::CONTINUE;

        bool isSelected = m_SelectionHandler.IsSelected(province);
        bool severalSelected = m_SelectionHandler.GetProvinces().size() > 1;

        // Clear selection without LSHIFT.
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
            m_SelectionHandler.ClearSelection();
        }

        // Unselect if selected and LSHIFT, select otherwise.
        if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
            m_SelectionHandler.Deselect(province);
        }
        else if(!isSelected || severalSelected) {
            m_SelectionHandler.Select(province);
        }

        return SelectionCallbackResult::CONTINUE;
    });

    m_SelectionHandler.AddCallback([&](sf::Mouse::Button button, Province* province, Title* title) {
        bool isSelected = m_SelectionHandler.IsSelected(title);
        bool severalSelected = m_SelectionHandler.GetTitles().size() > 1;

        if(button == sf::Mouse::Button::Left) {

            // Unwrap dejure titles when CTRL+LMB.
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                if(!title->Is(TitleType::BARONY)) {
                    if (isSelected)
                        m_SelectionHandler.Deselect(title);
                    title->SetSelectionFocus(false);
                    this->RefreshCurrentMapMode(false, false);
                }
                return SelectionCallbackResult::CONTINUE;
            }

            // Clear selection without LSHIFT.
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                m_SelectionHandler.ClearSelection();
            }

            // Unselect if selected and LSHIFT, select otherwise.
            if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                m_SelectionHandler.Deselect(title);
            }
            else if(!isSelected || severalSelected) {
                m_SelectionHandler.Select(title);
            }
        }

        // Wrap a title when MMB and the title was unfocus (unwrapped).
        else if(button == sf::Mouse::Button::Right) {
            if(title->GetLiegeTitle() != nullptr && !title->GetLiegeTitle()->HasSelectionFocus()) {
                title->GetLiegeTitle()->SetSelectionFocus(true);
                this->RefreshCurrentMapMode(false, false);
            }
        }

        return SelectionCallbackResult::CONTINUE;
    });
}

void EditorMenu::InitTabs() {
    m_Tabs[Tabs::TITLES] = MakeUnique<TitlesTab>(*this, true);
    m_Tabs[Tabs::PROPERTIES] = MakeUnique<PropertiesTab>(*this, true);
    m_Tabs[Tabs::PROVINCES] = MakeUnique<ProvincesTab>(*this, true);
    m_Tabs[Tabs::REGIONS] = MakeUnique<RegionsTab>(*this, true);
    m_Tabs[Tabs::LOG] = MakeUnique<LogTab>(*this, true);
    m_Tabs[Tabs::CULTURAL_NAMES] = MakeUnique<CulturalNamesTab>(*this, true);
    m_Tabs[Tabs::CULTURAL_NAMES] = MakeUnique<CulturalNamesTab>(*this, true);
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
    m_DockspaceID = ImGui::GetID("MainDockspace");
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDockingOverCentralNode | ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(m_DockspaceID, ImVec2(0, 0), dockspaceFlags);

    // Setup docking layout only once
    static bool dockspaceInitialized = false;
    if (!dockspaceInitialized) {
        dockspaceInitialized = true;

        ImGui::DockBuilderRemoveNode(m_DockspaceID);
        ImGui::DockBuilderAddNode(m_DockspaceID, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodePos(m_DockspaceID, workPos);
		ImGui::DockBuilderSetNodeSize(m_DockspaceID, workSize);

        // Split the right dockspace into top and bottom
        ImGuiID dockRight = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Right, 0.25f, nullptr, &m_DockspaceID);
        ImGuiID dockRightDown = ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.6f, nullptr, &dockRight);
        ImGuiID dockDown = ImGui::DockBuilderSplitNode(m_DockspaceID, ImGuiDir_Down, 0.1f, nullptr, &m_DockspaceID);

        // Create docked windows
        ImGui::DockBuilderDockWindow("Titles", dockRight);
        ImGui::DockBuilderDockWindow("Provinces", dockRight);
        ImGui::DockBuilderDockWindow("Geographical Regions", dockRight);
        ImGui::DockBuilderDockWindow("Cultural Names", dockRight);
        ImGui::DockBuilderDockWindow("Properties", dockRightDown);
        ImGui::DockBuilderDockWindow("Log", dockDown);

        ImGui::DockBuilderFinish(m_DockspaceID);
    }

    ImGui::End();
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void EditorMenu::RenderMenuBar() {

    // Reset the modal name at each frame.
    // This variable is used to open a modal by clicking a MenuItem.
    // The call to ImGui::OpenPopup() has to be made outside of
    // the MainMenuBar scope.
    m_ModalName = "";

    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Export")) {
                m_ModalName = "Export";
            }
            /*if(ImGui::MenuItem("Close")) {
                m_ModalName = "Close";
            }*/
            if(ImGui::MenuItem("Exit")) {
                m_ModalName = "Exit";
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {

            if (ImGui::BeginMenu("Map")) {
                for (int i = 0; i < (int)MapMode::COUNT; i++) {
                    if (ImGui::MenuItem(MapModeLabels[i], "", m_MapMode == (MapMode)i)) {
                        this->SwitchMapMode((MapMode)i);
                    }
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();

            for(const auto& [type, tab] : m_Tabs) {
                ImGui::MenuItem(tab->GetName().c_str(), "", &tab->IsVisible());
            }
            
            ImGui::Separator();

            ImGui::MenuItem("Borders", "", &m_DisplayBorders);
            if(ImGui::MenuItem("Compact Tooltip", "", &Configuration::compactTooltip)) {
                Configuration::Save();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Refresh all textures")) {
                this->UpdateTextures();
                this->SwitchMapMode(m_MapMode, false);
            }

            ImGui::EndMenu();
        }

        this->RenderMenuBarSelection();
        this->RenderMenuBarTools();

        ImGui::EndMainMenuBar();
    }

    if(!m_ModalName.empty()) {
        ImGui::OpenPopup(m_ModalName.c_str());
    }
}

void EditorMenu::RenderMenuBarSelection() {
    if(ImGui::BeginMenu("Selection")) {

        if(ImGui::MenuItem("Create title")) {
            m_ModalName = "Create a new title";
        }
        
        if(ImGui::MenuItem("Create geographical region")) {
            m_ModalName = "Create a new geographical region";
        }
        
        if(ImGui::MenuItem("Harmonize colors")) {
            m_ModalName = "Harmonize colors";
        }

        ImGui::EndMenu();
    }
}

void EditorMenu::RenderMenuBarTools() {
    if(ImGui::BeginMenu("Tools")) {

        if(ImGui::MenuItem("Generate missing provinces")) {
            m_ModalName = "Generate missing provinces";
        }
        
        if(ImGui::MenuItem("Generate missing baronies")) {
            m_ModalName = "Generate missing baronies";
        }
        
        if(ImGui::MenuItem("Generate titles localization")) {
            m_ModalName = "Generate titles localization";
        }
        
        if(ImGui::MenuItem("Generate provinces climate")) {
            m_ModalName = "Generate provinces climate";
        }

        ImGui::EndMenu();
    }
}

// TODO: move this to a seperate file in util directory.
static int FilterTitleName(ImGuiInputTextCallbackData* data) { 
    ImWchar c = data->EventChar;
    if((c >= 'a' && c <= 'z') || c >= '_') return 0;
    if(c >= 'A' && c <= 'Z') { data->EventChar += 'A'-'a'; return 0; }
    return 1;
}

void EditorMenu::RenderModals() {
    Mod& mod = m_App.GetMod();

    // CREATE TITLE: modal begin
    ImVec2 size = ImGui::GetMainViewport()->Size;
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Create a new title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new title with the attributes below:");
        ImGui::Separator();

        static std::string name;
        static TitleType type;
        static sf::Color color;
        static bool landless;

        bool hasSelectedTitle = (m_SelectionHandler.GetTitles().size() > 0);
        bool hasSelectedProvince = (m_SelectionHandler.GetProvinces().size() > 0);
        bool isNameTaken = mod.GetTitles().count(name) > 0;

        // If there is at least one title selected then use that title upper type ass
        // the default type for the new title (capping at the empire level).
        static bool initialized = false;
        if(!initialized) {
            initialized = true;

            name = "";
            type = TitleType::COUNTY;
            color = sf::Color::Red;
            landless = false;

            if(hasSelectedTitle) {
                Title* selectedTitle = m_SelectionHandler.GetTitles()[0];
                type = (TitleType) (std::min((int) selectedTitle->GetType() + 1, (int) TitleType::HEGEMONY));
            }
            else if(hasSelectedProvince) {
                type = TitleType::BARONY;
            }
        }

        ImGui::InputText("name", &name, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, FilterTitleName);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "?");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The title type prefix should not be included as it will be added afterwards (e.g b_ or k_).");

        if (ImGui::BeginCombo("type", TitleTypeLabels[(int) type])) {
            for(int i = 0; i < (int) TitleType::COUNT; i++) {
                const bool isSelected = ((TitleType) i == type);
                if (ImGui::Selectable(TitleTypeLabels[i], isSelected)) {
                    type = (TitleType) i;
                }
                if(isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::ColorEdit3("color", &color);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Landless", &landless);
        ImGui::PopStyleVar();
        
        if(isNameTaken) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This name is already taken by another title.");
        if(!hasSelectedProvince && type == TitleType::BARONY) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "No province is currently selected!");
        if(type == TitleType::BARONY) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This operation will change the province name!");

        if(isNameTaken) ImGui::BeginDisabled();
        if(ImGui::Button("Create", ImVec2(120, 0)) && !isNameTaken) {
            ImGui::CloseCurrentPopup();

            // To reset the name and type for the next time creating a title.
            initialized = false;

            // Determine the title type prefix.
            std::string prefix = GetTitlePrefixByType(type);

            // Create a new title using the attributes.
            UniquePtr<Title> title = MakeTitle(type, prefix + "_" + name, color, landless);

            if(type != TitleType::BARONY) {
                // If the title is at least a county, then add every selected titles of the
                // right type (one type lower than that of the title) as dejure titles.
                HighTitle* highTitle = static_cast<HighTitle*>(title.get());

                for(const auto& selectedTitle : m_SelectionHandler.GetTitles()) {
                    if((int) selectedTitle->GetType() != (int) type - 1)
                        continue;
                    highTitle->AddDejureTitle(selectedTitle);
                }

                if(!title->Is(TitleType::COUNTY) && hasSelectedTitle) {
                    Title* capitalTitle = m_SelectionHandler.GetTitles()[0];
                    if(!title->Is(TitleType::DUCHY)) {
                        while(capitalTitle != nullptr && !capitalTitle->Is(TitleType::COUNTY))
                            capitalTitle = dynamic_cast<HighTitle*>(capitalTitle)->GetCapitalTitle();
                    }
                    highTitle->SetCapitalTitle(dynamic_cast<CountyTitle*>(capitalTitle));
                }
            }
            else {
                // If no province is currently selected, the default province id for the barony will be 0.
                int provinceId = (hasSelectedProvince) ? m_SelectionHandler.GetProvinces()[0]->GetId() : 0;
                BaronyTitle* barony = static_cast<BaronyTitle*>(title.get());
                barony->SetProvinceId(provinceId);
            }

			Title* titlePtr = title.get();
            mod.AddTitle(std::move(title));

            this->SwitchMapMode(TitleTypeToMapMode(type), true);
            this->RefreshCurrentMapMode();
            m_SelectionHandler.Select(titlePtr);
        }
        if(isNameTaken) ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            initialized = false;
        }
        ImGui::EndPopup();
    }
    // CREATE TITLE: modal end
    
    // CREATE REGION: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Create a new geographical region", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new geographical region with the attributes below:");
        ImGui::Separator();

        static std::string name;
        static bool generateModifiers;

        bool isNameTaken = mod.GetRegions().count(name) > 0;

        static bool initialized = false;
        if(!initialized) {
            initialized = true;
            name = "";
            generateModifiers = false;
        }

        ImGui::InputText("name", &name, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, FilterTitleName);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Generate Modifiers", &generateModifiers);
        ImGui::PopStyleVar();
        
        if(isNameTaken) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This name is already taken by another region.");

        if(isNameTaken) ImGui::BeginDisabled();
        if(ImGui::Button("Create", ImVec2(120, 0)) && !isNameTaken) {
            ImGui::CloseCurrentPopup();

            // To reset the name and type for the next time creating a title.
            initialized = false;

            // Create a new region using the attributes.
            UniquePtr<Region> region = MakeUnique<Region>(name);
            region->SetGenerateModifiers(generateModifiers);

            // Add valid selected titles to the region.
            for (Title* title : m_SelectionHandler.GetTitles()) {
                if (title->Is(TitleType::EMPIRE) || title->Is(TitleType::HEGEMONY) || title->Is(TitleType::BARONY))
                    continue;
                region->AddTitle(title);
            }
            
            // Add selected provinces to the region.
            for (Province* province : m_SelectionHandler.GetProvinces()) {
                region->AddProvince(province);
            }
            
            // Add selected regions to the region.
            for (Region* subRegion : m_SelectionHandler.GetRegions()) {
                region->AddRegion(subRegion);
            }

            m_SelectionHandler.ClearSelection();
            m_SelectionHandler.Select(region.get());
            mod.AddRegion(std::move(region));
        }
        if(isNameTaken) ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            initialized = false;
        }
        ImGui::EndPopup();
    }
    // CREATE REGION: modal end

    // HARMONIZE COLOR: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Harmonize colors", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Harmonize the colors of titles to match the color below:");
        ImGui::Separator();

        bool hasTitlesSelected = (m_SelectionHandler.GetTitles().size() > 0);

        static sf::Color color;
        static bool initialized = false;
        static int hue = 3;
        static int saturation = 20;
        if(!initialized) {
            initialized = true;
            color = sf::Color::Red;

            // Use the first selected title as default color.
            if(hasTitlesSelected) {
                color = m_SelectionHandler.GetTitles().front()->GetColor();
            }
        }

        if(!hasTitlesSelected) ImGui::BeginDisabled();
        ImGui::ColorEdit3("color", &color, ImGuiColorEditFlags_PickerHueWheel);

        ImGui::DragInt("hue spread", &hue, 1, 0, 360, "%d%%", ImGuiSliderFlags_AlwaysClamp);
        ImGui::DragInt("saturation spread", &saturation, 1, 0, 100, "%d%%", ImGuiSliderFlags_AlwaysClamp);

        if(!hasTitlesSelected) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "You have to select at least one title.");

        if(ImGui::Button("Harmonize", ImVec2(120, 0)) && hasTitlesSelected) {
            ImGui::CloseCurrentPopup();
            initialized = false;
            mod.HarmonizeTitlesColors(m_SelectionHandler.GetTitles(), color, hue/100.f, saturation/100.f);
            this->RefreshCurrentMapMode(true, false);
        }
        if(!hasTitlesSelected) ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            initialized = false;
        }
        ImGui::EndPopup();
    }
    // HARMONIZE COLOR: modal end

    // GENERATE PROVINCES: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Generate missing provinces", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
        ImGui::Separator();

        if(ImGui::Button("Generate", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_App.GetMod().GenerateMissingProvinces();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // GENERATE PROVINCES: modal end
    
    // GENERATE BARONIES: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Generate missing baronies", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
        ImGui::Separator();

        if(ImGui::Button("Generate", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_App.GetMod().GenerateMissingBaronies();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // GENERATE BARONIES: modal end
    
    // GENERATE TITLES LOCALIZATION: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Generate titles localization", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
        ImGui::Separator();

        static bool generateNames = true;
        static bool generateAdjectives = true;

        ImGui::Text("This will not overwrite existing localization.");

        ImGui::Checkbox("names   ", &generateNames);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "k_isle_of_man -> Isle of Man");

        ImGui::Checkbox("adjectives   ", &generateAdjectives);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "k_bavaria -> Bavarian");

        if(ImGui::Button("Generate", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_App.GetMod().GenerateTitlesLocalization("english", generateNames, generateAdjectives, false);
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // GENERATE TITLES LOCALIZATION: modal end
    
    // GENERATE PROVINCES CLIMATE: modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(0.9f * size.x, 0.9f * size.y), ImGuiCond_Once);
    if(ImGui::BeginPopupModal("Generate provinces climate", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
        ImGui::Separator();

        static bool override = true;
        static float elevationOffset = 0.0f;
        static float elevationStrength = 4.f;
        static float elevationFactor = 0.75f;
        static int hemisphereOffset = 0;
        static int hemisphereSize = 5000;
        static float hemisphereStrength = 5.f;
        static float hemisphereFactor = 0.5f;
        static float mildWinterThreshold = 0.0f;
        static float normalWinterThreshold = 0.4f;
        static float severeWinterThreshold = 0.7f;
        static sf::Image previewImage;
        static sf::Texture previewTexture;
        static std::optional<sf::Sprite> sprite;

        const sf::Image& heightmapImage = m_App.GetMod().GetHeightmapImage();
        const sf::Image& provinceImage = m_App.GetMod().GetProvinceImage();

        static const auto UpdatePreview = [&](){
            previewImage = Image::MapPixels(provinceImage, [&](auto& mappedColors){
                for(const auto& [provinceColorId, province] : m_App.GetMod().GetProvinces()) {
                    float winterSeverityBias = m_App.GetMod().CalculateWinterSeverityBias(province.get(), override, elevationOffset, elevationStrength, elevationFactor, hemisphereOffset, hemisphereSize, hemisphereStrength, hemisphereFactor);
                    sf::Color color = sf::Color(winterSeverityBias * 255.f, winterSeverityBias * 255.f, winterSeverityBias * 255.f);
                    mappedColors[province->GetColor().toInteger()] = color.toInteger();
                }
            });
            previewTexture.loadFromImage(previewImage);
            if (sprite.has_value())
                sprite->setTexture(previewTexture);
            else
				sprite = sf::Sprite(previewTexture);
        };

        // Initialize values when opening the window for the first time.
        if (previewTexture.getSize().x == 0) {
            hemisphereSize = heightmapImage.getSize().y / 10;
            UpdatePreview();
        }

        if (ImGui::Checkbox("override   ", &override)) {
            UpdatePreview();
        }
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "replace any existing value");

        {
            ImGui::BeginGroup();
            if (ImGui::SliderFloat("elevation offset", &elevationOffset, 0.f, 2.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            if (ImGui::SliderFloat("elevation strength", &elevationStrength, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }

            if (ImGui::SliderFloat("elevation factor", &elevationFactor, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            ImGui::EndGroup();
        }
        ImGui::SameLine();
        {    
            ImGui::BeginGroup();
            if (ImGui::SliderInt("hemisphere offset ", &hemisphereOffset, (int) -heightmapImage.getSize().y/2.f, (int) heightmapImage.getSize().y/2.f, "%d", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            
            if (ImGui::SliderInt("hemisphere size", &hemisphereSize, 0, heightmapImage.getSize().y, "%d", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }

            if (ImGui::SliderFloat("hemisphere strength", &hemisphereStrength, 0.f, 10.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            
            if (ImGui::SliderFloat("hemisphere factor", &hemisphereFactor, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            ImGui::EndGroup();
        }
        ImGui::SameLine();
        {
            ImGui::BeginGroup();
            if (ImGui::SliderFloat("mild winter threshold", &mildWinterThreshold, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }

            if (ImGui::SliderFloat("normal winter threshold", &normalWinterThreshold, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }

            if (ImGui::SliderFloat("severe winter threshold", &severeWinterThreshold, 0.f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
                UpdatePreview();
            }
            ImGui::EndGroup();
        }

        ImVec2 imagePos = ImGui::GetCursorScreenPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        float scale = windowSize.y * 0.75f / std::max(1U, previewTexture.getSize().y);
        ImGui::Image(previewTexture, sf::Vector2f(previewTexture.getSize().x * scale, previewTexture.getSize().y * scale));
        if (ImGui::BeginItemTooltip()) {
            ImGuiIO& io = ImGui::GetIO();
            
            float regionSize = 32.0f;
            float zoom = 4.0f;
            sf::Vector2i pixelPos = sf::Vector2i(
                std::min(previewTexture.getSize().x-1.f, std::max(0.f, (io.MousePos.x - imagePos.x)/scale)),
                std::min(previewTexture.getSize().y-1.f, std::max(0.f, (io.MousePos.y - imagePos.y)/scale))
            );
            sf::Vector2i regionPos = sf::Vector2i(
                std::min(previewTexture.getSize().x - regionSize, std::max(0.f, (io.MousePos.x - imagePos.x)/scale - regionSize * 0.5f)),
                std::min(previewTexture.getSize().y - regionSize, std::max(0.f, (io.MousePos.y - imagePos.y)/scale - regionSize * 0.5f))
            );

            ImGui::Text("Coords: (%d, %d)", pixelPos.x, pixelPos.y);
            ImGui::Text("Winter Severity Bias: %.2f", previewImage.getPixel(sf::Vector2u(pixelPos.x, pixelPos.y)).r / 255.f);

            sprite->setTextureRect(sf::IntRect(regionPos, sf::Vector2i(regionSize, regionSize)));
            sprite->setScale(sf::Vector2f(zoom, zoom));
            ImGui::Image(*sprite);
            ImGui::EndTooltip();
        }

        if(ImGui::Button("Generate", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_App.GetMod().GenerateProvincesClimate(override, elevationOffset, elevationStrength, elevationFactor, hemisphereOffset, hemisphereSize, hemisphereStrength, hemisphereFactor, mildWinterThreshold, normalWinterThreshold, severeWinterThreshold);
            this->UpdateTexture(MapMode::WINTER_SEVERITY);
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // GENERATE PROVINCES CLIMATE: modal end
    
    // EXPORT : modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Export", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        static bool defaultMap = true;
        static bool provincesDefinition = true;
        static bool provincesTerrain = true;
        static bool provincesClimate = true;
        static bool provincesHistory = true;
        static bool titles = true;
        static bool titlesHistory = true;
        static bool titlesLocalization = true;
        static bool culturalNamesLocalization = true;
        static bool geographicalRegions = true;

        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "WARNING!");
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "The mod files will be completely overwritten.");
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Comments will be lost and the overall structure may be different (indentation, style, order).");
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone, so make sure to back-up your files.");
        if(ImGui::Button("Select all")) {
            defaultMap = true;
            provincesDefinition = true;
            provincesTerrain = true;
            provincesClimate = true;
            provincesHistory = true;
            titles = true;
            titlesHistory = true;
            titlesLocalization = true;
            culturalNamesLocalization = true;
            geographicalRegions = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Unselect all")) {
            defaultMap = false;
            provincesDefinition = false;
            provincesTerrain = false;
            provincesClimate = false;
            provincesHistory = false;
            titles = false;
            titlesHistory = false;
            titlesLocalization = false;
            culturalNamesLocalization = false;
            geographicalRegions = false;
        }
        ImGui::Separator();

        ImGui::Checkbox("default map  ", &defaultMap);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "map_data/default.map");
        
        ImGui::Checkbox("provinces definition  ", &provincesDefinition);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "map_data/definition.csv");
        
        ImGui::Checkbox("provinces terrain  ", &provincesTerrain);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "common/province_terrain/");
        
        
        ImGui::Checkbox("provinces climate  ", &provincesClimate);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "common/province_terrain/01_province_properties.txt  &");
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "map_data/climate.txt");
        
        ImGui::Checkbox("provinces history  ", &provincesHistory);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "history/provinces/");
        
        ImGui::Checkbox("titles  ", &titles);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "common/landed_titles/");
        
        ImGui::Checkbox("titles history  ", &titlesHistory);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "history/titles/");
        
        std::string titlesLocalizationPath = m_App.GetMod().GetTitlesLocalizationFilePath();
        titlesLocalizationPath = titlesLocalizationPath.substr(m_App.GetMod().GetDir().size());
        ImGui::Checkbox("titles localization  ", &titlesLocalization);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), titlesLocalizationPath.c_str());
        
        std::string culturalNamesLocalizationPath = m_App.GetMod().GetCulturalNamesLocalizationFilePath();
        culturalNamesLocalizationPath = culturalNamesLocalizationPath.substr(m_App.GetMod().GetDir().size());
        ImGui::Checkbox("cultural names localization  ", &culturalNamesLocalization);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), culturalNamesLocalizationPath.c_str());
        
        std::string geographicalRegionsLocalizationPath = "geographical_regions/geographical_region.txt";
        ImGui::Checkbox("geographical regions  ", &geographicalRegions);
        ImGui::SameLine();
        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), geographicalRegionsLocalizationPath.c_str());

        if(ImGui::Button("Export", ImVec2(120, 0))) {
            Mod& mod = m_App.GetMod();
            mod.Export(
                defaultMap,
                provincesDefinition,
                provincesTerrain,
                provincesClimate,
                provincesHistory,
                titles,
                titlesHistory,
                titlesLocalization,
                culturalNamesLocalization,
                geographicalRegions
            );
            ImGui::CloseCurrentPopup();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // EXPORT: modal end
    
    // CLOSE : modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Close", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "All progress will be lost.");
        ImGui::Separator();

        if(ImGui::Button("Confirm", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_ExitToMainMenu = true;
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // CLOSE: modal end
    
    // EXIT : modal begin
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Exit", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "All progress will be lost.");
        ImGui::Separator();

        if(ImGui::Button("Confirm", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            m_App.OpenMod(nullptr); // Unload the current mod to free memory
            m_App.GetWindow().close();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // EXIT: modal end
}