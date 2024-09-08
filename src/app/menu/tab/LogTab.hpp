#pragma once

class LogTab : public Tab {
public:
    LogTab(EditorMenu* menu, bool visible = true);

    virtual void Render() override;
};