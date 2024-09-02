#include "EditorMenu.hpp"
#include "HomeMenu.hpp"
#include "app/menu/tab/Tab.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"
#include "app/menu/ImGuiStyle.hpp"

EditorMenu::EditorMenu(App* app)
: Menu(app, "Editor"),
m_MapMode(MapMode::PROVINCES),
m_SelectionHandler(SelectionHandler(this)),
m_DisplayBorders(true),
m_ExitToMainMenu(false)
{
    // Update all the textures for the shader and then apply
    // the current map mode texture to the map sprite.
    this->UpdateTextures();
    this->SwitchMapMode(m_MapMode);

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

    this->InitSelectionCallbacks();
    this->InitTabs();
}

SharedPtr<Province> EditorMenu::GetHoveredProvince() {
    ToggleCamera(true);
    sf::Vector2f mousePosition = m_App->GetWindow().mapPixelToCoords(sf::Mouse::getPosition(m_App->GetWindow()));
    ToggleCamera(false);

    if(!m_MapSprite.getGlobalBounds().contains(mousePosition))
        return nullptr;

    SharedPtr<Mod> mod = m_App->GetMod();
    sf::Vector2f mapMousePosition = mousePosition - m_MapSprite.getPosition();

    sf::Color color = mod->GetProvinceImage().getPixel(mapMousePosition.x, mapMousePosition.y);
    uint32_t colorId = color.toInteger();

    if(mod->GetProvinces().count(colorId) == 0)
        return nullptr;
        
    return mod->GetProvinces()[colorId];
}

MapMode EditorMenu::GetMapMode() const {
    return m_MapMode;
}

SelectionHandler& EditorMenu::GetSelectionHandler() {
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
        const SharedPtr<Title>& title = m_App->GetMod()->GetProvinceFocusedTitle(province, MapModeToTileType(m_MapMode));
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

void EditorMenu::SwitchMapMode(MapMode mode, bool clearSelection) {
    // Update the map sprite with the corresponding map mode.
    //
    // This function does not update the base image, EditorMenu::UpdateTexture(mode)
    // needs to be called if any province/title/... has been modified.

    m_MapMode = mode;
    if(clearSelection)
        m_SelectionHandler.ClearSelection();
    m_MapSprite.setTexture(m_MapTextures[m_MapMode]);
}

void EditorMenu::RefreshMapMode(bool clearSelection, bool resetFocus) {
    // Recreate the image for the current map mode, update the shader
    // and update the map sprite on the screen.
    this->UpdateTexture(m_MapMode, resetFocus);
    this->SwitchMapMode(m_MapMode, clearSelection);
}

void EditorMenu::UpdateTexture(MapMode mode, bool resetFocus) {
    // Update the pixels of the specified image (from scratch) and then
    // update the corresponding texture in the shader.
    const SharedPtr<Mod>& mod = m_App->GetMod();
    switch(mode) {
        case MapMode::PROVINCES:
            // TODO: update pixel colors in mod->m_ProvinceImage
            m_MapTextures[mode].loadFromImage(mod->GetProvinceImage());
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform("provincesTexture", m_MapTextures[mode]);
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform("textureSize", sf::Vector2f(m_MapTextures[mode].getSize()));
            break;
        case MapMode::HEIGHTMAP:
            m_MapTextures[mode].loadFromImage(mod->GetHeightmapImage());
            break;
        case MapMode::RIVERS:
            m_MapTextures[mode].loadFromImage(mod->GetRiversImage());
            break;
        case MapMode::CULTURE:
            break;
        case MapMode::RELIGION:
            break;
        case MapMode::BARONY:
        case MapMode::COUNTY:
        case MapMode::DUCHY:
        case MapMode::KINGDOM:
        case MapMode::EMPIRE: {
            TitleType type = MapModeToTileType(mode);
            m_MapTextures[mode].loadFromImage(mod->GetTitleImage(type));
            Configuration::shaders.Get(Shaders::PROVINCES).setUniform(
                String::ToLowercase(TitleTypeLabels[(int) type]) + "Texture",
                m_MapTextures[mode]
            );

            // Reset the selection focus for every titles of that tier or below.
            if(resetFocus) {
                for(const auto& title : mod->GetTitlesByType()[type]) {
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
    std::vector<UniquePtr<sf::Thread>> threads;

    for(MapMode mode = MapMode::PROVINCES; mode < MapMode::COUNT; mode = (MapMode)((int) mode + 1)) {
        threads.push_back(MakeUnique<sf::Thread>([&, mode](){
            this->UpdateTexture(mode);
        }));
        threads[threads.size()-1]->launch();
    }

    for(auto& thread : threads)
        thread->wait();
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
        ToggleCamera(true);
        float delta = (-event.mouseWheel.delta)/50.f;

        // Reset zoom if scrolling in reverse.
        if((m_Zoom - 1.f) * (-delta) > 0.f) m_Zoom = 1.f;
        m_Zoom = m_Zoom + delta/2.f;

        float factor = std::max(0.9f, std::min(1.1f, m_Zoom));
        m_TotalZoom *= factor;
        m_Camera.zoom(factor);
        ToggleCamera(false);
    }
    else if(event.type == sf::Event::MouseButtonPressed) {
        if(event.mouseButton.button == sf::Mouse::Button::Left) {
            m_Dragging = true;
            m_LastMousePosition = sf::Mouse::getPosition(window);
            m_LastClickMousePosition = sf::Mouse::getPosition(window);
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased) {
        
        int d = 0;
        if(event.mouseButton.button == sf::Mouse::Button::Left) {
            m_Dragging = false;
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            int dx = (mousePosition.x - m_LastClickMousePosition.x);
            int dy = (mousePosition.y - m_LastClickMousePosition.y);
            d = sqrt(dx*dx + dy*dy);
        }

        if(d < 5) {

            if(m_MapMode == MapMode::PROVINCES || MapModeIsTitle(m_MapMode)) {
                SharedPtr<Province> province = this->GetHoveredProvince();
                if(province != nullptr) {

                    if(MapModeIsTitle(m_MapMode)) {
                        SharedPtr<Title> title = m_App->GetMod()->GetProvinceFocusedTitle(province, MapModeToTileType(m_MapMode));
                        if(title == nullptr)
                            return;
                        m_SelectionHandler.OnClick(event.mouseButton.button, province, title);
                    }
                    m_SelectionHandler.OnClick(event.mouseButton.button, province);
                }
            }

        }
    }
}

void EditorMenu::Render() {
    sf::RenderWindow& window = m_App->GetWindow();

    // Update provinces shader
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniform("texture", sf::Shader::CurrentTexture);
    provinceShader.setUniform("time", m_Clock.getElapsedTime().asSeconds());
    provinceShader.setUniform("mapMode", (int) m_MapMode);
    provinceShader.setUniform("displayBorders", m_DisplayBorders);

    ToggleCamera(true);

    if(m_MapMode == MapMode::PROVINCES || MapModeIsTitle(m_MapMode))
        window.draw(m_MapSprite, &Configuration::shaders.Get(Shaders::PROVINCES));
    else 
        window.draw(m_MapSprite);

    ToggleCamera(false);

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
        m_App->OpenMenu(MakeUnique<HomeMenu>(m_App));
    }
}

void EditorMenu::InitSelectionCallbacks() {
    m_SelectionHandler.AddCallback([&](sf::Mouse::Button button, SharedPtr<Province> province) {
        if(m_MapMode != MapMode::PROVINCES || button != sf::Mouse::Button::Left)
            return SelectionCallbackResult::CONTINUE;

        bool isSelected = m_SelectionHandler.IsSelected(province);
        bool severalSelected = m_SelectionHandler.GetProvinces().size() > 1;

        // Clear selection without LSHIFT.
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            m_SelectionHandler.ClearSelection();
        }

        // Unselect if selected and LSHIFT, select otherwise.
        if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            m_SelectionHandler.Deselect(province);
        }
        else if(!isSelected || severalSelected) {
            m_SelectionHandler.Select(province);
        }

        return SelectionCallbackResult::CONTINUE;
    });

    m_SelectionHandler.AddCallback([&](sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> title) {
        bool isSelected = m_SelectionHandler.IsSelected(title);
        bool severalSelected = m_SelectionHandler.GetTitles().size() > 1;

        if(button == sf::Mouse::Button::Left) {

            // Unwrap dejure titles when CTRL+LMB.
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if(!title->Is(TitleType::BARONY)) {
                    title->SetSelectionFocus(false);
                    this->RefreshMapMode(true, false);
                }
                return SelectionCallbackResult::CONTINUE;
            }

            // Clear selection without LSHIFT.
            if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                m_SelectionHandler.ClearSelection();
            }

            // Unselect if selected and LSHIFT, select otherwise.
            if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                m_SelectionHandler.Deselect(title);
            }
            else if(!isSelected || severalSelected) {
                m_SelectionHandler.Select(title);
            }
        }

        // Wrap a title when MMB and the title was unfocus (unwrapped).
        if(button == sf::Mouse::Button::Right) {
            if(title->GetLiegeTitle() != nullptr && !title->GetLiegeTitle()->HasSelectionFocus()) {
                title->GetLiegeTitle()->SetSelectionFocus(true);
                this->RefreshMapMode(true, false);
            }
        }

        return SelectionCallbackResult::CONTINUE;
    });
}

void EditorMenu::InitTabs() {
    m_Tabs[Tabs::TITLES] = MakeShared<TitlesTab>(this, true);
    m_Tabs[Tabs::PROPERTIES] = MakeShared<PropertiesTab>(this, true);
    m_Tabs[Tabs::PROVINCES] = MakeShared<ProvincesTab>(this, true);
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
        ImGui::DockBuilderDockWindow("Provinces", dockRight);
        ImGui::DockBuilderDockWindow("Properties", dockDown);

        ImGui::DockBuilderFinish(dockspaceID);
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
            if(ImGui::MenuItem("Save", "Ctrl+S")) {}
            if(ImGui::MenuItem("Export")) {
                SharedPtr<Mod> mod = m_App->GetMod();
                mod->Export();
            }
            if(ImGui::MenuItem("Close")) {
                m_ExitToMainMenu = true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View")) {

            for(const auto& [type, tab] : m_Tabs) {
                ImGui::MenuItem(tab->GetName().c_str(), "", &tab->IsVisible());
            }
            
            ImGui::MenuItem("Borders", "", &m_DisplayBorders);

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
    const SharedPtr<Mod> mod = m_App->GetMod();

    // CREATE TITLE: modal begin
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
        bool isNameTaken = mod->GetTitles().count(name) > 0;

        // If there is at least one title selected then use that title upper type ass
        // the default type for the new title (capping at the empire level).
        static bool initialized = false;
        if(!initialized) {
            initialized = true;

            name = "c_";
            type = TitleType::COUNTY;
            color = sf::Color::Red;
            landless = false;

            if(hasSelectedTitle) {
                const SharedPtr<Title>& selectedTitle = m_SelectionHandler.GetTitles()[0];
                type = (TitleType) (std::min((int) selectedTitle->GetType() + 1, (int) TitleType::EMPIRE));
                name = GetTitlePrefixByType(type) + "_";
            }
            else if(hasSelectedProvince) {
                type = TitleType::BARONY;
                name = GetTitlePrefixByType(type) + "_";
            }
        }

        if(ImGui::InputText("name", &name, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, FilterTitleName)) {
            if(!IsValidTitleName(name, type)) {
                std::string prefix = GetTitlePrefixByType(type);
                if(name.starts_with("_")) name = prefix + "_" + name;
                else name = prefix + name;
            }
        }

        if (ImGui::BeginCombo("type", TitleTypeLabels[(int) type])) {
            for(int i = 0; i < (int) ProvinceHolding::COUNT; i++) {
                const bool isSelected = ((TitleType) i == type);
                if (ImGui::Selectable(TitleTypeLabels[i], isSelected)) {
                    type = (TitleType) i;

                    // Update the prefix in the name input to match the new type.
                    std::string prefix = GetTitlePrefixByType(type);
                    name = name.substr(2, name.size()-2);
                    name = prefix + "_" + name;
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

            // Create a new title using the attributes.
            SharedPtr<Title> title = MakeTitle(type, name, color, landless);
            mod->GetTitles()[name] = title;
            mod->GetTitlesByType()[type].push_back(title);

            if(type != TitleType::BARONY) {
                // If the title is at least a county, then add every selected titles of the
                // right type (one type lower than that of the title) as dejure titles.
                const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);

                for(const auto& selectedTitle : m_SelectionHandler.GetTitles()) {
                    if((int) selectedTitle->GetType() != (int) type - 1)
                        continue;
                    highTitle->AddDejureTitle(selectedTitle);
                }

                if(!title->Is(TitleType::COUNTY) && hasSelectedTitle) {
                    SharedPtr<Title> capitalTitle = m_SelectionHandler.GetTitles()[0];
                    if(!title->Is(TitleType::DUCHY)) {
                        while(capitalTitle != nullptr && !capitalTitle->Is(TitleType::COUNTY))
                            capitalTitle = CastSharedPtr<HighTitle>(capitalTitle)->GetCapitalTitle();
                    }
                    highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(capitalTitle));
                }
            }
            else {
                // If no province is currently selected, the default province id for the barony will be 0.
                int provinceId = (hasSelectedProvince) ? m_SelectionHandler.GetProvinces()[0]->GetId() : 0;
                const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(title);
                barony->SetProvinceId(provinceId);

                mod->GetProvincesByIds()[provinceId]->SetName(name);
            }
            this->SwitchMapMode(TitleTypeToMapMode(type), true);
            this->RefreshMapMode();
            m_SelectionHandler.Select(title);
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
                color = m_SelectionHandler.GetTitles()[0]->GetColor();
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
            mod->HarmonizeTitlesColors(m_SelectionHandler.GetTitles(), color, hue/100.f, saturation/100.f);
            this->RefreshMapMode(true, false);
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
            m_App->GetMod()->GenerateMissingProvinces();
        }

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    // GENERATE PROVINCES: modal end
}