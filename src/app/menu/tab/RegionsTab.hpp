#pragma once

class RegionsTab : public Tab {
public:
    RegionsTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
};