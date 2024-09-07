#include "ProvincesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"

ProvincesTab::ProvincesTab(EditorMenu* menu, bool visible) : Tab("Provinces", Tabs::PROVINCES, menu, visible) {}

void ProvincesTab::Render() {
    if(!m_Visible)
        return;

    const SharedPtr<Mod> mod = this->GetMod();

    // Generate a map of whether a province is filtered by name or not.
    static std::string filter = "";
    static std::map<std::string, bool> filteredProvinces;
    if(ImGui::InputText("filter", &filter)) {
        filteredProvinces.clear();

        for(const auto& [colorId, province] : mod->GetProvinces()) {
            filteredProvinces[province->GetName()] = (province->GetName().find(filter) != std::string::npos);
        }
    }
    
    if(ImGui::BeginTable("Provinces Tree", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        for(const auto& [colorId, province] : mod->GetProvinces()) {
            if(filteredProvinces.count(province->GetName()) > 0 && !filteredProvinces[province->GetName()])
                continue;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
            if(m_Menu->GetSelectionHandler().IsSelected(province))
                flags |= ImGuiTreeNodeFlags_Selected;
            
            ImGui::TreeNodeEx(std::to_string(province->GetId()).c_str(), flags);
            ImGui::TableNextColumn();

            if(ImGui::IsItemClicked()) {
                m_Menu->GetSelectionHandler().ClearSelection();
                m_Menu->GetSelectionHandler().Select(province);
            }

            ImGui::Text(province->GetName().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("(%d, %d, %d)", province->GetColor().r, province->GetColor().g, province->GetColor().b);
        }

        ImGui::EndTable();
    }
}