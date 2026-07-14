#include "KingdomTitle.hpp"

KingdomTitle::KingdomTitle() :
    HighTitle()
{}

KingdomTitle::KingdomTitle(std::string name, sf::Color color, bool landless) :
    HighTitle(name, color, landless)
{}

//////////////////////////////////////////////////////

TitleType KingdomTitle::GetType() const {
    return TitleType::KINGDOM;
}

//////////////////////////////////////////////////////