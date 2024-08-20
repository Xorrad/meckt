#include "TitlesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"

TitlesTab::TitlesTab(EditorMenu* menu, bool visible) : Tab("Titles", Tabs::TITLES, menu, visible) {}

void TitlesTab::Render() {
    if(!m_Visible)
        return;

    const SharedPtr<Mod> mod = this->GetMod();

    // Generate a map of whether a title is filtered by name or not.
    static std::string filter = "";
    static std::map<std::string, bool> filteredTitles;
    if(ImGui::InputText("filter", &filter)) {
        filteredTitles.clear();

        // All titles below a title that passes the filter will be displayed.
        // This is done by passing whether the liege title is filtered or not
        // to the dejure titles.
        // On the other hand, if the liege title does not pass the filter but a dejure
        // title does, only the liege title and that very dejure title will be shown.
        // Example:
        //  filter = 'dom2'
        //  e_empire (shown)
        //      > k_kingdom
        //      > k_kingdom2 (shown)
        std::function<bool(const SharedPtr<Title>&, bool)> FilterTitles = [&](const SharedPtr<Title>& title, bool liegeFiltered) {
            bool filtered = (title->GetName().find(filter) != std::string::npos) || liegeFiltered;
            bool finalFiltered = filtered;
            if(!title->Is(TitleType::BARONY)) {
                // FilterTitles need to be first in the operation, otherwise
                // it will not be called if finalFiltered is already true.
                const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
                for(const auto& dejureTitle : highTitle->GetDejureTitles())
                    finalFiltered = FilterTitles(dejureTitle, filtered) || finalFiltered;
            }
            if(!filtered) filteredTitles[title->GetName()] = finalFiltered;
            return finalFiltered;
        };

        for(const auto& [name, title] : mod->GetTitles()) {
            if(title->GetLiegeTitle() == nullptr)
                FilterTitles(title, false);
        }
    }
    
    if(ImGui::BeginTable("Titles Tree", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        std::function<void(const SharedPtr<Title>&)> DisplayTitle = [&](const SharedPtr<Title>& title) {
            if(filteredTitles.count(title->GetName()) > 0 && !filteredTitles[title->GetName()])
                return;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if(title->Is(TitleType::BARONY)) {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                if(m_Menu->GetSelectionHandler().IsSelected(title))
                    flags |= ImGuiTreeNodeFlags_Selected;
                
                ImGui::TreeNodeEx(title->GetName().c_str(), flags);

                if(ImGui::IsItemClicked()) {
                    m_Menu->GetSelectionHandler().ClearSelection();
                    m_Menu->GetSelectionHandler().Select(title);
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", TitleTypeLabels[(int) title->GetType()]);
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d, %d)", title->GetColor().r, title->GetColor().g, title->GetColor().b);
            }
            else {
                SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);

                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                if(m_Menu->GetSelectionHandler().IsSelected(title))
                    flags |= ImGuiTreeNodeFlags_Selected;

                bool open = ImGui::TreeNodeEx(title->GetName().c_str(), flags);
                
                if(ImGui::IsItemClicked()) {
                    m_Menu->GetSelectionHandler().ClearSelection();
                    m_Menu->GetSelectionHandler().Select(title);
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", TitleTypeLabels[(int) title->GetType()]);
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d, %d)", title->GetColor().r, title->GetColor().g, title->GetColor().b);
                if(open) {
                    for(const auto& dejureTitle : highTitle->GetDejureTitles())
                        DisplayTitle(dejureTitle);
                    ImGui::TreePop();
                }
            }
        };

        for(const auto& [name, title] : mod->GetTitles()) {
            if(title->GetLiegeTitle() == nullptr)
                DisplayTitle(title);
        }

        ImGui::EndTable();
    }
}