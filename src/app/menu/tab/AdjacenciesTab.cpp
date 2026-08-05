#include "AdjacenciesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/App.hpp"

#include "core/mod/Mod.hpp"
#include "core/titles/TitleManager.hpp"
#include "core/provinces/ProvinceManager.hpp"

#include "imgui/imgui.hpp"

AdjacenciesTab::AdjacenciesTab(EditorMenu& menu, bool visible) :
    Tab("Adjacencies", Tabs::ADJACENCIES, menu, visible)
{}

void AdjacenciesTab::Render() {
    if (!m_Visible)
        return;

    ProvinceManager& provinceManager = m_Mod.GetProvinceManager();

    // Generate a map of whether an adjacency is filtered by name or not.
    static std::string filter = "";
    static std::vector<Adjacency*> filteredAdjacencies;
    static uint32_t lastAdjacenciesCount = 0;
    static ImGuiTableColumnSortSpecs lastSortingSpecs;
    bool updated = false;
    if (ImGui::InputText("filter", &filter) || ImGui::IsWindowAppearing() || lastAdjacenciesCount != provinceManager.GetAdjacencies().size()) {
        filteredAdjacencies.clear();
        lastAdjacenciesCount = provinceManager.GetAdjacencies().size(); 
        updated = true;

        std::string lowercaseFilter = String::ToLowercase(filter);
        for(const auto& [_, adjacency] : provinceManager.GetAdjacencies()) {
            std::string lowercaseComment = String::ToLowercase(adjacency->GetComment());
            if (lowercaseFilter.empty()
                || lowercaseFilter == std::to_string(adjacency->GetFromId())
                || lowercaseFilter == std::to_string(adjacency->GetToId())
                || lowercaseFilter == std::to_string(adjacency->GetThroughId())
                || String::ToLowercase(adjacency->GetType()).find(lowercaseFilter) != std::string::npos
                || String::ToLowercase(lowercaseComment).find(lowercaseFilter) != std::string::npos
                || lowercaseFilter.find(lowercaseComment) != std::string::npos
            )
                filteredAdjacencies.push_back(adjacency.get());
        }
    }

    const auto SortAdjacencies = [&](int column, ImGuiSortDirection dir) {
        auto comparator = [&](Adjacency* a, Adjacency* b) {
            switch (column) {
                case 0: // From
                    return dir == ImGuiSortDirection_Ascending ? a->GetFromId() < b->GetFromId() : a->GetFromId() > b->GetFromId();
                case 1: // To
                    return dir == ImGuiSortDirection_Ascending ? a->GetToId() < b->GetToId() : a->GetToId() > b->GetToId();
                case 2: // Type
                    return dir == ImGuiSortDirection_Ascending ? a->GetType() < b->GetType() : a->GetType() > b->GetType();
                case 3: // Through Id
                    return dir == ImGuiSortDirection_Ascending ? a->GetThroughId() < b->GetThroughId() : a->GetThroughId() > b->GetThroughId();
                case 4: // Start
                    return false;
                case 5: // Stop
                    return false;
                case 6: // Comment
                    return dir == ImGuiSortDirection_Ascending ? a->GetComment() < b->GetComment() : a->GetComment() > b->GetComment();
                default:
                    return false;
            }
        };
        std::sort(filteredAdjacencies.begin(), filteredAdjacencies.end(), comparator);
    };

    ///////////////////////////////////////////////////////
    
    if (ImGui::BeginTable("Adjacencies", 7, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Sortable)) {
        ImGui::TableSetupColumn("From Id", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed, 10.0f);
        ImGui::TableSetupColumn("To Id", ImGuiTableColumnFlags_WidthFixed, 10.0f);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 8.0f);
        ImGui::TableSetupColumn("Through Id", ImGuiTableColumnFlags_WidthFixed, 8.0f);
        ImGui::TableSetupColumn("Start", ImGuiTableColumnFlags_WidthFixed, 2.0f);
        ImGui::TableSetupColumn("Stop", ImGuiTableColumnFlags_WidthFixed, 2.0f);
        ImGui::TableSetupColumn("Comment", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        // Sort the adjacencies list using ImGui.
        ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
        if (sortSpecs && sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0) {
            const ImGuiTableColumnSortSpecs& spec = sortSpecs->Specs[0];
            SortAdjacencies(spec.ColumnIndex, spec.SortDirection);
            lastSortingSpecs = spec;
            sortSpecs->SpecsDirty = false;
        }
        else if (updated) {
            SortAdjacencies(lastSortingSpecs.ColumnIndex, lastSortingSpecs.SortDirection);
        }
        
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(filteredAdjacencies.size()));

        while (clipper.Step()) {
            for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; index++) {
                Adjacency* adjacency = filteredAdjacencies.at(index);
                bool isSelected = m_Menu.GetSelectionHandler().IsSelected(adjacency);

                ImGui::TableNextRow();
                ImGui::TableNextColumn();

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAllColumns;
                if(isSelected)
                    flags |= ImGuiTreeNodeFlags_Selected;
            
                ImGui::TreeNodeEx(fmt::format("##{}-{}", adjacency->GetFromId(), adjacency->GetToId()).c_str(), flags, "%d", adjacency->GetFromId());
                ImGui::TableNextColumn();

                if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    m_Menu.GetSelectionHandler().ClearSelection();

                    if (isSelected && (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))) {
                        m_Menu.GetSelectionHandler().Deselect(adjacency);
                    }
                    else {
                        m_Menu.GetSelectionHandler().Select(adjacency);
                    }
                }

                if(ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                    Province* fromProvince = provinceManager.GetProvinceById(adjacency->GetFromId());
                    if (fromProvince != nullptr) {
                        sf::Vector2i provincePos = fromProvince->GetImagePosition();
                        m_Menu.GetCamera().setCenter(sf::Vector2f(provincePos.x, provincePos.y));
                        m_Menu.UpdateCameraBounds();
                    }
                }

                ImGui::Text("%d", adjacency->GetToId());
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(adjacency->GetType().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%d", adjacency->GetThroughId());
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d)", adjacency->GetStart().x, adjacency->GetStart().y);
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d)", adjacency->GetStop().x, adjacency->GetStop().y);
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(adjacency->GetComment().c_str());
            } 
        }
        clipper.End();

        ImGui::EndTable();
    }
}