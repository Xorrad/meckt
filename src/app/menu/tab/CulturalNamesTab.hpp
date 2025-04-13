#pragma once

class CulturalNamesTab : public Tab {
public:
    CulturalNamesTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
};