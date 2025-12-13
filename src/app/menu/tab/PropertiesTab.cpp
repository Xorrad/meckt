#include "PropertiesTab.hpp"
#include "app/menu/EditorMenu.hpp"
#include "app/menu/selection/SelectionHandler.hpp"

#include "app/App.hpp"
#include "app/mod/Mod.hpp"
#include "app/map/Province.hpp"
#include "app/map/Region.hpp"
#include "app/map/Title.hpp"

#include "imgui/imgui.hpp"
#include "app/menu/ImGuiStyle.hpp"

PropertiesTab::PropertiesTab(EditorMenu* menu, bool visible) :
    Tab("Properties", Tabs::PROPERTIES, menu, visible),
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
    m_SelectingTitleText.setCharacterSize(24);
    m_SelectingTitleText.setString("Click on a title.");
    m_SelectingTitleText.setFillColor(sf::Color::Red);
    m_SelectingTitleText.setFont(Configuration::fonts.Get(Fonts::NOTO_SANS));
    m_SelectingTitleText.setPosition({10, 20});

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
    if ((m_SelectingTitle || m_SelectingProvince) && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        if (m_SelectingTitle) m_Menu->GetSelectionHandler().m_TitleCallbacks.pop_back();
        if (m_SelectingProvince) m_Menu->GetSelectionHandler().m_ProvinceCallbacks.pop_back();
        m_SelectingTitle = false;
        m_SelectingProvince = false;
    }
}

void PropertiesTab::Render() {
    if (!m_Visible)
        return;

    if (m_SelectingTitle || m_SelectingProvince) {
        // Draw a red outline around the view of the map.
        ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(m_Menu->GetDockspaceID());
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

        m_SelectingTitleText.setPosition({node->Pos.x + 10, node->Pos.y + 10});
        m_Menu->GetApp()->GetWindow().draw(m_SelectingTitleText);
    }

    if (m_Menu->GetSelectionHandler().GetProvinces().size() > 0) {
        if (m_Menu->GetSelectionHandler().GetProvinces().size() > 1) {
            this->RenderJointProvinces();
        }
        this->RenderProvinces();
    }
    else if (m_Menu->GetSelectionHandler().GetTitles().size() > 0) {
        this->RenderTitles();
    }
    else if (m_Menu->GetSelectionHandler().GetRegions().size() > 0) {
        this->RenderRegions();
    }
    
}

void PropertiesTab::RenderJointProvinces() {
    // Determine values based on selected provinces and
    // whether several provinces have different values for a property.
    const SharedPtr<Province>& firstProvince = m_Menu->GetSelectionHandler().GetProvinces().front();

    std::string culture = firstProvince->GetCulture();
    std::string religion = firstProvince->GetReligion();
    std::string holding = firstProvince->GetHolding();
    std::string terrain = firstProvince->GetTerrain();

    ClimateType climateType = firstProvince->GetClimateType();
    std::string winterSeverityBias = firstProvince->GetWinterSeverityBias();
    std::string mildWinterFactorOverride = firstProvince->GetMildWinterFactorOverride();
    std::string normalWinterFactorOverride = firstProvince->GetNormalWinterFactorOverride();
    std::string harshWinterFactorOverride = firstProvince->GetHarshWinterFactorOverride();

    int isCoastal = firstProvince->HasFlag(ProvinceFlags::COASTAL);
    int isLake = firstProvince->HasFlag(ProvinceFlags::LAKE);
    int isIsland = firstProvince->HasFlag(ProvinceFlags::ISLAND);
    int isLand = firstProvince->HasFlag(ProvinceFlags::LAND);
    int isSea = firstProvince->HasFlag(ProvinceFlags::SEA);
    int isRiver = firstProvince->HasFlag(ProvinceFlags::RIVER);
    int isImpassable = firstProvince->HasFlag(ProvinceFlags::IMPASSABLE);

    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
        if (province->GetCulture() != culture) culture = "*****";
        if (province->GetReligion() != religion) religion = "*****";
        if (province->GetHolding() != holding) holding = "*****";
        if (province->GetTerrain() != terrain) terrain = "*****";
        if (province->GetClimateType() != climateType) climateType = ClimateType::COUNT;
        if (province->GetWinterSeverityBias() != winterSeverityBias) winterSeverityBias = "*****";
        if (province->GetMildWinterFactorOverride() != mildWinterFactorOverride) mildWinterFactorOverride = "*****";
        if (province->GetNormalWinterFactorOverride() != normalWinterFactorOverride) normalWinterFactorOverride = "*****";
        if (province->GetHarshWinterFactorOverride() != harshWinterFactorOverride) harshWinterFactorOverride = "*****";
        if (province->HasFlag(ProvinceFlags::COASTAL) != isCoastal) isCoastal = -1;
        if (province->HasFlag(ProvinceFlags::LAKE) != isLake) isLake = -1;
        if (province->HasFlag(ProvinceFlags::ISLAND) != isIsland) isIsland = -1;
        if (province->HasFlag(ProvinceFlags::LAND) != isLand) isLand = -1;
        if (province->HasFlag(ProvinceFlags::SEA) != isSea) isSea = -1;
        if (province->HasFlag(ProvinceFlags::RIVER) != isRiver) isRiver = -1;
        if (province->HasFlag(ProvinceFlags::IMPASSABLE) != isImpassable) isImpassable = -1;
    }

    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(3, 48, 102, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(0, 57, 106, 255));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(2, 45, 86, 255));
    if (ImGui::CollapsingHeader("global", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginChild("##joint-provinces", ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
        ImGui::PushID("joint-provinces");

        // PROVINCE: terrain (combobox)
        if (ImGui::BeginCombo("terrain type", terrain.c_str())) {
            for (const auto& [newTerrain, _] : m_Menu->GetApp()->GetMod()->GetTerrainTypes()) {
                const bool isSelected = (terrain == newTerrain);
                if (ImGui::Selectable(newTerrain.c_str(), isSelected)) {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                        province->SetTerrain(newTerrain);
                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        
        // PROVINCE: flags (checkbox)
        #define UPDATE_FLAG(flag, var) \
            for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) { \
                province->SetFlag(ProvinceFlags::flag, var); \
            }

        if (ImGui::BeginTable("province flags", 2)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::BeginDisabled();
            if (ImGui::CheckBoxTristate("Coastal", &isCoastal)) UPDATE_FLAG(COASTAL, isCoastal);
            ImGui::EndDisabled();
            ImGui::TableSetColumnIndex(1);
            if (ImGui::CheckBoxTristate("Lake", &isLake)) UPDATE_FLAG(LAKE, isLake);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::BeginDisabled();
            if (ImGui::CheckBoxTristate("Island", &isIsland)) UPDATE_FLAG(ISLAND, isIsland);
            ImGui::EndDisabled();
            ImGui::TableSetColumnIndex(1);
            if (ImGui::CheckBoxTristate("Land", &isLand)) UPDATE_FLAG(LAND, isLand);
            
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (ImGui::CheckBoxTristate("Sea", &isSea)) UPDATE_FLAG(SEA, isSea);
            ImGui::TableSetColumnIndex(1);
            if (ImGui::CheckBoxTristate("River", &isRiver)) UPDATE_FLAG(RIVER, isRiver);
            
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            if (ImGui::CheckBoxTristate("Impassable", &isImpassable)) UPDATE_FLAG(IMPASSABLE, isImpassable);

            ImGui::EndTable();
        }

        // PROVINCE: culture (field)
        if (ImGui::InputText("culture", &culture)) {
            for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                province->SetCulture(culture);
            }
        }

        // PROVINCE: religion (field)
        if (ImGui::InputText("religion", &religion)) {
            for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                province->SetReligion(culture);
            }
        }

        // PROVINCE: holding type (combobox)
        if (ImGui::BeginCombo("holding", holding.c_str())) {
            for (const auto& [newHolding, _] : m_Menu->GetApp()->GetMod()->GetHoldingTypes()) {
                const bool isSelected = (holding == newHolding);
                if (ImGui::Selectable(newHolding.c_str(), isSelected)) {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                        province->SetHolding(newHolding);
                    }
                }
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // PROVINCE: climate (collapsing header + child window (for borders) + text inputs)
        ImGui::SetNextItemOpen(m_DisplayClimate, ImGuiCond_Appearing);
        if (ImGui::CollapsingHeader("climate")) {
            m_DisplayClimate = true;
            
            if (ImGui::BeginChild("climate", ImVec2(0, 175), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                ImGui::SetNextItemWidth(0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("climate").x - 10);
                if (ImGui::BeginCombo("climate", ClimateTypeLabels.at(climateType))) {
                    for (int i = 0; i < (int) ClimateType::COUNT; i++) {
                        ClimateType type = (ClimateType) i;
                        const bool isSelected = (type == climateType);
                        if (ImGui::Selectable(ClimateTypeLabels.at(type), isSelected)) {
                            for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                                province->SetClimateType(type);
                            }
                        }
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                
                // Use the same width for all items below so they are aligned.
                int width = 0.9f * ImGui::GetWindowWidth() - ImGui::CalcTextSize("normal winter factor override").x;

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputText("winter severity bias", &winterSeverityBias)) {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                        province->SetWinterSeverityBias(winterSeverityBias);
                    }
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputText("mild winter factor override", &mildWinterFactorOverride)) {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                        province->SetMildWinterFactorOverride(mildWinterFactorOverride);
                    }
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputText("normal winter factor override", &normalWinterFactorOverride))  {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                        province->SetNormalWinterFactorOverride(normalWinterFactorOverride);
                    }
                }

                ImGui::SetNextItemWidth(width);
                if (ImGui::InputText("harsh winter factor override", &harshWinterFactorOverride)) {
                    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
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
    for (auto& province : m_Menu->GetSelectionHandler().GetProvinces()) {
                
        if (ImGui::CollapsingHeader(fmt::format("#{} ({})", province->GetId(), province->GetName()).c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BeginChild(fmt::format("##province-{}", province->GetId()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
            ImGui::PushID(province->GetId());                        

            // PROVINCE: id (field)
            ImGui::BeginDisabled();
            std::string id = std::to_string(province->GetId());
            ImGui::InputText("id", &id);
            ImGui::EndDisabled();

            // PROVINCE: name (field)
            ImGui::InputText("name", &province->m_Name);

            // PROVINCE: color (colorpicker)
            sf::Color color = province->GetColor();
            ImGui::BeginDisabled();
            if (ImGui::ColorEdit3("color", &color)) {
                // TODO: error if color is already taken by another province.
                // TODO: change pixels color in provinces.png.
                province->SetColor(color);
            }
            ImGui::EndDisabled();

            // PROVINCE: terrain (combobox)
            if (ImGui::BeginCombo("terrain type", province->GetTerrain().c_str())) {
                for (const auto& [terrain, _] : m_Menu->GetApp()->GetMod()->GetTerrainTypes()) {
                    const bool isSelected = (province->GetTerrain() == terrain);
                    if (ImGui::Selectable(terrain.c_str(), isSelected))
                        province->SetTerrain(terrain);

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            // PROVINCE: flags (checkbox)
            bool isCoastal = province->HasFlag(ProvinceFlags::COASTAL);
            bool isLake = province->HasFlag(ProvinceFlags::LAKE);
            bool isIsland = province->HasFlag(ProvinceFlags::ISLAND);
            bool isLand = province->HasFlag(ProvinceFlags::LAND);
            bool isSea = province->HasFlag(ProvinceFlags::SEA);
            bool isRiver = province->HasFlag(ProvinceFlags::RIVER);
            bool isImpassable = province->HasFlag(ProvinceFlags::IMPASSABLE);

            if (ImGui::BeginTable("province flags", 2)) {
                ImGui::TableNextRow();
                
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginDisabled();
                if (ImGui::Checkbox("Coastal", &isCoastal)) province->SetFlag(ProvinceFlags::COASTAL, isCoastal);
                ImGui::EndDisabled();
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Checkbox("Lake", &isLake)) province->SetFlag(ProvinceFlags::LAKE, isLake);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::BeginDisabled();
                if (ImGui::Checkbox("Island", &isIsland)) province->SetFlag(ProvinceFlags::ISLAND, isIsland);
                ImGui::EndDisabled();
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Checkbox("Land", &isLand)) province->SetFlag(ProvinceFlags::LAND, isLand);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Checkbox("Sea", &isSea)) province->SetFlag(ProvinceFlags::SEA, isSea);
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Checkbox("River", &isRiver)) province->SetFlag(ProvinceFlags::RIVER, isRiver);
                
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (ImGui::Checkbox("Impassable", &isImpassable)) province->SetFlag(ProvinceFlags::IMPASSABLE, isImpassable);

                ImGui::EndTable();
            }

            // PROVINCE: culture (field)
            ImGui::InputText("culture", &province->m_Culture);

            // PROVINCE: religion (field)
            ImGui::InputText("religion", &province->m_Religion);

            // PROVINCE: holding type (combobox)
            if (ImGui::BeginCombo("holding", province->GetHolding().c_str())) {
                for (const auto& [holding, _] : m_Menu->GetApp()->GetMod()->GetHoldingTypes()) {
                    const bool isSelected = (province->GetHolding() == holding);
                    if (ImGui::Selectable(holding.c_str(), isSelected))
                        province->SetHolding(holding);
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            // PROVINCE: history (collapsing header + child window (for borders) + collapsing header for each dates)
            ImGui::SetNextItemOpen(m_DisplayHistory);
            if (ImGui::CollapsingHeader("history")) {
                m_DisplayHistory = true;

                if (ImGui::BeginChild((province->GetName() + "-history").c_str(), ImVec2(0, 250), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

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

                    for (auto const& [date, data] : province->GetHistory() | std::views::reverse) {
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
                
                if (ImGui::BeginChild((province->GetName() + "-climate").c_str(), ImVec2(0, 175), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

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
                    ImGui::InputText("winter severity bias", &province->m_WinterSeverityBias);

                    ImGui::SetNextItemWidth(width);
                    ImGui::InputText("mild winter factor override", &province->m_MildWinterFactorOverride);

                    ImGui::SetNextItemWidth(width);
                    ImGui::InputText("normal winter factor override", &province->m_NormalWinterFactorOverride);

                    ImGui::SetNextItemWidth(width);
                    ImGui::InputText("harsh winter factor override", &province->m_HarshWinterFactorOverride);

                    ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "note: leave fields empty if you don't want any value.");
                }
                ImGui::EndChild();
            }
            else {
                m_DisplayClimate = false;
            }

            // PROVINCE: switch to barony (button)
            if (m_Menu->GetApp()->GetMod()->GetBaroniesByProvinceIds().count(province->GetId()) > 0) {
                if (ImGui::Button("switch to barony")) {
                    const SharedPtr<Title>& title = m_Menu->GetApp()->GetMod()->GetBaroniesByProvinceIds()[province->GetId()];
                    m_Menu->SwitchMapMode(MapMode::BARONY, true);
                    m_Menu->GetSelectionHandler().Select(title);
                }
            }
            // PROVINCE: create barony (button)
            else if (province->HasFlag(ProvinceFlags::LAND) && !province->HasFlag(ProvinceFlags::IMPASSABLE)) {
                if (ImGui::Button("create barony title")) {
                    // Make sure to use a title name that isn't already taken.
                    std::string baronyName = "b_" + String::ToLowercase(province->GetName());
                    int i = 1;
                    while(m_Menu->GetApp()->GetMod()->GetTitles().count(baronyName) > 0) {
                        baronyName = "b_" + String::ToLowercase(province->GetName()) + std::to_string(i);
                        i++;
                    }

                    SharedPtr<Title> title = MakeTitle(TitleType::BARONY, baronyName, province->GetColor(), false);
                    SharedPtr<BaronyTitle> baronyTitle = CastSharedPtr<BaronyTitle>(title);
                    baronyTitle->SetProvinceId(province->GetId());

                    m_Menu->GetApp()->GetMod()->AddTitle(title);
                }
            }

            ImGui::PopID();
            ImGui::EndChild();
        }
    }
}

void PropertiesTab::RenderTitles() {
    for (auto& title : m_Menu->GetSelectionHandler().GetTitles()) {
                
        if (ImGui::CollapsingHeader(title->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BeginChild(fmt::format("##title-{}", title->GetName()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
            ImGui::PushID(title->GetName().c_str());

            // TITLE: name/tag (field)
            std::string formerName = title->m_Name;
            if (ImGui::InputText("name", &title->m_Name)) {
                // Rename the title globally, including titles history.
                m_Menu->GetApp()->GetMod()->RenameTitle(title, formerName);
            }
            
            // TITLE: localization name (field)
            ImGui::InputText("loc. name", &title->GetLocName("english"));
            
            // TITLE: localization adjective (field)
            ImGui::InputText("loc. adjective", &title->GetLocAdjective("english"));
            
            // TITLE: localization article (field)
            ImGui::InputText("loc. article", &title->GetLocArticle("english"));

            // TITLE: tier/type (combo)
            ImGui::BeginDisabled();
            if (ImGui::BeginCombo("type", TitleTypeLabels[(int) title->GetType()]))
                ImGui::EndCombo();
            ImGui::EndDisabled();

            // TITLE: color (colorpicker)
            sf::Color color = title->GetColor();
            if (ImGui::ColorEdit3("color", &color)) {
                title->SetColor(color);
                m_Menu->RefreshMapMode(false);
                m_Menu->GetSelectionHandler().Update();
            }

            // TITLE: landless (checkbox)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Landless", &title->m_Landless);
            ImGui::PopStyleVar();

            // TITLE: cultural names (collapsing header + child window (for borders) + collapsing header for each culture)
            ImGui::SetNextItemOpen(m_DisplayCulturalNames);
            if (ImGui::CollapsingHeader("cultural names")) {
                m_DisplayCulturalNames = true;
                
                if (ImGui::BeginChild((title->GetName() + "-cultural-names").c_str(), ImVec2(0, 100), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

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
            ImGui::SetNextItemOpen(m_DisplayHistory);
            if (ImGui::CollapsingHeader("history")) {
                m_DisplayHistory = true;

                if (ImGui::BeginChild((title->GetName() + "-history").c_str(), ImVec2(0, 250), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None)) {

                    static std::string date = "";
                    static bool isDateValid = true;

                    const auto& AddNewDate = [&]() {
                        try {
                            Jomini::Date newDate = Jomini::Date(date);
                            if (!title->GetHistory().contains(newDate))
                                title->AddHistory(newDate, MakeShared<Jomini::Object>(Jomini::ObjectMap{}));
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

                    for (auto const& [date, data] : title->GetHistory() | std::views::reverse) {
                        std::string stateKey = fmt::format("{}-{}", title->GetName(), date);

                        ImGui::SetNextItemAllowOverlap();
                        if (ImGui::TreeNodeEx(fmt::format("{}", date).c_str(), ImGuiTreeNodeFlags_SpanFullWidth)) {
                            ImGui::PushID(stateKey.c_str());
                            
                            // Insert the delete button on the smae line as the tree node.
                            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
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
                            if (ImGui::InputTextMultiline("data", &historyStates[stateKey].rawData, ImVec2(0,0), ImGuiInputTextFlags_AllowTabInput)) {
                                try {
                                    SharedPtr<Jomini::Object> newData = Jomini::ParseString(historyStates[stateKey].rawData);
                                    historyStates[stateKey].parsingError = "";
                                    title->AddHistory(date, newData);
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
                const SharedPtr<BaronyTitle>& barony = CastSharedPtr<BaronyTitle>(title);
                if (ImGui::InputInt("province id", &barony->m_ProvinceId)) {
                    if (m_Menu->GetApp()->GetMod()->GetProvincesByIds().count(barony->m_ProvinceId) == 0) {
                        LOG_ERROR("Barony with undefined province id: {},{}", barony->GetName(), barony->GetProvinceId());
                    }
                }

                // BARONY: province id (field)
                ImGui::NewLine();
                if (ImGui::Button((m_SelectingTitle) ? "click on a province..." : "change province") && !m_SelectingTitle) {
                    m_SelectingTitle = true;
                    MapMode previousMapMode = m_Menu->GetMapMode();
                    m_Menu->SwitchMapMode(MapMode::PROVINCES, false);
                    m_Menu->GetSelectionHandler().AddCallback(
                        [this, barony, previousMapMode](sf::Mouse::Button button, SharedPtr<Province> province) {
                            if (button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::INTERRUPT;
                            
                            barony->SetProvinceId(province->GetId());
                            m_Menu->GetApp()->GetMod()->GetBaroniesByProvinceIds()[barony->GetProvinceId()] = barony;

                            m_Menu->SwitchMapMode(previousMapMode, false);
                            m_SelectingTitle = false;
                            return SelectionCallbackResult::INTERRUPT | SelectionCallbackResult::DELETE_CALLBACK;
                        }
                    );
                }

                // BARONY: Switch to province (button)
                if (ImGui::Button("switch to province")) {
                    if (m_Menu->GetApp()->GetMod()->GetProvincesByIds().count(barony->GetProvinceId()) > 0) {
                        const SharedPtr<Province>& province = m_Menu->GetApp()->GetMod()->GetProvincesByIds()[barony->GetProvinceId()];
                        m_Menu->SwitchMapMode(MapMode::PROVINCES, true);
                        m_Menu->GetSelectionHandler().Select(province);
                    }
                }
            }
            else {
                const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);

                // HIGHTITLE: dejure titles (list)
                ImGui::SetNextItemOpen(m_DisplayDejureTitles);
                if (ImGui::CollapsingHeader("dejure titles")) {
                    m_DisplayDejureTitles = true;

                    ImGui::BeginChild("dejure titles", ImVec2(0, 250), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_None);

                    if (ImGui::BeginMenuBar()) {
                        if (ImGui::BeginMenu("dejure titles")) {
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenuBar();
                    }

                    // Make a copy to be able to use highTitle->RemoveDejureTitle
                    // without causing a crash while iterating.
                    std::vector<SharedPtr<Title>> dejureTitles = highTitle->GetDejureTitles();
                    int n = 0;
                    for (auto const& dejure : dejureTitles) {
                        ImGui::PushID(dejure->GetName().c_str());
                        ImGui::SetNextItemAllowOverlap();
                        ImGui::Selectable(dejure->GetName().c_str());

                        // Switch to the properties of the dejure title if not dragging the mouse.
                        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                            m_Menu->SwitchMapMode(TitleTypeToMapMode(dejure->GetType()), true);
                            m_Menu->GetSelectionHandler().Select(dejure);
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
                            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-90);
                            ImGui::TextColored(ImVec4(1.f, 0.6f, 0.f, 1.f), "(Capital)");
                        }
                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                        if (ImGui::SmallButton("x")) {
                            highTitle->RemoveDejureTitle(dejure);

                            // Update the map to remove the dejure title from the title color.
                            // TODO: it would be better not having to redraw the entire map
                            // but only the relevant colors.
                            MapMode liegeMapMode = TitleTypeToMapMode(highTitle->GetType());
                            m_Menu->UpdateTexture(liegeMapMode, false);
                            m_Menu->SwitchMapMode(liegeMapMode, false);
                        }
                        ImGui::PopID();
                        n++;
                    }

                    ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "note: drag to change order.");

                    // HIGHTITLE: add new dejure title (button)
                    if (ImGui::SmallButton((m_SelectingTitle) ? "click on a title..." : "add") && !m_SelectingTitle) {
                        TitleType dejureType = (TitleType)(((int) highTitle->GetType())-1);
                        m_SelectingTitle = true;
                        m_Menu->SwitchMapMode(TitleTypeToMapMode(dejureType), false);
                        m_Menu->GetSelectionHandler().AddCallback(
                            [this, highTitle, dejureType](sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> clickedTitle) {
                                if (button != sf::Mouse::Button::Left)
                                    return SelectionCallbackResult::INTERRUPT;
                                if (!clickedTitle->Is(dejureType))
                                    return SelectionCallbackResult::INTERRUPT;
                                highTitle->AddDejureTitle(clickedTitle);
                                MapMode liegeMapMode = TitleTypeToMapMode(highTitle->GetType());
                                m_Menu->UpdateTexture(liegeMapMode, false);
                                m_Menu->SwitchMapMode(liegeMapMode, false);
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
                    const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
                    
                    // HIGHTITLE: change capital county (button)
                    ImGui::NewLine();
                    if (ImGui::Button((m_SelectingTitle) ? "click on a title..." : "change capital county") && !m_SelectingTitle) {
                        m_SelectingTitle = true;
                        m_Menu->SwitchMapMode(MapMode::COUNTY, false);
                        m_Menu->GetSelectionHandler().AddCallback(
                            [this, highTitle](sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> clickedTitle) {
                                if (button != sf::Mouse::Button::Right)
                                    goto DeleteCallback;
                                if (button != sf::Mouse::Button::Left)
                                    return SelectionCallbackResult::INTERRUPT;
                                if (!clickedTitle->Is(TitleType::COUNTY))
                                    return SelectionCallbackResult::INTERRUPT;
                                
                                // Check if clicked title is a direct or undirect vassal of the title.
                                if (!clickedTitle->IsVassal(highTitle))
                                    return SelectionCallbackResult::INTERRUPT;
                                highTitle->SetCapitalTitle(CastSharedPtr<CountyTitle>(clickedTitle));
                                
                                DeleteCallback:
                                m_Menu->SwitchMapMode(TitleTypeToMapMode(highTitle->GetType()), false);
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
                        const SharedPtr<HighTitle>& liege = title->GetLiegeTitle();
                        liege->RemoveDejureTitle(title);
                    }

                    // Remove the title as the liege of all his dejure titles.
                    if (!title->Is(TitleType::BARONY)) {
                        const SharedPtr<HighTitle>& highTitle = CastSharedPtr<HighTitle>(title);
                        for (auto& dejure : highTitle->GetDejureTitles()) {
                            dejure->SetLiegeTitle(nullptr);
                        }
                    }

                    const SharedPtr<Mod>& mod = m_Menu->GetApp()->GetMod();
                    mod->RemoveTitle(title);

                    m_Menu->UpdateTextures();
                    m_Menu->RefreshMapMode(true, false);
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

void PropertiesTab::RenderRegions() {
    const SharedPtr<Mod>& mod = m_Menu->GetApp()->GetMod();

    std::vector<SharedPtr<Region>> selectedRegions = m_Menu->GetSelectionHandler().GetRegions();
    for (auto& region : selectedRegions) {
                
        if (ImGui::CollapsingHeader(region->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BeginChild(fmt::format("##region-{}", region->GetName()).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);
            ImGui::PushID(region->GetName().c_str());

            // REGION: name/tag (field)
            std::string formerName = region->m_Name;
            if (ImGui::InputText("name", &region->m_Name)) {
                m_Menu->GetApp()->GetMod()->RenameRegion(region, formerName);
            }

            // REGION: generate modifiers (checkbox)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            ImGui::Checkbox("Generate Modifiers", &region->m_GenerateModifiers);
            ImGui::PopStyleVar();

            // REGION: titles (list)
            ImGui::SetNextItemOpen(m_DisplayRegionsTitles);
            if (ImGui::CollapsingHeader("titles")) {
                m_DisplayRegionsTitles = true;

                ImGui::BeginChild("titles", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                const auto DisplayTitles = [&](auto titles) {
                    for (auto const& title : titles) {
                        ImGui::PushID(title->GetName().c_str());
                        ImGui::SetNextItemAllowOverlap();
                        ImGui::Selectable(title->GetName().c_str());

                        // Switch to the properties of the title if not dragging the mouse.
                        if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                            m_Menu->SwitchMapMode(TitleTypeToMapMode(title->GetType()), true);
                            m_Menu->GetSelectionHandler().Select(title);
                        }

                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                        if (ImGui::SmallButton("x")) {
                            region->RemoveTitle(title);
                            m_Menu->GetSelectionHandler().Update();
                        }

                        ImGui::PopID();
                    }
                };

                ImGui::SeparatorText("Kingdoms");
                DisplayTitles(std::vector<SharedPtr<KingdomTitle>>(region->GetKingdoms()));
                ImGui::SeparatorText("Duchies");
                DisplayTitles(std::vector<SharedPtr<DuchyTitle>>(region->GetDuchies()));
                ImGui::SeparatorText("Counties");
                DisplayTitles(std::vector<SharedPtr<CountyTitle>>(region->GetCounties()));

                // REGION: add new title (combobox)
                if (ImGui::BeginCombo("##titles-combo", "Pick a title to add")) {
                    static ImGuiTextFilter filter;
                    ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
                    filter.Draw("##Filter", -FLT_MIN);
                    if (ImGui::IsWindowAppearing())
                        ImGui::SetKeyboardFocusHere(-1);

                    for (auto& [name, title] : mod->GetTitles()) {
                        if (title->Is(TitleType::EMPIRE) || title->Is(TitleType::HEGEMONY) || title->Is(TitleType::BARONY))
                            continue;
                        if (!filter.PassFilter(name.c_str()))
                            continue;
                        if (ImGui::Selectable(name.c_str())) {
                            region->AddTitle(title);
                            m_Menu->GetSelectionHandler().Update();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                // REGION: add new title (button with callback)
                ImGui::PushFont(ImGui::notoSansNormalFont);
                if (m_SelectingTitle) ImGui::BeginDisabled();
                if (ImGui::SmallButton("📌") && !m_SelectingTitle) {
                    m_SelectingTitle = true;

                    if (m_SelectingProvince) {
                        m_Menu->GetSelectionHandler().m_ProvinceCallbacks.pop_back();
                        m_SelectingProvince = false;
                    }

                    if (!MapModeIsTitle(m_Menu->GetMapMode()))
                        m_Menu->SwitchMapMode(MapMode::KINGDOM, false);

                    m_Menu->GetSelectionHandler().AddCallback(
                        [this, region](sf::Mouse::Button button, SharedPtr<Province> clickedProvince, SharedPtr<Title> clickedTitle) {
                            // Allow user to wrap a title using RMB.
                            if (button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::CONTINUE;
                            // Allow user to unwrap a title using LCtrl+LMB.
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                                return SelectionCallbackResult::CONTINUE;
                            region->AddTitle(clickedTitle);
                            m_Menu->GetSelectionHandler().Update();
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
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
            ImGui::SetNextItemOpen(m_DisplayRegionsProvinces);
            if (ImGui::CollapsingHeader("provinces")) {
                m_DisplayRegionsProvinces = true;

                ImGui::BeginChild("provinces", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                std::vector<SharedPtr<Province>> provinces = region->GetProvinces();
                for (auto const& province : provinces) {
                    ImGui::PushID(province->GetName().c_str());
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Selectable(fmt::format("{}\t-\t{}", province->GetId(), province->GetName()).c_str());

                    // Switch to the properties of the province if not dragging the mouse.
                    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                        m_Menu->SwitchMapMode(MapMode::PROVINCES, true);
                        m_Menu->GetSelectionHandler().Select(province);
                    }

                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                    if (ImGui::SmallButton("x")) {
                        region->RemoveProvince(province);
                        m_Menu->GetSelectionHandler().Update();
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

                    for (auto& [id, province] : mod->GetProvincesByIds()) {
                        if (!filter.PassFilter(std::to_string(id).c_str()) && !filter.PassFilter(province->GetName().c_str()))
                            continue;
                        if (ImGui::Selectable(fmt::format("{}\t-\t{}", id, province->GetName()).c_str())) {
                            region->AddProvince(province);
                            m_Menu->GetSelectionHandler().Update();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();

                // REGION: add new title (button with callback)
                ImGui::PushFont(ImGui::notoSansNormalFont);
                if (m_SelectingProvince) ImGui::BeginDisabled();
                if (ImGui::SmallButton("📌") && !m_SelectingProvince) {
                    m_SelectingProvince = true;
                    m_Menu->SwitchMapMode(MapMode::PROVINCES, false);

                    if (m_SelectingTitle) {
                        m_Menu->GetSelectionHandler().m_TitleCallbacks.pop_back();
                        m_SelectingTitle = false;
                    }

                    m_Menu->GetSelectionHandler().AddCallback(
                        [this, region](sf::Mouse::Button button, SharedPtr<Province> clickedProvince) {
                            if (button != sf::Mouse::Button::Left)
                                return SelectionCallbackResult::INTERRUPT;
                            region->AddProvince(clickedProvince);
                            m_Menu->GetSelectionHandler().Update();
                            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
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
            ImGui::SetNextItemOpen(m_DisplayRegionsRegions);
            if (ImGui::CollapsingHeader("regions")) {
                m_DisplayRegionsRegions = true;

                ImGui::BeginChild("regions", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_None);

                std::vector<SharedPtr<Region>> regions = region->GetRegions();
                for (auto subRegion : regions) {
                    ImGui::PushID(subRegion->GetName().c_str());
                    ImGui::SetNextItemAllowOverlap();
                    ImGui::Selectable(subRegion->GetName().c_str());

                    if (subRegion->HasRegion(region)) {
                        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-50);
                        ImGui::PushFont(ImGui::notoSansNormalFont);
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
                        ImGui::Text("⚠️");
                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip("Geographical regions shouldn't have circular dependencies!");
                        ImGui::PopStyleColor();
                        ImGui::PopFont();
                    }

                    // Switch to the properties of the region if not dragging the mouse.
                    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0)) {
                        m_Menu->GetSelectionHandler().Select(subRegion);
                    }

                    ImGui::SameLine(ImGui::GetWindowContentRegionMax().x-20);
                    if (ImGui::SmallButton("x")) {
                        region->RemoveRegion(subRegion);
                        m_Menu->GetSelectionHandler().Update();
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

                    for (auto& [name, subRegion] : mod->GetRegions()) {
                        if (!filter.PassFilter(name.c_str()))
                            continue;
                        if (ImGui::Selectable(name.c_str())) {
                            region->AddRegion(subRegion);
                            m_Menu->GetSelectionHandler().Update();
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

                    mod->RemoveRegion(region);
                    m_Menu->GetSelectionHandler().Deselect(region);
                    m_Menu->GetSelectionHandler().Update();
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