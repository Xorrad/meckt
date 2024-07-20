#include "Configuration.hpp"

void Configuration::Initialize() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowResolution = {desktopMode.width, desktopMode.height};

    InitializeTextures();
    InitializeFonts();
}

void Configuration::InitializeTextures() {
    // textures.Load(Textures::TEST, "assets/textures/test.jpeg");
}

void Configuration::InitializeFonts() {
    fonts.Load(Fonts::FIGTREE, "assets/fonts/Figtree-Medium.ttf");
}