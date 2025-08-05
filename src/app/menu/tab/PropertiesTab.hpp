#pragma once

class PropertiesTab : public Tab {
public:
    PropertiesTab(EditorMenu* menu, bool visible = true);

    virtual void Update(sf::Time delta) override;
    virtual void Render() override;

    void RenderJointProvinces();
    void RenderProvinces();
    void RenderTitles();

private:
    sf::Clock m_Clock;

    sf::Text m_SelectingTitleText;
    bool m_SelectingTitle;

    bool m_DisplayCulturalNames;
    bool m_DisplayHistory;
    bool m_DisplayDejureTitles;
};