#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED
#include <imgui/imgui.hpp>

const float FONT_SIZE_SMALL = 20.0f;
const float FONT_SIZE_MEDIUM = 30.0f;
const float FONT_SIZE_LARGE = 72.0f;

namespace ImGui {
    extern ImFont* notoSansNormalFont;
    extern ImFont* notoSansMediumFont;
    extern ImFont* notoSansLargeFont;

    void SetupSettings();
    void SetupFonts();
    void SetupStyle();

    ImFont* AddFont(const std::string& path, float size = 18.0f, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);

    bool ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags = 0);
    bool CheckBoxTristate(const char* label, int* v_tristate);
    bool TextButton(const char* label);
    bool InputTextLocked(const char* label, std::string* str);
    bool InputTextCommitOnEnter(const char* label, std::string* value, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
}