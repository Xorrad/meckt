#pragma once

enum class MapMode {
    PROVINCES       = 0,
    HEIGHTMAP       = 1,
    RIVERS          = 2,
    TERRAIN         = 3,
    CLIMATE         = 4,
    WINTER_SEVERITY = 5,
    CULTURE         = 6,
    FAITH           = 7,
    BARONY          = 8,
    COUNTY          = 9,
    DUCHY           = 10,
    KINGDOM         = 11,
    EMPIRE          = 12,
    HEGEMONY        = 13,
    COUNT           = 14,
};
const int MapModeTitleStartIndex = static_cast<int>(MapMode::BARONY);

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Terrain", "Climate", "Winter Severity", "Culture", "Faith",
    "Barony", "County", "Duchy", "Kingdom", "Empire", "Hegemony"
};

inline TitleType MapModeToTileType(MapMode mode) {
    return static_cast<TitleType>(static_cast<int>(mode) - MapModeTitleStartIndex);
}

inline MapMode TitleTypeToMapMode(TitleType type) {
    return static_cast<MapMode>(static_cast<int>(type) + MapModeTitleStartIndex);
}

inline bool MapModeIsTitle(MapMode mode) {
    return static_cast<int>(mode) >= static_cast<int>(MapMode::BARONY) && static_cast<int>(mode) <= static_cast<int>(MapMode::HEGEMONY);
}