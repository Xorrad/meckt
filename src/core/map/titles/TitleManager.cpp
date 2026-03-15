#include "TitleManager.hpp"

#include "mod/Mod.hpp"

TitleManager::TitleManager(Mod& mod) : m_Mod(mod) {
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
        if (barony->GetProvinceId() != 0)
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
        if (barony->GetProvinceId() != 0)
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

//////////////////////////////////////////////////////

void TitleManager::LoadTitles() {
    // Remove any title and variable that might have been loaded beforehand.
    m_Titles.clear();
    m_TitlesByType.clear();
    m_BaroniesByProvinceId.clear();
    m_TitlesVariables.clear();
    m_TitlesHistoryVariables.clear();

    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDir() + "/common/landed_titles/");

    for(int i = 0; i < static_cast<int>(TitleType::COUNT); i++)
        m_TitlesByType[static_cast<TitleType>(i)] = std::vector<Title*>();

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            std::ignore = this->LoadTitlesFile(filePath, data);
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
    std::set<std::string> filesPath = File::ListFiles(m_Mod.GetDir() + + "/history/titles/");

    for(const auto& filePath : filesPath) {
        if(!filePath.ends_with(".txt"))
            continue;

        try {
            SharedPtr<Jomini::Object> data = Jomini::ParseFile(filePath);
            this->LoadTitlesHistoryFile(filePath, data);
        }
        catch (std::exception& e) {
            LOG_ERROR("Failed to parse titles history file '{}': {}", filePath, e.what());
        }
    }
}

void TitleManager::LoadTitlesHistoryFile(const std::string& filePath, SharedPtr<Jomini::Object> data) {
    // Loop over titles.
    for(auto& [titleName, titlePair] : data->GetMap()) {
        auto& [_, titleValue] = titlePair;

        // Handle variables that might be in the file and store them for export.
        if (titleName.starts_with("@")) {
            if (!m_TitlesHistoryVariables.contains(filePath))
                m_TitlesHistoryVariables[filePath] = MakeShared<Jomini::Object>();
            if (!m_TitlesHistoryVariables[filePath]->Contains(titleName))
                m_TitlesHistoryVariables[filePath]->Put(titleName, titleValue);
            continue;
        }

        // Ignore undefined titles.
        if(m_Titles.count(titleName) == 0) {
            LOG_WARNING("Unknown title '{}' is defined in history file '{}'", titleName, filePath);
            continue;
        }

        // Merge all objects into a single one when there are duplicate definitions for the same title.
        if (titleValue->Is(Jomini::Type::ARRAY))
            titleValue = titleValue->Flatten(false);

        // Assert that the value is a correct object where a key is a date.
        if (!titleValue->Is(Jomini::Type::OBJECT)) {
            LOG_ERROR("Title '{}' has invalid history entry in '{}'", titleName, filePath);
            continue;
        }

        m_Titles[titleName]->SetOriginalHistoryFilePath(filePath);

        // Loop over dates.
        for(const auto& [dateString, datePair] : titleValue->GetMap()) {
            auto [__, history] = datePair;

            Jomini::Date date;
            try {
                date = Date::ParseDate(dateString);
            }
            catch (std::exception& e) {
                LOG_ERROR("Title '{}' has invalid date syntax '{}' in '{}'", titleName, dateString, filePath);
                continue;
            }

            // Merge all objects into a single one when there are duplicate definitions for the same date.
            if (history->Is(Jomini::Type::ARRAY))
                history = history->Flatten(true);

            // Make sure that the history entry is correct.
            if (!history->Is(Jomini::Type::OBJECT)) {
                LOG_ERROR("Title '{}' has invalid history entry for '{}' in '{}'", titleName, dateString, filePath);
                continue;
            }
            
            m_Titles[titleName]->AddHistory(date, history);
        }
    }
}

std::vector<Title*> TitleManager::LoadTitlesFile(const std::string& filePath, SharedPtr<Jomini::Object> data) {
    std::vector<Title*> titles;
    titles.reserve(5);

    for(auto& [key, pair] : data->GetMap()) {
        auto& [op, value] = pair;

        // Handle variables that might be in the file and store them for export.
        if (key.starts_with("@")) {
            if (!m_TitlesVariables.contains(filePath))
                m_TitlesVariables[filePath] = MakeShared<Jomini::Object>();
            if (!m_TitlesVariables[filePath]->Contains(key))
                m_TitlesVariables[filePath]->Put(key, value);
            continue;
        }

        // We need to check if the key is a title (starts with h_, e_, k_, d_, c_ or b_)
        // because it could be properties such as color, capital, can_create...
        if (!IsValidTitleName(key))
            continue;

        try {
            titles.push_back(
                this->ParseTitle(filePath, key, value)
            );
        }
        catch(std::exception& e) {
            LOG_ERROR("Failed to parse title '{}' in file '{}': {}", key, filePath, e.what());
        }
    }

    return titles;
}

Title* TitleManager::ParseTitle(const std::string& filePath, const std::string& name, SharedPtr<Jomini::Object> data) {
    
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

        // TODO: reenable this alert using the refactored provinces manager.
        // if(m_ProvincesByIds.contains(provinceId))
        //     LOG_ERROR("Title '{}' has an unknown province id '{}'", name, provinceId);
        if(m_BaroniesByProvinceId.contains(provinceId)) {
            LOG_ERROR("Title '{}' assigned province '{}' is already assigned to barony '{}'", name, provinceId, m_BaroniesByProvinceId[provinceId]->GetName());
            barony->SetProvinceId(0);
        }
    }
    // Otherwise, if the title is a "high title", parse its vassal titles.
    else {
        HighTitle* highTitle = static_cast<HighTitle*>(title.get());

        // Recursively parse the vassal titles.
        std::vector<Title*> dejureTitles = this->LoadTitlesFile(filePath, data);

        if(landless && !dejureTitles.empty())
            LOG_WARNING("Title '{}' has dejure titles even though it is landless", name);

        for(const auto& dejureTitle : dejureTitles) {
            highTitle->AddDejureTitle(dejureTitle);
            data->Remove(dejureTitle->GetName());
        }
    }

    title->SetOriginalFilePath(filePath);
    title->SetOriginalData(data);

    Title* titlePtr = title.get();
    this->AddTitle(std::move(title));
    return titlePtr;
}