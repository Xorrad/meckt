#pragma once

class AdjacenciesTab : public Tab {
public:
    AdjacenciesTab(EditorMenu& menu, bool visible = true);

    virtual void Render() override;
};