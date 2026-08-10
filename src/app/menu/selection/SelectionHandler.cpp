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
    m_PositionCallbacks({}),
    m_ProvinceCallbacks({}),
    m_TitleCallbacks({}),
    m_PositionPickCallback(std::nullopt),
    m_ProvincePickCallback(std::nullopt),
    m_TitlePickCallback(std::nullopt),
    m_MapModeAfterPicking(std::nullopt),
    m_HighlightColor(85, 85, 85),
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

//////////////////////////////////////////////////////

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

bool SelectionHandler::HasProvinceHighlightColorOverride(Province* province) const {
    return m_ProvinceHighlightColorOverrides.contains(province);
}

bool SelectionHandler::IsPicking() const {
    return m_SelectionType != SelectionType::NONE;
}

bool SelectionHandler::IsPicking(SelectionType type) const {
    return m_SelectionType == type;
}

//////////////////////////////////////////////////////

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

const sf::Texture& SelectionHandler::GetSelectionPaletteTexture() const {
    return m_SelectionPalette;
}

sf::Color SelectionHandler::GetHighlightColor() const {
    return m_HighlightColor;
}

SelectionType SelectionHandler::GetSelectionType() const {
    return m_SelectionType;
}

//////////////////////////////////////////////////////

void SelectionHandler::SetHighlightColor(sf::Color color) {
    m_HighlightColor = color;
    this->Update();
}

void SelectionHandler::SetProvinceHighlightColor(Province* province, sf::Color color) {
    if (province == nullptr)
        return;
    m_ProvinceHighlightColorOverrides[province] = color;
    this->Update();
}

//////////////////////////////////////////////////////

void SelectionHandler::ResetProvinceHighlightColor(Province* province) {
    if (m_ProvinceHighlightColorOverrides.erase(province) > 0)
        this->Update();
}

//////////////////////////////////////////////////////

void SelectionHandler::PickProvince(
    std::function<void(Province*)> onPick,
    bool repeatWhileShiftHeld,
    std::optional<MapMode> mapModeWhilePicking,
    std::optional<MapMode> mapModeAfterPicking
) {
    this->CancelPick();

    m_SelectionType = SelectionType::PROVINCE;
    m_MapModeAfterPicking = mapModeAfterPicking;
    if (mapModeWhilePicking.has_value())
        m_Menu.SwitchMapMode(mapModeWhilePicking.value(), false);

    m_ProvincePickCallback = [onPick, repeatWhileShiftHeld](sf::Mouse::Button button, Province* province) {
        if (button != sf::Mouse::Button::Left || province == nullptr)
            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;

        onPick(province);

        if (repeatWhileShiftHeld && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
            return SelectionCallbackResult::INTERRUPT;

        return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
    };
}

void SelectionHandler::PickTitle(
    std::function<bool(Title*)> onPick,
    bool repeatWhileShiftHeld,
    std::optional<MapMode> mapModeWhilePicking,
    std::optional<MapMode> mapModeAfterPicking,
    bool cancelOnNonLeftClick
) {
    this->CancelPick();

    m_SelectionType = SelectionType::TITLE;
    m_MapModeAfterPicking = mapModeAfterPicking;
    if (mapModeWhilePicking.has_value())
        m_Menu.SwitchMapMode(mapModeWhilePicking.value(), false);

    m_TitlePickCallback = [onPick, repeatWhileShiftHeld, cancelOnNonLeftClick](sf::Mouse::Button button, Province* province, Title* title) {
        // A "plain" left click (no LCtrl) is the only kind that can complete a
        // pick; anything else either cancels it or, if `cancelOnNonLeftClick`
        // is false, is left untouched so normal click handlers (RMB-wrap,
        // LCtrl+LMB-unwrap) still work while a pick is in progress.
        bool isPlainLeftClick = button == sf::Mouse::Button::Left && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);
        if (!isPlainLeftClick) {
            if (!cancelOnNonLeftClick)
                return SelectionCallbackResult::CONTINUE;
            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
        }

        if (title == nullptr || !onPick(title))
            return SelectionCallbackResult::INTERRUPT;

        if (repeatWhileShiftHeld && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
            return SelectionCallbackResult::INTERRUPT;

        return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
    };
}

void SelectionHandler::PickPosition(
    std::function<void(sf::Vector2f)> onPick,
    std::optional<MapMode> mapModeWhilePicking,
    std::optional<MapMode> mapModeAfterPicking
) {
    this->CancelPick();

    m_SelectionType = SelectionType::POSITION;
    m_MapModeAfterPicking = mapModeAfterPicking;
    if (mapModeWhilePicking.has_value())
        m_Menu.SwitchMapMode(mapModeWhilePicking.value(), false);

    // See PickProvince()'s note: never calls CancelPick() itself.
    m_PositionPickCallback = [onPick](sf::Mouse::Button button, sf::Vector2f position) {
        if (button != sf::Mouse::Button::Left)
            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;

        onPick(position);

        return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
    };
}

void SelectionHandler::CancelPick() {
    if (m_SelectionType == SelectionType::NONE)
        return;

    m_SelectionType = SelectionType::NONE;
    m_ProvincePickCallback = std::nullopt;
    m_TitlePickCallback = std::nullopt;
    m_PositionPickCallback = std::nullopt;

    if (m_MapModeAfterPicking.has_value()) {
        m_Menu.SwitchMapMode(m_MapModeAfterPicking.value(), false);
        m_MapModeAfterPicking = std::nullopt;
    }
}

//////////////////////////////////////////////////////

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
    m_Adjacency = nullptr;

    this->Update();
}

//////////////////////////////////////////////////////

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)> callback) {
    m_PositionCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback) {
    m_ProvinceCallbacks.push_back(callback);
}

void SelectionHandler::AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback) {
    m_TitleCallbacks.push_back(callback);
}

void SelectionHandler::OnClick(sf::Mouse::Button button, sf::Vector2f position) {
    bool updateMap = false;

    if (m_PositionPickCallback.has_value()) {
        // Copy out before invoking because the callback could synchronously start another pick.
        auto callback = *m_PositionPickCallback;
        SelectionCallbackResult res = callback(button, position);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK))
            this->CancelPick();

        if (shouldInterrupt) {
            if (updateMap)
                m_Menu.RefreshCurrentMapMode(false);
            return;
        }
    }

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

    if (m_ProvincePickCallback.has_value()) {
        // Copy out before invoking because the callback could synchronously start another pick.
        auto callback = *m_ProvincePickCallback;
        SelectionCallbackResult res = callback(button, province);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK))
            this->CancelPick();

        if (shouldInterrupt) {
            if (updateMap)
                m_Menu.RefreshCurrentMapMode(false);
            return;
        }
    }

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

    if (m_TitlePickCallback.has_value()) {
        // Copy out before invoking because the callback could synchronously start another pick.
        auto callback = *m_TitlePickCallback;
        SelectionCallbackResult res = callback(button, province, title);
        bool shouldInterrupt = SelectionCallbackHasFlag(res, SelectionCallbackResult::INTERRUPT);

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::UPDATE_MAP))
            updateMap = true;

        if (SelectionCallbackHasFlag(res, SelectionCallbackResult::DELETE_CALLBACK))
            this->CancelPick();

        if (shouldInterrupt) {
            if (updateMap)
                m_Menu.RefreshCurrentMapMode(false);
            return;
        }
    }

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

//////////////////////////////////////////////////////

void SelectionHandler::Update() {
    // Only rebuilds the CPU-side palette texture; EditorMenu::BindPaletteUniforms()
    // binds it to the shader every frame, same as every other palette, so its
    // texture-unit assignment stays consistent whether or not anything is selected.
    this->BuildSelectionPalette();
}

void SelectionHandler::Tick() {
    if (this->IsPicking() && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        this->CancelPick();
}

void SelectionHandler::Render(sf::RenderTarget& target) {
    if (!this->IsPicking())
        return;

    // Draw a pulsing red outline around the view of the map.
    ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(m_Menu.GetDockspaceID());
    if (node == nullptr)
        return;

    int red = 255 - (abs(sin(2*3.1415*0.05*std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/100.f)) * 150);
    ImGui::GetBackgroundDrawList()->AddRect(
        node->Pos,
        { node->Pos.x + node->Size.x, node->Pos.y + node->Size.y },
        IM_COL32(red, 0, 0, 255),
        0.f,
        ImDrawFlags_None,
        3.f
    );

    static const std::string_view labels[]{ "", "title", "province", "pixel" };
    int step = 1 + time(NULL) % 3;

    m_SelectionText.setString(fmt::format(
        "Click on a {}{}",
        labels[static_cast<int>(m_SelectionType)],
        std::string(std::max(1, step), '.')
    ));
    m_SelectionText.setFillColor(sf::Color(red, 0, 0, 255));
    m_SelectionText.setCharacterSize(24 * Configuration::uiScale);
    m_SelectionText.setPosition({node->Pos.x + 10*Configuration::uiScale, node->Pos.y + 34*Configuration::uiScale});
    target.draw(m_SelectionText);
}

//////////////////////////////////////////////////////

void SelectionHandler::BuildSelectionPalette() {
    // Alpha serves as a signal to the shader for how to render a selected province:
    // - pulsing (actively highlighted)
    // - static (flat color, e.g. background),
    // - hidden (not highlighted at all, keep the original color).
    enum class HighlightStyle {
        PULSING,
        STATIC,
        HIDDEN,
    };

    ProvinceManager& provinceManager = m_Menu.GetApp().GetMod().GetProvinceManager();
    TitleManager& titleManager = m_Menu.GetApp().GetMod().GetTitleManager();
    const auto& indices = provinceManager.GetProvinceIndices();

    std::vector<sf::Color> palette(indices.size(), sf::Color(0, 0, 0, 0));

    const auto MarkProvinceWithColor = [&](Province* province, sf::Color color, HighlightStyle style = HighlightStyle::PULSING) {
        if (province == nullptr)
            return;
        auto it = indices.find(province->GetColorId());
        if (it == indices.end())
            return;

        switch (style) {
            case HighlightStyle::HIDDEN:  color.a = 0;   break;
            case HighlightStyle::STATIC:  color.a = 128; break;
            case HighlightStyle::PULSING: color.a = 255; break;
        }
        palette[it->second] = color;
    };

    const auto MarkProvince = [&](Province* province) {
        MarkProvinceWithColor(province, m_HighlightColor);
    };

    const auto MarkProvinces = [&](const auto& provinces) {
        for (Province* province : provinces)
            MarkProvince(province);
    };

    const auto MarkTitle = [&](Title* title) {
        for (const auto& [colorId, province] : provinceManager.GetProvincesByColors()) {
            if (province->GetProvinceFocusedTitle(titleManager, title->GetType()) == title)
                MarkProvince(province.get());
        }
    };

    const auto MarkTitles = [&](const auto& titles) {
        for (Title* title : titles)
            MarkTitle(title);
    };

    // Use a set to avoid infinite recursion because of circular dependencies.
    std::unordered_set<Region*> visitedRegions;
    const std::function<void(Region*)> MarkRegion = [&](Region* region) {
        if (visitedRegions.contains(region))
            return;
        visitedRegions.insert(region);
        MarkTitles(region->GetKingdoms());
        MarkTitles(region->GetDuchies());
        MarkTitles(region->GetCounties());
        MarkProvinces(region->GetProvinces());
        for (Region* subRegion : region->GetRegions())
            MarkRegion(subRegion);
    };

    MarkProvinces(m_Provinces);
    MarkTitles(m_Titles);
    for (Region* region : m_Regions)
        MarkRegion(region);

    if (m_Adjacency != nullptr) {
        // Color every province by land/sea with two static unique colors.
        for (const auto& [colorId, province] : provinceManager.GetProvincesByColors()) {
            sf::Color flagColor = province->HasFlag(ProvinceFlags::LAND) ? sf::Color::White : sf::Color(80, 80, 80);
            MarkProvinceWithColor(province.get(), flagColor, HighlightStyle::STATIC);
        }

        MarkProvinceWithColor(provinceManager.GetProvinceById(m_Adjacency->GetFromId()), sf::Color(46, 204, 113), HighlightStyle::STATIC);
        MarkProvinceWithColor(provinceManager.GetProvinceById(m_Adjacency->GetThroughId()), sf::Color(52, 152, 219), HighlightStyle::STATIC);
        MarkProvinceWithColor(provinceManager.GetProvinceById(m_Adjacency->GetToId()), sf::Color(231, 76, 60), HighlightStyle::STATIC);
    }

    // Per-province overrides take priority and apply regardless of selection.
    for (const auto& [province, color] : m_ProvinceHighlightColorOverrides)
        MarkProvinceWithColor(province, color);

    m_SelectionPalette = Image::BuildPaletteTexture(palette);
}

//////////////////////////////////////////////////////