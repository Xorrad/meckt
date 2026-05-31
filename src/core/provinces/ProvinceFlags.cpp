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