#include "EmpireTitle.hpp"

EmpireTitle::EmpireTitle() :
    HighTitle()
{}

EmpireTitle::EmpireTitle(std::string name, sf::Color color, bool landless) :
    HighTitle(name, color, landless)
{}

//////////////////////////////////////////////////////

TitleType EmpireTitle::GetType() const {
    return TitleType::EMPIRE;
}

//////////////////////////////////////////////////////