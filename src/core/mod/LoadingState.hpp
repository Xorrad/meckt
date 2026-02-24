#pragma once

enum class LoadingState {
    TEXTURES,
    HOLDING_TYPES,
    TERRAIN_TYPES,
    PROVINCES_DEFINITION,
    PROVINCES_IMAGE,
    DEFAULT_MAP,
    PROVINCES_TERRAIN,
    PROVINCES_CLIMATE,
    PROVINCES_HISTORY,
    TITLES,
    TITLES_HISTORY,
    GEOGRAPHICAL_REGIONS,
    CULTURES,
    RELIGIONS,
    LOCALIZATION,
    FINISHED,
};

const std::map<LoadingState, std::string> LoadingStateLabels = {
    { LoadingState::TEXTURES, "Textures" },
    { LoadingState::HOLDING_TYPES, " Holding Types" },
    { LoadingState::TERRAIN_TYPES, "Terrain Types" },
    { LoadingState::PROVINCES_DEFINITION, "Provinces Definition" },
    { LoadingState::PROVINCES_IMAGE, "Provinces Image" },
    { LoadingState::DEFAULT_MAP, "default.map" },
    { LoadingState::PROVINCES_TERRAIN, "Provinces Terrain" },
    { LoadingState::PROVINCES_CLIMATE, "Provinces Climate" },
    { LoadingState::PROVINCES_HISTORY, "Provinces History" },
    { LoadingState::TITLES, "Titles" },
    { LoadingState::TITLES_HISTORY, "Titles History" },
    { LoadingState::GEOGRAPHICAL_REGIONS, "Geographical Regions" },
    { LoadingState::CULTURES, "Cultures" },
    { LoadingState::RELIGIONS, "Religions" },
    { LoadingState::LOCALIZATION, "Localization" },
    { LoadingState::FINISHED, "Finished" }
};