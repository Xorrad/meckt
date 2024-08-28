#include "Title.hpp"
#include "parser/Parser.hpp"

Title::Title() : Title("", sf::Color(0, 0, 0)) {}

Title::Title(std::string name, sf::Color color, bool landless) :
    m_Name(name),
    m_Color(color),
    m_Landless(landless),
    m_SelectionFocus(true)
{}

// Title::Title(const Title& title) : Title(title.GetName(), title.GetColor()) {}

std::string Title::GetName() const {
    return m_Name;
}

sf::Color Title::GetColor() const {
    return m_Color;
}

SharedPtr<HighTitle>& Title::GetLiegeTitle() {
    return m_LiegeTitle;
}

bool Title::IsLandless() const {
    return m_Landless;
}

bool Title::Is(TitleType type) const {
    return this->GetType() == type;
}

void Title::SetName(std::string name) {
    m_Name = name;
}

void Title::SetColor(sf::Color color) {
    m_Color = color;
}

void Title::SetLiegeTitle(SharedPtr<HighTitle> title) {
    m_LiegeTitle = title;
}

void Title::SetLandless(bool landless) {
    m_Landless = landless;
}

std::string Title::GetOriginalFilePath() const {
    return m_OriginalFilePath;
}

SharedPtr<Parser::Node> Title::GetOriginalData() const {
    return m_OriginalData;
}

void Title::SetOriginalFilePath(const std::string& filePath) {
    m_OriginalFilePath = filePath;
}

void Title::SetOriginalData(const Parser::Node& data) {
    m_OriginalData = MakeShared<Parser::Node>(data);
}

bool Title::HasSelectionFocus() const {
    return m_SelectionFocus;
}

void Title::SetSelectionFocus(bool focus) {
    m_SelectionFocus = focus;
}

HighTitle::HighTitle() : Title("", sf::Color(0, 0, 0)) {}

HighTitle::HighTitle(std::string name, sf::Color color, bool landless) : Title(name, color, landless) {}

std::vector<SharedPtr<Title>>& HighTitle::GetDejureTitles() {
    return m_DejureTitles;
}

SharedPtr<CountyTitle>& HighTitle::GetCapitalTitle() {
    return m_CapitalTitle;
}

bool HighTitle::IsDejureTitle(const SharedPtr<Title>& title) {
    return std::find(m_DejureTitles.begin(), m_DejureTitles.end(), title) != m_DejureTitles.end();
}

void HighTitle::AddDejureTitle(SharedPtr<Title> title) {
    if(title == nullptr)
        return;

    if(!this->IsDejureTitle(title))
        m_DejureTitles.push_back(title);

    SharedPtr<HighTitle> previousLiege = title->GetLiegeTitle();
    if(previousLiege != nullptr) {
        previousLiege->RemoveDejureTitle(title);
    }
    title->SetLiegeTitle(shared_from_this());
}

void HighTitle::RemoveDejureTitle(SharedPtr<Title> title) {
    if(title == nullptr)
        return;
    m_DejureTitles.erase(std::remove(m_DejureTitles.begin(), m_DejureTitles.end(), title), m_DejureTitles.end());
    title->SetLiegeTitle(nullptr);
}

void HighTitle::SetCapitalTitle(SharedPtr<CountyTitle> title) {
    m_CapitalTitle = title;
}

BaronyTitle::BaronyTitle() : Title() {}
BaronyTitle::BaronyTitle(std::string name, sf::Color color, bool landless) : Title(name, color, landless) {}
BaronyTitle::BaronyTitle(std::string name, sf::Color color, bool landless, int provinceId) : Title(name, color, landless), m_ProvinceId(provinceId) {}

TitleType BaronyTitle::GetType() const {
    return TitleType::BARONY;
}

int BaronyTitle::GetProvinceId() const {
    return m_ProvinceId;
}

void BaronyTitle::SetProvinceId(int id) {
    m_ProvinceId = id;
}

bool BaronyTitle::HasSelectionFocus() const {
    return true;
}

CountyTitle::CountyTitle() : HighTitle() {}
CountyTitle::CountyTitle(std::string name, sf::Color color, bool landless) : HighTitle(name, color, landless) {}

TitleType CountyTitle::GetType() const {
    return TitleType::COUNTY;
}

DuchyTitle::DuchyTitle() : HighTitle() {}
DuchyTitle::DuchyTitle(std::string name, sf::Color color, bool landless) : HighTitle(name, color, landless) {}

TitleType DuchyTitle::GetType() const {
    return TitleType::DUCHY;
}

KingdomTitle::KingdomTitle() : HighTitle() {}
KingdomTitle::KingdomTitle(std::string name, sf::Color color, bool landless) : HighTitle(name, color, landless) {}

TitleType KingdomTitle::GetType() const {
    return TitleType::KINGDOM;
}

EmpireTitle::EmpireTitle() : HighTitle() {}
EmpireTitle::EmpireTitle(std::string name, sf::Color color, bool landless) : HighTitle(name, color, landless) {}

TitleType EmpireTitle::GetType() const {
    return TitleType::EMPIRE;
}