#include "SelectionHandler.hpp"
#include "app/menu/EditorMenu.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Region.hpp"
#include "app/map/Title.hpp"

SelectionHandler::SelectionHandler(EditorMenu& menu) :
    m_Menu(menu),
    m_Provinces({}),
    m_ProvincesLookup({}),
    m_Titles({}),
    m_Regions({}),
    m_ProvinceCallbacks({}),
    m_TitleCallbacks({}),
    m_Colors({}),
    m_Count(0)
{}

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

void SelectionHandler::ClearSelection() {
    m_Provinces.clear();
    m_ProvincesLookup.clear();
    m_Titles.clear();
    m_Regions.clear();
    m_Colors.clear();

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

std::span<Province*> SelectionHandler::GetProvinces() {
    return m_Provinces;
}

std::span<Title*> SelectionHandler::GetTitles() {
	return m_Titles;
}

std::span<Region*> SelectionHandler::GetRegions() {
    return m_Regions;
}

std::vector<sf::Glsl::Vec4>& SelectionHandler::GetColors() {
    return m_Colors;
}

std::size_t SelectionHandler::GetCount() const {
    return m_Count;
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback) {
    m_ProvinceCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback) {
    m_TitleCallbacks.push_back(callback);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, Province* province) {
    bool updateMap = false;

    for (auto it = m_ProvinceCallbacks.end(); it != m_ProvinceCallbacks.begin(); ) {
        --it;

        SelectionCallbackResult res = (*it)(button, province);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK)) {
            it = m_ProvinceCallbacks.erase(it);
            continue;
        }

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT))
            break;
    }

    if (updateMap)
        m_Menu.RefreshMapMode(false);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, Province* province, Title* title) {
    bool updateMap = false;

    for (auto it = m_TitleCallbacks.end(); it != m_TitleCallbacks.begin(); ) {
        --it;

        SelectionCallbackResult res = (*it)(button, province, title);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK)) {
            it = m_TitleCallbacks.erase(it);
            continue;
        }

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT))
            break;
    }

    if (updateMap)
        m_Menu.RefreshMapMode(false);
}

void SelectionHandler::Update() {
    this->UpdateColors();
    this->UpdateShader();
}

void SelectionHandler::UpdateColors() {
    m_Colors.clear();
    m_Count = 0;

    // The shader need the colors of provinces.
    // Ttherefore, we have to loop recursively through
    // each titles until we reach a barony tier and get the color.
    // std::function<void(Title*)> PushTitleProvincesColor = [*](Title* title) {
    //     if(title->Is(TitleType::BARONY)) {
    //         BaronyTitle> barony = CastSharedPtr<BaronyTitle>(title);
    //         Province* province = m_Menu->GetApp()->GetMod()->GetProvincesByIds()[barony->GetProvinceId()];
    //         sf::Color c = province->GetColor();
    //         m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
    //         m_Count++;
    //     }
    //     else {
    //         HighTitle* highTitle = CastSharedPtr<HighTitle>(title);
    //         for(const auto* dejureTitle : highTitle->GetDejureTitles())
    //             PushTitleProvincesColor(dejureTitle);
    //     }
    // };
    // for(const auto* title : m_Titles)
    //     PushTitleProvincesColor(title);

    // The method above was too slow. So instead of highlighting the color of every selected province
    // including the ones inside a title, we use the alpha channel to specify what is selected: a province
    // a barony, a county, a duchy, a kingdom or an empire. This way, the shader doesn't have to highlight
    // hundreds of provinces when selecting an empire, but only change the color of the empire title using
    // the empire titles image.

    // Define functions to push colors of selected objects (province, title, region).
    const auto PushProvinces = [&](const auto& provinces) {
        for (const Province* province : provinces) {
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
}

void SelectionHandler::UpdateShader() {
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniformArray("selectedEntities", m_Colors.data(), m_Colors.size());
    provinceShader.setUniform("selectedEntitiesCount", (int) m_Count);
}