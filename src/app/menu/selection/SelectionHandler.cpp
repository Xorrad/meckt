#include "SelectionHandler.hpp"
#include "app/menu/EditorMenu.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Region.hpp"
#include "app/map/Title.hpp"

SelectionHandler::SelectionHandler(EditorMenu* menu) :
    m_Menu(menu),
    m_Provinces({}),
    m_Titles({}),
    m_Regions({}),
    m_ProvinceCallbacks({}),
    m_TitleCallbacks({}),
    m_Colors({}),
    m_Count(0)
{}

void SelectionHandler::Select(const SharedPtr<Province>& province) {
    if(province == nullptr || this->IsSelected(province))
        return;
    m_Provinces.push_back(province);
    this->Update();
}

void SelectionHandler::Select(const SharedPtr<Title>& title) {
    if(title == nullptr || this->IsSelected(title))
        return;
    m_Titles.push_back(title);
    this->Update();
}

void SelectionHandler::Select(const SharedPtr<Region>& region) {
    if(region == nullptr || this->IsSelected(region))
        return;
    m_Regions.push_back(region);
    this->Update();
}

void SelectionHandler::Deselect(const SharedPtr<Province>& province) {
    if(province == nullptr)
        return;
    m_Provinces.erase(std::remove(m_Provinces.begin(), m_Provinces.end(), province));
    this->Update();
}

void SelectionHandler::Deselect(const SharedPtr<Title>& title) {
    if(title == nullptr)
        return;
    m_Titles.erase(std::remove(m_Titles.begin(), m_Titles.end(), title));
    this->Update();
}

void SelectionHandler::Deselect(const SharedPtr<Region>& region) {
    if(region == nullptr)
        return;
    m_Regions.erase(std::remove(m_Regions.begin(), m_Regions.end(), region));
    this->Update();
}

void SelectionHandler::ClearSelection() {
    m_Provinces.clear();
    m_Titles.clear();
    m_Regions.clear();
    m_Colors.clear();

    this->Update();
}

bool SelectionHandler::IsSelected(const SharedPtr<Province>& province) {
    return std::find(m_Provinces.begin(), m_Provinces.end(), province) != m_Provinces.end();
}

bool SelectionHandler::IsSelected(const SharedPtr<Title>& title) {
    return std::find(m_Titles.begin(), m_Titles.end(), title) != m_Titles.end();
}

bool SelectionHandler::IsSelected(const SharedPtr<Region>& region) {
    return std::find(m_Regions.begin(), m_Regions.end(), region) != m_Regions.end();
}

std::vector<SharedPtr<Province>>& SelectionHandler::GetProvinces() {
    return m_Provinces;
}

std::vector<SharedPtr<Title>>& SelectionHandler::GetTitles() {
    return m_Titles;
}

std::vector<SharedPtr<Region>>& SelectionHandler::GetRegions() {
    return m_Regions;
}

std::vector<sf::Glsl::Vec4>& SelectionHandler::GetColors() {
    return m_Colors;
}

std::size_t SelectionHandler::GetCount() const {
    return m_Count;
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>)> callback) {
    m_ProvinceCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>, SharedPtr<Title>)> callback) {
    m_TitleCallbacks.push_back(callback);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, SharedPtr<Province> province) {
    if(province == nullptr)
        return;

    bool updateMap = false;

    for(auto it = m_ProvinceCallbacks.end(); it-- != m_ProvinceCallbacks.begin();) {
        SelectionCallbackResult res = (*it)(button, province);
        if((int)(res & SelectionCallbackResult::DELETE_CALLBACK))
            it = m_ProvinceCallbacks.erase(it);
        if((int)(res & SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;
        if((int)(res & SelectionCallbackResult::INTERRUPT))
            break;
    }

    if(updateMap)
        m_Menu->RefreshMapMode(false);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> title) {
    if(title == nullptr)
        return;

    bool updateMap = false;

   for(auto it = m_TitleCallbacks.end(); it-- != m_TitleCallbacks.begin();) {
        SelectionCallbackResult res = (*it)(button, province, title);
        if((int)(res & SelectionCallbackResult::DELETE_CALLBACK))
            it = m_TitleCallbacks.erase(it);
        if((int)(res & SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;
        if((int)(res & SelectionCallbackResult::INTERRUPT))
            break;
    }

    if(updateMap)
        m_Menu->RefreshMapMode(false);
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
    // std::function<void(const SharedPtr<Title>&)> PushTitleProvincesColor = [&](const SharedPtr<Title>& title) {
    //     if(title->Is(TitleType::BARONY)) {
    //         const SharedPtr<BaronyTitle> barony = CastSharedPtr<BaronyTitle>(title);
    //         const SharedPtr<Province>& province = m_Menu->GetApp()->GetMod()->GetProvincesByIds()[barony->GetProvinceId()];
    //         sf::Color c = province->GetColor();
    //         m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, c.a/255.f));
    //         m_Count++;
    //     }
    //     else {
    //         const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
    //         for(const auto& dejureTitle : highTitle->GetDejureTitles())
    //             PushTitleProvincesColor(dejureTitle);
    //     }
    // };
    // for(const auto& title : m_Titles)
    //     PushTitleProvincesColor(title);

    // The method above was too slow. So instead of highlighting the color of every selected province
    // including the ones inside a title, we use the alpha channel to specify what is selected: a province
    // a barony, a county, a duchy, a kingdom or an empire. This way, the shader doesn't have to highlight
    // hundreds of provinces when selecting an empire, but only change the color of the empire title using
    // the empire titles image.

    // Define functions to push colors of selected objects (province, title, region).
    const auto PushProvinces = [&](const auto& provinces) {
        for (const auto& province : provinces) {
            sf::Color c = province->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, 0.f));
        }
        m_Count += provinces.size();
    };
    
    const auto PushTitles = [&](const auto& titles) {
        for (const auto& title : titles) {
            sf::Color c = title->GetColor();
            m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, ((int) title->GetType()) + 1.f));
        }
        m_Count += titles.size();
    };

    const std::function<void(SharedPtr<Region>)> PushRegion = [&](SharedPtr<Region> region) {
        PushTitles(region->GetKingdoms());
        PushTitles(region->GetDuchies());
        PushTitles(region->GetCounties());
        PushProvinces(region->GetProvinces());
        for (auto& subRegion : region->GetRegions())
            PushRegion(subRegion);
    };

    // Push the colors for selected provinces, titles and recursively regions.
    PushProvinces(m_Provinces);
    PushTitles(m_Titles);
    for (const auto& region : m_Regions) PushRegion(region);
}

void SelectionHandler::UpdateShader() {
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniformArray("selectedEntities", m_Colors.data(), m_Colors.size());
    provinceShader.setUniform("selectedEntitiesCount", (int) m_Count);
}