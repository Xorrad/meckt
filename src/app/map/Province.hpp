#pragma once

class Province {
public:
    Province(int id, sf::Color color, std::string name);

    int& GetId();
    sf::Color& GetColor();
    uint32_t GetColorId() const;
    std::string& GetName();

    void SetColor(sf::Color color);

private:
    int m_Id;
    std::string m_Name;
    sf::Color m_Color;
    // Terrain
    // Culture
    // Religion
};