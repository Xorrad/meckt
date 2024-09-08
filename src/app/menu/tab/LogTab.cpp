#include "LogTab.hpp"
#include "app/menu/EditorMenu.hpp"

#include "imgui/imgui.hpp"

LogTab::LogTab(EditorMenu* menu, bool visible) : Tab("Log", Tabs::LOG, menu, visible) {}

void LogTab::Render() {
    UniquePtr<Logger::Logger>& logger = Logger::Get();

    if(!m_Visible)
        return;

    if(ImGui::Button("Clear")) {
        logger->Clear();
    }

    ImGui::Separator();

    if(ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        // Use a list clipper as in imgui_demo.cpp to ignore
        // messages that will not be displayed.
        ImGuiListClipper clipper;
        clipper.Begin(logger->GetMessages().size());
        while(clipper.Step()) {
            for(int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
                const SharedPtr<Logger::Message>& message = logger->GetMessages()[i];
                ImGui::Text(message->ToString().c_str());
            }
        }
        clipper.End();

        ImGui::PopStyleVar();

        if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
}