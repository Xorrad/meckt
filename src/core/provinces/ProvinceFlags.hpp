#pragma once

enum class ProvinceFlags {
    NONE       = 0,
    COASTAL    = 1 << 0,
    LAKE       = 1 << 1,
    ISLAND     = 1 << 2,
    LAND       = 1 << 3,
    SEA        = 1 << 4,
    RIVER      = 1 << 5,
    IMPASSABLE = 1 << 6,
};

ProvinceFlags operator|(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator&(ProvinceFlags a, ProvinceFlags b);
ProvinceFlags operator~(ProvinceFlags a);
ProvinceFlags& operator|=(ProvinceFlags& a, ProvinceFlags b);
ProvinceFlags& operator&=(ProvinceFlags& a, ProvinceFlags b);

std::string ProvinceFlagsToString(ProvinceFlags flags);
sf::Color ProvincesFlagsToColor(ProvinceFlags flags);