#include "Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "regions/RegionManager.hpp"
#include "cultures/CultureManager.hpp"
#include "religions/ReligionManager.hpp"
#include "defines/DefineManager.hpp"

#include <filesystem>
#include <fmt/ostream.h>

Mod::Mod(const std::string& rootDirectory) :
    Mod(rootDirectory, sf::Image(), sf::Image())
{}

Mod::Mod(const std::string& rootDirectory, sf::Image heightmapImage, sf::Image provincesImage) :
    m_RootDirectory(rootDirectory)
{

}

Mod::~Mod() = default;

//////////////////////////////////////////////////////

bool Mod::HasMap() const {
    return std::filesystem::exists( this->GetAbsolutePath(Paths::MAP_DATA_PROVINCES) );
}

//////////////////////////////////////////////////////

const std::string& Mod::GetRootDirectory() const {
    return m_RootDirectory;
}

std::string Mod::GetDirectory(GamePath directory) const {
    std::string absoluteDirectoryPath = this->GetRootDirectory();
    
    // Make sure the directories are separated correctly.
    if (!absoluteDirectoryPath.ends_with("/"))
        absoluteDirectoryPath.append("/");

    absoluteDirectoryPath.append(std::string(directory));

    return std::move(absoluteDirectoryPath);
}

std::string Mod::GetAbsolutePath(GamePath directory, const std::string& relativeFileName) const {
    std::string absoluteFilePath = this->GetDirectory(directory);
    
    // Make sure the directories are separated correctly.
    // if (!absoluteFilePath.ends_with("/"))
    //     absoluteFilePath.append("/");

    absoluteFilePath.append(relativeFileName);
    
    return std::move(absoluteFilePath);
}

std::string Mod::GetRelativePath(GamePath directory, const std::string& absolutePath) const {
    std::string prefix = this->GetDirectory(directory);
    
    if (absolutePath.starts_with(prefix)) {
        return absolutePath.substr(prefix.length());
    }

    return std::move(absolutePath);
}

//////////////////////////////////////////////////////

#define DEFINE_MANAGER_GETTER(ManagerName) \
    ManagerName##Manager& Mod::Get##ManagerName##Manager() { return *m_##ManagerName##Manager; } \
    const ManagerName##Manager& Mod::Get##ManagerName##Manager() const { return *m_##ManagerName##Manager; }

DEFINE_MANAGER_GETTER(Title);
DEFINE_MANAGER_GETTER(Province);
DEFINE_MANAGER_GETTER(Region);
DEFINE_MANAGER_GETTER(Culture);
DEFINE_MANAGER_GETTER(Religion);
DEFINE_MANAGER_GETTER(Define);

////////////////////////////////////////////

void Mod::SetRootDirectory(const std::string& rootDirectory) {
    m_RootDirectory = rootDirectory;
}

////////////////////////////////////////////

void Mod::Load(
    std::function<void()> completeCallback,
    std::function<void(LoadingState)> changeCallback,
    std::function<void(const std::string&)> errorCallback
) {
    if(!this->HasMap()) {
        errorCallback(fmt::format("File {} is missing and required to load the mod.", m_RootDirectory + "/map_data/provinces.png"));
        return;
    }

    m_TitleManager = MakeUnique<TitleManager>(*this);
    m_ProvinceManager = MakeUnique<ProvinceManager>(*this);
    m_RegionManager = MakeUnique<RegionManager>(*this);
    m_CultureManager = MakeUnique<CultureManager>(*this);
    m_ReligionManager = MakeUnique<ReligionManager>(*this);
    m_DefineManager = MakeUnique<DefineManager>(*this);

    #define LOAD_CATCH(method, state, name, required) \
        try { \
            changeCallback(state); \
            method; \
        } \
        catch (std::exception& e) { \
            std::string msg = fmt::format("Failed to load {}\n{}", name, e.what()); \
            LOG_ERROR("{}", msg); \
            errorCallback(msg); \
            if (required) return; \
        }

    LOAD_CATCH(m_DefineManager->LoadDefines(), LoadingState::DEFINES, "defines", false);
    LOAD_CATCH(m_ProvinceManager->LoadHoldingTypes(), LoadingState::HOLDING_TYPES, "holding types", true);
    LOAD_CATCH(m_ProvinceManager->LoadTerrainTypes(), LoadingState::TERRAIN_TYPES, "terrain types", true);
    LOAD_CATCH(m_ProvinceManager->LoadProvincesDefinition(), LoadingState::PROVINCES_DEFINITION, "provinces definition", true);
    LOAD_CATCH(m_ProvinceManager->LoadProvincesImage(), LoadingState::PROVINCES_IMAGE, "provinces image", true);
    LOAD_CATCH(m_ProvinceManager->LoadHeightmapImage(), LoadingState::PROVINCES_IMAGE, "heightmap image", false);
    LOAD_CATCH(m_ProvinceManager->LoadRiversImage(), LoadingState::PROVINCES_IMAGE, "rivers image", false);
    LOAD_CATCH(m_ProvinceManager->LoadDefaultMapFile(), LoadingState::DEFAULT_MAP, "default map", true);
    LOAD_CATCH(m_ProvinceManager->LoadProvincesTerrain(), LoadingState::PROVINCES_TERRAIN, "provinces terrain", true);
    LOAD_CATCH(m_ProvinceManager->LoadProvincesClimate(), LoadingState::PROVINCES_CLIMATE, "provinces climate", true);
    LOAD_CATCH(m_ProvinceManager->LoadProvincesHistory(), LoadingState::PROVINCES_HISTORY, "provinces history", true);
    LOAD_CATCH(m_TitleManager->LoadTitles(*m_ProvinceManager), LoadingState::TITLES, "titles", true);
    LOAD_CATCH(m_TitleManager->LoadTitlesHistory(), LoadingState::TITLES_HISTORY, "titles history", true);
    LOAD_CATCH(m_TitleManager->LoadLocalization(), LoadingState::TITLES_LOCALIZATION, "titles localization", true);
    LOAD_CATCH(m_RegionManager->LoadGeographicalRegions(*m_ProvinceManager, *m_TitleManager), LoadingState::GEOGRAPHICAL_REGIONS, "geographical regions", true);
    LOAD_CATCH(m_CultureManager->LoadCultures(), LoadingState::CULTURES, "cultures", true);
    LOAD_CATCH(m_ReligionManager->LoadFaiths(), LoadingState::FAITHS, "religions", true);

    completeCallback();
}

void Mod::Export(
    bool defaultMap,
    bool provincesDefinition,
    bool provincesTerrain,
    bool provincesClimate,
    bool provincesHistory,
    bool titles,
    bool titlesHistory,
    bool titlesLocalization,
    bool culturalNamesLocalization,
    bool geographicalRegions
) {

    if (defaultMap)
        m_ProvinceManager->ExportDefaultMapFile();
    if (provincesDefinition)
        m_ProvinceManager->ExportProvincesDefinition();
    if (provincesTerrain)
        m_ProvinceManager->ExportProvincesTerrain();
    if (provincesClimate)
        m_ProvinceManager->ExportProvincesClimate();
    if (provincesHistory)
        m_ProvinceManager->ExportProvincesHistory(*m_TitleManager);

    if (titles)
        m_TitleManager->ExportTitles();
    if (titlesHistory)
        m_TitleManager->ExportTitlesHistory();
    if (titlesLocalization || culturalNamesLocalization)
        m_TitleManager->ExportLocalization(titlesLocalization, culturalNamesLocalization);

    if (geographicalRegions)
        m_RegionManager->ExportGeographicalRegions();
}