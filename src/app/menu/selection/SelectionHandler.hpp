#pragma once

enum class SelectionType {
    NONE      = 0,
    TITLE     = 1,
    PROVINCE  = 2,
    POSITION  = 3
};

class SelectionHandler {
friend PropertiesTab;
public:
    SelectionHandler(EditorMenu& menu);

    void Select(Province* province, bool update = true);
    void Select(Title* title);
    void Select(Region* region);
    void Select(Adjacency* adjacency);
    void Deselect(Province* province, bool update = true);
    void Deselect(Title* title);
    void Deselect(Region* region);
    void Deselect(Adjacency* adjacency);
    void ClearSelection();

    bool IsSelected(Province* province) const;
    bool IsSelected(const Title* title) const;
    bool IsSelected(const Region* region) const;
    bool IsSelected(const Adjacency* adjacency) const;

    std::span<Province*> GetProvinces();
    std::span<Title*> GetTitles();
    std::span<Region*> GetRegions();
    Adjacency* GetAdjacency();
    std::vector<sf::Glsl::Vec4>& GetColors();
    std::size_t GetCount() const;

    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)> callback);
    void AddCallback(std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)> callback);

    void RemoveLastProvinceCallback();
    void RemoveLastPositionCallback();

    void OnClick(sf::Mouse::Button button, sf::Vector2f position);
    void OnClick(sf::Mouse::Button button, Province* province);
    void OnClick(sf::Mouse::Button button, Province* province, Title* title);
    void Update();

    void UpdateSelectionText();
    sf::Text& GetSelectionText();
    void SetSelectionType(SelectionType type);
    SelectionType GetSelectionType() const;
    bool IsSelectionType(SelectionType type) const;
    
private:
    void UpdateColors();
    void UpdateShader();

private:
    EditorMenu& m_Menu;

    std::vector<Province*> m_Provinces;
    std::unordered_map<Province*, bool> m_ProvincesLookup;
    std::vector<Title*> m_Titles;
    std::vector<Region*> m_Regions;
    Adjacency* m_Adjacency;

    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, sf::Vector2f)>> m_PositionCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*)>> m_ProvinceCallbacks;
    std::vector<std::function<SelectionCallbackResult(sf::Mouse::Button, Province*, Title*)>> m_TitleCallbacks;

    // This vector is passed to the fragment shader to change
    // color of pixels in selected provinces.
    std::vector<sf::Glsl::Vec4> m_Colors;

    // Keep track of the number of provinces that are
    // selected, especially for titles. The value is updated
    // in UpdateColors() along side the colors.
    std::size_t m_Count;

    sf::Text m_SelectionText;
    SelectionType m_SelectionType;
};