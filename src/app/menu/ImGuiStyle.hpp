#pragma once

#include <imgui/imgui.hpp>

namespace ImGui {
    void SetupSettings();
    void SetupStyle();

    bool ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags = 0);
}