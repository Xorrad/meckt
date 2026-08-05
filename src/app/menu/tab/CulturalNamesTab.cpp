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
    std::map<std::string, std::string>& culturalNames = titleManager.GetLocCulturalNames("english");

    // Generate a map of whether a cultural name is filtered by name or not.
    static std::string filter = "";
    static std::vector<std::map<std::string, std::string>::iterator> filteredNames;

    const auto UpdateFilteredNames = [&]() {
        filteredNames.clear();
        filteredNames.reserve(culturalNames.size());

        // Build a list of visible entries.
        for (auto it = culturalNames.begin(); it != culturalNames.end(); ++it) {
            if (filter.empty()
                || it->first.find(filter) != std::string::npos
                || it->second.find(filter) != std::string::npos
            )
                filteredNames.push_back(it);
        }
    };

    if(ImGui::InputText("filter", &filter) || ImGui::IsWindowAppearing()) {
        UpdateFilteredNames();
    }

    static std::string newCulturalName = "";
    const auto& AddNewCulturalName = [&]() {
        if (!newCulturalName.starts_with("cn_")
            && !newCulturalName.starts_with("b_")
            && !newCulturalName.starts_with("c_")
            && !newCulturalName.starts_with("d_")
            && !newCulturalName.starts_with("k_")
            && !newCulturalName.starts_with("e_")
            && !newCulturalName.starts_with("h_")
        ) {
            newCulturalName = "cn_" + newCulturalName;
        }
        std::string key = newCulturalName;
        if(titleManager.HasLocCulturalName("english", key))
            return;
        titleManager.AddLocCulturalName("english", key, " ");
        newCulturalName = "";
        UpdateFilteredNames();
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

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(filteredNames.size()));

        while (clipper.Step()) {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                auto it = filteredNames[row];
                const std::string& key = it->first;
                std::string& name = it->second;

                ImGui::TableNextRow();
                
                ImGui::PushID(key.c_str());

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(key.c_str());
                
                ImGui::TableNextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN);
                ImGui::InputText("##name", &name);
                
                ImGui::TableNextColumn();
                if(ImGui::Button("x")) {
                    filteredNames.erase(filteredNames.begin() + row);
                    culturalNames.erase(it);
                    UpdateFilteredNames();
                    ImGui::PopID();
                    goto EndTable;
                }
                
                ImGui::PopID();
            }
        }
        EndTable:
        ImGui::EndTable();
    }
}