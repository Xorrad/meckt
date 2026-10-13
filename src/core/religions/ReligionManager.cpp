#include "ReligionManager.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"

ReligionManager::ReligionManager(Mod& mod) :
    m_Mod(mod)
{
}

size_t ReligionManager::CountRites() const {
    return m_Rites.size();
}

bool ReligionManager::HasRite(const std::string& name) const {
    return m_Rites.contains(name);
}

//////////////////////////////////////////////////////

Mod& ReligionManager::GetMod() {
    return m_Mod;
}

Rite* ReligionManager::GetRite(const std::string& name) {
    auto it = m_Rites.find(name);
    if (it == m_Rites.end())
        return nullptr;
    return it->second.get();
}
    
const Rite* ReligionManager::GetRite(const std::string& name) const {
    auto it = m_Rites.find(name);
    if (it == m_Rites.end())
        return nullptr;
    return it->second.get();
}

sf::Image ReligionManager::GetRiteImage(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    // - Map provinces colors to their rite color (province -> county -> county capital -> province).
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    // - Provinces with an explicit rite assigned will have alpha=0
    //   in order to inform the shader.
    sf::Color defaultColor = sf::Color(127, 127, 127);

    sf::Image image = Image::MapPixels(
        provinceManager.GetProvincesImage(),
        [&](auto& mappedColors){
            for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
                std::string rite = province->GetRite();
                sf::Color color = defaultColor;
                uint8_t alpha = rite.empty() ? 255 : 0;

                if(rite.empty()) {
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

                        if(!baronyProvince->GetRite().empty()) {
                            rite = baronyProvince->GetRite();
                            break;
                        }
                    }
                }

                if (rite.empty()) {
                    color = sf::Color::Black;
                    goto End;
                }
                
                {
                    auto it = m_Rites.find(rite);
                    if(it == m_Rites.end()) {
                        color = sf::Color(rite[0], rite[1], rite[2]);
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

std::vector<sf::Color> ReligionManager::GetRitePalette(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    sf::Color defaultColor = sf::Color(127, 127, 127);

    const auto& indices = provinceManager.GetProvinceIndices();
    std::vector<sf::Color> palette(indices.size(), defaultColor);

    for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
        auto indexIt = indices.find(provinceColorId);
        if (indexIt == indices.end())
            continue;

        std::string rite = province->GetRite();
        sf::Color color = defaultColor;
        uint8_t alpha = rite.empty() ? 255 : 0;

        if(rite.empty()) {
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

                if(!baronyProvince->GetRite().empty()) {
                    rite = baronyProvince->GetRite();
                    break;
                }
            }
        }

        if (rite.empty()) {
            color = sf::Color::Black;
            goto End;
        }

        {
            auto it = m_Rites.find(rite);
            if(it == m_Rites.end()) {
                color = sf::Color(rite[0], rite[1], rite[2]);
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

std::unordered_map<std::string, UniquePtr<Rite>>& ReligionManager::GetRites() {
    return m_Rites;
}

const std::unordered_map<std::string, UniquePtr<Rite>>& ReligionManager::GetRites() const {
    return m_Rites;
}

//////////////////////////////////////////////////////

void ReligionManager::AddRite(UniquePtr<Rite> rite) {
    m_Rites[rite->GetName()] = std::move(rite);
}

void ReligionManager::RemoveRite(const std::string& name) {
    m_Rites.erase(name);
}

//////////////////////////////////////////////////////

void ReligionManager::LoadRites() {
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDirectory(Paths::COMMON_RITE_TYPES));

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;
        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);

            for(auto& [key, pair] : data->GetMap()) {
                auto& [op, value] = pair;

                // Merge all objects into a single one when there are duplicate definitions for the same rite.
                if (value->Is(Jomini::Type::ARRAY))
                    value = value->Flatten(false);

                // Assert that the value is a correct object.
                if (!value->Is(Jomini::Type::OBJECT)) {
                    LOG_ERROR("Rite '{}' has invalid definition in '{}'", key, filePath);
                    continue;
                }

                sf::Color color = value->Get("color")->As<sf::Color>(sf::Color::White);
                this->AddRite(MakeUnique<Rite>(key, color));
            }
        }
        catch(const std::runtime_error& e) {
            LOG_ERROR("Failed to parse rite definition file '{}': {}", filePath, e.what());
        }
    }

    LOG_INFO("Loaded {} rites from {} files", m_Rites.size(), filesPath.size());
}