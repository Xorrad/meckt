#pragma once

class Region {
friend PropertiesTab;
public:
    Region(std::string name);

    std::string GetName() const;
    std::vector<SharedPtr<KingdomTitle>>& GetKingdoms();
    std::vector<SharedPtr<DuchyTitle>>& GetDuchies();
    std::vector<SharedPtr<CountyTitle>>& GetCounties();
    std::vector<SharedPtr<Province>>& GetProvinces();
    std::vector<SharedPtr<Region>>& GetRegions();
    bool HasTitle(SharedPtr<Title> title) const;
    bool HasProvince(SharedPtr<Province> province) const;
    bool HasRegion(SharedPtr<Region> region) const;
    bool DoesGenerateModifiers() const;

    void SetName(std::string name);
    void AddTitle(SharedPtr<Title> title);
    void RemoveTitle(SharedPtr<Title> title);
    void AddKingdom(SharedPtr<KingdomTitle> title);
    void RemoveKingdom(SharedPtr<KingdomTitle> title);
    void AddDuchy(SharedPtr<DuchyTitle> title);
    void RemoveDuchy(SharedPtr<DuchyTitle> title);
    void AddCounty(SharedPtr<CountyTitle> title);
    void RemoveCounty(SharedPtr<CountyTitle> title);
    void AddProvince(SharedPtr<Province> province);
    void RemoveProvince(SharedPtr<Province> province);
    void AddRegion(SharedPtr<Region> region);
    void RemoveRegion(SharedPtr<Region> region);
    void SetGenerateModifiers(bool generateModifiers);

private:
    std::string m_Name;
    std::vector<SharedPtr<KingdomTitle>> m_Kingdoms;
    std::vector<SharedPtr<DuchyTitle>> m_Duchies;
    std::vector<SharedPtr<CountyTitle>> m_Counties;
    std::vector<SharedPtr<Province>> m_Provinces;
    std::vector<SharedPtr<Region>> m_Regions;
    bool m_GenerateModifiers;
};