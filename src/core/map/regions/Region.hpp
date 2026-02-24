#pragma once

class Region {
public:
    Region(std::string name);

    std::string GetName() const;
    std::span<KingdomTitle*> GetKingdoms();
    std::span<DuchyTitle*> GetDuchies();
    std::span<CountyTitle*> GetCounties();
    std::span<Province*> GetProvinces();
    std::span<Region*> GetRegions();
    bool HasTitle(Title* title) const;
    bool HasProvince(Province* province) const;
    bool HasRegion(Region* region) const;
    bool DoesGenerateModifiers() const;

    void SetName(std::string name);
    void AddTitle(Title* title);
    void RemoveTitle(Title* title);
    void AddKingdom(KingdomTitle* title);
    void RemoveKingdom(KingdomTitle* title);
    void AddDuchy(DuchyTitle* title);
    void RemoveDuchy(DuchyTitle* title);
    void AddCounty(CountyTitle* title);
    void RemoveCounty(CountyTitle* title);
    void AddProvince(Province* province);
    void RemoveProvince(Province* province);
    void AddRegion(Region* region);
    void RemoveRegion(Region* region);
    void SetGenerateModifiers(bool generateModifiers);

private:
    std::string m_Name;
    std::vector<KingdomTitle*> m_Kingdoms;
    std::vector<DuchyTitle*> m_Duchies;
    std::vector<CountyTitle*> m_Counties;
    std::vector<Province*> m_Provinces;
    std::vector<Region*> m_Regions;
    bool m_GenerateModifiers;
};