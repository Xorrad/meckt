#include "ImGuiStyle.hpp"

ImFont* ImGui::notoSansNormalFont = nullptr;
ImFont* ImGui::notoSansMediumFont = nullptr;
ImFont* ImGui::notoSansLargeFont = nullptr;

void ImGui::SetupSettings() {
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Disable saving windows settings
    // io.IniFilename = NULL;
    // io.LogFilename = NULL;
}

void ImGui::SetupFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // Extend the default ImGui font with NotoSans for missing glyphs.
    {
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphOffset = ImVec2(0.0f, 1.0f); // Adjust vertically.

        // Define ranges of glyphs to merge into the default ImGui font.
        const ImWchar ranges[] = {
            0x0080, 0xFFFF,
            0
        };
        io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSans-VariableFont_wdth,wght.ttf", 16.0f, &config, ranges);
    }

    // Load NotoSans in different size for the main menu.
    notoSansLargeFont = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSans-VariableFont_wdth,wght.ttf", 72.0f);
    notoSansMediumFont = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSans-VariableFont_wdth,wght.ttf", 30.0f);
    notoSansNormalFont = io.Fonts->AddFontFromFileTTF("assets/fonts/NotoSans-VariableFont_wdth,wght.ttf", 20.0f);

    {
        // Extend the default ImGui font with emojis.
        ImFontConfig config;
        config.MergeMode = true;
        config.PixelSnapH = true;
        config.GlyphMinAdvanceX = 20.0f;
        
        const ImWchar ranges[] = {
            0x1F300, 0x1F5FF, // Misc Symbols and Pictographs
            0x1F600, 0x1F64F, // Emoticons
            0x1F680, 0x1F6FF, // Transport & Map Symbols
            0x1F900, 0x1F9FF, // Supplemental Symbols and Pictographs
            0x2600,  0x26FF,  // Misc symbols (sun, umbrella, etc.)
            0x2700,  0x27BF,  // Dingbats
            0x200D,  0x200D,  // Zero Width Joiner (for combining emojis)
            0xFE0F,  0xFE0F,  // Variation Selector-16 (emoji style)
            0
        };
        io.Fonts->AddFontFromFileTTF("assets/fonts/NotoEmoji-VariableFont_wght.ttf", 20.0f, &config, ranges);
    }

    if(!ImGui::SFML::UpdateFontTexture())
        LOG_ERROR("Failed to extend default font with complementary fonts.");
}

void ImGui::SetupStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
}

bool ImGui::ColorEdit3(const char* label, sf::Color* color, ImGuiColorEditFlags flags) {
    float col4[4] = { color->r/255.f, color->g/255.f, color->b/255.f, 1.0f };
    if (!ColorEdit4(label, col4, flags | ImGuiColorEditFlags_NoAlpha))
        return false;
    color->r = col4[0]*255.f; color->g = col4[1]*255.f; color->b = col4[2]*255.f;
    return true;
}

// https://github.com/ocornut/imgui/issues/2644
bool ImGui::CheckBoxTristate(const char* label, int* v_tristate) {
    bool ret;
    if (*v_tristate == -1) {
        ImGui::PushItemFlag(ImGuiItemFlags_MixedValue, true);
        bool b = false;
        ret = ImGui::Checkbox(label, &b);
        if (ret) *v_tristate = 1;
        ImGui::PopItemFlag();
    }
    else {
        bool b = (*v_tristate != 0);
        ret = ImGui::Checkbox(label, &b);
        if (ret) *v_tristate = (int)b;
    }
    return ret;
}

// Copy paste of ImGui::TextLink but without the underline bar.
bool ImGui::TextButton(const char* label)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiID id = window->GetID(label);
    const char* label_end = FindRenderedTextEnd(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = CalcTextSize(label, label_end, true);
    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(size, 0.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);
    RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_None);

    if (hovered)
        SetMouseCursor(ImGuiMouseCursor_Hand);

    ImVec4 text_colf = g.Style.Colors[ImGuiCol_TextLink];
    ImVec4 line_colf = text_colf;
    {
        float h, s, v;
        ColorConvertRGBtoHSV(text_colf.x, text_colf.y, text_colf.z, h, s, v);
        if (held || hovered) {
            v = ImSaturate(v + (held ? 0.4f : 0.3f));
            h = ImFmod(h + 0.02f, 1.0f);
        }
        ColorConvertHSVtoRGB(h, s, v, text_colf.x, text_colf.y, text_colf.z);
        v = ImSaturate(v - 0.20f);
        ColorConvertHSVtoRGB(h, s, v, line_colf.x, line_colf.y, line_colf.z);
    }

    PushStyleColor(ImGuiCol_Text, GetColorU32(text_colf));
    RenderText(bb.Min, label, label_end);
    PopStyleColor();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed;
}