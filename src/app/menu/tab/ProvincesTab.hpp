#pragma once

class ProvincesTab : public Tab {
public:
    ProvincesTab(EditorMenu& menu, bool visible = true, bool focusedAtStartup = true);

    virtual void Render() override;
};