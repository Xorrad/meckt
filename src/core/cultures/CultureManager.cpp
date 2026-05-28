#include "CultureManager.hpp"

#include "mod/Mod.hpp"

CultureManager::CultureManager(Mod& mod) :
    m_Mod(mod)
{
}

size_t CultureManager::CountCultures() const {
    return m_Cultures.size();
}

bool CultureManager::HasCulture(const std::string& name) const {
    return m_Cultures.contains(name);
}

//////////////////////////////////////////////////////

Mod& CultureManager::GetMod() {
    return m_Mod;
}

Culture* CultureManager::GetCulture(const std::string& name) {
    auto it = m_Cultures.find(name);
    if (it == m_Cultures.end())
        return nullptr;
    return it->second.get();
}
    
const Culture* CultureManager::GetCulture(const std::string& name) const {
    auto it = m_Cultures.find(name);
    if (it == m_Cultures.end())
        return nullptr;
    return it->second.get();
}

//////////////////////////////////////////////////////

void CultureManager::AddCulture(UniquePtr<Culture> culture) {
    m_Cultures[culture->GetName()] = std::move(culture);
}

void CultureManager::RemoveCulture(const std::string& name) {
    m_Cultures.erase(name);
}

//////////////////////////////////////////////////////

void CultureManager::LoadCultures() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_CULTURES));

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
                    LOG_ERROR("Culture '{}' has invalid definition in '{}'", key, filePath);
                    continue;
                }

                sf::Color color = value->Get("color")->As<sf::Color>(sf::Color::White);
                this->AddCulture(MakeUnique<Culture>(key, color));
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse culture definition file '{}': {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} cultures from {} files", m_Cultures.size(), filesPath.size());
}