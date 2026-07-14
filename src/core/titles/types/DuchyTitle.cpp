#include "DuchyTitle.hpp"

DuchyTitle::DuchyTitle() :
    HighTitle()
{}

DuchyTitle::DuchyTitle(std::string name, sf::Color color, bool landless) :
    HighTitle(name, color, landless)
{}

//////////////////////////////////////////////////////

TitleType DuchyTitle::GetType() const {
    return TitleType::DUCHY;
}

//////////////////////////////////////////////////////