#include "Mod.hpp"
#include <filesystem>

Mod::Mod(const std::string& dir)
: m_Dir(dir) {}

std::string Mod::GetDir() const {
    return m_Dir;
}

bool Mod::HasMap() const {
    return std::filesystem::exists(m_Dir + "/map_data/provinces.png");
}