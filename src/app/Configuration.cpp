#include "Configuration.hpp"

#include "MapTooltip.hpp"

#include <nlohmann/json.hpp>
#include <cmrc/cmrc.hpp>

void Configuration::Initialize() {
    sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
    windowResolution = { desktopMode.size.x, desktopMode.size.y };

    Configuration::uiScale = std::max(desktopMode.size.x / 1920.f, desktopMode.size.y / 1080.f);

    for (int i = 0; i < static_cast<int>(MapTooltip::COUNT); ++i)
        Configuration::mapTooltips[static_cast<MapTooltip>(i)] = false;
    Configuration::mapTooltips[MapTooltip::PROVINCE] = true;
    Configuration::mapTooltips[MapTooltip::TITLES] = true;

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
    // Import settings from the specified file as an json object.
    std::ifstream file(Configuration::settingsFile, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open settings file '{}' for reading: {}", Configuration::settingsFile, std::strerror(errno));
        return;
    }

    nlohmann::json data;
    try {
        data = nlohmann::json::parse(file);
        file.close();
    }
    catch (std::exception& e) {
        LOG_ERROR("Failed to parse settings file '{}': {}", Configuration::settingsFile, e.what());
    }

    Configuration::recentMods = data.value("recent_mods", std::list<std::string>{});
    Configuration::compactTooltip = data.value("compact_tooltip", false);
    Configuration::adjacenciesConnections = data.value("adjacenciesConnections", true);
    Configuration::uiScale = data.value("ui_scale", Configuration::uiScale);

    if (data.contains("map_tooltips") && data["map_tooltips"].is_object()) {
        for (const auto& [key, value] : data["map_tooltips"].items()) {
            MapTooltip tooltip = MapTooltipFromString(key);
            // Skip invalid tooltip keys.
            if (tooltip == MapTooltip::COUNT)
                continue;
            Configuration::mapTooltips[tooltip] = value.get<bool>();
        }
    }
}

void Configuration::Save() {
    // Build a json object with all settings to save.
    nlohmann::json json;
    json["recent_mods"] = Configuration::recentMods;
    json["compact_tooltip"] = Configuration::compactTooltip;
    json["adjacenciesConnections"] = Configuration::adjacenciesConnections;
    json["ui_scale"] = Configuration::uiScale;

    for (int i = 0; i < static_cast<int>(MapTooltip::COUNT); ++i) {
        MapTooltip tooltip = static_cast<MapTooltip>(i);
        json["map_tooltips"][MapTooltipToString(tooltip)] = Configuration::mapTooltips[tooltip];
    }

    // Dump that json object into the settings file.
    std::ofstream file(Configuration::settingsFile, std::ios::binary);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open settings file '{}' for writing: {}", Configuration::settingsFile, std::strerror(errno));
        return;
    }

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