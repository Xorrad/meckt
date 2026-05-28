#pragma once

class Culture {
public:
    Culture();
    Culture(const std::string& name, const sf::Color& color);

    std::string GetName() const;
    sf::Color GetColor() const;

    void SetName(const std::string& name);
    void SetColor(const sf::Color& color);

private:
    std::string m_Name;
    sf::Color m_Color;
};