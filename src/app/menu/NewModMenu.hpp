#pragma once

#include "Menu.hpp"

// All template types will be using Atlantis to generate an error-free mod:
//  - https://github.com/bombusfrigidus/Atlantis
// The template type determine what provinces should be generated or kept.
enum class TemplateType {
    DEFAULT, // Keep the default provinces and titles of Atlantis.
    BLANK, // Remove all provinces and titles
    PROVINCES_IMAGE, // Take a provinces.png file and create provinces for each color (check sea level for provinces type).
    HEIGHTMAP_IMAGE // Generate a provinces image using Voronoi and simulate conquests between neighbours.
};
const std::map<TemplateType, std::pair<std::string, std::string>> TemplateTypeLabels = {
    { TemplateType::DEFAULT, {"Default", "Clone the default Atlantis template."} },
    { TemplateType::BLANK, {"Blank", "Create an empty map without any provinces or titles."} },
    { TemplateType::PROVINCES_IMAGE, {"Provinces Image", "Generate provinces from an image."} },
    { TemplateType::HEIGHTMAP_IMAGE, {"Heightmap Image", "Generate provinces from an heightmap."} }
};

enum class CreationState {
    CLONING, // Clone the Atlantis GitHub repository.
    UNZIPPING, // Unzipping Atlantis files.
    SETTING_UP, // Delete and edit some files from the Atlantis template.

    COPYING_IMAGES, // Depending on template type: if images are provided.
    GENERATING_WORLD, // Only for heightmap template type: use voronoi and conquests to generate provinces.
    GENERATING_PROVINCES, // Generate province objects for each colors in the image.
    GENERATE_TERRAIN, // Determine the terrain (land or sea) of provinces depending on heightmap.
    GENERATE_RIVERS, // Generate the rivers image using the landmass.

    FINISHED
};
const std::map<CreationState, std::string> CreationStateLabels = {
    { CreationState::CLONING, "Cloning Atlantis repository" },
    { CreationState::UNZIPPING, "Unzipping Atlantis files" },
    { CreationState::SETTING_UP, "Setting-up template files" },
    { CreationState::COPYING_IMAGES, "Copying images to project" },
    { CreationState::GENERATING_WORLD, "Generating world based on heightmap image" },
    { CreationState::GENERATING_PROVINCES, "Generating provinces from image" },
    { CreationState::GENERATE_TERRAIN, "Determining provinces type" },
    { CreationState::GENERATE_RIVERS, "Generating rivers image" },
    { CreationState::FINISHED, "Finished" },
};

class NewModMenu : public Menu {
public:
    NewModMenu(App& app);

    virtual void Update(sf::Time delta);
    virtual void Event(const sf::Event& event);
    virtual void Render();

    void UpdateHeightmapImage(const std::string& filePath);
    void UpdateProvincesImage(const std::string& filePath);

    void UpdateLandmassTextures();

    void CreateMod();
    void SetupAtlantisTitles();

private:
    UniquePtr<Mod> m_Mod;

    std::string m_ModName;
    std::string m_ModPath;
    TemplateType m_TemplateType;
    std::string m_ProvincesImagePath;
    std::string m_HeightmapImagePath;
    float m_WaterLevel;

    sf::Texture m_HeightmapTexture;
    sf::Texture m_HeightmapLandmassTexture;

    sf::Texture m_ProvincesTexture;
    sf::Texture m_ProvincesLandTexture;

    bool m_IsCreating;
    CreationState m_CreationState;
    UniquePtr<sf::Thread> m_CreationThread;
};