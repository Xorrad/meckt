#include "PropertiesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"

PropertiesTab::PropertiesTab(EditorMenu* menu, bool visible) : Tab("Properties", Tabs::PROPERTIES, menu, visible) {}

void PropertiesTab::Render() {
    if(!m_Visible)
        return;

    // const SharedPtr<Mod> mod = this->GetMod();
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

            // Province id.
            ImGui::BeginDisabled();
            std::string id = std::to_string(province->GetId());
            ImGui::InputText("id", &id);
            ImGui::EndDisabled();

            // Province barony name.
            ImGui::InputText("name", &province->GetName());

            // Province color code.
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

            // Province terrain type.
            // if(province->IsSea()) ImGui::BeginDisabled();
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
            // if(province->IsSea()) ImGui::EndDisabled();
            
            // ProvinceFlag checkboxes.
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

            ImGui::InputText("culture", &province->GetCulture());
            ImGui::InputText("religion", &province->GetReligion());

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

            ImGui::PopID();
        }
    }
}

void PropertiesTab::RenderTitles() {
    for(auto& title : m_Menu->GetSelectionHandler().GetTitles()) {
                
        if(ImGui::CollapsingHeader(title->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::PushID(title->GetName().c_str());

            // Province id.
            ImGui::BeginDisabled();
            std::string name = title->GetName();
            ImGui::InputText("name", &name);
            ImGui::EndDisabled();

            ImGui::PopID();
        }

    }
}