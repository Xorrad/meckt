#include "Title.hpp"
#include "mod/Mod.hpp"

Title::Title() :
    Title("", sf::Color(0, 0, 0))
{}

Title::Title(std::string name, sf::Color color, bool landless) :
    m_Name(name),
    m_Color(color),
    m_LiegeTitle(nullptr),
    m_Landless(landless),
    m_OriginalData(MakeShared<Jomini::Object>(Jomini::ObjectMap{})),
    m_SelectionFocus(true)
{}

//////////////////////////////////////////////////////

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

bool Title::HasLocName(const std::string& lang) const {
    auto it = m_LocNames.find(lang);
    if (it == m_LocNames.end())
        return false;
    return !it->second.empty();
}

bool Title::HasLocAdjective(const std::string& lang) const {
    auto it = m_LocAdjectives.find(lang);
    if (it == m_LocAdjectives.end())
        return false;
    return !it->second.empty();
}

bool Title::HasLocArticle(const std::string& lang) const {
    auto it = m_LocArticles.find(lang);
    if (it == m_LocArticles.end())
        return false;
    return !it->second.empty();
}

//////////////////////////////////////////////////////

std::string Title::GetName() const {
    return m_Name;
}

sf::Color Title::GetColor() const {
    return m_Color;
}

HighTitle* Title::GetLiegeTitle() {
    return m_LiegeTitle;
}

HighTitle* Title::GetLiegeTitle(TitleType type) {
    if (m_LiegeTitle == nullptr)
        return nullptr;
    if (m_LiegeTitle->Is(type))
        return m_LiegeTitle;
    return m_LiegeTitle->GetLiegeTitle(type);
}

bool Title::IsLandless() const {
    return m_Landless;
}

std::string Title::GetOriginalFileName() const {
    return m_OriginalFileName;
}

SharedPtr<Jomini::Object> Title::GetOriginalData() const {
    return m_OriginalData;
}

std::string Title::GetOriginalHistoryFileName() const {
    return m_OriginalHistoryFileName;
}

std::map<Jomini::Date, SharedPtr<Jomini::Object>>& Title::GetHistory() {
    return m_History;
}

std::map<std::string, std::string>& Title::GetCulturalNames() {
    return m_CulturalNames;
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

bool Title::HasSelectionFocus() const {
    return m_SelectionFocus;
}

//////////////////////////////////////////////////////

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

void Title::SetOriginalFileName(const std::string& fileName) {
    m_OriginalFileName = fileName;
}

void Title::SetOriginalData(SharedPtr<Jomini::Object> data) {
    m_OriginalData = data;
}

void Title::SetOriginalHistoryFileName(const std::string& fileName) {
    m_OriginalHistoryFileName = fileName;
}

void Title::SetLocName(const std::string& lang, std::string name) {
    m_LocNames[lang] = name;
}

void Title::SetLocAdjective(const std::string& lang, std::string adjective) {
    m_LocAdjectives[lang] = adjective;
}

void Title::SetLocArticle(const std::string& lang, std::string article) {
    m_LocArticles[lang] = article;
}

void Title::SetSelectionFocus(bool focus) {
    m_SelectionFocus = focus;
}

//////////////////////////////////////////////////////

void Title::AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data) {
    m_History[date] = data;
}

void Title::RemoveHistory(Jomini::Date date) {
    m_History.erase(date);
}

void Title::AddCulturalName(const std::string& culture, std::string name) {
    m_CulturalNames[culture] = name;
}

void Title::RemoveCulturalName(const std::string& culture) {
    m_CulturalNames.erase(culture);
}

//////////////////////////////////////////////////////