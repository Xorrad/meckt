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
    SEA,
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
    "Drylands",
    "Count",
    "Sea"
};

const std::vector<sf::Color> TerrainTypeColors = {
    sf::Color(204, 163, 102),
    sf::Color(255, 50, 50),
    sf::Color(200, 200, 200),
    sf::Color(255, 255, 255),
    sf::Color(255, 255, 0),
    sf::Color(100, 100, 0),
    sf::Color(100, 100, 255),
    sf::Color(100, 0, 0),
    sf::Color(255, 0, 0),
    sf::Color(100, 20, 20),
    sf::Color(200, 100, 200),
    sf::Color(50, 50, 255),
    sf::Color(200, 200, 0),
    sf::Color(0, 0, 0),
    sf::Color(0, 0, 255),
};

TerrainType TerrainTypefromString(std::string str);