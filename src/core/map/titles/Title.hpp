#pragma once

class Title {
public:
    Title();
    Title(std::string name, sf::Color color, bool landless = false);
    virtual ~Title() = default;

    // Title(const Title& title) = delete;
    // Title& operator=(const Title& other) = delete;
    // Title& operator=(Title&& other) = delete;

    virtual TitleType GetType() const = 0;
    std::string GetName() const;
    sf::Color GetColor() const;
    HighTitle* GetLiegeTitle();
    HighTitle* GetLiegeTitle(TitleType type);
    bool IsLandless() const;

    bool Is(TitleType type) const;
    bool IsVassal(HighTitle* title) const;

    void SetName(std::string name);
    void SetColor(sf::Color color);
    void SetLiegeTitle(HighTitle* title);
    void SetLandless(bool landless);
    
    std::string GetOriginalFileName() const;
    SharedPtr<Jomini::Object> GetOriginalData() const;
    void SetOriginalFileName(const std::string& fileName);
    void SetOriginalData(SharedPtr<Jomini::Object> data);

    std::string GetOriginalHistoryFileName() const;
    void SetOriginalHistoryFileName(const std::string& fileName);
    std::map<Jomini::Date, SharedPtr<Jomini::Object>>& GetHistory();
    void AddHistory(Jomini::Date date, SharedPtr<Jomini::Object> data);
    void RemoveHistory(Jomini::Date date);

    std::map<std::string, std::string>& GetCulturalNames();
    void AddCulturalName(const std::string& culture, std::string name);
    void RemoveCulturalName(const std::string& culture);

    std::map<std::string, std::string>& GetLocNames();
    std::string& GetLocName(const std::string& lang);
    std::string GetLocName(const std::string& lang) const;
    bool HasLocName(const std::string& lang) const;
    void SetLocName(const std::string& lang, std::string name);

    std::map<std::string, std::string>& GetLocAdjectives();
    std::string& GetLocAdjective(const std::string& lang);
    std::string GetLocAdjective(const std::string& lang) const;
    bool HasLocAdjective(const std::string& lang) const;
    void SetLocAdjective(const std::string& lang, std::string adjective);
    
    std::map<std::string, std::string>& GetLocArticles();
    std::string& GetLocArticle(const std::string& lang);
    std::string GetLocArticle(const std::string& lang) const;
    bool HasLocArticle(const std::string& lang) const;
    void SetLocArticle(const std::string& lang, std::string article);

    virtual bool HasSelectionFocus() const;
    virtual void SetSelectionFocus(bool focus);

    virtual sf::Vector2i GetImagePosition(Mod& mod) const = 0;

protected:
    std::string m_Name;
    sf::Color m_Color;
    HighTitle* m_LiegeTitle;
    bool m_Landless;

    std::string m_OriginalFileName;
    SharedPtr<Jomini::Object> m_OriginalData;

    std::string m_OriginalHistoryFileName;
    std::map<Jomini::Date, SharedPtr<Jomini::Object>> m_History;
    std::map<std::string, std::string> m_CulturalNames;

    std::map<std::string, std::string> m_LocNames;
    std::map<std::string, std::string> m_LocAdjectives;
    std::map<std::string, std::string> m_LocArticles;

    bool m_SelectionFocus;
};

class HighTitle : public Title {
public:
    HighTitle();
    HighTitle(std::string name, sf::Color color, bool landless = false);
    virtual ~HighTitle() = default;

    virtual TitleType GetType() const = 0;
    std::vector<Title*>& GetDejureTitles();
    const std::vector<Title*>& GetDejureTitles() const;
    CountyTitle* GetCapitalTitle();
    bool HasDejureTitle(const Title* title) const;

    void AddDejureTitle(Title* title);
    void RemoveDejureTitle(Title* title);
    void SetCapitalTitle(CountyTitle* title);
    void ClearDejureTitles();

    virtual void SetSelectionFocus(bool focus) override;

    virtual sf::Vector2i GetImagePosition(Mod& mod) const;
protected:
    std::vector<Title*> m_DejureTitles;
    CountyTitle* m_CapitalTitle;
};

class BaronyTitle : public Title {
public:
    BaronyTitle();
    BaronyTitle(std::string name, sf::Color color, bool landless = false);
    BaronyTitle(std::string name, sf::Color color, bool landless, int provinceId);

    virtual TitleType GetType() const;
    int GetProvinceId() const;
    void SetProvinceId(int id);

    virtual bool HasSelectionFocus() const override;

    virtual sf::Vector2i GetImagePosition(Mod& mod) const;

private:
    int m_ProvinceId;
};

class CountyTitle : public HighTitle {
public:
    CountyTitle();
    CountyTitle(std::string name, sf::Color color, bool landless = false);

    virtual TitleType GetType() const;
};

class DuchyTitle : public HighTitle {
public:
    DuchyTitle();
    DuchyTitle(std::string name, sf::Color color, bool landless = false);

    virtual TitleType GetType() const;
};

class KingdomTitle : public HighTitle {
public:
    KingdomTitle();
    KingdomTitle(std::string name, sf::Color color, bool landless = false);

    virtual TitleType GetType() const;
};

class EmpireTitle : public HighTitle {
public:
    EmpireTitle();
    EmpireTitle(std::string name, sf::Color color, bool landless = false);

    virtual TitleType GetType() const;
};

class HegemonyTitle : public HighTitle {
public:
    HegemonyTitle();
    HegemonyTitle(std::string name, sf::Color color, bool landless = false);

    virtual TitleType GetType() const;
};

template <typename ...Args>
inline UniquePtr<Title> MakeTitle(TitleType type, Args&& ...args) {
    switch(type) {
        case TitleType::BARONY: return MakeUnique<BaronyTitle>(std::forward<Args>(args)...);
        case TitleType::COUNTY: return MakeUnique<CountyTitle>(std::forward<Args>(args)...);
        case TitleType::DUCHY: return MakeUnique<DuchyTitle>(std::forward<Args>(args)...);
        case TitleType::KINGDOM: return MakeUnique<KingdomTitle>(std::forward<Args>(args)...);
        case TitleType::EMPIRE: return MakeUnique<EmpireTitle>(std::forward<Args>(args)...);
        case TitleType::HEGEMONY: return MakeUnique<HegemonyTitle>(std::forward<Args>(args)...);
        default: break;
    }
    throw std::invalid_argument("MakeTitle: invalid title type");
}