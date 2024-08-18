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
        ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!");
        ImGui::Separator();

        static std::string name = "c_";
        static TitleType type = TitleType::COUNTY;
        static sf::Color color = sf::Color::Red;
        static bool landless = false;

        // If there is at least one title selected then use that title upper type ass
        // the default type for the new title (capping at the empire level).
        static bool initializing = true;
        if(initializing) {
            initializing = false;
            if(m_Menu->GetSelectionHandler().GetTitles().size() > 0) {
                const SharedPtr<Title>& selectedTitle = m_Menu->GetSelectionHandler().GetTitles()[0];
                type = (TitleType) (std::max((int) selectedTitle->GetType() + 1, (int) TitleType::EMPIRE));
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

        bool isNameTaken = mod->GetTitles().count(name) > 0;
        if(isNameTaken) ImGui::BeginDisabled();
        if(ImGui::Button("Create", ImVec2(120, 0)) && !isNameTaken) {
            ImGui::CloseCurrentPopup();

            // Create a new title using the attributes.
            SharedPtr<Title> title = MakeTitle(type, name, color);
            mod->GetTitles()[name] = title;
            mod->GetTitlesByType()[type].push_back(title);

            // TODO: use landless attribute when creating title.

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
                // TODO: use first selected barony as province id for the new title.
            }
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
    if(ImGui::Button("harmonize colors")) {
        
    }
}