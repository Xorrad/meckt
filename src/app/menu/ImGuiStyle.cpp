#include "ImGuiStyle.hpp"
#include "imgui/imgui.hpp"

void ImGui::SetupSettings() {
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    // Disable saving windows settings
    // io.IniFilename = NULL;
    // io.LogFilename = NULL;
}

void ImGui::SetupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}