#pragma once

#include <imgui/imgui.hpp>

namespace ImGui {
    extern ImFont* notoSansNormalFont;
    extern ImFont* notoSansMediumFont;
    extern ImFont* notoSansLargeFont;

    void SetupSettings();
    void SetupFonts();
    void SetupStyle();

    bool ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags = 0);
    bool CheckBoxTristate(const char* label, int* v_tristate);
    bool TextButton(const char* label);
}