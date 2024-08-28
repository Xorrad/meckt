#include "SelectionHandler.hpp"
#include "app/menu/EditorMenu.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Title.hpp"

SelectionHandler::SelectionHandler(EditorMenu* menu) : m_Menu(menu), m_Count(0) {}

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

void SelectionHandler::ClearSelection() {
    m_Provinces.clear();
    m_Titles.clear();
    m_Colors.clear();

    this->Update();
}

bool SelectionHandler::IsSelected(const SharedPtr<Province>& province) {
    return std::find(m_Provinces.begin(), m_Provinces.end(), province) != m_Provinces.end();
}

bool SelectionHandler::IsSelected(const SharedPtr<Title>& title) {
    return std::find(m_Titles.begin(), m_Titles.end(), title) != m_Titles.end();
}

std::vector<SharedPtr<Province>>& SelectionHandler::GetProvinces() {
    return m_Provinces;
}

std::vector<SharedPtr<Title>>& SelectionHandler::GetTitles() {
    return m_Titles;
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

    for(const auto& province : m_Provinces) {
        sf::Color c = province->GetColor();
        m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, 0.f));
    }
    m_Count = m_Provinces.size();

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

    for(const auto& title : m_Titles) {
        sf::Color c = title->GetColor();
        m_Colors.push_back(sf::Glsl::Vec4(c.r/255.f, c.g/255.f, c.b/255.f, ((int) title->GetType()) + 1.f));
    }
    m_Count += m_Titles.size();
}

void SelectionHandler::UpdateShader() {
    sf::Shader& provinceShader = Configuration::shaders.Get(Shaders::PROVINCES);
    provinceShader.setUniformArray("selectedEntities", m_Colors.data(), m_Colors.size());
    provinceShader.setUniform("selectedEntitiesCount", (int) m_Count);
}