#include "SelectionHandler.hpp"

#include "app/App.hpp"
#include "app/menu/EditorMenu.hpp"

#include "core/mod/Mod.hpp"
#include "core/provinces/ProvinceManager.hpp"
#include "core/regions/Region.hpp"
#include "core/titles/Title.hpp"

SelectionHandler::SelectionHandler(EditorMenu& menu) :
    m_Menu(menu),
    m_Provinces({}),
    m_ProvincesLookup({}),
    m_Titles({}),
    m_Regions({}),
    m_Adjacency(nullptr),
    m_ProvinceCallbacks({}),
    m_TitleCallbacks({}),
    m_Colors({}),
    m_Count(0),
    m_SelectionText(Configuration::fonts.Get(Fonts::NOTO_SANS)),
    m_SelectionType(SelectionType::NONE)
{
    m_SelectionText.setCharacterSize(24 * Configuration::uiScale);
    m_SelectionText.setString("");
    m_SelectionText.setFillColor(sf::Color::Red);
    m_SelectionText.setFont(Configuration::fonts.Get(Fonts::NOTO_SANS));
    m_SelectionText.setPosition({10, 20});
    m_SelectionText.setScale({ Configuration::uiScale, Configuration::uiScale });
}

void SelectionHandler::Select(Province* province, bool update) {
    if (m_ProvincesLookup.find(province) != m_ProvincesLookup.end())
        return;
    m_Provinces.push_back(province);
    m_ProvincesLookup.emplace(province, true);
    if (update)
        this->Update();
}

void SelectionHandler::Select(Title* title) {
    if(this->IsSelected(title))
        return;
    m_Titles.push_back(title);
    this->Update();
}

void SelectionHandler::Select(Region* region) {
    if(this->IsSelected(region))
        return;
    m_Regions.push_back(region);
    this->Update();
}

void SelectionHandler::Select(Adjacency* adjacency) {
    m_Adjacency = adjacency;
    this->Update();
}

void SelectionHandler::Deselect(Province* province, bool update) {
	auto it = m_ProvincesLookup.find(province);
	if (it == m_ProvincesLookup.end())
		return;
    m_Provinces.erase(
        std::remove(m_Provinces.begin(), m_Provinces.end(), province),
		m_Provinces.end()
    );
    m_ProvincesLookup.erase(it);
    if (update)
        this->Update();
}

void SelectionHandler::Deselect(Title* title) {
    m_Titles.erase(
        std::remove(m_Titles.begin(), m_Titles.end(), title),
        m_Titles.end()
    );
    this->Update();
}

void SelectionHandler::Deselect(Region* region) {
    m_Regions.erase(
        std::remove(m_Regions.begin(), m_Regions.end(), region),
        m_Regions.end()
    );
    this->Update();
}

void SelectionHandler::Deselect(Adjacency* adjacency) {
    if (m_Adjacency == adjacency) {
        m_Adjacency = nullptr;
        this->Update();
    }
        
}

void SelectionHandler::ClearSelection() {
    m_Provinces.clear();
    m_ProvincesLookup.clear();
    m_Titles.clear();
    m_Regions.clear();
    m_Colors.clear();
    m_Adjacency = nullptr;

    this->Update();
}

bool SelectionHandler::IsSelected(Province* province) const {
    return m_ProvincesLookup.find(province) != m_ProvincesLookup.end();
}

bool SelectionHandler::IsSelected(const Title* title) const {
    return std::find(m_Titles.begin(), m_Titles.end(), title) != m_Titles.end();
}

bool SelectionHandler::IsSelected(const Region* region) const {
    return std::find(m_Regions.begin(), m_Regions.end(), region) != m_Regions.end();
}

bool SelectionHandler::IsSelected(const Adjacency* adjacency) const {
    return m_Adjacency == adjacency;
}

std::span<Province*> SelectionHandler::GetProvinces() {
    return m_Provinces;
}

std::span<Title*> SelectionHandler::GetTitles() {
	return m_Titles;
}

std::span<Region*> SelectionHandler::GetRegions() {
    return m_Regions;
}

Adjacency* SelectionHandler::GetAdjacency() {
    return m_Adjacency;
}

std::vector<sf::Glsl::Vec4>& SelectionHandler::GetColors() {
    return m_Colors;
}

std::size_t SelectionHandler::GetCount() const {
    return m_Count;
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)> callback) {
    m_PositionCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback) {
    m_ProvinceCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback) {
    m_TitleCallbacks.push_back(callback);
}

void SelectionHandler::RemoveLastProvinceCallback() {
    if (m_ProvinceCallbacks.empty())
        return;
    m_ProvinceCallbacks.pop_back();
}

void SelectionHandler::RemoveLastPositionCallback() {
    if (m_PositionCallbacks.empty())
        return;
    m_PositionCallbacks.pop_back();
}

void SelectionHandler::OnClick(sf::Mouse::Button button, sf::Vector2f position) {
    bool updateMap = false;

    for (auto it = m_PositionCallbacks.end(); it != m_PositionCallbacks.begin(); ) {
        --it;

        SelectionCallbackResult res = (*it)(button, position);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK)) {
            it = m_PositionCallbacks.erase(it);
            if (shouldInterrupt)
                break;
            continue;
        }

        if (shouldInterrupt)
            break;
    }

    if (updateMap)
        m_Menu.RefreshCurrentMapMode(false);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, Province* province) {
    bool updateMap = false;

    for (auto it = m_ProvinceCallbacks.end(); it != m_ProvinceCallbacks.begin(); ) {
        --it;

        SelectionCallbackResult res = (*it)(button, province);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK)) {
            it = m_ProvinceCallbacks.erase(it);
            if (shouldInterrupt)
                break;
            continue;
        }

        if (shouldInterrupt)
            break;
    }

    if (updateMap)
        m_Menu.RefreshCurrentMapMode(false);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, Province* province, Title* title) {
    bool updateMap = false;

    for (auto it = m_TitleCallbacks.end(); it != m_TitleCallbacks.begin(); ) {
        --it;

        SelectionCallbackResult res = (*it)(button, province, title);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK)) {
            it = m_TitleCallbacks.erase(it);
            if (shouldInterrupt)
                break;
            continue;
        }

        if (shouldInterrupt)
            break;
    }

    if (updateMap)
        m_Menu.RefreshCurrentMapMode(false);
}

void SelectionHandler::Update() {
    this->UpdateColors();
    this->UpdateShader();
}

void SelectionHandler::UpdateSelectionText() {
    static const std::string_view labels[]{ "", "title", "province", "pixel" }; 
    int step = 1 + time(NULL) % 3;
    m_SelectionText.setString(fmt::format(
        "Click on a {}{}",
        labels[static_cast<int>(m_SelectionType)],
        std::string(std::max(1, step), '.')
    ));
}

sf::Text& SelectionHandler::GetSelectionText() {
    return m_SelectionText;
}

void SelectionHandler::SetSelectionType(SelectionType type) {
    m_SelectionType = type;
}

SelectionType SelectionHandler::GetSelectionType() const {
    return m_SelectionType;
}

bool SelectionHandler::IsSelectionType(SelectionType type) const {
    return m_SelectionType == type;
}

void SelectionHandler::UpdateColors() {
    /*sf::Vector2f center = m_Menu.GetCamera().getCenter();
    sf::Vector2f size = m_Menu.GetCamera().getSize()*2.f;

    sf::FloatRect viewBounds(
        { center.x - size.x / 2.0f, center.y - size.y / 2.0f },
        { size.x, size.y }
    );*/

    m_Colors.clear();
    m_Count = 0;

    // Define functions to push colors of selected objects (province, title, region).
    const auto PushProvinces = [&](const auto& provinces) {
        for (const Province* province : provinces) {
            /*sf::Vector2f worldPoint = m_Menu.GetMapSprite()->getTransform().transformPoint(sf::Vector2f(province->GetImagePosition()));
            if (!viewBounds.contains(worldPoint))
                continue;*/
            //m_Count++;
            sf::Color c = province->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, 0.f));
        }
        m_Count += provinces.size();
    };
    
    const auto PushTitles = [&](const auto& titles) {
        for (const Title* title : titles) {
            sf::Color c = title->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, (static_cast<int>(title->GetType())) + 1.f));
        }
        m_Count += titles.size();
    };

    // Use a map to avoid infinite recursion because of circular dependencies.
    std::unordered_set<Region*> visitedRegions;
    const std::function<void(Region*)> PushRegion = [&](Region* region) {
        if (visitedRegions.contains(region))
            return;
        visitedRegions.insert(region);
        PushTitles(region->GetKingdoms());
        PushTitles(region->GetDuchies());
        PushTitles(region->GetCounties());
        PushProvinces(region->GetProvinces());
        for (Region* subRegion : region->GetRegions())
            PushRegion(subRegion);
    };

    // Push the colors for selected provinces, titles and recursively regions.
    PushProvinces(m_Provinces);
    PushTitles(m_Titles);
    for (Region* region : m_Regions) PushRegion(region);

    // Push the colors for the selected province adjacency.
    if (m_Adjacency != nullptr) {
        Province* fromProvince = m_Menu.GetApp().GetMod().GetProvinceManager().GetProvinceById(m_Adjacency->GetFromId());
        Province* toProvince = m_Menu.GetApp().GetMod().GetProvinceManager().GetProvinceById(m_Adjacency->GetToId());
        Province* throughProvince = m_Menu.GetApp().GetMod().GetProvinceManager().GetProvinceById(m_Adjacency->GetThroughId());
        if (fromProvince != nullptr) PushProvinces(std::vector<Province*>{fromProvince});
        if (toProvince != nullptr) PushProvinces(std::vector<Province*>{toProvince});
        // if (throughProvince != nullptr) PushProvinces(std::vector<Province*>{throughProvince});
    }
}

void SelectionHandler::UpdateShader() {
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniformArray("selectedEntities", m_Colors.data(), m_Colors.size());
    provinceShader.setUniform("selectedEntitiesCount", (int) m_Count);
}