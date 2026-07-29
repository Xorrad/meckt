#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "regions/RegionManager.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "util/Yaml.hpp"

TEST_SUITE("[RegionManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[RegionManager] CountRegions") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK(regionManager.CountRegions() == 0);

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    CHECK(regionManager.CountRegions() == 1);

    regionManager.AddRegion(MakeUnique<Region>("region2"));
    CHECK(regionManager.CountRegions() == 2);

    regionManager.RemoveRegion("region1");
    CHECK(regionManager.CountRegions() == 1);

    regionManager.RemoveRegion("region2");
    CHECK(regionManager.CountRegions() == 0);
}

TEST_CASE("[RegionManager] HasRegion") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK_FALSE(regionManager.HasRegion("region1"));
    CHECK_FALSE(regionManager.HasRegion("region2"));

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    CHECK(regionManager.HasRegion("region1"));
    CHECK_FALSE(regionManager.HasRegion("region2"));

    regionManager.AddRegion(MakeUnique<Region>("region2"));
    CHECK(regionManager.HasRegion("region1"));
    CHECK(regionManager.HasRegion("region2"));

    regionManager.RemoveRegion("region1");
    CHECK_FALSE(regionManager.HasRegion("region1"));
    CHECK(regionManager.HasRegion("region2"));

    regionManager.RemoveRegion("region2");
    CHECK_FALSE(regionManager.HasRegion("region1"));
    CHECK_FALSE(regionManager.HasRegion("region2"));
}

//////////////////////////////////////////////////////

TEST_CASE("[RegionManager] GetRegion") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK(regionManager.GetRegion("region1") == nullptr);
    CHECK(regionManager.GetRegion("region2") == nullptr);

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    REQUIRE(regionManager.GetRegion("region1") != nullptr);
    CHECK(regionManager.GetRegion("region1")->GetName() == "region1");
    CHECK(regionManager.GetRegion("region2") == nullptr);

    regionManager.AddRegion(MakeUnique<Region>("region2"));
    REQUIRE(regionManager.GetRegion("region1") != nullptr);
    CHECK(regionManager.GetRegion("region1")->GetName() == "region1");
    REQUIRE(regionManager.GetRegion("region2") != nullptr);
    CHECK(regionManager.GetRegion("region2")->GetName() == "region2");

    regionManager.RemoveRegion("region1");
    CHECK(regionManager.GetRegion("region1") == nullptr);
    REQUIRE(regionManager.GetRegion("region2") != nullptr);
    CHECK(regionManager.GetRegion("region2")->GetName() == "region2");

    regionManager.RemoveRegion("region2");
    CHECK(regionManager.GetRegion("region1") == nullptr);
    CHECK(regionManager.GetRegion("region2") == nullptr);
}

//////////////////////////////////////////////////////

TEST_CASE("[RegionManager] AddRegion") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK(regionManager.CountRegions() == 0);

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    CHECK(regionManager.CountRegions() == 1);
    REQUIRE(regionManager.GetRegion("region1") != nullptr);
    CHECK(regionManager.GetRegion("region1")->GetName() == "region1");

    regionManager.AddRegion(MakeUnique<Region>("region2"));
    CHECK(regionManager.CountRegions() == 2);
    REQUIRE(regionManager.GetRegion("region2") != nullptr);
    CHECK(regionManager.GetRegion("region2")->GetName() == "region2");
}

TEST_CASE("[RegionManager] RemoveRegion") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK(regionManager.CountRegions() == 0);

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    regionManager.AddRegion(MakeUnique<Region>("region2"));
    CHECK(regionManager.CountRegions() == 2);

    regionManager.RemoveRegion("region1");
    CHECK(regionManager.CountRegions() == 1);
    CHECK(regionManager.GetRegion("region1") == nullptr);
    REQUIRE(regionManager.GetRegion("region2") != nullptr);
    CHECK(regionManager.GetRegion("region2")->GetName() == "region2");

    regionManager.RemoveRegion("region2");
    CHECK(regionManager.CountRegions() == 0);
    CHECK(regionManager.GetRegion("region1") == nullptr);
    CHECK(regionManager.GetRegion("region2") == nullptr);

    SUBCASE("region removed from all other regions") {
        regionManager.AddRegion(MakeUnique<Region>("region1"));
        regionManager.AddRegion(MakeUnique<Region>("region2"));
        regionManager.AddRegion(MakeUnique<Region>("region3"));

        REQUIRE(regionManager.GetRegion("region1") != nullptr);
        REQUIRE(regionManager.GetRegion("region2") != nullptr);
        REQUIRE(regionManager.GetRegion("region3") != nullptr);

        regionManager.GetRegion("region1")->AddRegion(regionManager.GetRegion("region2"));
        regionManager.GetRegion("region1")->AddRegion(regionManager.GetRegion("region3"));
        regionManager.GetRegion("region2")->AddRegion(regionManager.GetRegion("region3"));

        regionManager.RemoveRegion("region3");
        CHECK_FALSE(regionManager.GetRegion("region1")->HasRegion(regionManager.GetRegion("region3")));
        CHECK_FALSE(regionManager.GetRegion("region2")->HasRegion(regionManager.GetRegion("region3")));
    }
}

TEST_CASE("[RegionManager] RenameRegion") {
    Mod mod("");
    RegionManager regionManager(mod);

    CHECK(regionManager.CountRegions() == 0);

    regionManager.AddRegion(MakeUnique<Region>("region1"));
    CHECK(regionManager.CountRegions() == 1);
    REQUIRE(regionManager.GetRegion("region1") != nullptr);
    CHECK(regionManager.GetRegion("region1")->GetName() == "region1");

    regionManager.RenameRegion("region1", "new_region1");
    CHECK(regionManager.CountRegions() == 1);
    CHECK(regionManager.GetRegion("region1") == nullptr);
    REQUIRE(regionManager.GetRegion("new_region1") != nullptr);
    CHECK(regionManager.GetRegion("new_region1")->GetName() == "new_region1");
}

//////////////////////////////////////////////////////

TEST_CASE("[RegionManager] LoadGeographicalRegions") {
    Mod mod("resources/tests/region_manager/test_mod");
    RegionManager regionManager(mod);
    ProvinceManager provinceManager(mod);
    TitleManager titleManager(mod);

    REQUIRE_NOTHROW(provinceManager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(titleManager.LoadTitles(provinceManager));
    REQUIRE_NOTHROW(regionManager.LoadGeographicalRegions(provinceManager, titleManager));

    struct RegionTestData {
        std::vector<std::string> regions;
        std::vector<std::string> kingdoms;
        std::vector<std::string> duchies;
        std::vector<std::string> counties;
        std::vector<int> provinces;
        bool doesGenerateModifiers;
        bool shouldRememberCountiesOrder;
        std::string_view fileName;
    };
    const std::map<std::string, RegionTestData> expectedRegions {
        { "region1", { {},          {"k_test"},    {"d_test1"},  {"c_test1", "c_test2"}, {1, 2}, true, true, "geographical_region.txt" } },
        { "region2", { {"region1"}, {},            {},           {"c_test1"},            {},     false, false, "geographical_region.txt" } },
        { "region3", { {"region1"}, {},            {},           {},                     {},     false, false, "geographical_region.txt" } },
        { "region10", { {},         {},            {},           {"c_test1"},            {},     false, false, "other_region.txt" } }
    };

    CHECK(regionManager.CountRegions() == expectedRegions.size());

    for (auto [regionName, data] : expectedRegions) {
        REQUIRE(regionManager.HasRegion(regionName));
        Region* region = regionManager.GetRegion(regionName);
        REQUIRE(region != nullptr);
        CHECK(region->GetName() == regionName);

        for (const std::string& otherRegionName : data.regions) {
            REQUIRE(regionManager.HasRegion(otherRegionName));
            CHECK(region->HasRegion(regionManager.GetRegion(otherRegionName)));
        }
        
        for (const std::string& titleName : data.kingdoms) {
            REQUIRE(titleManager.HasTitle(titleName));
            CHECK(region->HasTitle(titleManager.GetTitle(titleName)));
        }
        
        for (const std::string& titleName : data.duchies) {
            REQUIRE(titleManager.HasTitle(titleName));
            CHECK(region->HasTitle(titleManager.GetTitle(titleName)));
        }
        
        for (const std::string& titleName : data.counties) {
            REQUIRE(titleManager.HasTitle(titleName));
            CHECK(region->HasTitle(titleManager.GetTitle(titleName)));
        }
        
        for (int provinceId : data.provinces) {
            REQUIRE(provinceManager.HasProvinceById(provinceId));
            CHECK(region->HasProvince(provinceManager.GetProvinceById(provinceId)));
        }

        CHECK(region->DoesGenerateModifiers() == data.doesGenerateModifiers);
        CHECK(region->ShouldRememberCountiesOrder() == data.shouldRememberCountiesOrder);
    }

    // Check that the vanilla override files have been stored.
    SUBCASE("vanilla overrides") {
        const std::string expectedExportedData =
            "﻿# Vanilla Overrides\r\n"
            "\r\n"
            "#tgp_ba_region = {\r\n"
            "#\tkingdoms = {\r\n"
            "#\t\tk_dongchuan k_xingyuan\r\n"
            "#\t}\r\n"
            "#}\r\n"
            "#tgp_bao_region = {\r\n"
            "#\tkingdoms = {\r\n"
            "#\t\tk_khotan\r\n"
            "#\t}\r\n"
            "#}";

        REQUIRE(regionManager.GetVanillaOverrideFiles().size() == 1);
        
        REQUIRE(regionManager.GetVanillaOverrideFiles().contains("tgp_chinesenaming_regions.txt"));
        CHECK(regionManager.GetVanillaOverrideFiles().at("tgp_chinesenaming_regions.txt") == expectedExportedData);
    }
}
 
TEST_CASE("[RegionManager] ExportGeographicalRegions") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/region_manager/test_mod_modified");

    Mod mod("resources/tests/region_manager/test_mod");
    RegionManager regionManager(mod);
    ProvinceManager provinceManager(mod);
    TitleManager titleManager(mod);

    REQUIRE_NOTHROW(provinceManager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(titleManager.LoadTitles(provinceManager));
    REQUIRE_NOTHROW(regionManager.LoadGeographicalRegions(provinceManager, titleManager));

    regionManager.GetRegion("region1")->AddDuchy(titleManager.GetTitleAs<DuchyTitle>("d_test2"));
    regionManager.GetRegion("region1")->RemoveCounty(titleManager.GetTitleAs<CountyTitle>("c_test2"));

    mod.SetRootDirectory("resources/tests/region_manager/test_mod_modified");
    REQUIRE_NOTHROW(regionManager.ExportGeographicalRegions());

    SUBCASE("Check that the exported content match") {
        const std::string expectedExportedData = R"(﻿region1 = {
	generate_modifiers = yes
	should_remember_counties_order = yes
	kingdoms = {  k_test }
	duchies = {   d_test1 d_test2 }
	counties = {  c_test1 }
	provinces = {    1 2 3 }
}

region2 = {
	counties = {  c_test1 }
	regions = {  region1 }
}

region3 = {
	regions = {  region1 }
}

)";

        std::ifstream exportedFile(mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, "geographical_region.txt"));
        REQUIRE(exportedFile.is_open());
        std::string exportedData = File::ReadString(exportedFile);
        CHECK(exportedData == expectedExportedData);
    }

    SUBCASE("Check that the exported content match of the other files") {
        const std::string expectedExportedData = R"(﻿region10 = {
	counties = {  c_test1 }
}

)";

        std::ifstream exportedFile(mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, "other_region.txt"));
        REQUIRE(exportedFile.is_open());
        std::string exportedData = File::ReadString(exportedFile);
        CHECK(exportedData == expectedExportedData);
    }

    SUBCASE("vanilla overrides") {
        const std::string expectedExportedData = R"(﻿# Vanilla Overrides

#tgp_ba_region = {
#	kingdoms = {
#		k_dongchuan k_xingyuan
#	}
#}
#tgp_bao_region = {
#	kingdoms = {
#		k_khotan
#	}
#})";

        std::ifstream exportedFile(mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, "tgp_chinesenaming_regions.txt"));
        REQUIRE(exportedFile.is_open());
        CHECK(File::ReadString(exportedFile) == expectedExportedData);
    }

    SUBCASE("UTF-8 BOM encoding") {
        std::string expectedPath = mod.GetAbsolutePath(Paths::MAP_DATA_GEOGRAPHICAL_REGIONS, "geographical_region.txt");

        // Check that the file exists and is a regular file.
        REQUIRE(std::filesystem::exists(expectedPath));
        CHECK(std::filesystem::is_regular_file(expectedPath));

        // Check that the file is UTF-8 BOM encoded by checking the first 3 bytes.
        std::ifstream file(expectedPath, std::ios::binary);
        char bom[3];
        file.read(bom, 3);
        CHECK(bom[0] == static_cast<char>(0xEF));
        CHECK(bom[1] == static_cast<char>(0xBB));
        CHECK(bom[2] == static_cast<char>(0xBF));
    }
}

}