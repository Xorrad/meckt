#pragma once

class TitlesTab : public Tab {
public:
    TitlesTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
};