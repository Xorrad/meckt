#include "CultureManager.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"

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

sf::Image CultureManager::GetCultureImage(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    // - Map provinces colors to their culture color (province -> county -> county capital -> province).
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    // - Provinces with an explicit culture assigned will have alpha=0
    //   in order to inform the shader.
    sf::Color defaultColor = sf::Color(127, 127, 127);

    sf::Image image = Image::MapPixels(
        provinceManager.GetProvincesImage(),
        [&](auto& mappedColors){
            for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
                std::string culture = province->GetCulture();
                sf::Color color = defaultColor;
                uint8_t alpha = culture.empty() ? 255 : 0;

                if(culture.empty()) {
                    CountyTitle* liege = static_cast<CountyTitle*>(province->GetProvinceLiegeTitle(titleManager, TitleType::COUNTY));

                    if(liege == nullptr) {
                        goto End;
                    }

                    for(const auto& dejureTitle : liege->GetDejureTitles()) {
                        BaronyTitle* barony = static_cast<BaronyTitle*>(dejureTitle);
                        Province* baronyProvince = provinceManager.GetProvinceById(barony->GetProvinceId());

                        if (baronyProvince == nullptr) {
                            LOG_ERROR("Barony '{}' has unknown province id '{}'", barony->GetName(), barony->GetProvinceId());
                            continue;
                        }

                        if(!baronyProvince->GetCulture().empty()) {
                            culture = baronyProvince->GetCulture();
                            break;
                        }
                    }
                }

                if (culture.empty()) {
                    color = sf::Color::Black;
                    goto End;
                }

                {
                    auto it = m_Cultures.find(culture);
                    if (it == m_Cultures.end()) {
                        color = sf::Color(culture[0], culture[1], culture[2]);
                    }
                    else {
                        color = it->second->GetColor();
                    }
                }

                End:
                color.a = alpha;
                mappedColors[province->GetColor().toInteger()] = color.toInteger();
            }    
        }
    );
    return image;
}

std::vector<sf::Color> CultureManager::GetCulturePalette(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    sf::Color defaultColor = sf::Color(127, 127, 127);

    const auto& indices = provinceManager.GetProvinceIndices();
    std::vector<sf::Color> palette(indices.size(), defaultColor);

    for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
        auto indexIt = indices.find(provinceColorId);
        if (indexIt == indices.end())
            continue;

        std::string culture = province->GetCulture();
        sf::Color color = defaultColor;
        uint8_t alpha = culture.empty() ? 255 : 0;

        if(culture.empty()) {
            CountyTitle* liege = static_cast<CountyTitle*>(province->GetProvinceLiegeTitle(titleManager, TitleType::COUNTY));

            if(liege == nullptr) {
                goto End;
            }

            for(const auto& dejureTitle : liege->GetDejureTitles()) {
                BaronyTitle* barony = static_cast<BaronyTitle*>(dejureTitle);
                Province* baronyProvince = provinceManager.GetProvinceById(barony->GetProvinceId());

                if (baronyProvince == nullptr) {
                    LOG_ERROR("Barony '{}' has unknown province id '{}'", barony->GetName(), barony->GetProvinceId());
                    continue;
                }

                if(!baronyProvince->GetCulture().empty()) {
                    culture = baronyProvince->GetCulture();
                    break;
                }
            }
        }

        if (culture.empty()) {
            color = sf::Color::Black;
            goto End;
        }

        {
            auto it = m_Cultures.find(culture);
            if (it == m_Cultures.end()) {
                color = sf::Color(culture[0], culture[1], culture[2]);
            }
            else {
                color = it->second->GetColor();
            }
        }

        End:
        color.a = alpha;
        palette[indexIt->second] = color;
    }

    return palette;
}

std::unordered_map<std::string, UniquePtr<Culture>>& CultureManager::GetCultures() {
    return m_Cultures;
}

const std::unordered_map<std::string, UniquePtr<Culture>>& CultureManager::GetCultures() const {
    return m_Cultures;
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