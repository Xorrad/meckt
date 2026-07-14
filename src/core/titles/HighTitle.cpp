#include "HighTitle.hpp"

HighTitle::HighTitle() :
    Title("", sf::Color(0, 0, 0)),
    m_CapitalTitle(nullptr)
{}

HighTitle::HighTitle(std::string name, sf::Color color, bool landless) :
    Title(name, color, landless),
    m_CapitalTitle(nullptr)
{}

//////////////////////////////////////////////////////

bool HighTitle::HasDejureTitle(const Title* title) const {
    return std::find(m_DejureTitles.begin(), m_DejureTitles.end(), title) != m_DejureTitles.end();
}

//////////////////////////////////////////////////////

std::vector<Title*>& HighTitle::GetDejureTitles() {
    return m_DejureTitles;
}

const std::vector<Title*>& HighTitle::GetDejureTitles() const {
    return m_DejureTitles;
}

CountyTitle* HighTitle::GetCapitalTitle() {
    return m_CapitalTitle;
}

sf::Vector2i HighTitle::GetImagePosition(const ProvinceManager& provinceManager) const {
    if(m_DejureTitles.empty())
        return sf::Vector2i(0, 0);
    return m_DejureTitles.front()->GetImagePosition(provinceManager);
}

//////////////////////////////////////////////////////

void HighTitle::SetCapitalTitle(CountyTitle* title) {
    m_CapitalTitle = title;
}

void HighTitle::SetSelectionFocus(bool focus) {
    m_SelectionFocus = focus;
    if(focus) {
        for(Title* dejureTitle : m_DejureTitles)
            dejureTitle->SetSelectionFocus(true);
    }
}

//////////////////////////////////////////////////////

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

void HighTitle::ClearDejureTitles() {
    for (Title* dejure : m_DejureTitles)
        dejure->SetLiegeTitle(nullptr);
    m_DejureTitles.clear();
}

//////////////////////////////////////////////////////