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

    if ((flags & ProvinceFlags::COASTAL) == ProvinceFlags::COASTAL) {
        result += "Coastal\n";
    }
    else if ((flags & ProvinceFlags::LAKE) == ProvinceFlags::LAKE) {
        result += "Lake\n";
    }
    else if ((flags & ProvinceFlags::ISLAND) == ProvinceFlags::ISLAND) {
        result += "Island\n";
    }
    else if ((flags & ProvinceFlags::LAND) == ProvinceFlags::LAND) {
        result += "Land\n";
    }
    else if ((flags & ProvinceFlags::SEA) == ProvinceFlags::SEA) {
        result += "Sea\n";
    }
    else if ((flags & ProvinceFlags::RIVER) == ProvinceFlags::RIVER) {
        result += "River\n";
    }
    else if ((flags & ProvinceFlags::IMPASSABLE) == ProvinceFlags::IMPASSABLE) {
        result += "Impassable\n";
    }

    // Remove the last newline character
    result = result.substr(0, result.size() - 1);

    return result;
}