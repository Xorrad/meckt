#include "ProvincesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"
#include "app/App.hpp"
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
    static std::vector<SharedPtr<Province>> filteredProvinces;
    // Keep track of how many provinces there were last time the list was updated.
    static size_t lastProvincesCount = 0;
    if(ImGui::InputText("filter", &filter) || mod->GetProvinces().size() != lastProvincesCount) {
        filteredProvinces.clear();
        lastProvincesCount = mod->GetProvinces().size();

        for(const auto& [colorId, province] : mod->GetProvinces()) {
            if (province->GetName().find(filter) != std::string::npos)
                filteredProvinces.push_back(province);
        }
    }
    
    if(ImGui::BeginTable("Provinces Tree", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(3, 1);
        ImGui::TableHeadersRow();

        for(const SharedPtr<Province>& province : filteredProvinces) {
            bool isSelected = m_Menu->GetSelectionHandler().IsSelected(province);
            bool severalSelected = m_Menu->GetSelectionHandler().GetProvinces().size() > 1;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
            if(isSelected)
                flags |= ImGuiTreeNodeFlags_Selected;
            
            ImGui::TreeNodeEx(std::to_string(province->GetId()).c_str(), flags);
            ImGui::TableNextColumn();

            if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                // Clear selection without LSHIFT.
                if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu->GetSelectionHandler().ClearSelection();
                }

                // Unselect if selected and LSHIFT, select otherwise.
                if(isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu->GetSelectionHandler().Deselect(province);
                }
                else if(!isSelected || severalSelected) {
                    m_Menu->GetSelectionHandler().Select(province);
                }
            }
            if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                sf::Vector2i titlePos = province->GetImagePosition();
                m_Menu->GetCamera().setCenter(titlePos.x, titlePos.y);
            }

            ImGui::Text(province->GetName().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("(%d, %d, %d)", province->GetColor().r, province->GetColor().g, province->GetColor().b);
        }

        ImGui::EndTable();
    }
}