#include "DefineManager.hpp"

#include "mod/Mod.hpp"

#include <fmt/ostream.h>

DefineManager::DefineManager(Mod& mod) :
    m_Mod(mod),
    m_WaterLevel(3.8f)
{
}

Mod& DefineManager::GetMod() {
    return m_Mod;
}

float DefineManager::GetWaterLevel() const {
    return m_WaterLevel;
}

//////////////////////////////////////////////////////

void DefineManager::LoadDefines() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_DEFINES), false);

    for(const auto& filePath : filesPath) {
        if (!std::filesystem::is_regular_file(filePath))
            continue;
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            if (data->Contains("NJominiMap")) {
                SharedPtr<Jomini::Object> mapData = data->Get("NJominiMap")->Flatten(true);

                if (mapData->Contains("WATERLEVEL")) {
                    try {
                        m_WaterLevel = static_cast<float>(mapData->Get("WATERLEVEL")->As<double>());
                        LOG_INFO("Loaded custom water level '{}' from '{}'", m_WaterLevel, filePath);
                    }
                    catch (const std::exception& e) {
                        LOG_ERROR("Failed to parse WATERLEVEL '{}' in file '{}' : {}", mapData->Get("WATERLEVEL")->Serialize(0, false, true), filePath, e.what());
                    }
                }
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse file {} : {}", filePath, e.what());
        }
    }
}