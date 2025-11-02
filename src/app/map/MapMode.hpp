#pragma once

enum class TitleType : int;

enum class MapMode {
    PROVINCES       = 0,
    HEIGHTMAP       = 1,
    RIVERS          = 2,
    TERRAIN         = 3,
    WINTER_SEVERITY = 4,
    CULTURE         = 5,
    RELIGION        = 6,
    BARONY          = 7,
    COUNTY          = 8,
    DUCHY           = 9,
    KINGDOM         = 10,
    EMPIRE          = 11,
    HEGEMONY        = 12,
    COUNT           = 13,
};
static int MapModeTitleStartIndex = static_cast<int>(MapMode::BARONY);

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Terrain", "Winter Severity", "Culture", "Religion",
    "Barony", "County", "Duchy", "Kingdom", "Empire", "Hegemony"
};

inline TitleType MapModeToTileType(MapMode mode) {
    return (TitleType) ((int) mode - MapModeTitleStartIndex);
}

inline MapMode TitleTypeToMapMode(TitleType type) {
    return (MapMode) ((int) type + MapModeTitleStartIndex);
}

inline bool MapModeIsTitle(MapMode mode) {
    return ((int) mode) >= (int) MapMode::BARONY && ((int) mode) <= (int) MapMode::HEGEMONY;
}