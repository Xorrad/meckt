#include "Configuration.hpp"

#include <nlohmann/json.hpp>

void Configuration::Initialize() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowResolution = {desktopMode.width, desktopMode.height};

#ifdef DEBUG
    buildVersion = buildVersion + " (debug)";
#endif

    Load();

    InitializeTextures();
    InitializeFonts();
    InitializeShaders();
}

void Configuration::Load() {
    // Import settings from the specified file as an json object.
    std::ifstream file(Configuration::settingsFile, std::ios::binary);
    if (!file) return;
    nlohmann::json data = nlohmann::json::parse(file);
    file.close();

    recentMods = data.value("recent_mods", std::list<std::string>{});
}

void Configuration::Save() {
    // Build a json object with all settings to save.
    nlohmann::json json;
    json["recent_mods"] = Configuration::recentMods;

    // Dump that json object into the settings file.
    std::ofstream file(Configuration::settingsFile, std::ios::out);
    file << json.dump(1, '\t');
    file.close();
}

void Configuration::InitializeTextures() {
    textures.Load(Textures::LOGO, "assets/textures/logo.png");
}

void Configuration::InitializeFonts() {
    fonts.Load(Fonts::FIGTREE, "assets/fonts/Figtree-Medium.ttf");
}

void Configuration::InitializeShaders() {
    shaders.Load(Shaders::PROVINCES, "assets/shaders/provinces.vert", "assets/shaders/provinces.frag");
    // shaders.Load(Shaders::PROVINCES, "assets/shaders/provinces.frag", sf::Shader::Fragment);
}