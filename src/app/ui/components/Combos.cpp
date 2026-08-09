#include "Combos.hpp"

#include "core/provinces/Province.hpp"
#include "core/provinces/TerrainType.hpp"

bool Components::TerrainTypeCombo(const OrderedMap<std::string, TerrainType>& terrainTypes, const std::string& currentTerrainType, std::function<void(const TerrainType*)> onSelect) {
    bool valueChanged = false;
    if (ImGui::BeginCombo("terrain type", currentTerrainType.c_str())) {
        static ImGuiTextFilter filter;
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        filter.Draw("##Filter", -FLT_MIN);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere(-1);

        for (const auto& [terrainTypeName, terrainType] : terrainTypes) {
            const bool isSelected = (currentTerrainType == terrainTypeName);
            if (!filter.PassFilter(terrainTypeName.c_str()))
                continue;
            if (ImGui::Selectable(terrainTypeName.c_str(), isSelected)) {
                onSelect(&terrainType);
                valueChanged = true;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return valueChanged;
}

bool Components::HoldingTypeCombo(const OrderedMap<std::string, HoldingType>& holdingTypes, const std::string& currentHoldingType, std::function<void(const HoldingType*)> onSelect) {
    bool valueChanged = false;
    if (ImGui::BeginCombo("holding type", currentHoldingType.c_str())) {
        static ImGuiTextFilter filter;
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        filter.Draw("##Filter", -FLT_MIN);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere(-1);

        for (const auto& [holdingTypeName, holdingType] : holdingTypes) {
            const bool isSelected = (currentHoldingType == holdingTypeName);
            if (!filter.PassFilter(holdingTypeName.c_str()))
                continue;
            if (ImGui::Selectable(holdingTypeName.c_str(), isSelected)) {
                onSelect(&holdingType);
                valueChanged = true;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return valueChanged;
}

bool Components::ClimateTypeCombo(ClimateType currentClimateType, std::function<void(ClimateType)> onSelect) {
    bool valueChanged = false;
    if (ImGui::BeginCombo("climate type", ClimateTypeLabels.at(currentClimateType))) {
        static ImGuiTextFilter filter;
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        filter.Draw("##Filter", -FLT_MIN);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere(-1);

        for (int i = 0; i < static_cast<int>(ClimateType::COUNT); i++) {
            ClimateType climateType = (ClimateType) i;
            const bool isSelected = (currentClimateType == climateType);
            if (!filter.PassFilter(ClimateTypeLabels.at(climateType)))
                continue;
            if (ImGui::Selectable(ClimateTypeLabels.at(climateType), isSelected)) {
                onSelect(climateType);
                valueChanged = true;
            }
            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return valueChanged;
}

bool Components::AdjacencyTypeCombo(std::string currentType, std::function<void(std::string)> onSelect) {
    static const std::vector<std::string> adjacencyTypes = {
        "sea",
        "river_large"
    };

    bool valueChanged = false;
    if (ImGui::BeginCombo("type", currentType.c_str())) {
        static ImGuiTextFilter filter;
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
        filter.Draw("##Filter", -FLT_MIN);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere(-1);
        
        for (std::string type : adjacencyTypes)  {
            const bool isSelected = (currentType == type);
            if (!filter.PassFilter(type.c_str()))
                continue;
            if (ImGui::Selectable(type.c_str(), isSelected)) {
                onSelect(type);
                valueChanged = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    return valueChanged;
}