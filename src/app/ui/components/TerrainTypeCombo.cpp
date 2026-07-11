#include "Components.hpp"

#include "core/provinces/Province.hpp"
#include "core/provinces/TerrainType.hpp"

bool Components::TerrainTypeCombo(const OrderedMap<std::string, TerrainType>& terrainTypes, const std::string& currentTerrainType, std::function<void(const TerrainType*)> onSelect) {
    bool valueChanged = false;
    if (ImGui::BeginCombo("terrain type", currentTerrainType.c_str())) {
        for (const auto& [terrainTypeName, terrainType] : terrainTypes) {
            const bool isSelected = (currentTerrainType == terrainTypeName);
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
        for (const auto& [holdingTypeName, holdingType] : holdingTypes) {
            const bool isSelected = (currentHoldingType == holdingTypeName);
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
        for (int i = 0; i < static_cast<int>(ClimateType::COUNT); i++) {
            ClimateType climateType = (ClimateType) i;
            const bool isSelected = (currentClimateType == climateType);
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