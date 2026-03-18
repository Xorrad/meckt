#include "Title.hpp"
#include "mod/Mod.hpp"
#include "map/provinces/Province.hpp"

Title::Title() : Title("", sf::Color(0, 0, 0)) {}

Title::Title(std::string name, sf::Color color, bool landless) :
    m_Name(name),
    m_Color(color),
    m_LiegeTitle(nullptr),
    m_Landless(landless),
    m_OriginalData(MakeShared<Jomini::Object>(Jomini::ObjectMap{})),
    m_SelectionFocus(true)
{}

// Title::Title(const Title& title) : Title(title.GetName(), title.GetColor()) {}

std::string Title::GetName() const {
    return m_Name;
}

sf::Color Title::GetColor() const {
    return m_Color;
}

HighTitle* Title::GetLiegeTitle() {
    return m_LiegeTitle;
}

bool Title::IsLandless() const {
    return m_Landless;
}

bool Title::Is(TitleType type) const {
    return this->GetType() == type;
}

bool Title::IsVassal(HighTitle* title) const {
    if (title == nullptr)
        return false;
    HighTitle* liege = m_LiegeTitle;
    while(liege != nullptr) {
        if(liege == title)
            return true;
        liege = liege->GetLiegeTitle();
    }
    return false;
}

void Title::SetName(std::string name) {
    m_Name = name;
}

void Title::SetColor(sf::Color color) {
    m_Color = color;
}

void Title::SetLiegeTitle(HighTitle* title) {
    m_LiegeTitle = title;
}

void Title::SetLandless(bool landless) {
    m_Landless = landless;
}

std::string Title::GetOriginalFileName() const {
    return m_OriginalFileName;
}

SharedPtr<Jomini::Object> Title::GetOriginalData() const {
    return m_OriginalData;
}

void Title::SetOriginalFileName(const std::string& filePath) {
    m_OriginalFileName = filePath;
}

void Title::SetOriginalData(SharedPtr<Jomini::Object> data) {
    m_OriginalData = data;
}

std::string Title::GetOriginalHistoryFileName() const {
    return m_OriginalHistoryFileName;
}

void Title::SetOriginalHistoryFileName(const std::string& filePath) {
    m_OriginalHistoryFileName = filePath;
}

std::map<Jomini::Date, SharedPtr<Jomini::Object>>& Title::GetHistory() {
    return m_History;
}

void Title::AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data) {
    m_History[date] = data;
}

void Title::RemoveHistory(Jomini::Date date) {
    m_History.erase(date);
}

std::map<std::string, std::string>& Title::GetCulturalNames() {
    return m_CulturalNames;
}

void Title::AddCulturalName(const std::string& culture, std::string name) {
    m_CulturalNames[culture] = name;
}

void Title::RemoveCulturalName(const std::string& culture) {
    m_CulturalNames.erase(culture);
}

std::map<std::string, std::string>& Title::GetLocNames() {
    return m_LocNames;
}

std::string& Title::GetLocName(const std::string& lang) {
    return m_LocNames[lang];
}

std::string Title::GetLocName(const std::string& lang) const {
    auto it = m_LocNames.find(lang);
    if (it == m_LocNames.end())
        return "";
    return it->second;
}

bool Title::HasLocName(const std::string& lang) const {
    auto it = m_LocNames.find(lang);
    if (it == m_LocNames.end())
        return false;
    return !it->second.empty();
}

void Title::SetLocName(const std::string& lang, std::string name) {
    m_LocNames[lang] = name;
}

std::map<std::string, std::string>& Title::GetLocAdjectives() {
    return m_LocAdjectives;
}

std::string& Title::GetLocAdjective(const std::string& lang) {
    return m_LocAdjectives[lang];
}

std::string Title::GetLocAdjective(const std::string& lang) const {
    if(m_LocAdjectives.count(lang) == 0)
        return "";
    return m_LocAdjectives.at(lang);
}

bool Title::HasLocAdjective(const std::string& lang) const {
    auto it = m_LocAdjectives.find(lang);
    if (it == m_LocAdjectives.end())
        return false;
    return !it->second.empty();
}

void Title::SetLocAdjective(const std::string& lang, std::string adjective) {
    m_LocAdjectives[lang] = adjective;
}

std::map<std::string, std::string>& Title::GetLocArticles() {
    return m_LocArticles;
}

std::string& Title::GetLocArticle(const std::string& lang) {
    return m_LocArticles[lang];
}

std::string Title::GetLocArticle(const std::string& lang) const {
    if(m_LocArticles.count(lang) == 0)
        return "";
    return m_LocArticles.at(lang);
}

bool Title::HasLocArticle(const std::string& lang) const {
    auto it = m_LocArticles.find(lang);
    if (it == m_LocArticles.end())
        return false;
    return !it->second.empty();
}

void Title::SetLocArticle(const std::string& lang, std::string article) {
    m_LocArticles[lang] = article;
}

bool Title::HasSelectionFocus() const {
    return m_SelectionFocus;
}

void Title::SetSelectionFocus(bool focus) {
    m_SelectionFocus = focus;
}

HighTitle::HighTitle() : Title("", sf::Color(0, 0, 0)), m_CapitalTitle(nullptr) {}

HighTitle::HighTitle(std::string name, sf::Color color, bool landless) : Title(name, color, landless), m_CapitalTitle(nullptr) {}

std::vector<Title*>& HighTitle::GetDejureTitles() {
    return m_DejureTitles;
}

const std::vector<Title*>& HighTitle::GetDejureTitles() const {
    return m_DejureTitles;
}

CountyTitle* HighTitle::GetCapitalTitle() {
    return m_CapitalTitle;
}

bool HighTitle::HasDejureTitle(const Title* title) const {
    return std::find(m_DejureTitles.begin(), m_DejureTitles.end(), title) != m_DejureTitles.end();
}

void HighTitle::AddDejureTitle(Title* title) {
    if (!this->HasDejureTitle(title)) {
        m_DejureTitles.push_back(title);

        HighTitle* previousLiege = title->GetLiegeTitle();
        if (previousLiege != nullptr) {
            previousLiege->RemoveDejureTitle(title);
        }
    }
    title->SetLiegeTitle(this);
}

void HighTitle::RemoveDejureTitle(Title* title) {
    m_DejureTitles.erase(
        std::remove(m_DejureTitles.begin(), m_DejureTitles.end(), title),
        m_DejureTitles.end()
    );
    title->SetLiegeTitle(nullptr);
}

void HighTitle::SetCapitalTitle(CountyTitle* title) {
    m_CapitalTitle = title;
}

void HighTitle::ClearDejureTitles() {
    for (Title* dejure : m_DejureTitles)
        dejure->SetLiegeTitle(nullptr);
    m_DejureTitles.clear();
}

void HighTitle::SetSelectionFocus(bool focus) {
    m_SelectionFocus = focus;
    if(focus) {
        for(Title* dejureTitle : m_DejureTitles)
            dejureTitle->SetSelectionFocus(true);
    }
}

sf::Vector2i HighTitle::GetImagePosition(Mod& mod) const {
    if(m_DejureTitles.empty())
        return sf::Vector2i(0, 0);
    return m_DejureTitles.front()->GetImagePosition(mod);
}

BaronyTitle::BaronyTitle() : Title(), m_ProvinceId(0) {}
BaronyTitle::BaronyTitle(std::string name, sf::Color color, bool landless) : Title(name, color, landless), m_ProvinceId(0) {}
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

sf::Vector2i BaronyTitle::GetImagePosition(Mod& mod) const {
	auto it = mod.GetProvincesByIds().find(m_ProvinceId);
    if (it == mod.GetProvincesByIds().end())
        return sf::Vector2i(0, 0);
    return it->second->GetImagePosition();
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

HegemonyTitle::HegemonyTitle() : HighTitle() {}
HegemonyTitle::HegemonyTitle(std::string name, sf::Color color, bool landless) : HighTitle(name, color, landless) {}

TitleType HegemonyTitle::GetType() const {
    return TitleType::HEGEMONY;
}