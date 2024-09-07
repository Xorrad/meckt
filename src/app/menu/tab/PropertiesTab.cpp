#include "PropertiesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/menu/selection/SelectionHandler.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"
#include "app/menu/ImGuiStyle.hpp"

PropertiesTab::PropertiesTab(EditorMenu* menu, bool visible) : Tab("Properties", Tabs::PROPERTIES, menu, visible), m_SelectingTitle(false) {
    m_SelectingTitleText.setCharacterSize(24);
    m_SelectingTitleText.setString("Click on a title.");
    m_SelectingTitleText.setFillColor(sf::Color::Red);
    m_SelectingTitleText.setFont(Configuration::fonts.Get(Fonts::FIGTREE));
    m_SelectingTitleText.setPosition({10, 20});
    m_Clock.restart();
}

void PropertiesTab::Update(sf::Time delta) {
    if(m_Clock.getElapsedTime().asSeconds() > 0.5) {
        if(m_Clock.getElapsedTime().asSeconds() > 1.5) {
            m_SelectingTitleText.setString("Click on a title...");
            m_Clock.restart();
        }
        else if(m_Clock.getElapsedTime().asSeconds() > 1.0)
            m_SelectingTitleText.setString("Click on a title..");
        else
            m_SelectingTitleText.setString("Click on a title.");
    }
}

void PropertiesTab::Render() {
    if(!m_Visible)
        return;

    if(m_SelectingTitle)
        m_Menu->GetApp()->GetWindow().draw(m_SelectingTitleText);

    if(m_Menu->GetSelectionHandler().GetProvinces().size() > 0) {
        this->RenderProvinces();
    }
    else if(m_Menu->GetSelectionHandler().GetTitles().size() > 0) {
        this->RenderTitles();
    }
    
}

void PropertiesTab::RenderProvinces() {
    for(auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                
        if(ImGui::CollapsingHeader(fmt::format("#{} ({})", province->GetId(), province->GetName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(province->GetId());                        

            // PROVINCE: id (field)
            ImGui::BeginDisabled();
            std::string id = std::to_string(province->GetId());
            ImGui::InputText("id", &id);
            ImGui::EndDisabled();

            // PROVINCE: name (field)
            ImGui::InputText("name", &province->m_Name);

            // PROVINCE: color (colorpicker)
            sf::Color color = province->GetColor();
            ImGui::BeginDisabled();
            if(ImGui::ColorEdit3("color", &color)) {
                province->SetColor(color);
            }
            ImGui::EndDisabled();

            // PROVINCE: terrain (combobox)
            if (ImGui::BeginCombo("terrain type", TerrainTypeLabels[(int) province->GetTerrain()])) {
                for (int i = 0; i < (int) TerrainType::COUNT; i++) {
                    const bool isSelected = ((int) province->GetTerrain() == i);
                    if (ImGui::Selectable(TerrainTypeLabels[i], isSelected))
                        province->SetTerrain((TerrainType) i);

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            // PROVINCE: flags (checkbox)
            bool isCoastal = province->HasFlag(ProvinceFlags::COASTAL);
            bool isLake = province->HasFlag(ProvinceFlags::LAKE);
            bool isIsland = province->HasFlag(ProvinceFlags::ISLAND);
            bool isLand = province->HasFlag(ProvinceFlags::LAND);
            bool isSea = province->HasFlag(ProvinceFlags::SEA);
            bool isRiver = province->HasFlag(ProvinceFlags::RIVER);
            bool isImpassable = province->HasFlag(ProvinceFlags::IMPASSABLE);

            if (ImGui::BeginTable("province flags", 2)) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(ImGui::Checkbox("Coastal", &isCoastal)) province->SetFlag(ProvinceFlags::COASTAL, isCoastal);
                ImGui::TableSetColumnIndex(1);
                if(ImGui::Checkbox("Lake", &isLake)) province->SetFlag(ProvinceFlags::LAKE, isLake);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(ImGui::Checkbox("Island", &isIsland)) province->SetFlag(ProvinceFlags::ISLAND, isIsland);
                ImGui::TableSetColumnIndex(1);
                if(ImGui::Checkbox("Land", &isLand)) province->SetFlag(ProvinceFlags::LAND, isLand);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(ImGui::Checkbox("Sea", &isSea)) province->SetFlag(ProvinceFlags::SEA, isSea);
                ImGui::TableSetColumnIndex(1);
                if(ImGui::Checkbox("River", &isRiver)) province->SetFlag(ProvinceFlags::RIVER, isRiver);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if(ImGui::Checkbox("Impassable", &isImpassable)) province->SetFlag(ProvinceFlags::IMPASSABLE, isImpassable);

                ImGui::EndTable();
            }

            // PROVINCE: culture (field)
            ImGui::InputText("culture", &province->m_Culture);

            // PROVINCE: religion (field)
            ImGui::InputText("religion", &province->m_Religion);

            // PROVINCE: holding type (combobox)
            if (ImGui::BeginCombo("holding", ProvinceHoldingLabels[(int) province->GetHolding()])) {
                for (int i = 0; i < (int) ProvinceHolding::COUNT; i++) {
                    const bool isSelected = ((int) province->GetHolding() == i);
                    if (ImGui::Selectable(ProvinceHoldingLabels[i], isSelected))
                        province->SetHolding((ProvinceHolding) i);
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // PROVINCE: switch to barony (button)
            if(m_Menu->GetApp()->GetMod()->GetTitles().count(province->GetName()) > 0) {
                if(ImGui::Button("switch to barony")) {
                    const SharedPtr<Title>& title = m_Menu->GetApp()->GetMod()->GetTitles()[province->GetName()];
                    m_Menu->SwitchMapMode(MapMode::BARONY, true);
                    m_Menu->GetSelectionHandler().Select(title);
                }
            }

            ImGui::PopID();
        }
    }
}

void PropertiesTab::RenderTitles() {
    for(auto& title : m_Menu->GetSelectionHandler().GetTitles()) {
                
        if(ImGui::CollapsingHeader(title->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(title->GetName().c_str());

            // TITLE: name/tag (field)
            ImGui::InputText("name", &title->m_Name);

            // TITLE: tier/type (combo)
            ImGui::BeginDisabled();
            if(ImGui::BeginCombo("type", TitleTypeLabels[(int) title->GetType()]))
                ImGui::EndCombo();
            ImGui::EndDisabled();

            // TITLE: color (colorpicker)
            sf::Color color = title->GetColor();
            if(ImGui::ColorEdit3("color", &color)) {
                title->SetColor(color);
                m_Menu->RefreshMapMode(false);
                m_Menu->GetSelectionHandler().Update();
            }

            // TITLE: landless (checkbox)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Landless", &title->m_Landless);
            ImGui::PopStyleVar();

            if(title->Is(TitleType::BARONY)) {

                // BARONY: province id (field)
                const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(title);
                ImGui::InputInt("province id", &barony->m_ProvinceId);

                // BARONY: province id (field)
                ImGui::NewLine();
                if(ImGui::Button((m_SelectingTitle) ? "click on a province..." : "change province") && !m_SelectingTitle) {
                    m_SelectingTitle = true;
                    MapMode previousMapMode = m_Menu->GetMapMode();
                    m_Menu->SwitchMapMode(MapMode::PROVINCES, false);
                    m_Menu->GetSelectionHandler().AddCallback(
                        [this, barony, previousMapMode](sf::Mouse::Button button, SharedPtr<Province> province) {
                            if(button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::INTERRUPT;
                            barony->SetProvinceId(province->GetId());
                            m_Menu->SwitchMapMode(previousMapMode, false);
                            m_SelectingTitle = false;
                            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                        }
                    );
                }

                // BARONY: Switch to province (button)
                if(ImGui::Button("switch to province")) {
                    if(m_Menu->GetApp()->GetMod()->GetProvincesByIds().count(barony->GetProvinceId()) > 0) {
                        const SharedPtr<Province>& province = m_Menu->GetApp()->GetMod()->GetProvincesByIds()[barony->GetProvinceId()];
                        m_Menu->SwitchMapMode(MapMode::PROVINCES, true);
                        m_Menu->GetSelectionHandler().Select(province);
                    }
                }
            }
            else {
                const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);

                // HIGHTITLE: dejure titles (list)
                ImGui::BeginChild("dejure titles", ImVec2(0, 250), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY | ImGuiChildFlags_Border, ImGuiWindowFlags_None);

                if(ImGui::BeginMenuBar()) {
                    if(ImGui::BeginMenu("dejure titles")) {
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }

                // Make a copy to be able to use highTitle->RemoveDejureTitle
                // without causing a crash while iterating.
                std::vector<SharedPtr<Title>> dejureTitles = highTitle->GetDejureTitles();
                int n = 0;
                for(auto const& dejure : dejureTitles) {
                    ImGui::PushID(dejure->GetName().c_str());
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Selectable(dejure->GetName().c_str());

                    // Switch to the properties of the dejure title if not dragging the mouse.
                    if(ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                        m_Menu->SwitchMapMode(TitleTypeToMapMode(dejure->GetType()), true);
                        m_Menu->GetSelectionHandler().Select(dejure);
                    }

                    // Reorder the dejure titles by dragging the mouse.
                    else if(ImGui::IsItemActive() && !ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped)) {
                        int nNext = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                        if(nNext >= 0 && nNext < dejureTitles.size()) {
                            std::iter_swap(highTitle->GetDejureTitles().begin() + n, highTitle->GetDejureTitles().begin() + nNext);
                            ImGui::ResetMouseDragDelta();
                        }
                    }

                    // First barony of a county is the capital.
                    if(dejure == highTitle->GetCapitalTitle() || (highTitle->Is(TitleType::COUNTY) && n == 0)) {
                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-90);
                        ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "(Capital)");
                    }
                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                    if(ImGui::SmallButton("x")) {
                        highTitle->RemoveDejureTitle(dejure);

                        // Update the map to remove the dejure title from the title color.
                        // TODO: it would be better not having to redraw the entire map
                        // but only the relevant colors.
                        m_Menu->RefreshMapMode();
                    }
                    ImGui::PopID();
                    n++;
                }

                // HIGHTITLE: add new dejure title (button)
                ImGui::NewLine();
                if(ImGui::SmallButton((m_SelectingTitle) ? "click on a title..." : "add") && !m_SelectingTitle) {
                    TitleType dejureType = (TitleType)(((int) highTitle->GetType())-1);
                    m_SelectingTitle = true;
                    m_Menu->SwitchMapMode(TitleTypeToMapMode(dejureType), false);
                    m_Menu->GetSelectionHandler().AddCallback(
                        [this, highTitle, dejureType](sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> clickedTitle) {
                            if(button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::INTERRUPT;
                            if(!clickedTitle->Is(dejureType))
                                return SelectionCallbackResult::INTERRUPT;
                            highTitle->AddDejureTitle(clickedTitle);
                            m_Menu->SwitchMapMode(TitleTypeToMapMode(highTitle->GetType()), false);
                            m_SelectingTitle = false;
                            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                        }
                    );
                }
                ImGui::EndChild();

                if(!title->Is(TitleType::COUNTY)) {
                    const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
                    
                    // HIGHTITLE: change capital county (button)
                    ImGui::NewLine();
                    if(ImGui::Button((m_SelectingTitle) ? "click on a title..." : "change capital county") && !m_SelectingTitle) {
                        m_SelectingTitle = true;
                        m_Menu->SwitchMapMode(MapMode::COUNTY, false);
                        m_Menu->GetSelectionHandler().AddCallback(
                            [this, highTitle](sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> clickedTitle) {
                                if(button != sf::Mouse::Button::Right)
                                    goto DeleteCallback;
                                if(button != sf::Mouse::Button::Left)
                                    return SelectionCallbackResult::INTERRUPT;
                                if(!clickedTitle->Is(TitleType::COUNTY))
                                    return SelectionCallbackResult::INTERRUPT;
                                
                                // Check if clicked title is a direct or undirect vassal of the title.
                                if(!clickedTitle->IsVassal(highTitle))
                                    return SelectionCallbackResult::INTERRUPT;
                                highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(clickedTitle));
                                
                                DeleteCallback:
                                m_Menu->SwitchMapMode(TitleTypeToMapMode(highTitle->GetType()), false);
                                m_SelectingTitle = false;
                                return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                            }
                        );
                    }
                }

            }

            // TITLE: delete title (button)
            if(ImGui::Button("delete"))
                ImGui::OpenPopup("Delete this title");

            // TITLE: delete title (modal)
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal("Delete this title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
                ImGui::Separator();

                if(ImGui::Button("Delete", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();

                    // Remove the title from his liege's dejure titles.
                    if(!title->Is(TitleType::EMPIRE) && title->GetLiegeTitle()) {
                        const SharedPtr<HighTitle>& liege = title->GetLiegeTitle();
                        liege->RemoveDejureTitle(title);
                    }

                    // Remove the title as the liege of all his dejure titles.
                    if(!title->Is(TitleType::BARONY)) {
                        const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
                        for(auto& dejure : highTitle->GetDejureTitles()) {
                            dejure->SetLiegeTitle(nullptr);
                        }
                    }

                    const SharedPtr<Mod>& mod = m_Menu->GetApp()->GetMod();
                    auto& l = mod->GetTitlesByType()[title->GetType()];

                    mod->GetTitles().erase(title->GetName());
                    l.erase(std::remove(l.begin(), l.end(), title));

                    m_Menu->RefreshMapMode(true);
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if(ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

    }
}