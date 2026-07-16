#include "Configuration.hpp"

#include <nlohmann/json.hpp>
#include <cmrc/cmrc.hpp>

void Configuration::Initialize() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowResolution = { desktopMode.size.x, desktopMode.size.y };

    Configuration::uiScale = std::max(desktopMode.size.x / 1920.f, desktopMode.size.y / 1080.f);

#ifdef DEBUG
    buildVersion = buildVersion + " (debug)";
#endif

#ifdef DEB
    Configuration::resourcesPath = "/usr/local/share/meckt/resources";
    Configuration::settingsFile = "/usr/local/share/meckt/" + Configuration::settingsFile;
#endif

    Load();

    InitializeTextures();
    InitializeFonts();
    InitializeShaders();
}

void Configuration::Deinitialize() {
    Save();
    textures.Clear();
    fonts.Clear();
    shaders.Clear();
}

void Configuration::Load() {
    // Check if there is a legacy settings file saved as yaml, if so, rename it to json.
    if (std::filesystem::exists("settings.yml"))
        std::filesystem::rename("settings.yml", Configuration::settingsFile);

    // Import settings from the specified file as an json object.
    std::ifstream file(Configuration::settingsFile, std::ios::binary);
    if (!file) return;
    nlohmann::json data = nlohmann::json::parse(file);
    file.close();

    Configuration::recentMods = data.value("recent_mods", std::list<std::string>{});
    Configuration::compactTooltip = data.value("compact_tooltip", false);
    Configuration::uiScale = data.value("ui_scale", Configuration::uiScale);
}

void Configuration::Save() {
    // Build a json object with all settings to save.
    nlohmann::json json;
    json["recent_mods"] = Configuration::recentMods;
    json["compact_tooltip"] = Configuration::compactTooltip;
    json["ui_scale"] = Configuration::uiScale;

    // Dump that json object into the settings file.
    std::ofstream file(Configuration::settingsFile, std::ios::out);
    file << json.dump(1, '\t');
    file.close();
}

void Configuration::InitializeTextures() {
    // textures.Load(Textures::LOGO, Configuration::resourcesPath + "/textures/logo.png");
}

void Configuration::InitializeFonts() {
    fonts.Load(Fonts::FIGTREE, Configuration::resourcesPath + "fonts/figtree_medium.ttf");
    fonts.Load(Fonts::NOTO_SANS, Configuration::resourcesPath + "fonts/notosans.ttf");
}

void Configuration::InitializeShaders() {
    shaders.Load(Shaders::PROVINCES, Configuration::resourcesPath + "shaders/provinces.vert", Configuration::resourcesPath + "shaders/provinces.frag");
    shaders.Load(Shaders::HEIGHTMAP_LANDMASS, Configuration::resourcesPath + "shaders/provinces.vert", Configuration::resourcesPath + "shaders/heightmap_landmass.frag");
    shaders.Load(Shaders::PROVINCES_LANDMASS, Configuration::resourcesPath + "shaders/provinces.vert", Configuration::resourcesPath + "shaders/provinces_landmass.frag");
    // shaders.Load(Shaders::PROVINCES, Configuration::resourcesPath + "shaders/provinces.frag", sf::Shader::Fragment);
}