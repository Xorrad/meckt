#include "TitlesTab.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"

TitlesTab::TitlesTab(EditorMenu* menu, bool visible) : Tab("Titles", Tabs::TITLES, menu, visible) {}

void TitlesTab::Render() {
    if(!m_Visible)
        return;

    const SharedPtr<Mod> mod = this->GetMod();
    
    if (ImGui::BeginTable("Titles Tree", 3, ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        std::function<void(const SharedPtr<Title>&)> DisplayTitle = [&](const SharedPtr<Title>& title) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if(title->Is(TitleType::BARONY)) {
                ImGui::TreeNodeEx(title->GetName().c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen);
                ImGui::TableNextColumn();
                ImGui::Text("%s", TitleTypeLabels[(int) title->GetType()]);
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d, %d)", title->GetColor().r, title->GetColor().g, title->GetColor().b);
            }
            else {
                SharedPtr<HighTitle> highTitle = CastSharedPtr<HighTitle>(title);
                bool open = ImGui::TreeNodeEx(title->GetName().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", TitleTypeLabels[(int) title->GetType()]);
                ImGui::TableNextColumn();
                ImGui::Text("(%d, %d, %d)", title->GetColor().r, title->GetColor().g, title->GetColor().b);
                if (open) {
                    for(const auto& dejureTitle : highTitle->GetDejureTitles())
                        DisplayTitle(dejureTitle);
                    ImGui::TreePop();
                }
            }
        };

        for(const auto& empireTitle : mod->GetTitlesByType()[TitleType::EMPIRE])
            DisplayTitle(empireTitle);

        ImGui::EndTable();
    }
}