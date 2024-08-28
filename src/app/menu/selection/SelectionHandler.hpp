#pragma once

class SelectionHandler {
public:
    SelectionHandler(EditorMenu* menu);

    void Select(const SharedPtr<Province>& province);
    void Select(const SharedPtr<Title>& title);
    void Deselect(const SharedPtr<Province>& province);
    void Deselect(const SharedPtr<Title>& title);
    void ClearSelection();

    bool IsSelected(const SharedPtr<Province>& province);
    bool IsSelected(const SharedPtr<Title>& title);

    std::vector<SharedPtr<Province>>& GetProvinces();
    std::vector<SharedPtr<Title>>& GetTitles();
    std::vector<sf::Glsl::Vec4>& GetColors();
    std::size_t GetCount() const;

    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>, SharedPtr<Title>)> callback);

    void OnClick(sf::Mouse::Button button, SharedPtr<Province> province);
    void OnClick(sf::Mouse::Button button, SharedPtr<Province> province, SharedPtr<Title> title);
    void Update();
    
private:
    void UpdateColors();
    void UpdateShader();

private:
    EditorMenu* m_Menu;

    std::vector<SharedPtr<Province>> m_Provinces;
    std::vector<SharedPtr<Title>> m_Titles;

    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>)>> m_ProvinceCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, SharedPtr<Province>, SharedPtr<Title>)>> m_TitleCallbacks;

    // This vector is passed to the fragment shader to change
    // color of pixels in selected provinces.
    std::vector<sf::Glsl::Vec4> m_Colors;

    // Keep track of the number of provinces that are
    // selected, especially for titles. The value is updated
    // in UpdateColors() along side the colors.
    std::size_t m_Count;
};