#include "Province.hpp"

Province::Province(int id, sf::Color color, std::string name)
: m_Id(id), m_Color(color), m_Name(name) {}

int& Province::GetId() {
    return m_Id;
}

sf::Color& Province::GetColor() {
    return m_Color;
}
 
uint32_t Province::GetColorId() const {
    return (m_Color.r << 16) + (m_Color.g << 8) + m_Color.b;
}

std::string& Province::GetName() {
    return m_Name;
}

void Province::SetColor(sf::Color color) {
    m_Color = color;
}