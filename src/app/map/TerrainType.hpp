#pragma once

enum class TerrainType {
    PLAINS,
    FARMLANDS,
    HILLS,
    MOUNTAINS,
    DESERT,
    DESERT_MOUNTAINS,
    OASIS,
    JUNGLE,
    FOREST,
    WETLANDS,
    STEPPE,
    FLOODPLAINS,
    DRYLANDS,
    COUNT,
};

const std::vector<const char*> TerrainTypeLabels = {
    "Plains",
    "Farmlands",
    "Hills",
    "Mountains",
    "Desert",
    "Desert Mountains",
    "Oasis",
    "Jungle",
    "Forest",
    "Wetlands",
    "Steppe",
    "Floodplains",
    "Drylands"
};

TerrainType TerrainTypefromString(std::string str);