#include "ProvinceFlags.hpp"

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) | static_cast<int>(b));
}


ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b) {
    return static_cast<ProvinceFlags>(static_cast<int>(a) & static_cast<int>(b));
}

ProvinceFlags operator~(ProvinceFlags a) {
    return static_cast<ProvinceFlags>(~static_cast<int>(a));
}

ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a | b;
}

ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b) {
    return a = a & b;
}

std::string ProvinceFlagsToString(ProvinceFlags flags) {
    std::string result = "";

    if (flags == ProvinceFlags::NONE) {
        return "None";
    }

    if ((flags & ProvinceFlags::COASTAL) == ProvinceFlags::COASTAL) result += "Coastal, ";
    if ((flags & ProvinceFlags::LAKE) == ProvinceFlags::LAKE) result += "Lake, ";
    if ((flags & ProvinceFlags::ISLAND) == ProvinceFlags::ISLAND) result += "Island, ";
    if ((flags & ProvinceFlags::LAND) == ProvinceFlags::LAND) result += "Land, ";
    if ((flags & ProvinceFlags::SEA) == ProvinceFlags::SEA) result += "Sea, ";
    if ((flags & ProvinceFlags::RIVER) == ProvinceFlags::RIVER) result += "River, ";
    if ((flags & ProvinceFlags::IMPASSABLE) == ProvinceFlags::IMPASSABLE) result += "Impassable, ";

    if (result.ends_with(", ")) {
        result = result.substr(0, result.size() - 2);
    }

    return result;
}

sf::Color ProvincesFlagsToColor(ProvinceFlags flags) {
    sf::Color color = sf::Color(0, 0, 0); // Default to black.

    if ((flags & ProvinceFlags::SEA) == ProvinceFlags::SEA) {
        color = sf::Color(50, 50, 255); // Blue for SEA.
    }
    else if ((flags & ProvinceFlags::LAND) == ProvinceFlags::LAND) {
        color = sf::Color(0, 150, 70); // Green for LAND.
    }
    else if ((flags & ProvinceFlags::RIVER) == ProvinceFlags::RIVER) {
        color = sf::Color(0, 200, 255); // Light Blue for RIVER.
    }
    else if ((flags & ProvinceFlags::LAKE) == ProvinceFlags::LAKE) {
        color = sf::Color(0, 0, 50); // Dark Blue for LAKE.
    }

    if ((flags & ProvinceFlags::IMPASSABLE) == ProvinceFlags::IMPASSABLE) {
        color.a = 0; // Add stripes for impassable provinces.
    }

    return color;
}