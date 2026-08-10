#include "ReligionManager.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"

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

sf::Image ReligionManager::GetFaithImage(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    // - Map provinces colors to their faith color (province -> county -> county capital -> province).
    // - Copy province image.
    // - Replace province pixels by their mapped color.
    // - Provinces with an explicit faith assigned will have alpha=0
    //   in order to inform the shader.
    sf::Color defaultColor = sf::Color(127, 127, 127);

    sf::Image image = Image::MapPixels(
        provinceManager.GetProvincesImage(),
        [&](auto& mappedColors){
            for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
                std::string faith = province->GetFaith();
                sf::Color color = defaultColor;
                uint8_t alpha = faith.empty() ? 255 : 0;

                if(faith.empty()) {
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

                        if(!baronyProvince->GetFaith().empty()) {
                            faith = baronyProvince->GetFaith();
                            break;
                        }
                    }
                }

                if (faith.empty()) {
                    color = sf::Color::Black;
                    goto End;
                }
                
                {
                    auto it = m_Faiths.find(faith);
                    if(it == m_Faiths.end()) {
                        color = sf::Color(faith[0], faith[1], faith[2]);
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

std::vector<sf::Color> ReligionManager::GetFaithPalette(ProvinceManager& provinceManager, TitleManager& titleManager) const {
    sf::Color defaultColor = sf::Color(127, 127, 127);

    const auto& indices = provinceManager.GetProvinceIndices();
    std::vector<sf::Color> palette(indices.size(), defaultColor);

    for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
        auto indexIt = indices.find(provinceColorId);
        if (indexIt == indices.end())
            continue;

        std::string faith = province->GetFaith();
        sf::Color color = defaultColor;
        uint8_t alpha = faith.empty() ? 255 : 0;

        if(faith.empty()) {
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

                if(!baronyProvince->GetFaith().empty()) {
                    faith = baronyProvince->GetFaith();
                    break;
                }
            }
        }

        if (faith.empty()) {
            color = sf::Color::Black;
            goto End;
        }

        {
            auto it = m_Faiths.find(faith);
            if(it == m_Faiths.end()) {
                color = sf::Color(faith[0], faith[1], faith[2]);
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

std::unordered_map<std::string, UniquePtr<Faith>>& ReligionManager::GetFaiths() {
    return m_Faiths;
}

const std::unordered_map<std::string, UniquePtr<Faith>>& ReligionManager::GetFaiths() const {
    return m_Faiths;
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

                // Merge all objects into a single one when there are duplicate definitions for the same faith.
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

                    // Merge all objects into a single one when there are duplicate definitions for the same faith.
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