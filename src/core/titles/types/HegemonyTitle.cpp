#include "HegemonyTitle.hpp"

HegemonyTitle::HegemonyTitle() :
    HighTitle()
{}

HegemonyTitle::HegemonyTitle(std::string name, sf::Color color, bool landless) :
    HighTitle(name, color, landless)
{}

//////////////////////////////////////////////////////

TitleType HegemonyTitle::GetType() const {
    return TitleType::HEGEMONY;
}

//////////////////////////////////////////////////////