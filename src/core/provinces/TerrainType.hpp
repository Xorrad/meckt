#pragma once

class TerrainType {
public:
    TerrainType();
    TerrainType(const std::string& name, const sf::Color& color);

    std::string GetName() const;
    void SetName(const std::string& name);
    
    sf::Color GetColor() const;
    void SetColor(const sf::Color& color);

private:
    std::string m_Name;
    sf::Color m_Color;
};