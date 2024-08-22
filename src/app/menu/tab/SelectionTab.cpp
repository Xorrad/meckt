#include "SelectionTab.hpp"
#include "app/menu/EditorMenu.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"
#include "app/menu/ImGuiStyle.hpp"

SelectionTab::SelectionTab(EditorMenu* menu, bool visible) : Tab("Selection", Tabs::SELECTION, menu, visible) {}

void SelectionTab::Render() {
    if(!m_Visible)
        return;
    
    this->RenderCreateTitle();
    this->RenderHarmonizeColors();
}

static int FilterTitleName(ImGuiInputTextCallbackData* data) { 
    ImWchar c = data->EventChar;
    if((c >= 'a' && c <= 'z') || c >= '_') return 0;
    if(c >= 'A' && c <= 'Z') { data->EventChar += 'A'-'a'; return 0; }
    return 1;
}

void SelectionTab::RenderCreateTitle() {
    const SharedPtr<Mod> mod = this->GetMod();

    if(ImGui::Button("create new title"))
        ImGui::OpenPopup("Create a new title");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Create a new title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new title with the attributes below:");
        ImGui::Separator();

        static std::string name;
        static TitleType type;
        static sf::Color color;
        static bool landless;

        bool hasSelectedTitle = (m_Menu->GetSelectionHandler().GetTitles().size() > 0);
        bool hasSelectedProvince = (m_Menu->GetSelectionHandler().GetProvinces().size() > 0);
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
                const SharedPtr<Title>& selectedTitle = m_Menu->GetSelectionHandler().GetTitles()[0];
                type = (TitleType) (std::max((int) selectedTitle->GetType() + 1, (int) TitleType::EMPIRE));
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

                for(const auto& selectedTitle : m_Menu->GetSelectionHandler().GetTitles()) {
                    if((int) selectedTitle->GetType() != (int) type - 1)
                        continue;
                    highTitle->AddDejureTitle(selectedTitle);
                }
            }
            else {
                // If no province is currently selected, the default province id for the barony will be 0.
                int provinceId = (hasSelectedProvince) ? m_Menu->GetSelectionHandler().GetProvinces()[0]->GetId() : 0;
                const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(title);
                barony->SetProvinceId(provinceId);

                mod->GetProvincesByIds()[provinceId]->SetName(name);
            }

            m_Menu->SwitchMapMode(TitleTypeToMapMode(type), true);
        }
        if(isNameTaken) ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(120, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void SelectionTab::RenderHarmonizeColors() {
    if(ImGui::Button("harmonize colors"))
        ImGui::OpenPopup("Harmonize colors");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("Harmonize colors", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Harmonize the colors of titles to match the color below:");
        ImGui::Separator();

        bool hasTitlesSelected = (m_Menu->GetSelectionHandler().GetTitles().size() > 0);

        static sf::Color color;
        static bool initialized = false;
        static int hue = 3;
        static int saturation = 20;
        if(!initialized) {
            initialized = true;
            color = sf::Color::Red;

            // Use the first selected title as default color.
            if(hasTitlesSelected) {
                color = m_Menu->GetSelectionHandler().GetTitles()[0]->GetColor();
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

            m_Menu->GetApp()->GetMod()->HarmonizeTitlesColors(m_Menu->GetSelectionHandler().GetTitles(), color, hue/100.f, saturation/100.f);
            m_Menu->SwitchMapMode(m_Menu->GetMapMode(), true);
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
}