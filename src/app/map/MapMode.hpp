#pragma once

enum class MapMode {
    PROVINCES,
    HEIGHTMAP,
    RIVERS,
    TERRAIN,
    CULTURE,
    RELIGION,
    COUNTY,
    DUCHY,
    KINGDOM,
    EMPIRE,
    COUNT,
};

const std::vector<const char*> MapModeLabels = {
    "Provinces", "Heightmap", "Rivers",
    "Terrain", "Culture", "Religion",
    "County", "Duchy", "Kingdom", "Empire" 
};