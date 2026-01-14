#pragma once

class SelectionHandler {
friend PropertiesTab;
public:
    SelectionHandler(EditorMenu& menu);

    void Select(Province* province);
    void Select(Title* title);
    void Select(Region* region);
    void Deselect(Province* province);
    void Deselect(Title* title);
    void Deselect(Region* region);
    void ClearSelection();

    bool IsSelected(const Province* province) const;
    bool IsSelected(const Title* title) const;
    bool IsSelected(const Region* region) const;

    std::span<Province*> GetProvinces();
    std::span<Title*> GetTitles();
    std::span<Region*> GetRegions();
    std::vector<sf::Glsl::Vec4>& GetColors();
    std::size_t GetCount() const;

    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback);

    void OnClick(sf::Mouse::Button button, Province* province);
    void OnClick(sf::Mouse::Button button, Province* province, Title* title);
    void Update();
    
private:
    void UpdateColors();
    void UpdateShader();

private:
    EditorMenu& m_Menu;

    std::vector<Province*> m_Provinces;
    std::vector<Title*> m_Titles;
    std::vector<Region*> m_Regions;

    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)>> m_ProvinceCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)>> m_TitleCallbacks;

    // This vector is passed to the fragment shader to change
    // color of pixels in selected provinces.
    std::vector<sf::Glsl::Vec4> m_Colors;

    // Keep track of the number of provinces that are
    // selected, especially for titles. The value is updated
    // in UpdateColors() along side the colors.
    std::size_t m_Count;
};