#include "ProvincesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"
#include "app/App.hpp"
#include "app/map/Title.hpp"
#include "app/map/Province.hpp"
#include "app/map/Region.hpp"

#include "imgui/imgui.hpp"

RegionsTab::RegionsTab(EditorMenu& menu, bool visible) : Tab("Geographical Regions", Tabs::REGIONS, menu, visible) {}

void RegionsTab::Render() {
    if (!m_Visible)
        return;

    Mod& mod = this->GetMod();

    // Generate a map of whether a region is filtered by name or not.
    static std::string filter = "";
    static std::vector<Region*> filteredRegions;
    static ImGuiTableColumnSortSpecs lastSortingSpecs;
    // Keep track of how many regions there were last time the list was updated.
    static size_t lastRegionsCount = 0;
    bool updated = false;
    if (ImGui::InputText("filter", &filter) || mod.GetRegions().size() != lastRegionsCount) {
        filteredRegions.clear();
        lastRegionsCount = mod.GetRegions().size();
        updated = true;

        for(const auto& [regionName, region] : mod.GetRegions()) {
            if (regionName.find(filter) != std::string::npos)
                filteredRegions.push_back(region.get());
        }
    }

    const auto SortRegions = [&](int column, ImGuiSortDirection dir) {
        auto comparator = [&](Region* a, Region* b) {
            switch (column) {
                case 0: // Name
                    return dir == ImGuiSortDirection_Ascending ? a->GetName() < b->GetName() : a->GetName() > b->GetName();
                case 1: // Kingdoms
                    return dir == ImGuiSortDirection_Ascending ? a->GetKingdoms().size() < b->GetKingdoms().size() : a->GetKingdoms().size() > b->GetKingdoms().size();
                case 2: // Duchies
                    return dir == ImGuiSortDirection_Ascending ? a->GetDuchies().size() < b->GetDuchies().size() : a->GetDuchies().size() > b->GetDuchies().size();
                case 3: // Counties
                    return dir == ImGuiSortDirection_Ascending ? a->GetCounties().size() < b->GetCounties().size() : a->GetCounties().size() > b->GetCounties().size();
                case 4: // Provinces
                    return dir == ImGuiSortDirection_Ascending ? a->GetProvinces().size() < b->GetProvinces().size() : a->GetProvinces().size() > b->GetProvinces().size();
                case 5: // Regions
                    return dir == ImGuiSortDirection_Ascending ? a->GetRegions().size() < b->GetRegions().size() : a->GetRegions().size() > b->GetRegions().size();
                default:
                    return false;
            }
        };
        std::sort(filteredRegions.begin(), filteredRegions.end(), comparator);
    };

    ///////////////////////////////////////////////////////

    const auto DisplayTitles = [&](const auto& titles) {
        for (auto title : titles) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool isSelected = m_Menu.GetSelectionHandler().IsSelected(static_cast<Title*>(title));
            bool severalTitlesSelected = m_Menu.GetSelectionHandler().GetTitles().size() > 1;
            
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
            if (m_Menu.GetSelectionHandler().IsSelected(static_cast<Title*>(title)))
                flags |= ImGuiTreeNodeFlags_Selected;
            
            ImGui::TreeNodeEx(title->GetName().c_str(), flags);
            
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().ClearSelection();
                }
                if (isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().Deselect(static_cast<Title*>(title));
                }
                else if (!isSelected || severalTitlesSelected) {
                    m_Menu.GetSelectionHandler().Select(static_cast<Title*>(title));
                }
            }

            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
        }
    };
    
    const auto DisplayProvinces = [&](const auto& provinces) {

        for (auto province : provinces) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool isSelected = m_Menu.GetSelectionHandler().IsSelected(province);
            bool severalProvincesSelected = m_Menu.GetSelectionHandler().GetTitles().size() > 1;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
            if (m_Menu.GetSelectionHandler().IsSelected(province))
                flags |= ImGuiTreeNodeFlags_Selected;
            
            ImGui::TreeNodeEx(province->GetName().c_str(), flags);

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().ClearSelection();
                }
                if (isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().Deselect(province);
                }
                else if (!isSelected || severalProvincesSelected) {
                    m_Menu.GetSelectionHandler().Select(province);
                }
            }

            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
        }
    };
    
    const auto DisplayRegions = [&](const auto& regions) {
        for (auto region : regions) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            bool isSelected = m_Menu.GetSelectionHandler().IsSelected(region);
            bool severalRegionsSelected = m_Menu.GetSelectionHandler().GetTitles().size() > 1;

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
            if (m_Menu.GetSelectionHandler().IsSelected(region))
                flags |= ImGuiTreeNodeFlags_Selected;

            ImGui::TreeNodeEx(region->GetName().c_str(), flags);

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().ClearSelection();
                }
                if (isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().Deselect(region);
                }
                else if (!isSelected || severalRegionsSelected) {
                    m_Menu.GetSelectionHandler().Select(region);
                }
            }

            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
            ImGui::TableNextColumn();
        }
    };
    ///////////////////////////////////////////////////
    
    if (ImGui::BeginTable("Regions Tree", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Kingdoms", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Duchies", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Counties", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Provinces", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableSetupColumn("Regions", ImGuiTableColumnFlags_WidthFixed, 20.0f);
        ImGui::TableHeadersRow();

        // Sort the regions list using ImGui.
        ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
        if (sortSpecs && sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
            const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
            SortRegions(spec.ColumnIndex, spec.SortDirection);
            lastSortingSpecs = spec;
            sortSpecs->SpecsDirty = false;
        }
        else if (updated) {
            SortRegions(lastSortingSpecs.ColumnIndex, lastSortingSpecs.SortDirection);
        }

        for(Region* region : filteredRegions) {
            bool isSelected = m_Menu.GetSelectionHandler().IsSelected(region);
            bool severalSelected = m_Menu.GetSelectionHandler().GetRegions().size() > 1;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAllColumns;
            if (m_Menu.GetSelectionHandler().IsSelected(region))
                flags |= ImGuiTreeNodeFlags_Selected;
            
            bool isOpen = ImGui::TreeNodeEx(region->GetName().c_str(), flags);
            ImGui::TableNextColumn();

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                // Clear selection without LSHIFT.
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().ClearSelection();
                }

                // Unselect if selected and LSHIFT, select otherwise.
                if (isSelected && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    m_Menu.GetSelectionHandler().Deselect(region);
                }
                else if (!isSelected || severalSelected) {
                    m_Menu.GetSelectionHandler().Select(region);
                }
            }

            ImGui::Text("%ld", region->GetKingdoms().size());
            ImGui::TableNextColumn();
            ImGui::Text("%ld", region->GetDuchies().size());
            ImGui::TableNextColumn();
            ImGui::Text("%ld", region->GetCounties().size());
            ImGui::TableNextColumn();
            ImGui::Text("%ld", region->GetProvinces().size());
            ImGui::TableNextColumn();
            ImGui::Text("%ld", region->GetRegions().size());

            if (isOpen) {
                DisplayTitles(region->GetKingdoms());
                DisplayTitles(region->GetDuchies());
                DisplayTitles(region->GetCounties());
                DisplayProvinces(region->GetProvinces());
                DisplayRegions(region->GetRegions());
                ImGui::TreePop();
            }
        }

        ImGui::EndTable();
    }
}