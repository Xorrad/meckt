#include "Mod.hpp"
#include <filesystem>

Mod::Mod(const std::string& dir)
: m_Dir(dir) {}

std::string Mod::GetDir() const {
    return m_Dir;
}

sf::Image& Mod::getHeightmapImage() {
    return m_HeightmapImage;
}

sf::Image& Mod::getProvinceImage() {
    return m_ProvinceImage;
}

sf::Image& Mod::getRiversImage() {
    return m_RiversImage;
}

bool Mod::HasMap() const {
    return std::filesystem::exists(m_Dir + "/map_data/provinces.png");
}

void Mod::LoadMapModeTexture(sf::Texture& texture, MapMode mode) {
    switch(mode) {
        case MapMode::PROVINCES:
            texture.loadFromImage(m_ProvinceImage);
            return;
        case MapMode::HEIGHTMAP:
            texture.loadFromImage(m_HeightmapImage);
            return;
        case MapMode::RIVERS:
            texture.loadFromImage(m_RiversImage);
            return;
    }
}

void Mod::Load() {
    if(!this->HasMap())
        return;

    // TODO: catch exceptions.
    m_HeightmapImage.loadFromFile(m_Dir + "/map_data/heightmap.png");
    m_ProvinceImage.loadFromFile(m_Dir + "/map_data/provinces.png");
    m_RiversImage.loadFromFile(m_Dir + "/map_data/rivers.png");
}