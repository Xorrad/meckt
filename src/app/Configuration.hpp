#pragma once

#include "app/ResourceManager.hpp"

enum class Textures : int {
    COUNT
};

enum class Fonts : int {
    FIGTREE,
    COUNT
};

class Configuration {
public:
    Configuration() = delete;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    //Graphics
    inline static sf::Vector2u windowResolution = {800, 600};
    
    // Resources
    inline static ResourceManager<sf::Texture, Textures> textures = ResourceManager<sf::Texture, Textures>("texture");
    inline static ResourceManager<sf::Font, Fonts> fonts = ResourceManager<sf::Font, Fonts>("font");

    static void Initialize();

private:
    static void InitializeTextures();
    static void InitializeFonts();
};