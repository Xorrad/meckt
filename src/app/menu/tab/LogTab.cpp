#include "LogTab.hpp"
#include "app/menu/EditorMenu.hpp"

#include <imgui/imgui.hpp>

LogTab::LogTab(EditorMenu& menu, bool visible) : Tab("Log", Tabs::LOG, menu, visible) {}

void LogTab::Render() {
    UniquePtr<Logger::Logger>& logger = Logger::Get();

    if (!m_Visible)
        return;

    if (ImGui::Button("Clear")) {
        logger->Clear();
    }

    ImGui::Separator();

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        // Use a list clipper as in imgui_demo.cpp to ignore messages that will not be displayed.
        ImGuiListClipper clipper;
        clipper.Begin(logger->GetMessages().size()+1);
        while (clipper.Step()) {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd-1; i++) {
                const SharedPtr<Logger::Message>& message = logger->GetMessages()[i];
                sf::Color color = message->GetColor();
                std::string text = fmt::format("[{}]: {}", message->GetTimeHMS(), message->GetText());
                ImGui::TextColored(
                    ImVec4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f),
                    text.c_str()
                );
            }
            ImGui::Text("");
        }
        clipper.End();

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}