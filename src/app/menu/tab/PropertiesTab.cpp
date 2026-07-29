#include "PropertiesTab.hpp"

#include "app/App.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/menu/selection/SelectionHandler.hpp"

#include "core/mod/Mod.hpp"
#include "core/provinces/ProvinceManager.hpp"
#include "core/titles/TitleManager.hpp"
#include "core/regions/RegionManager.hpp"

#include "imgui/imgui.hpp"
#include "app/menu/ImGuiStyle.hpp"
#include "provinces/ProvinceFlags.hpp"

PropertiesTab::PropertiesTab(EditorMenu& menu, bool visible) :
    Tab("Properties", Tabs::PROPERTIES, menu, visible),
	m_SelectingTitleText(Configuration::fonts.Get(Fonts::NOTO_SANS)),
    m_SelectingTitle(false),
    m_SelectingProvince(false),
    m_DisplayCulturalNames(false),
    m_DisplayHistory(false),
    m_DisplayDejureTitles(false),
    m_DisplayClimate(false),
    m_DisplayRegionsTitles(true),
    m_DisplayRegionsProvinces(true),
    m_DisplayRegionsRegions(true)
{
    m_SelectingTitleText.setCharacterSize(24 * Configuration::uiScale);
    m_SelectingTitleText.setString("Click on a title.");
    m_SelectingTitleText.setFillColor(sf::Color::Red);
    m_SelectingTitleText.setFont(Configuration::fonts.Get(Fonts::NOTO_SANS));
    m_SelectingTitleText.setPosition({10, 20});
    m_SelectingTitleText.setScale({ Configuration::uiScale, Configuration::uiScale });

    m_Clock.restart();
}

void PropertiesTab::Update(sf::Time delta) {
    if (m_Clock.getElapsedTime().asSeconds() > 0.5) {
        std::string selectionText = fmt::format("Click on a {}", m_SelectingTitle ? "title" : "province");

        if (m_Clock.getElapsedTime().asSeconds() > 1.5) { m_Clock.restart(); selectionText += "..."; }
        else if (m_Clock.getElapsedTime().asSeconds() > 1.0) selectionText += "..";
        else selectionText += ".";

        m_SelectingTitleText.setString(selectionText);
    }

    // Cancel selecting a title or province by pressing escape.
    if ((m_SelectingTitle || m_SelectingProvince) && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        if (m_SelectingTitle) m_Menu.GetSelectionHandler().m_TitleCallbacks.pop_back();
        if (m_SelectingProvince) m_Menu.GetSelectionHandler().m_ProvinceCallbacks.pop_back();
        m_SelectingTitle = false;
        m_SelectingProvince = false;
    }
}

void PropertiesTab::Render() {
    if (!m_Visible)
        return;

    if (m_SelectingTitle || m_SelectingProvince) {
        // Draw a red outline around the view of the map.
        ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(m_Menu.GetDockspaceID());
        if (node != nullptr) {
            int red = 255 - (abs(sin(2*3.1415*0.05*std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()/100.f)) * 150);
            ImGui::GetBackgroundDrawList()->AddRect(
                node->Pos,
                { node->Pos.x + node->Size.x, node->Pos.y + node->Size.y },
                IM_COL32(red, 0, 0, 255),
                0.f,
                ImDrawFlags_None,
                3.f
            );
            m_SelectingTitleText.setFillColor(sf::Color(red, 0, 0, 255));
        }

        m_SelectingTitleText.setCharacterSize(24 * Configuration::uiScale);
        m_SelectingTitleText.setPosition({node->Pos.x + 10*Configuration::uiScale, node->Pos.y + 10*Configuration::uiScale});
        m_Menu.GetApp().GetWindow().draw(m_SelectingTitleText);
    }

    if (m_Menu.GetSelectionHandler().GetProvinces().size() > 0) {
        if (m_Menu.GetSelectionHandler().GetProvinces().size() > 1) {
            this->RenderJointProvinces();
        }
        this->RenderProvinces();
    }
    else if (m_Menu.GetSelectionHandler().GetTitles().size() > 0) {
        this->RenderTitles();
    }
    else if (m_Menu.GetSelectionHandler().GetRegions().size() > 0) {
        this->RenderRegions();
    }
    
}

void PropertiesTab::RenderJointProvinces() {
    // Determine values based on selected provinces and
    // whether several provinces have different values for a property.
    Province* firstProvince = m_Menu.GetSelectionHandler().GetProvinces().front();

    std::string culture = firstProvince->GetCulture();
    std::string religion = firstProvince->GetFaith();
    std::string holding = firstProvince->GetHolding();
    std::string terrain = firstProvince->GetTerrain();

    ClimateType climateType = firstProvince->GetClimateType();
    std::string winterSeverityBias = firstProvince->GetWinterSeverityBias();
    std::string mildWinterFactorOverride = firstProvince->GetMildWinterFactorOverride();
    std::string normalWinterFactorOverride = firstProvince->GetNormalWinterFactorOverride();
    std::string harshWinterFactorOverride = firstProvince->GetHarshWinterFactorOverride();

    int isCoastal = firstProvince->HasFlag(ProvinceFlags::COASTAL);
    int isIsland = firstProvince->HasFlag(ProvinceFlags::ISLAND);
    int isLand = firstProvince->HasFlag(ProvinceFlags::LAND);
    int isSea = firstProvince->HasFlag(ProvinceFlags::SEA);
    int isRiver = firstProvince->HasFlag(ProvinceFlags::RIVER);
    int isLake = firstProvince->HasFlag(ProvinceFlags::LAKE);
    int isImpassable = firstProvince->HasFlag(ProvinceFlags::IMPASSABLE);

    for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
        if (province->GetCulture() != culture) culture = "*****";
        if (province->GetFaith() != religion) religion = "*****";
        if (province->GetHolding() != holding) holding = "*****";
        if (province->GetTerrain() != terrain) terrain = "*****";
        if (province->GetClimateType() != climateType) climateType = ClimateType::COUNT;
        if (province->GetWinterSeverityBias() != winterSeverityBias) winterSeverityBias = "*****";
        if (province->GetMildWinterFactorOverride() != mildWinterFactorOverride) mildWinterFactorOverride = "*****";
        if (province->GetNormalWinterFactorOverride() != normalWinterFactorOverride) normalWinterFactorOverride = "*****";
        if (province->GetHarshWinterFactorOverride() != harshWinterFactorOverride) harshWinterFactorOverride = "*****";
        if (province->HasFlag(ProvinceFlags::COASTAL) != isCoastal) isCoastal = -1;
        if (province->HasFlag(ProvinceFlags::ISLAND) != isIsland) isIsland = -1;
        if (province->HasFlag(ProvinceFlags::LAND) != isLand) isLand = -1;
        if (province->HasFlag(ProvinceFlags::SEA) != isSea) isSea = -1;
        if (province->HasFlag(ProvinceFlags::RIVER) != isRiver) isRiver = -1;
        if (province->HasFlag(ProvinceFlags::LAKE) != isLake) isLake = -1;
        if (province->HasFlag(ProvinceFlags::IMPASSABLE) != isImpassable) isImpassable = -1;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(3, 48, 102, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 57, 106, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(2, 45, 86, 255));
    if (ImGui::CollapsingHeader("global", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("##joint-provinces", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        ImGui::PushID("joint-provinces");

        // PROVINCE: terrain type (combobox)
        Components::TerrainTypeCombo(
            m_Mod.GetProvinceManager().GetTerrainTypes(),
            terrain, 
            [&](const auto& newTerrainType) {
                for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                    province->SetTerrain(newTerrainType ? newTerrainType->GetName() : "");
                }
                m_Menu.RefreshMapMode(MapMode::TERRAIN);
            }
        );

        
        // PROVINCE: flags (checkbox)
        #define UPDATE_FLAG(flag, var) \
            for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) { \
                province->SetFlag(ProvinceFlags::flag, var); \
            }
        #define UPDATE_FLAG_SEA_RIVER_LAKE(flag, var, isSea, isRiver, isLake) \
            for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) { \
                province->SetFlag(ProvinceFlags::flag, var); \
                province->SetFlag(ProvinceFlags::LAND, !(isSea || isRiver || isLake)); \
            }

        if (ImGui::BeginTable("province flags", 2)) {
            
            // Coastal - Island

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::BeginDisabled();
            if (ImGui::CheckBoxTristate("Coastal", &isCoastal)) UPDATE_FLAG(COASTAL, isCoastal);
            ImGui::EndDisabled();
            
            ImGui::TableSetColumnIndex(1);
            ImGui::BeginDisabled();
            if (ImGui::CheckBoxTristate("Island", &isIsland)) UPDATE_FLAG(ISLAND, isIsland);
            ImGui::EndDisabled();

            // Land - Sea

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::BeginDisabled();
            if (ImGui::CheckBoxTristate("Land", &isLand)) UPDATE_FLAG(LAND, isLand);
            ImGui::EndDisabled();

            ImGui::TableSetColumnIndex(1);
            if (ImGui::CheckBoxTristate("Sea", &isSea)) UPDATE_FLAG_SEA_RIVER_LAKE(SEA, isSea, isSea, isRiver, isLake);

            // River - Lake

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (ImGui::CheckBoxTristate("River", &isRiver)) UPDATE_FLAG_SEA_RIVER_LAKE(RIVER, isRiver, isSea, isRiver, isLake);
            
            ImGui::TableSetColumnIndex(1);
            if (ImGui::CheckBoxTristate("Lake", &isLake)) UPDATE_FLAG_SEA_RIVER_LAKE(LAKE, isLake, isSea, isRiver, isLake);

            // Impassable

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (ImGui::CheckBoxTristate("Impassable", &isImpassable)) UPDATE_FLAG(IMPASSABLE, isImpassable);

            ImGui::EndTable();
        }

        // PROVINCE: culture (field)
        if (ImGui::InputTextCommitOnEnter("culture", &culture)) {
            for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                province->SetCulture(culture);
            }
            m_Menu.RefreshMapMode(MapMode::CULTURE);
        }

        // PROVINCE: religion (field)
        if (ImGui::InputTextCommitOnEnter("religion", &religion)) {
            for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                province->SetFaith(religion);
            }
            m_Menu.RefreshMapMode(MapMode::FAITH);
        }

        // PROVINCE: holding type (combobox)
        Components::HoldingTypeCombo(
            m_Mod.GetProvinceManager().GetHoldingTypes(),
            holding,
            [&](const auto& newHoldingType) {
                for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                    province->SetHolding(newHoldingType ? newHoldingType->GetName() : "");
                }
            }
        );

        // PROVINCE: climate (collapsing header + child window (for borders) + text inputs)
        ImGui::SetNextItemOpen(m_DisplayClimate, ImGuiCond_Appearing);
        if (ImGui::CollapsingHeader("climate")) {
            m_DisplayClimate = true;
            
            if (ImGui::BeginChild("climate", ImVec2(0, 175), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                ImGui::SetNextItemWidth(0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("climate type").x - 10);
                Components::ClimateTypeCombo(
                    climateType,
                    [&](ClimateType newClimateType) {
                        for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                            province->SetClimateType(newClimateType);
                        }
                    }
                );
                
                // Use the same width for all items below so they are aligned.
                int width = 0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("normal winter factor override").x;

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputTextCommitOnEnter("winter severity bias", &winterSeverityBias)) {
                    for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                        province->SetWinterSeverityBias(winterSeverityBias);
                    }
                    m_Menu.RefreshMapMode(MapMode::WINTER_SEVERITY);
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputTextCommitOnEnter("mild winter factor override", &mildWinterFactorOverride)) {
                    for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                        province->SetMildWinterFactorOverride(mildWinterFactorOverride);
                    }
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputTextCommitOnEnter("normal winter factor override", &normalWinterFactorOverride))  {
                    for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                        province->SetNormalWinterFactorOverride(normalWinterFactorOverride);
                    }
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputTextCommitOnEnter("harsh winter factor override", &harshWinterFactorOverride)) {
                    for (auto& province : m_Menu.GetSelectionHandler().GetProvinces()) {
                        province->SetHarshWinterFactorOverride(harshWinterFactorOverride);
                    }
                }

                ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "note: leave fields empty if you don't want any value.");
            }
            ImGui::EndChild();
        }
        else {
            m_DisplayClimate = false;
        }

        ImGui::PopID();
        ImGui::EndChild();
    }
    ImGui::PopStyleColor(3);

    ImGui::Separator();
}

// Because the user inputs are strings for the history data,
// we can't directly use a pointer to a variable in the
// Title class.
// Therefore, TitleHistoryState is used as a temporary buffer
// for the input, which will be parsed and added to the history
// Parser::Node in the Title class.
struct TitleHistoryState {
    std::string rawData;
    std::string parsingError;
};

void PropertiesTab::RenderProvinces() {
    auto provinces = m_Menu.GetSelectionHandler().GetProvinces();
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(provinces.size()));

    while (clipper.Step()) {
        for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; index++) {
            Province* province = provinces[index];
                
            if (ImGui::CollapsingHeader(fmt::format("#{} ({})", province->GetId(), province->GetName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BeginChild(fmt::format("##province-{}", province->GetId()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
                ImGui::PushID(province->GetId());                        

                // PROVINCE: id (field)
                ImGui::BeginDisabled();
                std::string id = std::to_string(province->GetId());
                ImGui::InputText("id", &id);
                ImGui::EndDisabled();

                // PROVINCE: name (field)
                std::string name = province->GetName();
                if (ImGui::InputTextCommitOnEnter("name", &name)) {
                    province->SetName(name);
                }

                // PROVINCE: color (colorpicker)
                sf::Color color = province->GetColor();
                ImGui::BeginDisabled();
                if (ImGui::ColorEdit3("color", &color)) {
                    // TODO: error if color is already taken by another province.
                    // TODO: change pixels color in provinces.png.
                    province->SetColor(color);
                }
                ImGui::EndDisabled();


                // PROVINCE: terrain type (combobox)
                Components::TerrainTypeCombo(
                    m_Mod.GetProvinceManager().GetTerrainTypes(),
                    province->GetTerrain(), 
                    [&](const auto& newTerrainType) {
                        province->SetTerrain(newTerrainType ? newTerrainType->GetName() : "");
                        m_Menu.RefreshMapMode(MapMode::TERRAIN);
                    }
                );
            
                // PROVINCE: flags (checkbox)
                bool isCoastal = province->HasFlag(ProvinceFlags::COASTAL);
                bool isIsland = province->HasFlag(ProvinceFlags::ISLAND);
                bool isLand = province->HasFlag(ProvinceFlags::LAND);
                bool isSea = province->HasFlag(ProvinceFlags::SEA);
                bool isRiver = province->HasFlag(ProvinceFlags::RIVER);
                bool isLake = province->HasFlag(ProvinceFlags::LAKE);
                bool isImpassable = province->HasFlag(ProvinceFlags::IMPASSABLE);

                if (ImGui::BeginTable("province flags", 2)) {
                    
                    // Coastal - Island

                    ImGui::TableNextRow();                
                    ImGui::TableSetColumnIndex(0);
                    ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Coastal", &isCoastal)) province->SetFlag(ProvinceFlags::COASTAL, isCoastal);
                    ImGui::EndDisabled();

                    ImGui::TableSetColumnIndex(1);
                    ImGui::BeginDisabled();
                    if (ImGui::Checkbox("Island", &isIsland)) province->SetFlag(ProvinceFlags::ISLAND, isIsland);
                    ImGui::EndDisabled();
                    
                    // Land - Sea

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::BeginDisabled();
                    ImGui::Checkbox("Land", &isLand);
                    ImGui::EndDisabled();
                    
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::Checkbox("Sea", &isSea)) {
                        province->SetFlag(ProvinceFlags::SEA, isSea);
                        province->SetFlag(ProvinceFlags::LAND, !(province->HasFlag(ProvinceFlags::SEA | ProvinceFlags::RIVER | ProvinceFlags::LAKE)));
                    }

                    // River - Lake

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Checkbox("River", &isRiver)) {
                        province->SetFlag(ProvinceFlags::RIVER, isRiver);
                        province->SetFlag(ProvinceFlags::LAND, !(province->HasFlag(ProvinceFlags::SEA | ProvinceFlags::RIVER | ProvinceFlags::LAKE)));
                    }
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::Checkbox("Lake", &isLake)) {
                        province->SetFlag(ProvinceFlags::LAKE, isLake);
                        province->SetFlag(ProvinceFlags::LAND, !(province->HasFlag(ProvinceFlags::SEA | ProvinceFlags::RIVER | ProvinceFlags::LAKE)));
                    }

                    // Impassable

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Checkbox("Impassable", &isImpassable)) province->SetFlag(ProvinceFlags::IMPASSABLE, isImpassable);

                    ImGui::EndTable();
                }

                // PROVINCE: culture (field)
                std::string culture = province->GetCulture();
                if (ImGui::InputTextCommitOnEnter("culture", &culture)) {
                    province->SetCulture(culture);
                    m_Menu.RefreshMapMode(MapMode::CULTURE);
                }

                // PROVINCE: faith (field)
                std::string faith = province->GetFaith();
                if (ImGui::InputTextCommitOnEnter("faith", &faith)) {
                    province->SetFaith(faith);
                    m_Menu.RefreshMapMode(MapMode::FAITH);
                }


                // PROVINCE: holding type (combobox)
                Components::HoldingTypeCombo(
                    m_Mod.GetProvinceManager().GetHoldingTypes(),
                    province->GetHolding(),
                    [&](const auto& newHoldingType) {
                        province->SetHolding(newHoldingType ? newHoldingType->GetName() : "");
                    }
                );

                // PROVINCE: history (collapsing header + child window (for borders) + collapsing header for each dates)
                ImGui::SetNextItemOpen(m_DisplayHistory, ImGuiCond_Appearing);
                if (ImGui::CollapsingHeader("history")) {
                    m_DisplayHistory = true;

                    if (ImGui::BeginChild((province->GetName() + "-history").c_str(), ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                        static std::string date = "";
                        static bool isDateValid = true;

                        const auto& AddNewDate = [&]() {
                            try {
                                Jomini::Date newDate = Jomini::Date(date);
                                if (!province->GetHistory().contains(newDate))
                                    province->AddHistory(newDate, MakeShared<Jomini::Object>(Jomini::ObjectMap{}));
                                isDateValid = true;
                            }
                            catch(std::exception& e) {
                                isDateValid = false;
                            }
                        };

                        if (ImGui::InputText("##date", &date, ImGuiInputTextFlags_EnterReturnsTrue)) {
                            AddNewDate();
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("add")) {
                            AddNewDate();
                        }
                        if (!isDateValid)
                            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Invalid date format");
            

                        // TODO: improve this to avoid "memory leaks" when switching titles or even tabs.

                        // Each date has its own data/history and each title can have
                        // several dates. To avoid overwritting user inputs, the buffer are saved
                        // in a map using the key: title_name-date.
                        // Keys are erased from the map when the date TreeNode has been closed
                        // and if the edits have been saved successfully (no parsing error).
                        static std::unordered_map<std::string, TitleHistoryState> historyStates;

                        std::map<Jomini::Date, SharedPtr<Jomini::Object>> history = province->GetHistory();
                        for (auto const& [date, data] : history | std::views::reverse) {
                            std::string stateKey = fmt::format("{}-{}", province->GetName(), date);

                            ImGui::SetNextItemAllowOverlap();
                            if (ImGui::TreeNodeEx(fmt::format("{}", date).c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
                                ImGui::PushID(stateKey.c_str());
                            
                                // Insert the delete button on the smae line as the tree node.
                                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                                if (ImGui::SmallButton("x")) {
                                    historyStates.erase(stateKey);
                                    province->RemoveHistory(date);
                                }

                                if (historyStates.count(stateKey) == 0) {
                                    historyStates[stateKey] = TitleHistoryState{
                                        fmt::format("{}", data->Serialize()),
                                        "",
                                    };
                                }

                                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 10);
                                if (ImGui::InputTextMultiline("data", &historyStates[stateKey].rawData, ImVec2(0,0), ImGuiInputTextFlags_AllowTabInput)) {
                                    try {
                                        SharedPtr<Jomini::Object> newData = Jomini::ParseString(historyStates[stateKey].rawData);
                                        historyStates[stateKey].parsingError = "";
                                        province->AddHistory(date, newData);
                                    }
                                    catch(const std::exception& e) {
                                        historyStates[stateKey].parsingError = e.what();
                                    }
                                }

                                if (!historyStates[stateKey].parsingError.empty()) {
                                    ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), fmt::format("Failed to parse data: {}", historyStates[stateKey].parsingError).c_str());
                                }

                                ImGui::PopID();
                                ImGui::TreePop();
                            }
                            else {
                                ImGui::PushID(stateKey.c_str());

                                // Insert the delete button on the smae line as the tree node.
                                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                                if (ImGui::SmallButton("x"))
                                    province->RemoveHistory(date);

                                if (historyStates.count(stateKey) > 0 && historyStates[stateKey].parsingError.empty())
                                    historyStates.erase(stateKey);

                                ImGui::PopID();
                            }
                        }
                    }
                    ImGui::EndChild();
                }
                else {
                    m_DisplayHistory = false;
                }

                // PROVINCE: climate (collapsing header + child window (for borders) + text inputs)
                ImGui::SetNextItemOpen(m_DisplayClimate, ImGuiCond_Appearing);
                if (ImGui::CollapsingHeader("climate")) {
                    m_DisplayClimate = true;
                
                    if (ImGui::BeginChild((province->GetName() + "-climate").c_str(), ImVec2(0, 175), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                        ImGui::SetNextItemWidth(0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("climate").x - 10);
                        if (ImGui::BeginCombo("climate", ClimateTypeLabels.at(province->GetClimateType()))) {
                            for (int i = 0; i < (int) ClimateType::COUNT; i++) {
                                ClimateType type = (ClimateType) i;
                                const bool isSelected = (type == province->GetClimateType());
                                if (ImGui::Selectable(ClimateTypeLabels.at(type), isSelected))
                                    province->SetClimateType(type);
                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    
                        // Use the same width for all items below so they are aligned.
                        int width = 0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("normal winter factor override").x;

                        ImGui::SetNextItemWidth(width);
                        std::string winterSeverityBias = province->GetWinterSeverityBias();
                        if (ImGui::InputTextCommitOnEnter("winter severity bias", &winterSeverityBias)) {
                            province->SetWinterSeverityBias(winterSeverityBias);
                            m_Menu.RefreshMapMode(MapMode::WINTER_SEVERITY);
                        }

                        ImGui::SetNextItemWidth(width);
                        std::string mildWinterFactorOverride = province->GetMildWinterFactorOverride();
                        if (ImGui::InputTextCommitOnEnter("mild winter factor override", &mildWinterFactorOverride)) {
                            province->SetMildWinterFactorOverride(mildWinterFactorOverride);
                        }

                        ImGui::SetNextItemWidth(width);
                        std::string normalWinterFactorOverride = province->GetNormalWinterFactorOverride();
                        if (ImGui::InputTextCommitOnEnter("normal winter factor override", &normalWinterFactorOverride)) {
                            province->SetNormalWinterFactorOverride(normalWinterFactorOverride);
                        }

                        ImGui::SetNextItemWidth(width);
                        std::string harshWinterFactorOverride = province->GetHarshWinterFactorOverride();
                        if (ImGui::InputTextCommitOnEnter("harsh winter factor override", &harshWinterFactorOverride)) {
                            province->SetHarshWinterFactorOverride(harshWinterFactorOverride);
                        }

                        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "note: leave fields empty if you don't want any value.");
                    }
                    ImGui::EndChild();
                }
                else {
                    m_DisplayClimate = false;
                }

                // PROVINCE: switch to barony (button)
                if (BaronyTitle* baronyTitle = m_Mod.GetTitleManager().GetBaronyByProvinceId(province->GetId())) {
                    if (ImGui::Button("switch to barony")) {
                        m_Menu.SwitchMapMode(MapMode::BARONY, true);
                        m_Menu.GetSelectionHandler().Select(baronyTitle);
                    }
                }
                // PROVINCE: create barony (button)
                else if (province->HasFlag(ProvinceFlags::LAND) && !province->HasFlag(ProvinceFlags::IMPASSABLE)) {
                    if (ImGui::Button("create barony title")) {
                        // Make sure to use a title name that isn't already taken.
                        std::string baronyName = "b_" + String::ToLowercase(province->GetName());
                        int i = 1;
                        while(m_Mod.GetTitleManager().HasTitle(baronyName)) {
                            baronyName = "b_" + String::ToLowercase(province->GetName()) + std::to_string(i);
                            i++;
                        }

                        UniquePtr<Title> title = MakeTitle(TitleType::BARONY, baronyName, province->GetColor(), false);
                        BaronyTitle* baronyTitle = dynamic_cast<BaronyTitle*>(title.get());
                        baronyTitle->SetProvinceId(province->GetId());

                        m_Mod.GetTitleManager().AddTitle(std::move(title));
                    }
                }

                ImGui::PopID();
                ImGui::EndChild();
            }
        }
    }
    clipper.End();
}

void PropertiesTab::RenderTitles() {
    auto titles = m_Menu.GetSelectionHandler().GetTitles();
    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(titles.size()));

    while (clipper.Step()) {
        for (int index = clipper.DisplayStart; index < clipper.DisplayEnd; index++) {
            Title* title = titles[index];

            if (ImGui::CollapsingHeader(title->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::BeginChild(fmt::format("##title-{}", title->GetName()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
                ImGui::PushID(title->GetName().c_str());

                // TITLE: name/tag (field)
                std::string titleName = title->GetName();
                if (ImGui::InputTextCommitOnEnter("name", &titleName)) {
                    // Rename the title globally, including titles history.
                    m_Mod.GetTitleManager().RenameTitle(title->GetName(), titleName);
                }

                // TITLE: localization name (field)
                std::string locName = title->GetLocName("english");
                if (ImGui::InputTextCommitOnEnter("loc. name", &locName)) {
                    title->SetLocName("english", locName);
                }

                // TITLE: localization adjective (field)
                std::string locAdjective = title->GetLocAdjective("english");
                if (ImGui::InputTextCommitOnEnter("loc. adjective", &locAdjective)) {
                    title->SetLocAdjective("english", locAdjective);
                }

                // TITLE: localization article (field)
                std::string locArticle = title->GetLocArticle("english");
                if (ImGui::InputTextCommitOnEnter("loc. article", &locArticle)) {
                    title->SetLocArticle("english", locArticle);
                }

                // TITLE: tier/type (combo)
                ImGui::BeginDisabled();
                if (ImGui::BeginCombo("type", TitleTypeLabels[(int)title->GetType()]))
                    ImGui::EndCombo();
                ImGui::EndDisabled();

                // TITLE: color (colorpicker)
                sf::Color color = title->GetColor();
                if (ImGui::ColorEdit3("color", &color)) {
                    title->SetColor(color);
					m_Menu.RefreshMapMode(TitleTypeToMapMode(title->GetType()));
                    m_Menu.GetSelectionHandler().Update();
                }

                // TITLE: landless (checkbox)
                bool landless = title->IsLandless();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                if (ImGui::Checkbox("Landless", &landless)) {
                    title->SetLandless(landless);
                }
                ImGui::PopStyleVar();

                // TITLE: cultural names (collapsing header + child window (for borders) + collapsing header for each culture)
                ImGui::SetNextItemOpen(m_DisplayCulturalNames, ImGuiCond_Appearing);
                if (ImGui::CollapsingHeader("cultural names")) {
                    m_DisplayCulturalNames = true;

                    if (ImGui::BeginChild((title->GetName() + "-cultural-names").c_str(), ImVec2(0, 100), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                        static std::string newCulture = "";

                        const auto& AddNewCulture = [&]() {
                            if (title->GetCulturalNames().count(newCulture) > 0)
                                return;
                            title->AddCulturalName(newCulture, "cn_");
                            };

                        if (ImGui::InputText("##culture", &newCulture, ImGuiInputTextFlags_EnterReturnsTrue)) {
                            AddNewCulture();
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("add")) {
                            AddNewCulture();
                        }

                        std::map<std::string, std::string>& culturalNames = title->GetCulturalNames();
                        for (auto it = culturalNames.begin(); it != culturalNames.end(); ) {
                            std::string culture = it->first;
                            std::string& name = it->second;

                            ImGui::PushID(culture.c_str());

                            if (ImGui::Button("x")) {
                                it = culturalNames.erase(it);
                            }
                            else {
                                ++it;
                            }

                            ImGui::SameLine();
                            ImGui::InputText(culture.c_str(), &name);

                            ImGui::PopID();
                        }
                    }
                    ImGui::EndChild();
                }
                else {
                    m_DisplayCulturalNames = false;
                }

                // TITLE: history (collapsing header + child window (for borders) + collapsing header for each dates)
                ImGui::SetNextItemOpen(m_DisplayHistory, ImGuiCond_Appearing);
                if (ImGui::CollapsingHeader("history")) {
                    m_DisplayHistory = true;

                    if (ImGui::BeginChild((title->GetName() + "-history").c_str(), ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                        static std::string date = "";
                        static bool isDateValid = true;

                        const auto& AddNewDate = [&]() {
                            try {
                                Jomini::Date newDate = Jomini::Date(date);
                                if (!title->GetHistory().contains(newDate))
                                    title->AddHistory(newDate, MakeShared<Jomini::Object>(Jomini::ObjectMap{}));
                                isDateValid = true;
                            }
                            catch (std::exception& e) {
                                isDateValid = false;
                            }
                            };

                        if (ImGui::InputText("##date", &date, ImGuiInputTextFlags_EnterReturnsTrue)) {
                            AddNewDate();
                        }
                        ImGui::SameLine();
                        if (ImGui::SmallButton("add")) {
                            AddNewDate();
                        }
                        if (!isDateValid)
                            ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Invalid date format");


                        // TODO: improve this to avoid "memory leaks" when switching titles or even tabs.

                        // Each date has its own data/history and each title can have
                        // several dates. To avoid overwritting user inputs, the buffer are saved
                        // in a map using the key: title_name-date.
                        // Keys are erased from the map when the date TreeNode has been closed
                        // and if the edits have been saved successfully (no parsing error).
                        static std::unordered_map<std::string, TitleHistoryState> historyStates;

                        std::map<Jomini::Date, SharedPtr<Jomini::Object>> history = title->GetHistory();
                        for (auto const& [date, data] : history | std::views::reverse) {
                            std::string stateKey = fmt::format("{}-{}", title->GetName(), date);

                            ImGui::SetNextItemAllowOverlap();
                            if (ImGui::TreeNodeEx(fmt::format("{}", date).c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
                                ImGui::PushID(stateKey.c_str());

                                // Insert the delete button on the smae line as the tree node.
                                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20);
                                if (ImGui::SmallButton("x")) {
                                    historyStates.erase(stateKey);
                                    title->RemoveHistory(date);
                                }

                                if (historyStates.count(stateKey) == 0) {
                                    historyStates[stateKey] = TitleHistoryState{
                                        fmt::format("{}", data->Serialize()),
                                        "",
                                    };
                                }

                                ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - 10);
                                if (ImGui::InputTextMultiline("data", &historyStates[stateKey].rawData, ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput)) {
                                    try {
                                        SharedPtr<Jomini::Object> newData = Jomini::ParseString(historyStates[stateKey].rawData);
                                        historyStates[stateKey].parsingError = "";
                                        title->AddHistory(date, newData);
                                    }
                                    catch (const std::exception& e) {
                                        historyStates[stateKey].parsingError = e.what();
                                    }
                                }

                                if (!historyStates[stateKey].parsingError.empty()) {
                                    ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), fmt::format("Failed to parse data: {}", historyStates[stateKey].parsingError).c_str());
                                }

                                ImGui::PopID();
                                ImGui::TreePop();
                            }
                            else {
                                ImGui::PushID(stateKey.c_str());

                                // Insert the delete button on the smae line as the tree node.
                                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20);
                                if (ImGui::SmallButton("x"))
                                    title->RemoveHistory(date);

                                if (historyStates.count(stateKey) > 0 && historyStates[stateKey].parsingError.empty())
                                    historyStates.erase(stateKey);

                                ImGui::PopID();
                            }
                        }
                    }
                    ImGui::EndChild();
                }
                else {
                    m_DisplayHistory = false;
                }

                if (title->Is(TitleType::BARONY)) {

                    // BARONY: province id (field)
                    BaronyTitle* barony = static_cast<BaronyTitle*>(title);
                    int provinceId = barony->GetProvinceId();
                    if (ImGui::InputInt("province id", &provinceId)) {
                        if (!m_Mod.GetProvinceManager().HasProvinceById(provinceId)) {
                            ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
                            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
                            ImGui::Text("⚠️");
                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("No province with this id exists!");
                            ImGui::PopStyleColor();
                            ImGui::PopFont();
                            LOG_ERROR("Barony with undefined province id: {},{}", barony->GetName(), provinceId);
                        }
                    }

                    // BARONY: province id (field)
                    ImGui::NewLine();
                    if (ImGui::Button((m_SelectingTitle) ? "click on a province..." : "change province") && !m_SelectingTitle) {
                        m_SelectingTitle = true;
                        MapMode previousMapMode = m_Menu.GetMapMode();
                        m_Menu.SwitchMapMode(MapMode::PROVINCES, false);
                        m_Menu.GetSelectionHandler().AddCallback(
                            [this, barony, previousMapMode](sf::Mouse::Button button, Province* province) {
                                if (button != sf::Mouse::Button::Left)
                                    return SelectionCallbackResult::INTERRUPT;

                                m_Mod.GetTitleManager().ChangeBaronyProvinceId(barony, province->GetId());

                                m_Menu.SwitchMapMode(previousMapMode, false);
                                m_SelectingTitle = false;
                                return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                            }
                        );
                    }

                    // BARONY: Switch to province (button)
                    if (ImGui::Button("switch to province")) {
                        if (Province* province = m_Mod.GetProvinceManager().GetProvinceById(barony->GetProvinceId())) {
                            m_Menu.SwitchMapMode(MapMode::PROVINCES, true);
                            m_Menu.GetSelectionHandler().Select(province);
                        }
                    }
                }
                else {
                    HighTitle* highTitle = static_cast<HighTitle*>(title);

                    // HIGHTITLE: dejure titles (list)
                    ImGui::SetNextItemOpen(m_DisplayDejureTitles, ImGuiCond_Appearing);
                    if (ImGui::CollapsingHeader("dejure titles")) {
                        m_DisplayDejureTitles = true;

                        ImGui::BeginChild("dejure titles", ImVec2(0, 250), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_None);

                        if (ImGui::BeginMenuBar()) {
                            if (ImGui::BeginMenu("dejure titles")) {
                                ImGui::EndMenu();
                            }
                            ImGui::EndMenuBar();
                        }

                        // Make a copy to be able to use highTitle->RemoveDejureTitle
                        // without causing a crash while iterating.
                        std::vector<Title*> dejureTitles = highTitle->GetDejureTitles();
                        int n = 0;
                        for (auto const& dejure : dejureTitles) {
                            ImGui::PushID(dejure->GetName().c_str());
                            ImGui::SetNextItemAllowOverlap();
                            ImGui::Selectable(dejure->GetName().c_str());

                            // Switch to the properties of the dejure title if not dragging the mouse.
                            if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                                m_Menu.SwitchMapMode(TitleTypeToMapMode(dejure->GetType()), true);
                                m_Menu.GetSelectionHandler().Select(dejure);
                            }

                            // Reorder the dejure titles by dragging the mouse.
                            else if (ImGui::IsItemActive() && !ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped)) {
                                int nNext = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                                if (nNext >= 0 && nNext < dejureTitles.size()) {
                                    std::iter_swap(highTitle->GetDejureTitles().begin() + n, highTitle->GetDejureTitles().begin() + nNext);
                                    ImGui::ResetMouseDragDelta();
                                }
                            }

                            // First barony of a county is the capital.
                            if (dejure == highTitle->GetCapitalTitle() || (highTitle->Is(TitleType::COUNTY) && n == 0)) {
                                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 90);
                                ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "(Capital)");
                            }
                            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20);
                            if (ImGui::SmallButton("x")) {
                                highTitle->RemoveDejureTitle(dejure);

                                // Update the map to remove the dejure title from the title color.
                                // TODO: it would be better not having to redraw the entire map
                                // but only the relevant colors.
                                MapMode liegeMapMode = TitleTypeToMapMode(highTitle->GetType());
                                for (int i = (int)liegeMapMode; i <= (int)MapMode::HEGEMONY; i++) {
                                    m_Menu.UpdateTexture((MapMode)i, false);
                                }
                                m_Menu.SwitchMapMode(liegeMapMode, false);
                            }
                            ImGui::PopID();
                            n++;
                        }

                        ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "note: drag to change order.");

                        // HIGHTITLE: add new dejure title (button)
                        if (ImGui::SmallButton((m_SelectingTitle) ? "click on a title..." : "add") && !m_SelectingTitle) {
                            TitleType dejureType = static_cast<TitleType>(static_cast<int>(highTitle->GetType()) - 1);
                            MapMode liegeMapMode = TitleTypeToMapMode(highTitle->GetType());
                            m_SelectingTitle = true;
                            m_Menu.SwitchMapMode(TitleTypeToMapMode(dejureType), false);
                            m_Menu.GetSelectionHandler().AddCallback(
                                [this, highTitle, dejureType, liegeMapMode](sf::Mouse::Button button, Province* province, Title* clickedTitle) {
                                    if (button != sf::Mouse::Button::Left)
                                        goto StopSelecting;

                                    if (!clickedTitle->Is(dejureType))
                                        return SelectionCallbackResult::INTERRUPT;

                                    // Add the clicked title as a dejure vassals,
                                    // and then update the textures for all map modes above the clicked title type.
                                    highTitle->AddDejureTitle(clickedTitle);
                                    for (int i = static_cast<int>(liegeMapMode); i <= static_cast<int>(MapMode::HEGEMONY); i++) {
                                        m_Menu.UpdateTexture(static_cast<MapMode>(i), false);
									}

                                    // Keep selecting if the user is holding the left shift key, otherwise stop selecting.
                                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                                        m_Menu.SwitchMapMode(TitleTypeToMapMode(dejureType), false);
                                        return SelectionCallbackResult::INTERRUPT;
                                    }

                                    StopSelecting:
                                    m_Menu.SwitchMapMode(liegeMapMode, false);
                                    m_SelectingTitle = false;
                                    return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                                }
                            );
                        }
                        ImGui::EndChild();
                    }
                    else {
                        m_DisplayDejureTitles = false;
                    }

                    if (!title->Is(TitleType::COUNTY)) {

                        // HIGHTITLE: change capital county (button)
                        ImGui::NewLine();
                        if (ImGui::Button((m_SelectingTitle) ? "click on a title..." : "change capital county") && !m_SelectingTitle) {
                            m_SelectingTitle = true;
                            m_Menu.SwitchMapMode(MapMode::COUNTY, false);
                            m_Menu.GetSelectionHandler().AddCallback(
                                [this, highTitle](sf::Mouse::Button button, Province* province, Title* clickedTitle) {
                                    if (button != sf::Mouse::Button::Right)
                                        goto DeleteCallback;
                                    if (button != sf::Mouse::Button::Left)
                                        return SelectionCallbackResult::INTERRUPT;
                                    if (!clickedTitle->Is(TitleType::COUNTY))
                                        return SelectionCallbackResult::INTERRUPT;

                                    // Check if clicked title is a direct or undirect vassal of the title.
                                    if (!clickedTitle->IsVassal(highTitle))
                                        return SelectionCallbackResult::INTERRUPT;
                                    highTitle->SetCapitalTitle(static_cast<CountyTitle*>(clickedTitle));

                                DeleteCallback:
                                    m_Menu.SwitchMapMode(TitleTypeToMapMode(highTitle->GetType()), false);
                                    m_SelectingTitle = false;
                                    return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                                }
                            );
                        }
                    }

                }

                // TITLE: delete title (button)
                if (ImGui::Button("delete"))
                    ImGui::OpenPopup("Delete this title");

                // TITLE: Switch to liege (button)
                bool hasLiege = (!title->Is(TitleType::HEGEMONY) && title->GetLiegeTitle() != nullptr);
                if (!hasLiege) ImGui::BeginDisabled();
                if (ImGui::Button("switch to liege") && hasLiege) {
					Title* liege = title->GetLiegeTitle();
                    m_Menu.SwitchMapMode(TitleTypeToMapMode(liege->GetType()), true);
                    m_Menu.GetSelectionHandler().Select(liege);
                }
                if (!hasLiege) ImGui::EndDisabled();

                // TITLE: delete title (modal)
                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                if (ImGui::BeginPopupModal("Delete this title", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
                    ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
                    ImGui::Separator();

                    if (ImGui::Button("Delete", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();

                        // Remove the title from his liege's dejure titles.
                        if (!title->Is(TitleType::HEGEMONY) && title->GetLiegeTitle() != nullptr) {
                            HighTitle* liege = title->GetLiegeTitle();
                            liege->RemoveDejureTitle(title);
                        }

                        // Remove the title as the liege of all his dejure titles.
                        if (!title->Is(TitleType::BARONY)) {
                            HighTitle* highTitle = static_cast<HighTitle*>(title);
                            for (auto& dejure : highTitle->GetDejureTitles()) {
                                dejure->SetLiegeTitle(nullptr);
                            }
                        }

                        m_Mod.GetTitleManager().RemoveTitle(title);

                        m_Menu.UpdateTextures();
                        m_Menu.RefreshCurrentMapMode(true, false);
                    }

                    ImGui::SetItemDefaultFocus();
                    ImGui::SameLine();
                    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();
                ImGui::EndChild();
            }
        }
    }
    clipper.End();
}

void PropertiesTab::RenderRegions() {
    std::vector<Region*> regions = std::vector<Region*>(m_Menu.GetSelectionHandler().GetRegions().begin(), m_Menu.GetSelectionHandler().GetRegions().end());
    for (Region* region : regions) {
                
        if (ImGui::CollapsingHeader(region->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BeginChild(fmt::format("##region-{}", region->GetName()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
            ImGui::PushID(region->GetName().c_str());

            // REGION: name/tag (field)
            std::string formerName = region->GetName();
            if (ImGui::InputTextCommitOnEnter("name", &formerName)) {
                m_Mod.GetRegionManager().RenameRegion(region->GetName(), formerName);
            }
            
            // REGION: file name (field)
            std::string formerFileName = region->GetFileName();
            if (ImGui::InputTextCommitOnEnter("file name", &formerFileName)) {
                region->SetFileName(formerFileName);
            }

            // REGION: generate modifiers (checkbox)
            bool generateModifiers = region->DoesGenerateModifiers();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            if (ImGui::Checkbox("Generate Modifiers", &generateModifiers)) {
                region->SetGenerateModifiers(generateModifiers);
            }
            ImGui::PopStyleVar();
            
            // REGION: should remember counties order (checkbox)
            bool shouldRememberCountiesOrder = region->ShouldRememberCountiesOrder();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            if (ImGui::Checkbox("Should Remember Counties Order", &shouldRememberCountiesOrder)) {
                region->SetShouldRememberCountiesOrder(shouldRememberCountiesOrder);
            }
            ImGui::PopStyleVar();

            // REGION: titles (list)
            ImGui::SetNextItemOpen(m_DisplayRegionsTitles, ImGuiCond_Appearing);
            if (ImGui::CollapsingHeader("titles")) {
                m_DisplayRegionsTitles = true;

                ImGui::BeginChild("titles", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                const auto DisplayTitles = [&](auto titles) {
                    for (const auto& title : titles) {
                        ImGui::PushID(title->GetName().c_str());
                        ImGui::SetNextItemAllowOverlap();
                        ImGui::Selectable(title->GetName().c_str());

                        // Switch to the properties of the title if not dragging the mouse.
                        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                            m_Menu.SwitchMapMode(TitleTypeToMapMode(title->GetType()), true);
                            m_Menu.GetSelectionHandler().Select(title);
                        }

                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                        if (ImGui::SmallButton("x")) {
                            region->RemoveTitle(title);
                            m_Menu.GetSelectionHandler().Update();
                        }

                        ImGui::PopID();
                    }
                };

                ImGui::SeparatorText("Kingdoms");
                DisplayTitles(std::vector<KingdomTitle*>(region->GetKingdoms().begin(), region->GetKingdoms().end()));
                ImGui::SeparatorText("Duchies");
                DisplayTitles(std::vector<DuchyTitle*>(region->GetDuchies().begin(), region->GetDuchies().end()));
                ImGui::SeparatorText("Counties");
                DisplayTitles(std::vector<CountyTitle*>(region->GetCounties().begin(), region->GetCounties().end()));

                // REGION: add new title (combobox)
                if (ImGui::BeginCombo("##titles-combo", "Pick a title to add")) {
                    static ImGuiTextFilter filter;
                    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
                    filter.Draw("##Filter", -FLT_MIN);
                    if (ImGui::IsWindowAppearing())
                        ImGui::SetKeyboardFocusHere(-1);

                    for (const auto& [name, title] : m_Mod.GetTitleManager().GetTitles()) {
                        if (title->Is(TitleType::EMPIRE) || title->Is(TitleType::HEGEMONY) || title->Is(TitleType::BARONY))
                            continue;
                        if (!filter.PassFilter(name.c_str()))
                            continue;
                        if (ImGui::Selectable(name.c_str())) {
                            region->AddTitle(title.get());
                            m_Menu.GetSelectionHandler().Update();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                // REGION: add new title (button with callback)
                ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
                if (m_SelectingTitle) ImGui::BeginDisabled();
                if (ImGui::SmallButton("📌") && !m_SelectingTitle) {
                    m_SelectingTitle = true;

                    if (m_SelectingProvince) {
                        m_Menu.GetSelectionHandler().m_ProvinceCallbacks.pop_back();
                        m_SelectingProvince = false;
                    }

                    if (!MapModeIsTitle(m_Menu.GetMapMode()))
                        m_Menu.SwitchMapMode(MapMode::KINGDOM, false);

                    m_Menu.GetSelectionHandler().AddCallback(
                        [this, region](sf::Mouse::Button button, Province* clickedProvince, Title* clickedTitle) {
                            // Allow user to wrap a title using RMB.
                            if (button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::CONTINUE;
                            // Allow user to unwrap a title using LCtrl+LMB.
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                                return SelectionCallbackResult::CONTINUE;
                            region->AddTitle(clickedTitle);
                            m_Menu.GetSelectionHandler().Update();
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
                                return SelectionCallbackResult::INTERRUPT;
                            m_SelectingTitle = false;
                            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                        }
                    );
                }
                if (m_SelectingTitle) ImGui::EndDisabled();
                ImGui::PopFont();
                ImGui::EndChild();
            }
            else {
                m_DisplayRegionsTitles = false;
            }
            
            // REGION: provinces (list)
            ImGui::SetNextItemOpen(m_DisplayRegionsProvinces, ImGuiCond_Appearing);
            if (ImGui::CollapsingHeader("provinces")) {
                m_DisplayRegionsProvinces = true;

                ImGui::BeginChild("provinces", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                std::vector<Province*> provinces = std::vector<Province*>(region->GetProvinces().begin(), region->GetProvinces().end());
                for (Province* province : provinces) {
                    ImGui::PushID(province->GetName().c_str());
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Selectable(fmt::format("{}\t-\t{}", province->GetId(), province->GetName()).c_str());

                    // Switch to the properties of the province if not dragging the mouse.
                    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                        m_Menu.SwitchMapMode(MapMode::PROVINCES, true);
                        m_Menu.GetSelectionHandler().Select(province);
                    }

                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                    if (ImGui::SmallButton("x")) {
                        region->RemoveProvince(province);
                        m_Menu.GetSelectionHandler().Update();
                    }

                    ImGui::PopID();
                }

                // REGION: add new province (combobox)
                if (ImGui::BeginCombo("##provinces-combo", "Pick a province to add")) {
                    static ImGuiTextFilter filter;
                    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
                    filter.Draw("##Filter", -FLT_MIN);
                    if (ImGui::IsWindowAppearing())
                        ImGui::SetKeyboardFocusHere(-1);

                    for (const auto& [id, province] : m_Mod.GetProvinceManager().GetProvincesByIds()) {
                        if (!filter.PassFilter(std::to_string(id).c_str()) && !filter.PassFilter(province->GetName().c_str()))
                            continue;
                        if (ImGui::Selectable(fmt::format("{}\t-\t{}", id, province->GetName()).c_str())) {
                            region->AddProvince(province);
                            m_Menu.GetSelectionHandler().Update();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                // REGION: add new title (button with callback)
                ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
                if (m_SelectingProvince) ImGui::BeginDisabled();
                if (ImGui::SmallButton("📌") && !m_SelectingProvince) {
                    m_SelectingProvince = true;
                    m_Menu.SwitchMapMode(MapMode::PROVINCES, false);

                    if (m_SelectingTitle) {
                        m_Menu.GetSelectionHandler().m_TitleCallbacks.pop_back();
                        m_SelectingTitle = false;
                    }

                    m_Menu.GetSelectionHandler().AddCallback(
                        [this, region](sf::Mouse::Button button, Province* clickedProvince) {
                            if (button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::INTERRUPT;
                            region->AddProvince(clickedProvince);
                            m_Menu.GetSelectionHandler().Update();
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
                                return SelectionCallbackResult::INTERRUPT;
                            m_SelectingProvince = false;
                            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                        }
                    );
                }
                if (m_SelectingProvince) ImGui::EndDisabled();
                ImGui::PopFont();
                ImGui::EndChild();
            }
            else {
                m_DisplayRegionsProvinces = false;
            }
            
            // REGION: regions (list)
            ImGui::SetNextItemOpen(m_DisplayRegionsRegions, ImGuiCond_Appearing);
            if (ImGui::CollapsingHeader("regions")) {
                m_DisplayRegionsRegions = true;

                ImGui::BeginChild("regions", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                std::vector<Region*> regions = std::vector<Region*>(region->GetRegions().begin(), region->GetRegions().end());
                for (Region* subRegion : regions) {
                    ImGui::PushID(subRegion->GetName().c_str());
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Selectable(subRegion->GetName().c_str());

                    if (subRegion->HasRegion(region)) {
                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-50);
                        ImGui::PushFont(ImGui::notoSansNormalFont, FONT_SIZE_SMALL);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
                        ImGui::Text("⚠️");
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Geographical regions shouldn't have circular dependencies!");
                        ImGui::PopStyleColor();
                        ImGui::PopFont();
                    }

                    // Switch to the properties of the region if not dragging the mouse.
                    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                        m_Menu.GetSelectionHandler().Select(subRegion);
                    }

                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                    if (ImGui::SmallButton("x")) {
                        region->RemoveRegion(subRegion);
                        m_Menu.GetSelectionHandler().Update();
                    }

                    ImGui::PopID();
                }

                // REGION: add new region (combobox)
                if (ImGui::BeginCombo("##regions-combo", "Pick a region to add")) {
                    static ImGuiTextFilter filter;
                    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
                    filter.Draw("##Filter", -FLT_MIN);
                    if (ImGui::IsWindowAppearing())
                        ImGui::SetKeyboardFocusHere(-1);

                    for (const auto& [name, subRegion] : m_Mod.GetRegionManager().GetRegions()) {
                        if (!filter.PassFilter(name.c_str()))
                            continue;
                        if (ImGui::Selectable(name.c_str())) {
                            region->AddRegion(subRegion.get());
                            m_Menu.GetSelectionHandler().Update();
                        }
                    }
                    ImGui::EndCombo();
                }

                ImGui::EndChild();
            }
            else {
                m_DisplayRegionsRegions = false;
            }

            // REGION: delete region (button)
            if (ImGui::Button("delete"))
                ImGui::OpenPopup("Delete this geographical region");

            // REGION: delete region (modal)
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Delete this geographical region", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "This action cannot be undone!");
                ImGui::Separator();

                if (ImGui::Button("Delete", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();

                    m_Mod.GetRegionManager().RemoveRegion(region);
                    m_Menu.GetSelectionHandler().Deselect(region);
                    m_Menu.GetSelectionHandler().Update();
                }

                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
            ImGui::EndChild();
        }

    }
}