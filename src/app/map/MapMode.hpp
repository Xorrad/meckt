#pragma once

enum class MapMode {
    PROVINCES,
    HEIGHTMAP,
    RIVERS,
    COUNT,
};

const std::vector<const char*> MapModeLabels = { "Provinces", "Heightmap", "Rivers" };