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

    io.Fonts->Clear();

    // Use NotoSans with extended latin as the global default font.
    ImFontGlyphRangesBuilder builder;
    ImVector<ImWchar> defaultRanges;
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesDefault()); // Basic Latin + Latin Supplement
    builder.AddRanges(new ImWchar[]{ 0x0080, 0xFFFF, 0 }); // Latin-1 Supplement + Latin Extended-A + Latin Extended-B 
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesGreek()); // Greek and Coptic
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic()); // Cyrillic + Cyrillic Supplement + Cyrillic Extended-A + Cyrillic Extended-B
    builder.BuildRanges(&defaultRanges);
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF((Configuration::assetsPath + "/fonts/NotoSans-VariableFont_wdth,wght.ttf").c_str(), 18.0f, NULL, defaultRanges.Data);
    io.FontDefault = defaultFont;
    
    // Extend the normal sized noto sans font with emojis.
    notoSansNormalFont = io.Fonts->AddFontFromFileTTF((Configuration::assetsPath + "/fonts/NotoSans-VariableFont_wdth,wght.ttf").c_str(), 20.0f);
    ImFontConfig emojiConfig;
    emojiConfig.MergeMode = true;
    emojiConfig.PixelSnapH = true;
    emojiConfig.GlyphMinAdvanceX = 20.0f;
    const ImWchar emojiRanges[] = {
        0x1F300, 0x1F5FF, // Miscellaneous Symbols and Pictographs
        0x1F600, 0x1F64F, // Emoticons
        0x1F680, 0x1F6FF, // Transport & Map Symbols
        0x1F900, 0x1F9FF, // Supplemental Symbols and Pictographs
        0x2600,  0x26FF,  // Misc symbols (sun, umbrella, etc.)
        0x2700,  0x27BF,  // Dingbats
        0x200D,  0x200D,  // Zero Width Joiner (for combining emojis)
        0xFE0F,  0xFE0F,  // Variation Selector-16 (emoji style)
        0
    };
    io.Fonts->AddFontFromFileTTF((Configuration::assetsPath + "/fonts/NotoEmoji-VariableFont_wght.ttf").c_str(), 20.0f, &emojiConfig, emojiRanges);


    // Load NotoSans in different size for the main menu.
    notoSansMediumFont = io.Fonts->AddFontFromFileTTF((Configuration::assetsPath + "/fonts/NotoSans-VariableFont_wdth,wght.ttf").c_str(), 30.0f);
    notoSansLargeFont = io.Fonts->AddFontFromFileTTF((Configuration::assetsPath + "/fonts/NotoSans-VariableFont_wdth,wght.ttf").c_str(), 72.0f);

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

// Copy paste of ImGui::InputTextEx but without the cursor, nor the selection.
bool ImGui::InputTextLocked(const char* label, std::string* str) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;

    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 frame_size = CalcItemSize(ImVec2(0, 0), CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
    const ImVec2 total_size = ImVec2(frame_size.x + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), frame_size.y);

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + frame_size);
    const ImRect total_bb(frame_bb.Min, frame_bb.Min + total_size);

    ImGuiWindow* draw_window = window;
    ImVec2 inner_size = frame_size;

    ItemSize(total_bb, style.FramePadding.y);

    const bool hovered = ItemHoverable(frame_bb, id, g.LastItemData.InFlags);
    const bool pressed = hovered && io.MouseClicked[0];

    // Render frame
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);

    const ImVec4 clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + inner_size.x, frame_bb.Min.y + inner_size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 draw_pos = frame_bb.Min + style.FramePadding;
    ImVec2 text_size(0.0f, 0.0f);

    const char* buf_display = str->c_str();
    const char* buf_display_end = buf_display + strlen(buf_display);

    // Render text only (no selection, no cursor)
    ImU32 col = GetColorU32(ImGuiCol_Text);
    draw_window->DrawList->AddText(g.Font, g.FontSize, draw_pos, col, buf_display, buf_display_end, 0.0f, &clip_rect);

    if (label_size.x > 0)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    return pressed;
}