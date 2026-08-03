#pragma once

#include "core/mod/Mod.hpp"
#include "core/provinces/ProvinceManager.hpp"
#include "core/titles/TitleManager.hpp"

enum class MapTooltip {
    PROVINCE        = 0,
    FLAGS           = 1,
    HOLDING         = 2,
    TERRAIN         = 3,
    CULTURE         = 4,
    FAITH           = 5,
    CLIMATE         = 6,
    WINTER_SEVERITY = 7,
    IMAGE_POSITION  = 8,
    TITLES          = 9,
    COUNT           = 10,
};

inline std::string_view MapTooltipToString(MapTooltip tooltip) {
    switch(tooltip) {
        case MapTooltip::PROVINCE: return "Province";
        case MapTooltip::FLAGS: return "Flags";
        case MapTooltip::HOLDING: return "Holding";
        case MapTooltip::TERRAIN: return "Terrain";
        case MapTooltip::CULTURE: return "Culture";
        case MapTooltip::FAITH: return "Faith";
        case MapTooltip::CLIMATE: return "Climate";
        case MapTooltip::WINTER_SEVERITY: return "Winter Severity";
        case MapTooltip::IMAGE_POSITION: return "Image Position";
        case MapTooltip::TITLES: return "Titles";
        default: return "Unknown";
    }
}

inline MapTooltip MapTooltipFromString(std::string_view str) {
    if (str == "Province") return MapTooltip::PROVINCE;
    if (str == "Flags") return MapTooltip::FLAGS;
    if (str == "Holding") return MapTooltip::HOLDING;
    if (str == "Terrain") return MapTooltip::TERRAIN;
    if (str == "Culture") return MapTooltip::CULTURE;
    if (str == "Faith") return MapTooltip::FAITH;
    if (str == "Climate") return MapTooltip::CLIMATE;
    if (str == "Winter Severity") return MapTooltip::WINTER_SEVERITY;
    if (str == "Image Position") return MapTooltip::IMAGE_POSITION;
    if (str == "Titles") return MapTooltip::TITLES;
    return MapTooltip::COUNT;
}

inline std::string GetMapTooltipString(Mod& mod, Province* province, MapMode mapMode, MapTooltip tooltip, bool onlyMainTitle = false) {
    switch(tooltip) {
        case MapTooltip::PROVINCE:        return fmt::format("#{} - {}", province->GetId(), province->GetName());
        case MapTooltip::FLAGS:           return fmt::format("Flags  : {}", ProvinceFlagsToString(province->GetFlags()));
        case MapTooltip::HOLDING:         return fmt::format("Holding: {}", province->GetHolding());
        case MapTooltip::TERRAIN:         return fmt::format("Terrain: {}", province->GetTerrain());
        case MapTooltip::CULTURE:         return fmt::format("Culture: {}", province->GetCulture());
        case MapTooltip::FAITH:           return fmt::format("Faith  : {}", province->GetFaith());
        case MapTooltip::CLIMATE:         return fmt::format("Climate: {}", ClimateTypeLabels.at(province->GetClimateType()));
        case MapTooltip::WINTER_SEVERITY: return fmt::format("Winter Severity Bias: {}", province->GetWinterSeverityBias());
        case MapTooltip::IMAGE_POSITION:  return fmt::format("Image Position: {}, {}", province->GetImagePosition().x, province->GetImagePosition().y);
        case MapTooltip::TITLES: {
            std::string titlesText;

            Title* hoveredBarony = province->GetProvinceLiegeTitle(
                mod.GetTitleManager(), 
                TitleType::BARONY
            );

            Title* hoveredTitle = province->GetProvinceFocusedTitle(
                mod.GetTitleManager(), 
                MapModeToTileType(mapMode)
            );

            Title* title = hoveredBarony;
            while(title != nullptr) {
                bool isMainTitle = (title == hoveredTitle && MapModeIsTitle(mapMode));
                titlesText += fmt::format("{}\n", (isMainTitle == onlyMainTitle ? title->GetName() : ""));
                title = title->GetLiegeTitle();
            }
            return titlesText;
        }
        default: return "";
    }
}