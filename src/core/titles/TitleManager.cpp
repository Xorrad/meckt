#include "TitleManager.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "util/Yaml.hpp"

#include <fmt/ostream.h>

TitleManager::TitleManager(Mod& mod) :
    m_Mod(mod),
    m_LocCulturalNames({std::make_pair("english", std::map<std::string, std::string>{})}),
    m_TitlesLocalizationFileName(std::format("{}/{}", Paths::LOCALIZATION_ENGLISH, "00_titles_l_english.yml")), // Required for now since the localization directory can be 'replace/english' instead of 'english'
    m_CulturalNamesLocalizationFileName(std::format("{}/{}", Paths::LOCALIZATION_ENGLISH, "00_cultural_titles_l_english.yml"))
{
    for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++)
        m_TitlesByType[static_cast<TitleType>(i)] = std::vector<Title*>();
}

size_t TitleManager::CountTitles() const {
    return m_Titles.size();
}

size_t TitleManager::CountTitles(TitleType type) const {
    return m_TitlesByType.at(type).size();
}

bool TitleManager::HasTitle(const std::string& name) const {
    return m_Titles.contains(name);
}

bool TitleManager::HasLocCulturalName(const std::string& lang, const std::string& key) const {
    auto it1 = m_LocCulturalNames.find(lang);
    if (it1 == m_LocCulturalNames.end())
        return false;
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end())
        return false;
    return true;
}

//////////////////////////////////////////////////////

Mod& TitleManager::GetMod() {
    return m_Mod;
}

Title* TitleManager::GetTitle(const std::string& name) {
    auto it = m_Titles.find(name);
    if (it == m_Titles.end())
        return nullptr;
    return it->second.get();
}

const Title* TitleManager::GetTitle(const std::string& name) const {
    auto it = m_Titles.find(name);
    if (it == m_Titles.end())
        return nullptr;
    return it->second.get();
}

template <typename T> 
T* TitleManager::GetTitleAs(const std::string& name) {
    auto it = m_Titles.find(name);
    if (it == m_Titles.end())
        return nullptr;
    return dynamic_cast<T*>(it->second.get());
}
template BaronyTitle* TitleManager::GetTitleAs<BaronyTitle>(const std::string& name);
template CountyTitle* TitleManager::GetTitleAs<CountyTitle>(const std::string& name);
template DuchyTitle* TitleManager::GetTitleAs<DuchyTitle>(const std::string& name);
template KingdomTitle* TitleManager::GetTitleAs<KingdomTitle>(const std::string& name);
template EmpireTitle* TitleManager::GetTitleAs<EmpireTitle>(const std::string& name);
template HegemonyTitle* TitleManager::GetTitleAs<HegemonyTitle>(const std::string& name);
template HighTitle* TitleManager::GetTitleAs<HighTitle>(const std::string& name);

template <typename T>
const T* TitleManager::GetTitleAs(const std::string& name) const {
    auto it = m_Titles.find(name);
    if (it == m_Titles.end())
        return nullptr;
    return dynamic_cast<const T*>(it->second.get());
}
template const BaronyTitle* TitleManager::GetTitleAs<BaronyTitle>(const std::string& name) const;
template const CountyTitle* TitleManager::GetTitleAs<CountyTitle>(const std::string& name) const;
template const DuchyTitle* TitleManager::GetTitleAs<DuchyTitle>(const std::string& name) const;
template const KingdomTitle* TitleManager::GetTitleAs<KingdomTitle>(const std::string& name) const;
template const EmpireTitle* TitleManager::GetTitleAs<EmpireTitle>(const std::string& name) const;
template const HegemonyTitle* TitleManager::GetTitleAs<HegemonyTitle>(const std::string& name) const;
template const HighTitle* TitleManager::GetTitleAs<HighTitle>(const std::string& name) const;

BaronyTitle* TitleManager::GetBaronyByProvinceId(int provinceId) {
    auto it = m_BaroniesByProvinceId.find(provinceId);
    if (it == m_BaroniesByProvinceId.end())
        return nullptr;
    return it->second;
}

const BaronyTitle* TitleManager::GetBaronyByProvinceId(int provinceId) const {
    auto it = m_BaroniesByProvinceId.find(provinceId);
    if (it == m_BaroniesByProvinceId.end())
        return nullptr;
    return it->second;
}

sf::Image TitleManager::GetTitleImage(ProvinceManager& provinceManager, TitleType type) {
    sf::Image image = Image::MapPixels(
        provinceManager.GetProvincesImage(),
        [this, &provinceManager, type](auto& mappedColors){
            for(const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
                Title* liege = province->GetProvinceFocusedTitle(*this, type);

                if(liege == nullptr) {
                    mappedColors[province->GetColor().toInteger()] = 0x505050ff;
                    continue;
                }

                mappedColors[province->GetColor().toInteger()] = liege->GetColor().toInteger();
            }    
        }
    );
    return image;
}

std::vector<sf::Color> TitleManager::GetTierPalette(ProvinceManager& provinceManager, TitleType type) {
    const sf::Color noLiegeColor = sf::Color(0x50, 0x50, 0x50, 0xff);

    const auto& indices = provinceManager.GetProvinceIndices();
    std::vector<sf::Color> palette(indices.size(), noLiegeColor);

    for (const auto& [provinceColorId, province] : provinceManager.GetProvincesByColors()) {
        auto it = indices.find(provinceColorId);
        if (it == indices.end())
            continue;

        Title* liege = province->GetProvinceFocusedTitle(*this, type);
        palette[it->second] = (liege != nullptr) ? liege->GetColor() : noLiegeColor;
    }

    return palette;
}

std::map<std::string, UniquePtr<Title>>& TitleManager::GetTitles() {
    return m_Titles;
}

const std::map<std::string, UniquePtr<Title>>& TitleManager::GetTitles() const {
    return m_Titles;
}

std::map<TitleType, std::vector<Title*>>& TitleManager::GetTitlesByType() {
    return m_TitlesByType;
}

const std::map<TitleType, std::vector<Title*>>& TitleManager::GetTitlesByType() const {
    return m_TitlesByType;
}

std::map<int, BaronyTitle*>& TitleManager::GetBaroniesByProvinceId() {
    return m_BaroniesByProvinceId;
}

const std::map<int, BaronyTitle*>& TitleManager::GetBaroniesByProvinceId() const {
    return m_BaroniesByProvinceId;
}

const std::map<std::string, SharedPtr<Jomini::Object>>& TitleManager::GetTitlesVariables() const {
    return m_TitlesVariables;
}

const std::map<std::string, SharedPtr<Jomini::Object>>& TitleManager::GetTitlesHistoryVariables() const {
    return m_TitlesHistoryVariables;
}

std::map<std::string, std::map<std::string, std::string>>& TitleManager::GetLocCulturalNames() {
    return m_LocCulturalNames;
}

const std::map<std::string, std::map<std::string, std::string>>& TitleManager::GetLocCulturalNames() const {
    return m_LocCulturalNames;
}

std::map<std::string, std::string>& TitleManager::GetLocCulturalNames(const std::string& lang) {
    return m_LocCulturalNames[lang];
}

const std::map<std::string, std::string>& TitleManager::GetLocCulturalNames(const std::string& lang) const {
    return m_LocCulturalNames.at(lang);
}

std::string& TitleManager::GetLocCulturalName(const std::string& lang, const std::string& key) {
    return m_LocCulturalNames[lang][key];
}

std::string TitleManager::GetLocCulturalName(const std::string& lang, const std::string& key) const {
    auto it1 = m_LocCulturalNames.find(lang);
    if (it1 == m_LocCulturalNames.end())
        return "";
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end())
        return "";
    return it2->second;
}

std::string TitleManager::GetTitlesLocalizationFileName() const {
    return m_TitlesLocalizationFileName;
}

std::string TitleManager::GetCulturalNamesLocalizationFileName() const {
    return m_CulturalNamesLocalizationFileName;
}

const std::map<std::string, std::string>& TitleManager::GetVanillaOverrideTitleFiles() const {
    return m_VanillaOverrideTitleFiles;
}

//////////////////////////////////////////////////////

void TitleManager::AddTitle(UniquePtr<Title> title) {
    const std::string& name = title->GetName();
    auto it = m_Titles.find(name);

    // Remove any titles with the same name from the lists before adding it.
    if (it != m_Titles.end()) {
        m_TitlesByType[title->GetType()].erase(
            std::remove_if(m_TitlesByType[title->GetType()].begin(), m_TitlesByType[title->GetType()].end(), [name](Title* t) {
                return t->GetName() == name;
            }),
            m_TitlesByType[title->GetType()].end()
        );
    }

    // Add title to the specific type list.
    m_TitlesByType[title->GetType()].push_back(title.get());

    // Add barony title to map of baronies.
    if(title->Is(TitleType::BARONY)) {
        BaronyTitle* barony = static_cast<BaronyTitle*>(title.get());
        if (barony->GetProvinceId() != 0 && !m_BaroniesByProvinceId.contains(barony->GetProvinceId()))
            m_BaroniesByProvinceId[barony->GetProvinceId()] = barony;
    }

    // Add title to map of titles
    m_Titles[name] = std::move(title);
}

void TitleManager::RemoveTitle(const std::string& name) {
    auto it = m_Titles.find(name);
    if (it == m_Titles.end())
        return;
    const Title* title = it->second.get();

    // If the title is a county, remove it as the capital of any title.
    if (title->Is(TitleType::COUNTY)) {
        for (auto& [_, t] : m_Titles) {
            HighTitle* highTitle = dynamic_cast<HighTitle*>(t.get());
            if (highTitle != nullptr && highTitle->GetCapitalTitle() == title) {
                highTitle->SetCapitalTitle(nullptr);
            }
        }
    }

    // Remove any titles with the same name from the list.
    m_TitlesByType[title->GetType()].erase(
        std::remove_if(m_TitlesByType[title->GetType()].begin(), m_TitlesByType[title->GetType()].end(), [name](Title* t) {
            return t->GetName() == name;
        }),
        m_TitlesByType[title->GetType()].end()
    );

    // Remove barony from map of baronies.
    if(title->Is(TitleType::BARONY)) {
        const BaronyTitle* barony = static_cast<const BaronyTitle*>(title);
        if (barony->GetProvinceId() != 0 && m_BaroniesByProvinceId[barony->GetProvinceId()] == barony)
            m_BaroniesByProvinceId.erase(barony->GetProvinceId());
    }
    
    // Remove title from map of titles.
    m_Titles.erase(it);

    // TODO: search for files where the title was used in every files and log a warning.
}

void TitleManager::RemoveTitle(const Title* title) {
    if (title == nullptr)
        return;
    this->RemoveTitle(title->GetName());
}

void TitleManager::RenameTitle(const std::string& formerName, const std::string& newName) {
    auto it = m_Titles.find(formerName);
    if (it == m_Titles.end())
		return;
    if (m_Titles.contains(newName))
        throw std::invalid_argument(std::format("TitleManager::RenameTitle: couldn't rename title '{}' to '{}' because it is already used by another title", formerName, newName));
    
    Title* title = it->second.get();

    // 1. Rename the title.
    title->SetName(newName);

    // 2. Change the key in the titles map.
    auto nodeHandler = m_Titles.extract(it);
    nodeHandler.key() = newName;
    m_Titles.insert(std::move(nodeHandler));

    // 3. Rename every occurences of the title in a data object.
    const std::function<void(SharedPtr<Jomini::Object>)> RenameTitleHistoryRecursively = [&](SharedPtr<Jomini::Object> object) {
        if (object == nullptr)
            return;
        if (object->Is(Jomini::Type::SCALAR)) {
            std::string value = object->GetString();
            String::ReplaceAll(value, formerName, newName);
            object->Set(value);
        }
        else if (object->Is(Jomini::Type::ARRAY)) {
            for (SharedPtr<Jomini::Object> arrayObject : object->GetArrayUnsafe()) {
                RenameTitleHistoryRecursively(arrayObject);
            }
        }
        else if (object->Is(Jomini::Type::OBJECT)) {
            // Make a copy of the map to allow renaming keys.
            Jomini::ObjectMap map = object->GetMapUnsafe();

             for (auto& [key, pair] : map) {
                // 1. Check if the key needs renaming.
                if (key.find(formerName) != std::string::npos) {
                    std::string newKey = key;
                    String::ReplaceAll(newKey, formerName, newName);
                    object->GetMapUnsafe().erase(std::string_view(key));
                    object->GetMapUnsafe().insert(newKey, pair);
                }

                // 2. Recursively rename the child objects.
                RenameTitleHistoryRecursively(pair.second);
            }
        }
    };

    // 4. Apply it to every title original data and history.
    for (auto& [n, t] : m_Titles) {
        RenameTitleHistoryRecursively(t->GetOriginalData());
        for(auto& [date, history] : t->GetHistory()) {
            RenameTitleHistoryRecursively(history);
        }
        
        // for(auto [date, history] : t->GetHistory()) {
        //     if(history->Contains("liege") && history->Get("liege")->As<std::string>("") == formerName) {
        //         history->Put("liege", title->GetName());
        //     }
        //     if(history->Contains("de_jure_liege") && history->Get("de_jure_liege")->As<std::string>("") == formerName) {
        //         history->Put("de_jure_liege", title->GetName());
        //     }
        // }
    }

    // 4. 
    // TODO: replace using regex every occurence of 'title:{former_name}' in every files.
}

void TitleManager::ChangeBaronyProvinceId(BaronyTitle* barony, int newProvinceId) {
    if (barony == nullptr)
        return;

    // Remove the barony from the old province ID map
    m_BaroniesByProvinceId.erase(barony->GetProvinceId());

    // Update the barony's province ID
    barony->SetProvinceId(newProvinceId);

    // Add the barony to the new province ID map
    if (newProvinceId != 0)
        m_BaroniesByProvinceId[newProvinceId] = barony;
}

void TitleManager::AddLocCulturalName(const std::string& lang, const std::string& key, const std::string& name) {
    m_LocCulturalNames[lang][key] = name;
}

//////////////////////////////////////////////////////

void TitleManager::LoadTitles(const ProvinceManager& provinceManager) {
    // Remove any title and variable that might have been loaded beforehand.
    m_Titles.clear();
    m_TitlesByType.clear();
    m_BaroniesByProvinceId.clear();
    m_TitlesVariables.clear();
    m_TitlesHistoryVariables.clear();
    m_VanillaOverrideTitleFiles.clear();

    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::COMMON_LANDED_TITLES) );

    for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++)
        m_TitlesByType[static_cast<TitleType>(i)] = std::vector<Title*>();

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::COMMON_LANDED_TITLES, filePath);

            if (data->GetMap().empty()) {
                std::ifstream overrideFile = std::ifstream(filePath, std::ios::binary);
                m_VanillaOverrideTitleFiles[fileName] = File::ReadString(overrideFile);
                continue;
            }

            std::ignore = this->LoadTitlesFile(provinceManager, fileName, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("Failed to parse title definition file '{}': {}", filePath, e.what());
        }
    }

    this->LoadTitlesCapitals();

    LOG_INFO("Loaded {} titles from {} files", m_Titles.size(), filesPath.size());
    
    for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++)
        LOG_INFO("Loaded {} {} titles", m_TitlesByType[static_cast<TitleType>(i)].size(), TitleTypeLabels[i]);
}

std::vector<Title*> TitleManager::LoadTitlesFile(const ProvinceManager& provinceManager, const std::string& fileName, SharedPtr<Jomini::Object> data) {
    std::vector<Title*> titles;
    titles.reserve(5);

    for(auto& [key, pair] : data->GetMap()) {
        auto& [op, value] = pair;

        // Handle variables that might be in the file and store them for export.
        if (key.starts_with("@")) {
            if (!m_TitlesVariables.contains(fileName))
                m_TitlesVariables[fileName] = MakeShared<Jomini::Object>();
            if (!m_TitlesVariables[fileName]->Contains(key))
                m_TitlesVariables[fileName]->Put(key, value);
            continue;
        }

        // We need to check if the key is a title (starts with h_, e_, k_, d_, c_ or b_)
        // because it could be properties such as color, capital, can_create...
        if (!IsValidTitleName(key))
            continue;

        if (this->HasTitle(key)) {
            LOG_ERROR("Title '{}' has multiple definitions in file '{}'", key, fileName);
            continue;
        }

        // Ignore scalar values in the landed titles files.
        if (value->Is(Jomini::Type::SCALAR))
            continue;

        // Check if there wasn't a duplicate of the title definition in the same file.
        // And if so, keep only the first definition.
        if (!value->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Title '{}' has multiple definitions in file '{}'", key, fileName);
            value = data->GetFirst(key);
        }

        try {
            UniquePtr<Title> title = this->ParseTitle(provinceManager, fileName, key, value);
            titles.push_back(title.get());
            this->AddTitle(std::move(title));
        }
        catch(std::exception& e) {
            LOG_ERROR("Failed to parse title '{}' in file '{}': {}", key, fileName, e.what());
        }
    }

    return titles;
}

UniquePtr<Title> TitleManager::ParseTitle(const ProvinceManager& provinceManager, const std::string& fileName, const std::string& name, SharedPtr<Jomini::Object> data) {
    
    // TODO: make this function a proper one since it will be reused for provinces, cultures, religions...
    const auto GetProperty = [&]<typename T>(const std::string& propertyName, T defaultValue, bool required) -> T {
        if (!data->Contains(propertyName)) {
            if (required)
                LOG_ERROR("Title '{}' is missing {} property in definition", name, propertyName);
            return defaultValue;
        }

        // Check if the property is the right type of object.
        // For color, it has to be a 1-depth array.
        if constexpr (std::is_same_v<T, sf::Color>) {
            SharedPtr<Jomini::Object> propertyObject = data->Get(propertyName);
            data->Remove(propertyName);
            if (!propertyObject->Is(Jomini::Type::ARRAY)) {
                LOG_ERROR("Title '{}' has invalid {} property in definition", name, propertyName);
                return defaultValue;
            }
            if (propertyObject->GetArrayUnsafe().size() < 3 || !propertyObject->GetArrayUnsafe().front()->Is(Jomini::Type::SCALAR)) {
                LOG_WARNING("Title '{}' has duplicate {} property in definition", name, propertyName);

                // When there is a duplicate of the same property in the definition, then the property
                // should be an array of twice the same property.
                // We only need to keep the first one and ignore the second.
                propertyObject = propertyObject->GetArrayUnsafe().front();
                // if (!propertyObject->Is(Jomini::Type::ARRAY) || propertyObject->GetArrayUnsafe().size() < 3 || !propertyObject->GetArrayUnsafe().front()->Is(Jomini::Type::SCALAR)) {
                //     return defaultValue;
            }
            return propertyObject->As<sf::Color>(defaultValue);
        }
        // Do nothing if you need a map.
        else if constexpr (std::is_same_v<T, SharedPtr<Jomini::Object>>) {
            SharedPtr<Jomini::Object> propertyObject = data->GetFirst(propertyName);
            data->Remove(propertyName);
            if (!propertyObject->Is(Jomini::Type::OBJECT)) {
                LOG_ERROR("Title '{}' has invalid {} property in definition", name, propertyName);
                return defaultValue;
            }
            return propertyObject;
        }
        // For other properties, it has to be a scalar.
        else {
            SharedPtr<Jomini::Object> propertyObject = data->GetFirst(propertyName);
            data->Remove(propertyName);
            if (!propertyObject->Is(Jomini::Type::SCALAR)) {
                LOG_ERROR("Title '{}' has invalid {} property in definition", name, propertyName);
                return defaultValue;
            }
            return propertyObject->As<T>(defaultValue);
        }
    };
    
    // Define the title properties.
    TitleType type = GetTitleTypeByName(name);
    sf::Color color = GetProperty("color", sf::Color::Black, true);
    bool landless = GetProperty("landless", false, false);
    int provinceId = GetProperty("province", 0, (type == TitleType::BARONY));
    auto culturalNames = GetProperty("cultural_names", MakeShared<Jomini::Object>(Jomini::Type::OBJECT), false);

    // Create the title.
    UniquePtr<Title> title = MakeTitle(type, name, color, landless);

    // Add cultural names to the title.
    for(auto& [culture, pair] : culturalNames->GetMap()) {
        auto& [_, cultureObject] = pair;
        
        std::string culturalName = (
            cultureObject->Is(Jomini::Type::ARRAY)
            ? cultureObject->GetArrayUnsafe().front()->As<std::string>("")
            : cultureObject->As<std::string>("")
        );

        if(!culturalName.empty())
            title->AddCulturalName(culture, culturalName);
    }

    // Set the province id if the title is a barony.
    if(type == TitleType::BARONY) {
        BaronyTitle* barony = static_cast<BaronyTitle*>(title.get());
        barony->SetProvinceId(provinceId);

        if(!provinceManager.HasProvinceById(provinceId))
            LOG_ERROR("Title '{}' assigned province '{}' doesn't exist", name, provinceId);
        if(m_BaroniesByProvinceId.contains(provinceId)) {
            LOG_ERROR("Province '{}' has been assigned to multiple baronies: '{}' and '{}'; in file '{}'", provinceId, name, m_BaroniesByProvinceId[provinceId]->GetName(), fileName);
            // barony->SetProvinceId(0);
        }
    }
    // Otherwise, if the title is a "high title", parse its vassal titles.
    else {
        HighTitle* highTitle = static_cast<HighTitle*>(title.get());

        // Recursively parse the vassal titles.
        std::vector<Title*> dejureTitles = this->LoadTitlesFile(provinceManager, fileName, data);

        if(landless && !dejureTitles.empty())
            LOG_WARNING("Title '{}' has dejure titles even though it is landless", name);

        for(const auto& dejureTitle : dejureTitles) {
            highTitle->AddDejureTitle(dejureTitle);
            data->Remove(dejureTitle->GetName());
        }
    }

    title->SetOriginalFileName(fileName);
    title->SetOriginalData(data);

    return std::move(title);
}

void TitleManager::LoadTitlesCapitals() {
    // Because titles can have capitals that have not been loaded yet (defined in another
    // file for example), so we have to do a second pass after loading all titles to link
    // the remaining capitals.

    for (auto& [name, title] : m_Titles) {
        if (title->Is(TitleType::BARONY) || title->Is(TitleType::COUNTY))
            continue;

        HighTitle* highTitle = static_cast<HighTitle*>(title.get());
        if (highTitle == nullptr)
            continue;

        SharedPtr<Jomini::Object> value = highTitle->GetOriginalData();
        if (!value->Contains("capital")) {
            LOG_ERROR("Title '{}' has no capital title", name);
            continue;
        }

        std::string capitalName = value->GetFirst("capital")->As<std::string>();
        auto it = m_Titles.find(capitalName);
        if (it == m_Titles.end()) {
            LOG_ERROR("Title '{}' has unknown capital title '{}'", name, capitalName);
            continue;
        }
        
        CountyTitle* capital = dynamic_cast<CountyTitle*>(it->second.get());
        if (capital == nullptr) {
            LOG_ERROR("Title '{}' capital title '{}' is not a county", name, capitalName);
            continue;
        }

        highTitle->SetCapitalTitle(capital);
        value->Remove("capital");
    }
}

void TitleManager::LoadTitlesHistory() {
    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::HISTORY_TITLES) );

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::string fileName = m_Mod.GetRelativePath(Paths::HISTORY_TITLES, filePath);
            this->LoadTitlesHistoryFile(fileName, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("Failed to parse titles history file '{}': {}", filePath, e.what());
        }
    }
}

void TitleManager::LoadTitlesHistoryFile(const std::string& fileName, SharedPtr<Jomini::Object> data) {
    // Loop over titles.
    for(auto& [titleName, titlePair] : data->GetMap()) {
        auto& [_, titleValue] = titlePair;

        // Handle variables that might be in the file and store them for export.
        if (titleName.starts_with("@")) {
            if (!m_TitlesHistoryVariables.contains(fileName))
                m_TitlesHistoryVariables[fileName] = MakeShared<Jomini::Object>();
            if (!m_TitlesHistoryVariables[fileName]->Contains(titleName))
                m_TitlesHistoryVariables[fileName]->Put(titleName, titleValue);
            continue;
        }

        // Ignore undefined titles.
        if(m_Titles.count(titleName) == 0) {
            LOG_WARNING("Unknown title '{}' is defined in history file '{}'", titleName, fileName);
            continue;
        }

        // Merge all objects into a single one when there are duplicate definitions for the same title.
        if (titleValue->Is(Jomini::Type::ARRAY))
            titleValue = titleValue->Flatten(false);

        // Assert that the value is a correct object where a key is a date.
        if (!titleValue->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Title '{}' has invalid history entry in '{}'", titleName, fileName);
            continue;
        }

        m_Titles[titleName]->SetOriginalHistoryFileName(fileName);

        // Loop over dates.
        for(const auto& [dateString, datePair] : titleValue->GetMap()) {
            auto [__, history] = datePair;

            Jomini::Date date;
            try {
                date = Date::ParseDate(dateString);
            }
            catch (std::exception& e) {
                LOG_ERROR("Title '{}' has invalid date syntax '{}' in '{}'", titleName, dateString, fileName);
                continue;
            }

            // Merge all objects into a single one when there are duplicate definitions for the same date.
            if (history->Is(Jomini::Type::ARRAY))
                history = history->Flatten(true);

            // Make sure that the history entry is correct.
            if (!history->Is(Jomini::Type::OBJECT)) {
                LOG_ERROR("Title '{}' has invalid history entry for '{}' in '{}'", titleName, dateString, fileName);
                continue;
            }
            
            m_Titles[titleName]->AddHistory(date, history);
        }
    }
}

void TitleManager::LoadLocalization() {
    m_TitlesLocalizationFileName = std::format("{}/{}", Paths::LOCALIZATION_ENGLISH, "00_titles_l_english.yml");
    m_CulturalNamesLocalizationFileName = std::format("{}/{}", Paths::LOCALIZATION_ENGLISH, "00_cultural_titles_l_english.yml");

    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::LOCALIZATION_ENGLISH) );
    std::set<std::string> filesPath2 = File::ListFiles( m_Mod.GetDirectory(Paths::LOCALIZATION_REPLACE_ENGLISH) );
    filesPath.insert(filesPath2.begin(), filesPath2.end());

    size_t countNames = 0;
    size_t countAdjectives = 0;
    size_t countArticles = 0;
    size_t countCulturalNamesTotal = 0;

    size_t maxCount = 0;
    size_t maxCountCulturalNames = 0;

    if(filesPath.empty())
        LOG_WARNING("No localization files have been found in 'localization/english', nor in 'localization/replace/english'");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".yml"))
            continue;
        if(filePath.find("titles") == std::string::npos && filePath.find("cultural") == std::string::npos)
            continue;

        std::map<std::string, std::string> loc = Yaml::ParseFile(filePath);

        // Count the number of localization for this file.
        size_t count = countNames + countAdjectives;
        size_t countCulturalNames = 0;

        for(auto [key, value] : loc) {
            // TODO: handle cultural names.

            // Parse a cultural name.
            if(key.starts_with("cn_")) {
                this->AddLocCulturalName("english", key, value);
                countCulturalNames++;
                continue;
            }

            // Skip localization that are not related to titles.
            if(!key.starts_with("b_")
                && !key.starts_with("c_")
                && !key.starts_with("d_")
                && !key.starts_with("k_")
                && !key.starts_with("e_")
                && !key.starts_with("h_"))
                continue;
            
            std::string titleId = key;
            enum LocType { NAME, ADJECTIVE, ARTICLE };
            LocType locType = NAME;

            // Titles have names, adjectives, articles and cultural names.
            if(key.ends_with("_adj")) {
                titleId = key.substr(0, key.size()-4);
                locType = ADJECTIVE;
            }
            else if(key.ends_with("_article")) {
                titleId = key.substr(0, key.size()-8);
                locType = ARTICLE;
            }

            auto it = m_Titles.find(titleId);

            // If there are no title with that name, then we assume that it's a cultural name.
            if(it == m_Titles.end()) {
                this->AddLocCulturalName("english", key, value);
                countCulturalNames++;
                continue;
            }

            switch(locType) {
                case NAME:
                    it->second->SetLocName("english", value);
                    countNames++;
                    break;
                case ADJECTIVE:
                    it->second->SetLocAdjective("english", value);
                    countAdjectives++;
                    break;
                case ARTICLE:
                    it->second->SetLocArticle("english", value);
                    countArticles++;
                    break;
            }
        }

        count = countNames + countAdjectives + countArticles - count;
        countCulturalNamesTotal += countCulturalNames;

        // Use the most used localization file for titles
        // as the main file for export.
        if(count > maxCount) {
            maxCount = count;
            m_TitlesLocalizationFileName = m_Mod.GetRelativePath("", filePath);
        }
        
        // Same for cultural names.
        if(countCulturalNames > maxCountCulturalNames) {
            maxCountCulturalNames = countCulturalNames;
            m_CulturalNamesLocalizationFileName = m_Mod.GetRelativePath("", filePath);
        }
    }

    LOG_INFO("Default titles localization file will be {}", m_TitlesLocalizationFileName);
    LOG_INFO("Loaded {} titles names, {} adjectives and {} articles from {} localization files", countNames, countAdjectives, countArticles, filesPath.size());
    LOG_INFO("Default cultural names localization file will be {}", m_CulturalNamesLocalizationFileName);
    LOG_INFO("Loaded {} cultural names from {} localization files", countCulturalNamesTotal, filesPath.size());
}

void TitleManager::ExportTitles() {
    std::string dir = m_Mod.GetDirectory(Paths::COMMON_LANDED_TITLES);
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::map<std::string, std::ofstream> files;

    for(const auto& [name, title] : m_Titles) {
        if(title->GetLiegeTitle() != nullptr)
            continue;
        std::string fileName = title->GetOriginalFileName();
        if(fileName.empty())
            fileName = "01_landed_titles.txt";
        if(files.count(fileName) == 0) {
            std::string filePath = m_Mod.GetAbsolutePath(Paths::COMMON_LANDED_TITLES, fileName);
            files[fileName] = std::ofstream(filePath, std::ios::binary);
            if (!files[fileName].is_open()) {
                LOG_ERROR("Failed to open file '{}' for writing: {}", filePath, std::strerror(errno));
                continue;
            }
            File::EncodeToUTF8BOM(files[fileName]);

            // Export original script variables.
            auto it = m_TitlesVariables.find(fileName);
            if (it != m_TitlesVariables.end() && !it->second->GetMap().empty()) {
                fmt::println(files[fileName], "{}\n", it->second->Serialize(0, true));
            }
        }

        std::ofstream& file = files[fileName];
        this->ExportTitle(title.get(), file, 0);
    }

    for(auto& [key, file] : files)
        file.close();

    // Create empty files for the vanilla overrides.
    for (const auto& [fileName, fileContent] : m_VanillaOverrideTitleFiles) {
        std::ofstream file = std::ofstream(m_Mod.GetAbsolutePath(Paths::COMMON_LANDED_TITLES, fileName), std::ios::binary);
        // File::EncodeToUTF8BOM(file); // Encoding bytes are present alongside the file content.
        fmt::print(file, "{}", fileContent);
    }
}

void TitleManager::ExportTitle(Title* title, std::ofstream& file, int depth) {
    // Define the different indentations levels for the header and the content.
    std::string headerIndent = std::string(depth, '\t');
    std::string indent = headerIndent + '\t';

    // Write the title header to the file.
    fmt::println(file, "{}{} = {{", headerIndent, title->GetName());

    SharedPtr<Jomini::Object> data = (title->GetOriginalData() == nullptr) ? MakeShared<Jomini::Object>(Jomini::ObjectMap{}) : title->GetOriginalData()->Copy();

    const auto ExportProperties = [&]<typename T>(const std::string& key, T value) {
        fmt::println(file, "{}{} = {}", indent, key, value);
    };

    const auto ExportCulturalNames = [&]() {
        if(!title->GetCulturalNames().empty()) {
            fmt::println(file, "\n{}cultural_names = {{", indent);
            for(auto [culture, name] : title->GetCulturalNames()) {
                fmt::println(file, "{}\t{} = {}", indent, culture, name);
            }
            fmt::println(file, "{}}}", indent);
        }
    };

    ExportProperties("color", fmt::format("{{ {} {} {} }}", title->GetColor().r, title->GetColor().g, title->GetColor().b));
        
    if(title->Is(TitleType::BARONY)) {
        BaronyTitle* baronyTitle = static_cast<BaronyTitle*>(title);
        ExportProperties("province", baronyTitle->GetProvinceId());
        // TODO: warning if there is no province with this id.

        ExportCulturalNames();

        if(!data->GetMap().empty())
            fmt::println(file, "{}", data->Serialize(depth, true));
    }
    else {
        HighTitle* highTitle = static_cast<HighTitle*>(title);

        // TODO: Reimplement this using the refactored provinces manager.
        // Raise an error if the main barony of a county does not have any holding type.
        // if(title->Is(TitleType::COUNTY) && !highTitle->GetDejureTitles().empty()) {
        //     BaronyTitle* vassalTitle = dynamic_cast<BaronyTitle*>(highTitle->GetDejureTitles().front());
        //     if(m_ProvincesByIds.count(vassalTitle->GetProvinceId()) > 0) {
        //         Province* province = m_ProvincesByIds[vassalTitle->GetProvinceId()];
        //         if(province->GetHolding() == "none") {
        //             LOG_ERROR("Title '{}' is the first barony of '{}', but it does not have any holding", vassalTitle->GetName(), title->GetName());
        //         }
        //     }
        // }

        if(!title->Is(TitleType::COUNTY) && highTitle->GetCapitalTitle() != nullptr)
            ExportProperties("capital", highTitle->GetCapitalTitle()->GetName());
            
        if(title->IsLandless())
            ExportProperties("landless", "yes");

        ExportCulturalNames();

        if(!data->GetMap().empty())
            fmt::println(file, "\n{}", data->Serialize(depth+1, true));

        for(Title* dejureTitle : highTitle->GetDejureTitles()) {
            this->ExportTitle(dejureTitle, file, depth+1);
        }
    }

    // Write the title closing brace to the file.
    fmt::println(file, "{}}}\n", headerIndent);
}
    
void TitleManager::ExportTitlesHistory() {
    std::string dir = m_Mod.GetDirectory(Paths::HISTORY_TITLES);
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);

    std::map<std::string, std::ofstream> files;

    // 1. Use original history file if the title has one.
    // 2. Use hegemony_titles.txt for hegemony tier titles.
    // 3. Use empire_titles.txt for empire tier titles.
    // 4. Use kingdom tier liege for other titles (i.e k_the_wall).
    // 5. Use "landless_titles.txt" for landless titles.
    // 6. Use "special_titles.txt" for everything else.
    const std::function<std::string(Title*)> GetTitleFileName = [&](Title* liege) {
        if(liege->Is(TitleType::HEGEMONY))
            return std::string("hegemony_titles");
        if(liege->Is(TitleType::EMPIRE))
            return std::string("empire_titles");
        if(liege->Is(TitleType::KINGDOM))
            return liege->GetName();
        if(liege->IsLandless()) 
            return std::string("landless_titles");
        if(liege->GetLiegeTitle() == nullptr)
            return std::string("special_titles");
        return GetTitleFileName(liege->GetLiegeTitle());
    };

    for(const auto& [name, title] : m_Titles) {
        if(title->GetHistory().size() == 0)
            continue;
        std::string fileName = title->GetOriginalHistoryFileName();
        if(fileName.empty())
            fileName = GetTitleFileName(title.get()) + ".txt";
        if(files.count(fileName) == 0) {
            std::string filePath = m_Mod.GetAbsolutePath(Paths::HISTORY_TITLES, fileName);
            files[fileName] = std::ofstream(filePath, std::ios::binary);
            File::EncodeToUTF8BOM(files[fileName]);

            // Export original script variables.
            auto it = m_TitlesHistoryVariables.find(fileName);
            if (it != m_TitlesHistoryVariables.end() && !it->second->GetMap().empty()) {
                fmt::println(files[fileName], "{}\n", it->second->Serialize(0, true));
            }
        }
        std::ofstream& file = files[fileName];
        
        SharedPtr<Jomini::Object> history = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
        for(auto const& [date, data] : title->GetHistory()) {
            history->Put((std::string) date, data);
        }

        // Because of indentation and curly brackets, we need to put the history inside an object and print that object.
        SharedPtr<Jomini::Object> object = MakeShared<Jomini::Object>(Jomini::ObjectMap{});
        object->Put(title->GetName(), history);

        fmt::println(file, "{}", object->Serialize());
    }

    for(auto& [key, file] : files)
        file.close();
}

void TitleManager::ExportLocalization(bool exportTitlesLocalization, bool exportCulturalNamesLocalization) {
    this->DeleteLocalization(exportTitlesLocalization, exportCulturalNamesLocalization);

    if (exportTitlesLocalization)
        this->ExportTitlesLocalization();
    if (exportCulturalNamesLocalization)
        this->ExportCulturalNamesLocalization();
}

void TitleManager::ExportTitlesLocalization() {
    std::string filePath = m_Mod.GetAbsolutePath("", m_TitlesLocalizationFileName);
    std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
    std::ofstream file(filePath, std::ios::binary);
    File::EncodeToUTF8BOM(file);

    fmt::println(file, "l_english:");
    for(auto [type, titles] : m_TitlesByType) {
        for(auto title : titles) {
            std::string name = title->GetLocName("english");
            std::string adjective = title->GetLocAdjective("english");
            std::string article = title->GetLocArticle("english");

            if(!name.empty()) fmt::println(file, " {}: \"{}\"", title->GetName(), name);
            if(!adjective.empty()) fmt::println(file, " {}_adj: \"{}\"", title->GetName(), adjective);
            if(!article.empty()) fmt::println(file, " {}_article: \"{}\"", title->GetName(), article);
        }
    }

    file.close();
}

void TitleManager::ExportCulturalNamesLocalization() {
    std::string filePath = m_Mod.GetAbsolutePath("", m_CulturalNamesLocalizationFileName);
    std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());
    std::ofstream file(filePath, std::ios::binary);
    File::EncodeToUTF8BOM(file);

    fmt::println(file, "l_english:");
    for(auto [key, name] : m_LocCulturalNames["english"]) {
        if(!name.empty()) fmt::println(file, " {}: \"{}\"", key, name);
    }

    file.close();
}

void TitleManager::DeleteLocalization(bool exportTitlesLocalization, bool exportCulturalNamesLocalization) {
    std::set<std::string> filesPath = File::ListFiles( m_Mod.GetDirectory(Paths::LOCALIZATION_ENGLISH) );
    std::set<std::string> filesPath2 = File::ListFiles( m_Mod.GetDirectory(Paths::LOCALIZATION_REPLACE_ENGLISH) );
    filesPath.insert(filesPath2.begin(), filesPath2.end());

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".yml"))
            continue;
        if(filePath.find("titles") == std::string::npos && filePath.find("cultural") == std::string::npos)
            continue;
        if(filePath.ends_with(m_TitlesLocalizationFileName) || filePath.ends_with(m_CulturalNamesLocalizationFileName))
            continue;

        std::ifstream file(filePath);
        std::ofstream tmpFile(filePath + ".tmp");
        std::map<std::string, std::string> loc = Yaml::ParseFile(filePath);

        // Rewrite the localization line by line while
        // omitting titles and adjectives localization.
        std::string line;
        while(std::getline(file, line)) {
            // Remove all blanks from the string until reading ':'
            // in order to extract the key.
            std::string key = "";
            for(int i = 0; i < line.size(); i++) {
                if(line[i] == ' ' || line[i] == '\t' || line[i] == '\r')
                    continue;
                if(line[i] == ':' || line[i] == '#')
                    break;
                key.push_back(line[i]);
            }

            // If the line is empty or contains a comment, then we keep it.
            if(key.empty()) {
                fmt::println(tmpFile, "{}", line);
                continue;
            }

            // Ignore lines with a cultural name.
            if(key.starts_with("cn_") && exportCulturalNamesLocalization) {
                continue;
            }

            // If the line is not related to titles, then we keep it.
            if(!exportTitlesLocalization || (!key.starts_with("b_")
            && !key.starts_with("c_")
            && !key.starts_with("d_")
            && !key.starts_with("k_")
            && !key.starts_with("e_")
            && !key.starts_with("h_"))) {
                fmt::println(tmpFile, "{}", line);
                continue;
            }

            // Determine the title name/id from the key.
            std::string titleId = key;
            if(key.ends_with("_adj")) {
                titleId = key.substr(0, key.size()-4);
            }
            else if(key.ends_with("_article")) {
                titleId = key.substr(0, key.size()-8);
            }
            
            // If there isn't any titles by that name, then we assumes it's a cultural name.
            if(!m_Titles.contains(titleId)) {
                if (!exportCulturalNamesLocalization)
                    fmt::println(tmpFile, "{}", line);
                continue;
            }
        }

        file.close();
        tmpFile.close();
        std::remove(filePath.c_str());
        std::rename((filePath + ".tmp").c_str(), filePath.c_str());
    }
}

////////////////////////////////////////////////////

void TitleManager::GenerateMissingBaronies(ProvinceManager& provinceManager) {
    int count = 0;
    for(auto& [id, province] : provinceManager.GetProvincesByIds()) {
        if(!province->HasFlag(ProvinceFlags::LAND))
            continue;
        if(province->HasFlag(ProvinceFlags::IMPASSABLE))
            continue;
        if (m_BaroniesByProvinceId.contains(id))
            continue;
        
        // Make sure to use a title name that isn't already taken.
        std::string baronyName = "b_" + String::ToLowercase(province->GetName());
        int i = 1;
        while(m_Titles.count(baronyName) > 0) {
            baronyName = "b_" + String::ToLowercase(province->GetName()) + std::to_string(i);
            i++;
        }

        // Create a new barony title for that land province.
        UniquePtr<Title> title = MakeTitle(TitleType::BARONY, baronyName, province->GetColor(), false);
        BaronyTitle* baronyTitle = dynamic_cast<BaronyTitle*>(title.get());
        baronyTitle->SetProvinceId(province->GetId());

        // Add the barony title.
        this->AddTitle(std::move(title));
        count++;
    }
    LOG_INFO("Generated {} new barony titles for passable land provinces without any barony.", count);
}

void TitleManager::GenerateTitlesLocalization(const std::string& lang, bool names, bool adjectives, bool articles) {
    const auto FormatLocName = [&](const std::string& key) {
        std::string str = "";
        bool capitalize = true;
        for (size_t i = 2; i < key.size(); i++) {
            char ch = key[i];
            if (ch == '_') {
                str += ' ';
                capitalize = true;
            }
            else {
                str += (capitalize ? std::toupper(ch) : ch);
                capitalize = false;
            }
        }
        return str;
    };
    const auto FormatLocAdjective = [&](const std::string& key) {
        std::string str = FormatLocName(key);
        if (str.ends_with("ian"))
            return str;
        if (str.ends_with("i") || str.ends_with("y"))
            str.pop_back();
        if (str.ends_with("e"))
            return str + "an";
        if (str.ends_with("ea") || str.ends_with("ia"))
            return str + "n";
        if (str.ends_with("land"))
            return str + "er";
        return str + "ian";
    };

    size_t countNames = 0;
    size_t countAdjectives = 0;

    for (auto& [key, title] : m_Titles) {
        if (names && !title->HasLocName(lang)) {
            title->SetLocName(lang, FormatLocName(key));
            countNames++;
        }
        if (adjectives && !title->Is(TitleType::BARONY) && !title->HasLocAdjective(lang)) {
            title->SetLocAdjective(lang, FormatLocAdjective(key));
            countAdjectives++;
        }
    }

    LOG_INFO("Generated name localization for {} titles.", countNames);
    LOG_INFO("Generated adjective localization for {} titles.", countAdjectives);
}

void TitleManager::HarmonizeTitlesColors(std::span<Title*> titles, sf::Color rgb, float hue, float saturation) {
    // Generate a list of colors with uniformly spaced saturations around
    // the saturation of the original color while picking a random hue.
    sf::HSVColor defaultColor = rgb;
    std::vector<sf::HSVColor> colors;

    // Define the range of values for hue and saturation.
    float hues[] = { std::max(0.f, defaultColor.h - hue), std::min(360.f, defaultColor.h + hue) };
    float saturations[] = { std::max(0.f, defaultColor.s - saturation), std::min(1.f, defaultColor.s + saturation) };

    // Initialize first saturation value and the speed/step at which to increment it.
    float s = saturations[0];
    float saturationStep = (saturations[1] - saturations[0]) / (float) titles.size();

    while(colors.size() < titles.size()) {
        // TODO: check if the color isn't already used by another title when generating one.

        float h = Math::RandomFloat(hues[0], hues[1]);
        sf::HSVColor color = sf::HSVColor(h, s, defaultColor.v);
        colors.push_back(color);
        s += saturationStep;
    }

    // Shuffle the colors not to have a gradient but random
    // distribution which may make it easier to discern titles.
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(colors.begin(), colors.end(), g);

    // Apply those colors to the titles.
    for(int i = 0; i < titles.size(); i++) {
        titles[i]->SetColor(colors[i]);
    }
}