#pragma once

#include "util/String.hpp"
enum class MapMode {
    PROVINCES       = 0,
    HEIGHTMAP       = 1,
    RIVERS          = 2,
    FLAGS           = 3,
    TERRAIN         = 4,
    CLIMATE         = 5,
    WINTER_SEVERITY = 6,
    CULTURE         = 7,
    FAITH           = 8,
    BARONY          = 9,
    COUNTY          = 10,
    DUCHY           = 11,
    KINGDOM         = 12,
    EMPIRE          = 13,
    HEGEMONY        = 14,
    COUNT           = 15,
};
const int MapModeTitleStartIndex = static_cast<int>(MapMode::BARONY);

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Flags", "Terrain", "Climate", "Winter Severity", "Culture", "Faith",
    "Barony", "County", "Duchy", "Kingdom", "Empire", "Hegemony"
};

inline MapMode MapModeFromString(std::string str) {
    str = String::ToLowercase(str);
    if (str == "province") return MapMode::PROVINCES;
    if (str == "heightmap") return MapMode::HEIGHTMAP;
    if (str == "rivers") return MapMode::RIVERS;
    if (str == "flags") return MapMode::FLAGS;
    if (str == "terrain") return MapMode::TERRAIN;
    if (str == "climate") return MapMode::CLIMATE;
    if (str == "winter_severity") return MapMode::WINTER_SEVERITY;
    if (str == "culture") return MapMode::CULTURE;
    if (str == "faith") return MapMode::FAITH;
    if (str == "barony") return MapMode::BARONY;
    if (str == "county") return MapMode::COUNTY;
    if (str == "duchy") return MapMode::DUCHY;
    if (str == "kingdom") return MapMode::KINGDOM;
    if (str == "empire") return MapMode::EMPIRE;
    if (str == "hegemony") return MapMode::HEGEMONY;
    return MapMode::PROVINCES;
}

inline TitleType MapModeToTileType(MapMode mode) {
    return static_cast<TitleType>(static_cast<int>(mode) - MapModeTitleStartIndex);
}

inline MapMode TitleTypeToMapMode(TitleType type) {
    return static_cast<MapMode>(static_cast<int>(type) + MapModeTitleStartIndex);
}

inline bool MapModeIsTitle(MapMode mode) {
    return static_cast<int>(mode) >= static_cast<int>(MapMode::BARONY) && static_cast<int>(mode) <= static_cast<int>(MapMode::HEGEMONY);
}

inline bool MapModeIsProvince(MapMode mode) {
    switch (mode) {
        case MapMode::PROVINCES:
        case MapMode::FLAGS:
        case MapMode::TERRAIN:
        case MapMode::CLIMATE:
        case MapMode::WINTER_SEVERITY:
        case MapMode::CULTURE:
        case MapMode::FAITH:
            return true;
        default:
            return false;
    }
}