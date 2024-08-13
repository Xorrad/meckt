#pragma once

class Title {
public:
    Title();
    Title(std::string name, sf::Color color);
    virtual ~Title() = default;

    // Title(const Title& title) = delete;
    // Title& operator=(const Title& other) = delete;
    // Title& operator=(Title&& other) = delete;

    virtual TitleType GetType() const = 0;
    std::string GetName() const;
    sf::Color GetColor() const;
    SharedPtr<HighTitle>& GetLiegeTitle();
    
    bool Is(TitleType type) const;

    void SetName(std::string name);
    void SetColor(sf::Color color);
    void SetLiegeTitle(SharedPtr<HighTitle> title);

protected:
    std::string m_Name;
    sf::Color m_Color;
    SharedPtr<HighTitle> m_LiegeTitle;
};

class HighTitle : public Title, public std::enable_shared_from_this<HighTitle> {
public:
    HighTitle();
    HighTitle(std::string name, sf::Color color);
    virtual ~HighTitle() = default;

    virtual TitleType GetType() const = 0;
    std::vector<SharedPtr<Title>>& GetDejureTitles();
    SharedPtr<CountyTitle>& GetCapitalTitle();

    void AddDejureTitle(SharedPtr<Title> title);
    void RemoveDejureTitle(SharedPtr<Title> title);
    void SetCapitalTitle(SharedPtr<CountyTitle> title);

protected:
    std::vector<SharedPtr<Title>> m_DejureTitles;
    SharedPtr<CountyTitle> m_CapitalTitle;
};

class BaronyTitle : public Title {
public:
    BaronyTitle();
    BaronyTitle(std::string name, sf::Color color);
    BaronyTitle(std::string name, sf::Color color, int provinceId);

    virtual TitleType GetType() const;
    int GetProvinceId() const;
    void SetProvinceId(int id);

private:
    int m_ProvinceId;
};

class CountyTitle : public HighTitle {
public:
    CountyTitle();
    CountyTitle(std::string name, sf::Color color);

    virtual TitleType GetType() const;
};

class DuchyTitle : public HighTitle {
public:
    DuchyTitle();
    DuchyTitle(std::string name, sf::Color color);

    virtual TitleType GetType() const;
};

class KingdomTitle : public HighTitle {
public:
    KingdomTitle();
    KingdomTitle(std::string name, sf::Color color);

    virtual TitleType GetType() const;
};

class EmpireTitle : public HighTitle {
public:
    EmpireTitle();
    EmpireTitle(std::string name, sf::Color color);

    virtual TitleType GetType() const;
};

// template <typename ...Args>
// inline SharedPtr<Title> MakeShared(Args&& ...args) {
//     return MakeTitle(std::forward<Args>(args)...);
// }

template <typename ...Args>
inline SharedPtr<Title> MakeTitle(TitleType type, Args&& ...args) {
    switch(type) {
        case TitleType::BARONY: return MakeShared<BaronyTitle>(std::forward<Args>(args)...);
        case TitleType::COUNTY: return MakeShared<CountyTitle>(std::forward<Args>(args)...);
        case TitleType::DUCHY: return MakeShared<DuchyTitle>(std::forward<Args>(args)...);
        case TitleType::KINGDOM: return MakeShared<KingdomTitle>(std::forward<Args>(args)...);
        case TitleType::EMPIRE: return MakeShared<EmpireTitle>(std::forward<Args>(args)...);
        default: break;
    }
    throw std::runtime_error("error: failed to create SharedPtr<Title> with unknown title type.");
}