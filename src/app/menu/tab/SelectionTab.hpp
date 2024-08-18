#pragma once

class SelectionTab : public Tab {
public:
    SelectionTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
    void RenderHarmonizeColors();
    void RenderCreateTitle();
};