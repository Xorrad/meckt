#pragma once

enum class TitleType : int;

enum class MapMode {
    PROVINCES,
    HEIGHTMAP,
    RIVERS,
    TERRAIN,
    CULTURE,
    RELIGION,
    BARONY,
    COUNTY,
    DUCHY,
    KINGDOM,
    EMPIRE,
    COUNT,
};

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Terrain", "Culture", "Religion",
    "Barony", "County", "Duchy", "Kingdom", "Empire" 
};

inline TitleType MapModeToTileType(MapMode mode) {
    return (TitleType) ((int) mode - 6);
}

inline MapMode TitleTypeToMapMode(TitleType type) {
    return (MapMode) ((int) type + 6);
}

inline bool MapModeIsTitle(MapMode mode) {
    return ((int) mode) >= (int) MapMode::BARONY && ((int) mode) <= (int) MapMode::EMPIRE;
}