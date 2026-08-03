#pragma once

using GamePath = std::string_view;

namespace Paths {
    // Common directory paths.
    static constexpr GamePath COMMON_LANDED_TITLES = "common/landed_titles/";
    static constexpr GamePath COMMON_HOLDINGS = "common/holdings/";
    static constexpr GamePath COMMON_TERRAIN_TYPES = "common/terrain_types/";
    static constexpr GamePath COMMON_PROVINCE_TERRAIN = "common/province_terrain/";
    static constexpr GamePath COMMON_CULTURES = "common/culture/cultures/";
    static constexpr GamePath COMMON_RELIGIONS_TYPES = "common/religion/religion_types/";
    static constexpr GamePath COMMON_DEFINES = "common/defines/";

    // History directory paths.
    static constexpr GamePath HISTORY_TITLES = "history/titles/";
    static constexpr GamePath HISTORY_PROVINCES = "history/provinces/";

    // Map data directory paths.
    static constexpr GamePath MAP_DATA_DEFINITIONS = "map_data/definition.csv";
    static constexpr GamePath MAP_DATA_PROVINCES = "map_data/provinces.png";
    static constexpr GamePath MAP_DATA_HEIGHTMAP = "map_data/heightmap.png";
    static constexpr GamePath MAP_DATA_RIVERS = "map_data/rivers.png";
    static constexpr GamePath MAP_DATA_DEFAULT_MAP = "map_data/default.map";
    static constexpr GamePath MAP_DATA_CLIMATE = "map_data/climate.txt";
    static constexpr GamePath MAP_DATA_GEOGRAPHICAL_REGIONS = "map_data/geographical_regions/";

    // Localization directory paths.
    // TODO: rework paths system for different localization languages.
    static constexpr GamePath LOCALIZATION_ENGLISH = "localization/english/";
    static constexpr GamePath LOCALIZATION_REPLACE_ENGLISH = "localization/replace/english/";
}