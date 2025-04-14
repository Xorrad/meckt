#include "CulturalNamesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"

#include "imgui/imgui.hpp"

CulturalNamesTab::CulturalNamesTab(EditorMenu* menu, bool visible) : Tab("Cultural Names", Tabs::CULTURAL_NAMES, menu, visible) {}

void CulturalNamesTab::Render() {
    if(!m_Visible)
        return;

    const SharedPtr<Mod> mod = this->GetMod();

    // Generate a map of whether a title is filtered by name or not.
    static std::string filter = "";
    static std::map<std::string, bool> filteredNames;
    if(ImGui::InputText("filter", &filter)) {
        filteredNames.clear();

        for(const auto& [key, name] : mod->GetLocCulturalNames("english")) {
            if(key.find(filter) == std::string::npos && name.find(filter) == std::string::npos)
                continue;
            filteredNames[key] = true;
        }
    }

    static std::string newCulturalName = "";
    const auto& AddNewCulturalName = [&]() {
        if(mod->GetLocCulturalNames("english").count(newCulturalName) > 0)
            return;
        mod->SetLocCulturalName("english", newCulturalName, " ");
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

        std::map<std::string, std::string>& culturalNames = mod->GetLocCulturalNames("english");
        for(auto it = culturalNames.begin(); it != culturalNames.end(); ) {
            std::string key = it->first;
            std::string& name = it->second;

            if(filteredNames.count(key) > 0 && !filteredNames[key])
                return;

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