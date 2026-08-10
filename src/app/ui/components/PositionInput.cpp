#include "PositionInput.hpp"

#include "core/provinces/ProvinceManager.hpp"
#include "app/menu/EditorMenu.hpp"

bool Components::PositionInput(std::string_view label, EditorMenu& menu, ProvinceManager& provinceManager, sf::Vector2u currentPosition, std::function<void(sf::Vector2u)> onChange) {
    bool valueChanged = false;

    ImGui::PushID(ImHashStr(fmt::format("##position-input-{}", label).c_str()));

    // POSITION TEXT INPUT
    std::string start = fmt::format("{}, {}", currentPosition.x, currentPosition.y);
    if (ImGui::InputTextCommitOnEnter("", &start)) {
        try {
            auto parts = String::Split(start, ",");
            int x = std::stoi(parts.at(0));
            int y = std::stoi(parts.at(1));
            if (x != currentPosition.x || y != currentPosition.y) {
                onChange(sf::Vector2u(x, y));
                valueChanged = true;
            }
        }
        catch (...) {}
    }

    bool wasPickingPosition = menu.GetSelectionHandler().IsPicking(SelectionType::POSITION);
    ImGui::SameLine();
    ImGui::PushFont(ImGui::notoSansNormalFont, ImGui::GetFontSize());
    if (wasPickingPosition) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
    if (ImGui::Button("📌")) {
        if (wasPickingPosition) {
            menu.GetSelectionHandler().CancelPick();
        }
        else {
            menu.GetSelectionHandler().PickPosition(
                [&provinceManager, currentPosition, onChange](sf::Vector2f clickedPosition) {
                    clickedPosition.x = std::max(0.f, clickedPosition.x);
                    clickedPosition.y = std::max(0.f, provinceManager.GetProvincesImage().getSize().y - clickedPosition.y);
                    sf::Vector2u newPosition = sf::Vector2u(clickedPosition);

                    if (newPosition != currentPosition) {
                        onChange(newPosition);
                    }
                }
            );
        }
    }
    if (ImGui::IsItemHovered()) ImGui::SetItemTooltip("Click to pick a position from the map");
    if (wasPickingPosition) ImGui::PopStyleColor();
    ImGui::PopFont();

    ImGui::SameLine();
    ImGui::Text("%s", label.data());

    ImGui::PopID();

    return valueChanged;
}
