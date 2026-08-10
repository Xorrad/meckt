#include "ProvinceInput.hpp"

#include "core/provinces/ProvinceManager.hpp"
#include "app/menu/EditorMenu.hpp"

bool Components::ProvinceInput(std::string_view label, EditorMenu& menu, ProvinceManager& provinceManager, int currentProvinceId, std::function<void(int)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##province-input-{}", label).c_str()));

    // PROVINCE ID TEXT INPUT
    std::string provinceId = std::to_string(currentProvinceId);
    if (ImGui::InputTextCommitOnEnter("", &provinceId, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter, Components::Filters::Numeric)) {
        try {
            if (std::stoi(provinceId) != currentProvinceId) {
                onChange(std::stoi(provinceId));
                valueChanged = true;
            }
        }
        catch(...) {}
    }

    // PROVINCE PICKER BUTTON
    bool wasPickingProvince = menu.GetSelectionHandler().IsPicking(SelectionType::PROVINCE);
    ImGui::SameLine();
    ImGui::PushFont(ImGui::notoSansNormalFont, ImGui::GetFontSize());
    if (wasPickingProvince) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    if (ImGui::Button("📌")) {
        if (wasPickingProvince) {
            menu.GetSelectionHandler().CancelPick();
        }
        else {
            // Pick from the PROVINCES map mode, restoring whatever mode we're
            // currently in once the pick stops (success or cancel).
            menu.GetSelectionHandler().PickProvince(
                [onChange](Province* clickedProvince) {
                    onChange(clickedProvince->GetId());
                },
                false,
                MapMode::PROVINCES,
                menu.GetMapMode()
            );
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetItemTooltip("Click to pick a province from the map");
    if (wasPickingProvince) ImGui::PopStyleColor();

    // PROVINCE FOCUS BUTTON
    ImGui::SameLine();
    if (ImGui::Button("🔎")) {
        Province* province = provinceManager.GetProvinceById(currentProvinceId);
        if (province != nullptr) {
            menu.SwitchMapMode(MapMode::PROVINCES, true);
            menu.GetSelectionHandler().Select(province);
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetItemTooltip("Focus this province");
    ImGui::PopFont();

    // LABEL
    ImGui::SameLine();
    ImGui::Text("%s", label.data());

    // UNDEFINED PROVINCE ICON
    if (!provinceManager.HasProvinceById(currentProvinceId)) {
        ImGui::SameLine();
        ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
        ImGui::Text("⚠️");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("No province with this id exists!");
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    ImGui::PopID();

    return valueChanged;
}
