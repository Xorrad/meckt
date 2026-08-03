#include "CountyTitle.hpp"

CountyTitle::CountyTitle() :
    HighTitle()
{}

CountyTitle::CountyTitle(std::string name, sf::Color color, bool landless) :
    HighTitle(name, color, landless)
{}

//////////////////////////////////////////////////////

TitleType CountyTitle::GetType() const {
    return TitleType::COUNTY;
}

//////////////////////////////////////////////////////