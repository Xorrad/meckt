#include "ProvincesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/App.hpp"

#include "core/mod/Mod.hpp"
#include "core/provinces/ProvinceManager.hpp"
#include "core/titles/TitleManager.hpp"

#include <imgui/imgui.hpp>

ProvincesTab::ProvincesTab(EditorMenu& menu, bool visible) : Tab("Provinces", Tabs::PROVINCES, menu, visible) {}

void ProvincesTab::Render() {
    if(!m_Visible)
        return;

    // Generate a map of whether a province is filtered by name or not.
    static std::string filter = "";
    static std::vector<Province*> filteredProvinces;
    static ImGuiTableColumnSortSpecs lastSortingSpecs;
	static Province* lastSelectedProvince = nullptr;

    // Keep track of how many provinces there were last time the list was updated.
    static size_t lastProvincesCount = 0;
    bool updated = false;
    if(ImGui::InputText("filter", &filter) || m_Mod.GetProvinceManager().GetProvincesByColors().size() != lastProvincesCount) {
        filteredProvinces.clear();
        lastProvincesCount = m_Mod.GetProvinceManager().GetProvincesByColors().size();
        updated = true;

        for(const auto& [colorId, province] : m_Mod.GetProvinceManager().GetProvincesByColors()) {
            if (province->GetName().find(filter) != std::string::npos || std::to_string(province->GetId()).find(filter) != std::string::npos)
                filteredProvinces.push_back(province.get());
        }
    }

    const auto SortProvinces = [&](int column, ImGuiSortDirection dir) {
        auto comparator = [&](Province* a, Province* b) {
            switch (column) {
                case 0: // Id
                    return dir == ImGuiSortDirection_Ascending ? a->GetId() < b->GetId() : a->GetId() > b->GetId();
                case 1: // Name
                    return dir == ImGuiSortDirection_Ascending ? a->GetName() < b->GetName() : a->GetName() > b->GetName();
                case 2: // Color
                    return dir == ImGuiSortDirection_Ascending ? a->GetColorId() < b->GetColorId() : a->GetColorId() > b->GetColorId();
                default:
                    return false;
            }
        };
        std::sort(filteredProvinces.begin(), filteredProvinces.end(), comparator);
    };
    
    if(ImGui::BeginTable("Provinces Tree", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort, 40.0f);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupScrollFreeze(3, 1);
        ImGui::TableHeadersRow();

        // Sort the provinces list using ImGui.
        ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
        if (sortSpecs && sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
            const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
            SortProvinces(spec.ColumnIndex, spec.SortDirection);
            lastSortingSpecs = spec;
            sortSpecs->SpecsDirty = false;
            lastSelectedProvince = nullptr;
        }
        else if (updated) {
            SortProvinces(lastSortingSpecs.ColumnIndex, lastSortingSpecs.SortDirection);
            lastSelectedProvince = nullptr;
        }

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(filteredProvinces.size()));

        while (clipper.Step()) {
            for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; index++) {
                Province* province = filteredProvinces.at(index);
                bool isSelected = m_Menu.GetSelectionHandler().IsSelected(province);
                bool severalSelected = m_Menu.GetSelectionHandler().GetProvinces().size() > 1;

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
                if(isSelected)
                    flags |= ImGuiTreeNodeFlags_Selected;
            
                ImGui::TreeNodeEx(std::to_string(province->GetId()).c_str(), flags);
                ImGui::TableNextColumn();

                if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    // Clear selection without LCTRL.
                    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
                        m_Menu.GetSelectionHandler().ClearSelection();
                    }

                    // Select a range of provinces between the last selected province and the clicked one if LSHIFT.
                    if (lastSelectedProvince != nullptr && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                        auto it1 = std::find(filteredProvinces.begin(), filteredProvinces.end(), lastSelectedProvince);
                        auto it2 = std::find(filteredProvinces.begin(), filteredProvinces.end(), province);
                        if (it1 != filteredProvinces.end() && it2 != filteredProvinces.end()) {
                            if (it1 > it2) std::swap(it1, it2);
                            for (auto it = it1; it != it2; it++) {
                                m_Menu.GetSelectionHandler().Select(*it, false);
                            }
                        }
                        m_Menu.GetSelectionHandler().Select(*it2, true);
                    }
                    else {
                        if (isSelected && (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))) {
                            m_Menu.GetSelectionHandler().Deselect(province);
					    }
                        else if (isSelected && !severalSelected) {
                            m_Menu.GetSelectionHandler().Deselect(province);
                        }
                        else {
                            m_Menu.GetSelectionHandler().Select(province);
                        }
                        lastSelectedProvince = province;
                    }
                }

                if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    sf::Vector2i titlePos = province->GetImagePosition();
                    m_Menu.GetCamera().setCenter(sf::Vector2f(titlePos.x, titlePos.y));
                    m_Menu.UpdateCameraBounds();
                }

                ImGui::Text(province->GetName().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d, %d)", province->GetColor().r, province->GetColor().g, province->GetColor().b);
            } 
        }
        clipper.End();

        ImGui::EndTable();
    }
}