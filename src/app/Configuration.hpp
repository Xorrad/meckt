#pragma once

#include "app/ResourceManager.hpp"

enum class Textures : int {
    LOGO,
    COUNT
};

enum class Fonts : int {
    FIGTREE,
    COUNT
};

enum class Shaders : int {
    PROVINCES,
    HEIGHTMAP_LANDMASS,
    PROVINCES_LANDMASS,
    COUNT
};

class Configuration {
public:
    Configuration() = delete;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    //Settings
    inline static std::string buildVersion = "1.7.0";
    inline static std::string buildCredits = "made by Xorrad";
    inline static std::string githubURL = "https://github.com/Xorrad/meckt";
    inline static std::string atlantisURL = "https://codeload.github.com/bombusfrigidus/Atlantis/zip/refs/heads/main";
    
    // Saved settings
    inline static std::string settingsFile = "settings.json";
    inline static std::list<std::string> recentMods = std::list<std::string>{};
    inline static float defaultWaterLevel = 3.8f;
    inline static bool compactTooltip = false;

    // Graphics
    inline static sf::Vector2u windowResolution = {800, 600};
    
    // Resources
    inline static ResourceManager<sf::Texture, Textures> textures = ResourceManager<sf::Texture, Textures>("texture");
    inline static ResourceManager<sf::Font, Fonts> fonts = ResourceManager<sf::Font, Fonts>("font");
    inline static ResourceManager<sf::Shader, Shaders> shaders = ResourceManager<sf::Shader, Shaders>();

    static void Initialize();
    static void Load();
    static void Save();

private:
    static void InitializeTextures();
    static void InitializeFonts();
    static void InitializeShaders();
};