#pragma once

class Mod {
public:
    Mod(const std::string& dir);

    std::string GetDir() const;
    sf::Image& getHeightmapImage();
    sf::Image& getProvinceImage();
    sf::Image& getRiversImage();
    bool HasMap() const;

    void LoadMapModeTexture(sf::Texture& texture, MapMode mode);
    void Load();

private:
    std::string m_Dir;
    sf::Image m_HeightmapImage;
    sf::Image m_ProvinceImage;
    sf::Image m_RiversImage;
};