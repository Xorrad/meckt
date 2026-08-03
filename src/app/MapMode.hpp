#pragma once

enum class MapMode {
    PROVINCES       = 0,
    HEIGHTMAP       = 1,
    RIVERS          = 2,
    TERRAIN         = 3,
    WINTER_SEVERITY = 4,
    CULTURE         = 5,
    FAITH        = 6,
    BARONY          = 7,
    COUNTY          = 8,
    DUCHY           = 9,
    KINGDOM         = 10,
    EMPIRE          = 11,
    HEGEMONY        = 12,
    COUNT           = 13,
};
const int MapModeTitleStartIndex = static_cast<int>(MapMode::BARONY);

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Terrain", "Winter Severity", "Culture", "Faith",
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