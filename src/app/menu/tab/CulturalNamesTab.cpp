#include "CulturalNamesTab.hpp"

#include "app/menu/EditorMenu.hpp"

#include "core/mod/Mod.hpp"
#include "core/titles/TitleManager.hpp"

#include <imgui/imgui.hpp>

CulturalNamesTab::CulturalNamesTab(EditorMenu& menu, bool visible) : Tab("Cultural Names", Tabs::CULTURAL_NAMES, menu, visible) {}

void CulturalNamesTab::Render() {
    if(!m_Visible)
        return;

    TitleManager& titleManager = m_Mod.GetTitleManager();

    // Generate a map of whether a cultural name is filtered by name or not.
    static std::string filter = "";
    static std::map<std::string, bool> filteredNames;
    if(ImGui::InputText("filter", &filter)) {
        filteredNames.clear();

        for(const auto& [key, name] : titleManager.GetLocCulturalNames("english")) {
            filteredNames[key] = (key.find(filter) != std::string::npos || name.find(filter) != std::string::npos);
        }
    }

    static std::string newCulturalName = "";
    const auto& AddNewCulturalName = [&]() {
        std::string key = "cn_" + newCulturalName;
        if(titleManager.HasLocCulturalName("english", key))
            return;
        titleManager.AddLocCulturalName("english", key, " ");
        newCulturalName = "";
    };
    if(ImGui::InputText("##key", &newCulturalName, ImGuiInputTextFlags_EnterReturnsTrue)) {
        AddNewCulturalName();
    }
    ImGui::SameLine();
    if(ImGui::SmallButton("add")) {
        AddNewCulturalName();
    }
    
    if(ImGui::BeginTable("Cultural Names", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Localization", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableHeadersRow();

        std::map<std::string, std::string>& culturalNames = titleManager.GetLocCulturalNames("english");
        for(auto it = culturalNames.begin(); it != culturalNames.end(); ) {
            std::string key = it->first;
            std::string& name = it->second;

            if(filteredNames.contains(key) && !filteredNames[key]) {
                ++it;
                continue;
            }

            ImGui::TableNextRow();
            
            ImGui::PushID(key.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", key.c_str());
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::InputText("##name", &name);
            
            ImGui::TableNextColumn();
            if(ImGui::Button("x")) {
                it = culturalNames.erase(it);
                continue;
            }
            
            ImGui::PopID();

            ++it;
        }

        ImGui::EndTable();
    }
}