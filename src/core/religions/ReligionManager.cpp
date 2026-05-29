#include "ReligionManager.hpp"

#include "mod/Mod.hpp"

ReligionManager::ReligionManager(Mod& mod) :
    m_Mod(mod)
{
}

size_t ReligionManager::CountFaiths() const {
    return m_Faiths.size();
}

bool ReligionManager::HasFaith(const std::string& name) const {
    return m_Faiths.contains(name);
}

//////////////////////////////////////////////////////

Mod& ReligionManager::GetMod() {
    return m_Mod;
}

Faith* ReligionManager::GetFaith(const std::string& name) {
    auto it = m_Faiths.find(name);
    if (it == m_Faiths.end())
        return nullptr;
    return it->second.get();
}
    
const Faith* ReligionManager::GetFaith(const std::string& name) const {
    auto it = m_Faiths.find(name);
    if (it == m_Faiths.end())
        return nullptr;
    return it->second.get();
}

//////////////////////////////////////////////////////

void ReligionManager::AddFaith(UniquePtr<Faith> faith) {
    m_Faiths[faith->GetName()] = std::move(faith);
}

void ReligionManager::RemoveFaith(const std::string& name) {
    m_Faiths.erase(name);
}

//////////////////////////////////////////////////////

void ReligionManager::LoadFaiths() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_RELIGIONS_TYPES));

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                auto& [op, value] = pair;

                // Merge all objects into a single one when there are duplicate definitions for the same culture.
                if (value->Is(Jomini::Type::ARRAY))
                    value = value->Flatten(false);

                // Assert that the value is a correct object.
                if (!value->Is(Jomini::Type::OBJECT)) {
                    LOG_ERROR("Religion '{}' has invalid definition in '{}'", key, filePath);
                    continue;
                }

                if(!value->Contains("faiths"))
                    continue;

                for(auto& [faithKey, faithPair] : value->Get("faiths")->GetMap()) {
                    auto& [op2, faithValue] = faithPair;

                    // Merge all objects into a single one when there are duplicate definitions for the same culture.
                    if (faithValue->Is(Jomini::Type::ARRAY))
                        faithValue = faithValue->Flatten(false);

                    // Assert that the value is a correct object.
                    if (!faithValue->Is(Jomini::Type::OBJECT)) {
                        LOG_ERROR("Faith '{}' has invalid definition in '{}'", faithKey, filePath);
                        continue;
                    }

                    sf::Color color = faithValue->Get("color")->As<sf::Color>(sf::Color::White);
                    this->AddFaith(MakeUnique<Faith>(faithKey, color));
                }
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse faith definition file '{}': {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} faiths from {} files", m_Faiths.size(), filesPath.size());
}