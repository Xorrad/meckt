#pragma once

class PropertiesTab : public Tab {
public:
    PropertiesTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
};