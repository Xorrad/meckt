#pragma once

class PropertiesTab : public Tab {
public:
    PropertiesTab(EditorMenu& menu, bool visible = true);

    virtual void Update(sf::Time delta) override;
    virtual void Render() override;

    void RenderJointProvinces();
    void RenderProvinces();
    void RenderTitles();
    void RenderRegions();
    void RenderAdjacency();

private:
    bool m_DisplayCulturalNames;
    bool m_DisplayHistory;
    bool m_DisplayDejureTitles;
    bool m_DisplayClimate;
    bool m_DisplayRegionsTitles;
    bool m_DisplayRegionsProvinces;
    bool m_DisplayRegionsRegions;
};