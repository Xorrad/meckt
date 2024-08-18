#pragma once

class PropertiesTab : public Tab {
public:
    PropertiesTab(EditorMenu* menu, bool visible = true);

    virtual void Update(sf::Time delta) override;
    virtual void Render() override;
    void RenderProvinces();
    void RenderTitles();

private:
    sf::Clock m_Clock;

    sf::Text m_SelectingTitleText;
    bool m_SelectingTitle;
};