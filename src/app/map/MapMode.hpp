#pragma once

enum class MapMode {
    PROVINCES,
    HEIGHTMAP,
    RIVERS,
    TERRAIN,
    CULTURE,
    RELIGION,
    COUNT,
};

const std::vector<const char*> MapModeLabels = { "Provinces", "Heightmap", "Rivers", "Terrain", "Culture", "Religion" };