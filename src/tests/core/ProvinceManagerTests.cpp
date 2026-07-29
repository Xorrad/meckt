#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "util/Yaml.hpp"

TEST_SUITE("[ProvinceManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[ProvinceManager] CountProvinces") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK_EQ(manager.CountProvinces(), 0);

    manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
    CHECK_EQ(manager.CountProvinces(), 1);

    manager.AddProvince(MakeUnique<Province>(2, sf::Color::Green, "2"));
    manager.AddProvince(MakeUnique<Province>(3, sf::Color::Blue, "3"));
    CHECK_EQ(manager.CountProvinces(), 3);
}

TEST_CASE("[ProvinceManager] HasProvinceByColor") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK_FALSE(manager.HasProvinceByColor(sf::Color::Red.toInteger()));

    manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
    CHECK(manager.HasProvinceByColor(sf::Color::Red.toInteger()));
    CHECK_FALSE(manager.HasProvinceByColor(sf::Color::Green.toInteger()));
}

TEST_CASE("[ProvinceManager] HasProvinceById") {
    Mod mod("");
    ProvinceManager manager(mod);
    
    CHECK_FALSE(manager.HasProvinceById(1));
    
    manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
    CHECK(manager.HasProvinceById(1));
    CHECK_FALSE(manager.HasProvinceById(2));
}

TEST_CASE("[ProvinceManager] HasHoldingType") {
    Mod mod("");
    ProvinceManager manager(mod);
    
    CHECK_FALSE(manager.HasHoldingType("castle_holding"));  
    
    manager.GetHoldingTypes().insert("castle_holding", HoldingType("castle_holding"));
    CHECK(manager.HasHoldingType("castle_holding"));
    CHECK_FALSE(manager.HasHoldingType("city_holding"));
}

TEST_CASE("[ProvinceManager] HasTerrainType") {
    Mod mod("");
    ProvinceManager manager(mod);
    
    CHECK_FALSE(manager.HasTerrainType("plains"));  
    
    manager.GetTerrainTypes().insert("plains", TerrainType("plains", sf::Color::Green));
    CHECK(manager.HasTerrainType("plains"));
    CHECK_FALSE(manager.HasTerrainType("sea"));
}

TEST_CASE("[ProvinceManager] GetProvinceByColor") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK(manager.GetProvinceByColor(sf::Color::Red.toInteger()) == nullptr);
    CHECK(std::as_const(manager).GetProvinceByColor(sf::Color::Red.toInteger()) == nullptr);

    manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
    
    Province* province = manager.GetProvinceByColor(sf::Color::Red.toInteger());
    REQUIRE(province != nullptr);
    CHECK_EQ(province->GetId(), 1);
    CHECK_EQ(province->GetName(), "1");

    const Province* constProvince = std::as_const(manager).GetProvinceByColor(sf::Color::Red.toInteger());
    REQUIRE(constProvince != nullptr);
    CHECK_EQ(constProvince->GetId(), 1);
    CHECK_EQ(constProvince->GetName(), "1");
}

TEST_CASE("[ProvinceManager] GetProvinceById") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK(manager.GetProvinceById(1) == nullptr);
    CHECK(std::as_const(manager).GetProvinceById(1) == nullptr);

    manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));

    Province* province = manager.GetProvinceById(1);
    REQUIRE(province != nullptr);
    CHECK_EQ(province->GetColor(), sf::Color::Red);
    CHECK_EQ(province->GetName(), "1");

    const Province* constProvince = std::as_const(manager).GetProvinceById(1);
    REQUIRE(constProvince != nullptr);
    CHECK_EQ(constProvince->GetColor(), sf::Color::Red);
    CHECK_EQ(constProvince->GetName(), "1");
}

//////////////////////////////////////////////////////

TEST_CASE("[ProvinceManager] AddProvince") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on adding a nullptr province") {
        REQUIRE_NOTHROW(manager.AddProvince(nullptr));
        CHECK_EQ(manager.CountProvinces(), 0);

    }

    SUBCASE("Add a new province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        CHECK(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Add a province with a color already taken by another province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.AddProvince(MakeUnique<Province>(2, sf::Color::Red, "2"));

        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(2));
        CHECK(manager.GetProvinceById(2)->GetColor() == sf::Color::Red);
        CHECK(manager.GetProvinceById(2)->GetName() == "2");
    }

    SUBCASE("Add a province with an id already taken by another province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Green, "2"));

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvincesByColors().contains(sf::Color::Green.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvinceById(1)->GetColor() == sf::Color::Green);
        CHECK(manager.GetProvinceById(1)->GetName() == "2");
    }
}

TEST_CASE("[ProvinceManager] RemoveProvinceByColor") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on removing a non-existing province") {
        REQUIRE_NOTHROW(manager.RemoveProvinceByColor(sf::Color::Red.toInteger()));
    }

    SUBCASE("Remove a province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.RemoveProvinceByColor(sf::Color::Red.toInteger());

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Assigned baronies are correctly unassigned") {
        // TODO: check that assigned barony title of the former province is unassigned.
    }
}

TEST_CASE("[ProvinceManager] RemoveProvinceById") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on removing a non-existing province") {
        REQUIRE_NOTHROW(manager.RemoveProvinceById(1));
    }

    SUBCASE("Remove a province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.RemoveProvinceById(1);

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Assigned baronies are correctly unassigned") {
        // TODO: check that assigned barony title of the former province is unassigned.
    }
}

TEST_CASE("[ProvinceManager] RemoveProvince") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on removing a nullptr") {
        REQUIRE_NOTHROW(manager.RemoveProvince(nullptr));
    }
    
    SUBCASE("No crashes on removing a non-existing province") {
        UniquePtr<Province> province = MakeUnique<Province>(2, sf::Color::Green, "2");
        REQUIRE_NOTHROW(manager.RemoveProvince(province.get()));
    }

    SUBCASE("Remove a province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.RemoveProvince(manager.GetProvinceById(1));

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Assigned baronies are correctly unassigned") {
        // TODO: check that assigned barony title of the former province is unassigned.
    }
}

TEST_CASE("[ProvinceManager] RenameProvinceColor") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on renaming a non-existing province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Green, "1"));

        REQUIRE_NOTHROW(manager.RenameProvinceColor(sf::Color::Red.toInteger(), sf::Color::Green.toInteger()));

        CHECK(manager.GetProvincesByColors().contains(sf::Color::Green.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Rename a province's color") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));

        manager.RenameProvinceColor(sf::Color::Red.toInteger(), sf::Color::Green.toInteger());

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvincesByColors().contains(sf::Color::Green.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvinceById(1)->GetColor() == sf::Color::Green);
    }

    SUBCASE("Renaming delete any existing province with the new color") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.AddProvince(MakeUnique<Province>(2, sf::Color::Green, "2"));

        manager.RenameProvinceColor(sf::Color::Red.toInteger(), sf::Color::Green.toInteger());

        CHECK_FALSE(manager.GetProvincesByIds().contains(2));
        CHECK(manager.GetProvinceById(1)->GetColor() == sf::Color::Green);
    }
}

TEST_CASE("[ProvinceManager] RenameProvinceId") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No changes on invalid id") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Green, "1"));
        
        REQUIRE_NOTHROW(manager.RenameProvinceId(1, 0));
        CHECK_FALSE(manager.GetProvincesByIds().contains(0));
        CHECK(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvinceById(1)->GetId() == 1);

        REQUIRE_NOTHROW(manager.RenameProvinceId(1, -1));
        CHECK_FALSE(manager.GetProvincesByIds().contains(-1));
        CHECK(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvinceById(1)->GetId() == 1);
    }

    SUBCASE("No crashes on renaming a non-existing province") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Green, "1"));

        REQUIRE_NOTHROW(manager.RenameProvinceId(2, 1));

        CHECK(manager.GetProvincesByColors().contains(sf::Color::Green.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(1));
    }

    SUBCASE("Rename a province's id") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));

        manager.RenameProvinceId(1, 2);

        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvincesByIds().contains(2));
        CHECK(manager.GetProvinceById(2)->GetColor() == sf::Color::Red);
    }

    SUBCASE("Renaming delete any existing province with the new id") {
        manager.AddProvince(MakeUnique<Province>(1, sf::Color::Red, "1"));
        manager.AddProvince(MakeUnique<Province>(2, sf::Color::Green, "2"));

        manager.RenameProvinceId(1, 2);

        CHECK_FALSE(manager.GetProvincesByColors().contains(sf::Color::Green.toInteger()));
        CHECK_FALSE(manager.GetProvincesByIds().contains(1));
        CHECK(manager.GetProvincesByColors().contains(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvincesByIds().contains(2));
        CHECK(manager.GetProvinceById(2) == manager.GetProvinceByColor(sf::Color::Red.toInteger()));
        CHECK(manager.GetProvinceById(2)->GetId() == 2);
        CHECK(manager.GetProvinceById(2)->GetColor() == sf::Color::Red);
        CHECK(manager.GetProvinceById(2)->GetName() == "1");
    }

}

//////////////////////////////////////////////////////

TEST_CASE("[ProvinceManager] LoadHoldingTypes") {
    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    REQUIRE_NOTHROW(manager.LoadHoldingTypes());

    SUBCASE("Vanilla holding types are loaded by default") {
        CHECK(manager.HasHoldingType("none"));
        CHECK(manager.HasHoldingType("tribal_holding"));
        CHECK(manager.HasHoldingType("castle_holding"));
        CHECK(manager.HasHoldingType("city_holding"));
        CHECK(manager.HasHoldingType("church_holding"));
        CHECK(manager.HasHoldingType("herder_holding"));
        CHECK(manager.HasHoldingType("nomad_holding"));
    }

    SUBCASE("Check that custom holding types are loaded") {
        CHECK(manager.HasHoldingType("test1_holding"));
        CHECK(manager.HasHoldingType("test2_holding"));
    }
}

TEST_CASE("[ProvinceManager] LoadTerrainTypes") {
    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    REQUIRE_NOTHROW(manager.LoadTerrainTypes());

    SUBCASE("Vanilla terrain types are loaded by default") {
        const std::vector<std::string> vanillaTypes = {
            "plains",
            "sea",
            "coastal_sea",
            "farmlands",
            "hills",
            "mountains",
            "desert",
            "desert_mountains",
            "oasis",
            "jungle",
            "forest",
            "taiga",
            "wetlands",
            "steppe",
            "floodplains",
            "drylands"
        };
        for (const std::string& type : vanillaTypes) {
            CHECK(manager.HasTerrainType(type));
        }
    }

    SUBCASE("Check that custom terrain types are loaded") {
        REQUIRE(manager.HasTerrainType("test1"));
        CHECK(manager.GetTerrainTypes().at("test1").GetColor() == sf::Color(127, 127, 127));

        REQUIRE(manager.HasTerrainType("test2"));
        CHECK(manager.GetTerrainTypes().at("test2").GetColor() == sf::Color(25, 25, 25));
    }
    
    SUBCASE("Check that custom terrain types overrides vanilla terrain types") {
        REQUIRE(manager.HasTerrainType("plains"));
        CHECK(manager.GetTerrainTypes().at("plains").GetColor() == sf::Color(229, 229, 229));
    }
}

TEST_CASE("[ProvinceManager] LoadProvincesDefinition") {
    SUBCASE("No crashes if there is no definition file") {
        Mod mod("resources/tests/province_manager/test_mod_no_definitions");
        ProvinceManager manager(mod);

        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
        CHECK_EQ(manager.CountProvinces(), 0);
    }

    SUBCASE("Throws exception if the definition file is invalid") {
        Mod mod("resources/tests/province_manager/test_mod_invalid_definitions");
        ProvinceManager manager(mod);

        // Load the provinces.
        REQUIRE_THROWS_AS(manager.LoadProvincesDefinition(), std::exception);

        // Check that provinces were loaded before the invalid data.
        CHECK_EQ(manager.CountProvinces(), 2);
        
        CHECK(manager.HasProvinceById(1));
        CHECK(manager.GetProvinceById(1)->GetColor() == sf::Color(1, 1, 1));
        CHECK(manager.GetProvinceById(1)->GetName() == "TEST1");

        CHECK(manager.HasProvinceById(2));
        CHECK(manager.GetProvinceById(2)->GetColor() == sf::Color(2, 2, 2));
        CHECK(manager.GetProvinceById(2)->GetName() == "TEST2");
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    SUBCASE("Load provinces from a valid definition file") {
        // Initialize the expected provinces data.
        struct ProvinceTestData {
            int id;
            sf::Color color;
            std::string name;
        };
        const std::vector<ProvinceTestData> expectedProvinces = {
            {1, sf::Color(1, 1, 1), "TEST1"},
            {2, sf::Color(2, 2, 2), "TEST2"},
            {3, sf::Color(3, 3, 3), "TEST3"},
            {4, sf::Color(4, 4, 4), "TEST4"},
            {5, sf::Color(5, 5, 5), "TEST5"},
            {6, sf::Color(6, 6, 6), "TEST6"},
            {7, sf::Color(7, 7, 7), "TEST7"},
            {8, sf::Color(8, 8, 8), "TEST8"}
        };

        // Load the provinces.
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());

        // Check that the loaded provinces match the expected data.
        CHECK_EQ(manager.CountProvinces(), 8);

        for (const auto& expected : expectedProvinces) {
            const Province* province = manager.GetProvinceById(expected.id);
            REQUIRE(province != nullptr);
            CHECK_EQ(province->GetColor(), expected.color);
            CHECK_EQ(province->GetName(), expected.name);
        }
    }
}

TEST_CASE("[ProvinceManager] LoadProvincesImage") {
    SUBCASE("Throws exception if there is no provinces image file") {
        Mod mod("resources/tests/province_manager/test_mod_no_provinces_image");
        ProvinceManager manager(mod);

        REQUIRE_THROWS_AS(manager.LoadProvincesImage(), std::exception);
    }
    
    SUBCASE("Throws exception if the provinces image file isn't an image or invalid") {
        Mod mod("resources/tests/province_manager/test_mod_invalid_provinces_image");
        ProvinceManager manager(mod);
        
        // Disable sfml error output to avoid printing expected error messages to the console.
        sf::err().rdbuf(NULL);

        REQUIRE_THROWS_AS(manager.LoadProvincesImage(), std::exception);

        // Reenable sfml error output for other test cases.
        sf::err().rdbuf(std::cerr.rdbuf());
    }

    // TODO: add test cases for missing colors and transparent pixels once the "error system" is revamped.
}

TEST_CASE("[ProvinceManager] LoadDefaultMapFile") {
SUBCASE("Throws exception if there is no default map file") {
        Mod mod("resources/tests/province_manager/test_mod_no_default_map");
        ProvinceManager manager(mod);

        REQUIRE_THROWS_AS(manager.LoadDefaultMapFile(), std::exception);
    }
    
    SUBCASE("Throws exception if the default map file is invalid") {
        Mod mod("resources/tests/province_manager/test_mod_invalid_default_map");
        ProvinceManager manager(mod);

        REQUIRE_THROWS_AS(manager.LoadDefaultMapFile(), std::exception);
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    // Load the provinces.
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadDefaultMapFile());

    SUBCASE("Check sea zones") {
        std::vector<int> seaZoneProvinceIds = {1, 2, 5};

        for (int provinceId : seaZoneProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::SEA));
        }
    }
    
    SUBCASE("Check rivers") {
        std::vector<int> riverProvinceIds = {1, 2, 5};

        for (int provinceId : riverProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::RIVER));
        }
    }
    
    SUBCASE("Check lakes") {
        std::vector<int> lakeProvinceIds = {1, 2, 5};

        for (int provinceId : lakeProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::LAKE));
        }
    }
    
    SUBCASE("Check impassable mountains") {
        std::vector<int> mountainProvinceIds = {6, 7, 8};

        for (int provinceId : mountainProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::IMPASSABLE));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::LAND));
        }
    }
    
    SUBCASE("Check impassable seas") {
        std::vector<int> seaProvinceIds = {1, 2, 5};

        for (int provinceId : seaProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::IMPASSABLE));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::SEA));
        }
    }

    SUBCASE("Check that other provinces have LAND flag by default") {
        std::vector<int> otherProvinceIds = {3, 4};

        for (int provinceId : otherProvinceIds) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(ProvinceFlags::LAND));
        }
    }
}

TEST_CASE("[ProvinceManager] LoadProvincesTerrain") {
    SUBCASE("No exception if there is no province terrain file") {
        Mod mod("resources/tests/province_manager/test_mod_no_definitions");
        ProvinceManager manager(mod);

        REQUIRE_NOTHROW(manager.LoadProvincesTerrain());
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    // Load the provinces.
    REQUIRE_NOTHROW(manager.LoadTerrainTypes());
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadDefaultMapFile());
    REQUIRE_NOTHROW(manager.LoadProvincesTerrain());

    SUBCASE("Check that default terrain types are defined") {
        CHECK(manager.GetDefaultLandTerrain() == "plains");
        CHECK(manager.GetDefaultSeaTerrain() == "sea");
        CHECK(manager.GetDefaultCoastalSeaTerrain() == "coastal_sea");
    }

    SUBCASE("Check that provinces have the correct terrain types") {
        struct ProvinceTerrainTestData {
            int id;
            std::string expectedTerrain;
        };
        const std::vector<ProvinceTerrainTestData> testData = {
            {1, "mountains"},
            {2, "taiga"},
            {3, "plains"},
            {4, "hills"},
            {5, "sea"}
        };

        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetTerrain(), data.expectedTerrain);
        }
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainFileName() == "terrain.txt");
    }

    // Check that the vanilla override files have been stored.
    SUBCASE("vanilla overrides") {
        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().size() == 2);

        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("00_province_terrain.txt"));
        CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("00_province_terrain.txt") == "\xEF\xBB\xBF");
        
        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("01_province_properties.txt"));
        CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("01_province_properties.txt") == "\xEF\xBB\xBF# Vanilla Overrides");
    }
}

TEST_CASE("[ProvinceManager] LoadProvincesClimate") {
    SUBCASE("No exception if there is no province terrain file") {
        Mod mod("resources/tests/province_manager/test_mod_no_definitions");
        ProvinceManager manager(mod);

        REQUIRE_NOTHROW(manager.LoadProvincesClimate());
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    // Load the provinces.
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadDefaultMapFile());
    REQUIRE_NOTHROW(manager.LoadProvincesClimate());

    SUBCASE("Check that provinces have the correct climate types") {
        struct ProvinceClimateTestData {
            int id;
            ClimateType expectedClimate;
        };
        const std::vector<ProvinceClimateTestData> testData = {
            {1, ClimateType::MILD_WINTER},
            {2, ClimateType::NORMAL_WINTER},
            {3, ClimateType::SEVERE_WINTER},
            {4, ClimateType::MILD_WINTER},
            {5, ClimateType::NORMAL_WINTER}
        };

        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetClimateType(), data.expectedClimate);
        }
    }

    SUBCASE("Check that provinces have the correct climate severity factors") {
        struct ProvinceClimateFactorTestData {
            int id;
            std::string expectedFactor;
            std::string expectedMildFactor;
            std::string expectedNormalFactor;
            std::string expectedHarshFactor;
        };
        const std::vector<ProvinceClimateFactorTestData> testData = {
            {1, "0.1", "0.2", "0.3", "0.4"},
            {2, "0.2", "", "", ""},
            {3, "0.3", "", "", ""},
            {4, "@the_alps", "", "", ""},
            {5, "", "", "", ""}
        };

        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetWinterSeverityBias(), data.expectedFactor);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetMildWinterFactorOverride(), data.expectedMildFactor);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetNormalWinterFactorOverride(), data.expectedNormalFactor);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetHarshWinterFactorOverride(), data.expectedHarshFactor);
        }
    }

    SUBCASE("Check that the properties file variables are saved") {
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Is(Jomini::Type::OBJECT));
        
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Contains("@azerbaijan_mountains"));
        CHECK(manager.GetProvinceTerrainPropertiesVariables()->Get("@azerbaijan_mountains")->As<std::string>() == "0.40");
        
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Contains("@the_alps"));
        CHECK(manager.GetProvinceTerrainPropertiesVariables()->Get("@the_alps")->As<std::string>() == "0.80");
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainPropertiesFileName() == "winter.txt");
    }
}

TEST_CASE("[ProvinceManager] LoadProvincesHistory") {
    SUBCASE("No exception if there are no province history files") {
        Mod mod("resources/tests/province_manager/test_mod_no_definitions");
        ProvinceManager manager(mod);

        REQUIRE_NOTHROW(manager.LoadProvincesHistory());
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    // Load the provinces.
    REQUIRE_NOTHROW(manager.LoadHoldingTypes());
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadProvincesHistory());

    SUBCASE("Check that provinces have the correct history data") {
        struct ProvinceHistoryTestData {
            int id;
            std::string culture;
            std::string religion;
            std::string holding;
            std::string extra;
            std::map<Jomini::Date, std::string> history;
        };
        const std::vector<ProvinceHistoryTestData> testData = {
            {1, "breton", "catholic", "castle_holding", "", std::map<Jomini::Date, std::string>{}},
            {2, "french", "insular", "none", "", std::map<Jomini::Date, std::string>{std::make_pair(Jomini::Date(1104, 1, 1), "holding = city_holding culture = breton")}},
            {3, "czech", "slavic_pagan", "castle_holding", "", std::map<Jomini::Date, std::string>{}},
            {4, "breton", "catholic", "church_holding", "", std::map<Jomini::Date, std::string>{}},
            {5, "breton", "catholic", "castle_holding", "special_building_slot = kutna_hora_mines_01", std::map<Jomini::Date, std::string>{}}
        };

        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetCulture(), data.culture);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetFaith(), data.religion);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetHolding(), data.holding);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetExtraHistoryData()->Serialize(0, true, true), data.extra);

            CHECK_EQ(manager.GetProvinceById(data.id)->GetHistory().size(), data.history.size());
            for (const auto& [date, content] : data.history) {
                REQUIRE(manager.GetProvinceById(data.id)->GetHistory().contains(date));
                CHECK_EQ(manager.GetProvinceById(data.id)->GetHistory().at(date)->Serialize(0, true, true), content);
            }
        }
    }

    SUBCASE("Check that the variables are saved") {
        const std::string filePath = "00_k_test_prov.txt";

        REQUIRE(manager.GetProvincesHistoryVariables().contains(filePath));
        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Is(Jomini::Type::OBJECT));

        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Contains("@test"));
        CHECK(manager.GetProvincesHistoryVariables().at(filePath)->Get("@test")->As<std::string>() == "1.0");
    }
}

//////////////////////////////////////////////////////

TEST_CASE("[ProvinceManager] ExportProvincesDefinition") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/province_manager/test_mod_modified");

    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));

    {
        ProvinceManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());

        // Edit some provinces.
        manager.GetProvinceById(1)->SetName("TEST1_MODIFIED");
        manager.GetProvinceById(2)->SetColor(sf::Color(10, 10, 10));
        manager.AddProvince(MakeUnique<Province>(6, sf::Color(6, 6, 6), "TEST6"));

        // 2. Export the provinces definition.
        mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportProvincesDefinition());
    }

    // Reload the provinces definition.
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());

    // 3. Asserts
    struct ProvinceTestData {
        int id;
        sf::Color color;
        std::string name;
    };
    const std::vector<ProvinceTestData> testData = {
        {1, sf::Color(1, 1, 1), "TEST1_MODIFIED"},
        {2, sf::Color(10, 10, 10), "TEST2"},
        {4, sf::Color(4, 4, 4), "TEST4"},
        {5, sf::Color(5, 5, 5), "TEST5"},
        {6, sf::Color(6, 6, 6), "TEST6"}
    };

    for (const auto& data : testData) {
        REQUIRE(manager.HasProvinceById(data.id));
        CHECK_EQ(manager.GetProvinceById(data.id)->GetColor(), data.color);
        CHECK_EQ(manager.GetProvinceById(data.id)->GetName(), data.name);
    }
}

TEST_CASE("[ProvinceManager] ExportDefaultMapFile") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/province_manager/test_mod_modified");
    
    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));
    
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadDefaultMapFile());
    
    // Edit some provinces.
    manager.GetProvinceById(1)->SetFlags(ProvinceFlags::IMPASSABLE);
    manager.GetProvinceById(2)->SetFlags(ProvinceFlags::RIVER | ProvinceFlags::LAKE);
    manager.GetProvinceById(5)->SetFlags(ProvinceFlags::NONE);
    
    // 2. Export the default map file.
    mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
    REQUIRE_NOTHROW(manager.ExportDefaultMapFile());
    
    // Reload the provinces flags.
    REQUIRE_NOTHROW(manager.LoadDefaultMapFile());
    
    // 3. Asserts
    struct ProvinceFlagTestData {
        int id;
        ProvinceFlags expectedFlags;
    };
    const std::vector<ProvinceFlagTestData> testData = {
        {1, ProvinceFlags::LAND | ProvinceFlags::IMPASSABLE},
        {2, ProvinceFlags::RIVER | ProvinceFlags::LAKE},
        {3, ProvinceFlags::LAND},
        {4, ProvinceFlags::LAND},
        {5, ProvinceFlags::LAND},
        {6, ProvinceFlags::LAND | ProvinceFlags::IMPASSABLE},
        {7, ProvinceFlags::LAND | ProvinceFlags::IMPASSABLE},
        {8, ProvinceFlags::LAND | ProvinceFlags::IMPASSABLE}
    };
    
    for (const auto& data : testData) {
        REQUIRE(manager.HasProvinceById(data.id));
        CHECK_EQ(manager.GetProvinceById(data.id)->GetFlags(), data.expectedFlags);
    }
}

TEST_CASE("[ProvinceManager] ExportProvincesTerrain") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/province_manager/test_mod_modified");
    
    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));
    
    {
        ProvinceManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadTerrainTypes());
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
        REQUIRE_NOTHROW(manager.LoadProvincesTerrain());
        
        // Only land provinces can have terrain.
        for (auto& [id, province] : manager.GetProvincesByIds()) {
            province->SetFlags(ProvinceFlags::LAND);
        }

        manager.GetProvinceById(1)->SetTerrain("hills");
        manager.GetProvinceById(2)->SetTerrain("mountains");
        manager.GetProvinceById(3)->SetTerrain("taiga");
        manager.GetProvinceById(4)->SetTerrain("");
        manager.GetProvinceById(5)->SetTerrain("plains");
        
        // 2. Export the provinces terrain.
        mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportProvincesDefinition());
        REQUIRE_NOTHROW(manager.ExportProvincesTerrain());
    }
    
    // Reload the provinces.
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadTerrainTypes());
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadProvincesTerrain());
    
    // 3. Asserts
    SUBCASE("terrains") {
        struct ProvinceTerrainTestData {
            int id;
            std::string terrain;
        };
        const std::vector<ProvinceTerrainTestData> testData = {
            {1, "hills"},
            {2, "mountains"},
            {3, "taiga"},
            {4, "plains"},
            {5, "plains"}
        };
        
        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetTerrain(), data.terrain);
        }
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainFileName() == "terrain.txt");
    }

    // Check that the vanilla override files have been stored.
    SUBCASE("vanilla overrides") {
        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().size() == 2);

        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("00_province_terrain.txt"));
        CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("00_province_terrain.txt") == "\xEF\xBB\xBF");
        CHECK(std::filesystem::exists("resources/tests/province_manager/test_mod_modified/common/province_terrain/00_province_terrain.txt"));
        
        REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("01_province_properties.txt"));
        CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("01_province_properties.txt") == "\xEF\xBB\xBF# Vanilla Overrides");
        CHECK(std::filesystem::exists("resources/tests/province_manager/test_mod_modified/common/province_terrain/01_province_properties.txt"));
    }
}

TEST_CASE("[ProvinceManager] ExportProvincesClimate") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/province_manager/test_mod_modified");
    
    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));
    
    {
        ProvinceManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
        REQUIRE_NOTHROW(manager.LoadProvincesClimate());
        
        manager.GetProvinceById(1)->SetClimateType(ClimateType::MILD_WINTER);
        manager.GetProvinceById(1)->SetWinterSeverityBias("0.5");
        manager.GetProvinceById(1)->SetMildWinterFactorOverride("0.6");
        manager.GetProvinceById(1)->SetNormalWinterFactorOverride("0.7");
        manager.GetProvinceById(1)->SetHarshWinterFactorOverride("0.8");
        
        manager.GetProvinceById(2)->SetClimateType(ClimateType::NORMAL_WINTER);
        
        manager.GetProvinceById(3)->SetClimateType(ClimateType::SEVERE_WINTER);
        
        manager.GetProvinceById(4)->SetClimateType(ClimateType::MILD_WINTER);
        
        manager.GetProvinceById(5)->SetClimateType(ClimateType::NORMAL_WINTER);
        
        // 2. Export the provinces climate.
        mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportProvincesDefinition());
        REQUIRE_NOTHROW(manager.ExportProvincesClimate());
    }
    
    // Reload the provinces.
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadProvincesClimate());
    
    // 3. Asserts
    SUBCASE("Check that provinces have the correct climate types and factors") {
        struct ProvinceClimateTestData {
            int id;
            ClimateType expectedClimate;
            std::string expectedWinterSeverityBias;
            std::string expectedMildWinterFactorOverride;
            std::string expectedNormalWinterFactorOverride;
            std::string expectedHarshWinterFactorOverride;
        };
        const std::vector<ProvinceClimateTestData> testData = {
            {1, ClimateType::MILD_WINTER, "0.5", "0.6", "0.7", "0.8"},
            {2, ClimateType::NORMAL_WINTER, "0.2", "", "", ""},
            {3, ClimateType::SEVERE_WINTER, "0.3", "", "", ""},
            {4, ClimateType::MILD_WINTER, "@the_alps", "", "", ""},
            {5, ClimateType::NORMAL_WINTER, "", "", "", ""}
        };
        
        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetClimateType(), data.expectedClimate);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetWinterSeverityBias(), data.expectedWinterSeverityBias);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetMildWinterFactorOverride(), data.expectedMildWinterFactorOverride);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetNormalWinterFactorOverride(), data.expectedNormalWinterFactorOverride);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetHarshWinterFactorOverride(), data.expectedHarshWinterFactorOverride);
        }
    }

    SUBCASE("Check that the properties file variables are saved") {
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Is(Jomini::Type::OBJECT));
        
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Contains("@azerbaijan_mountains"));
        CHECK(manager.GetProvinceTerrainPropertiesVariables()->Get("@azerbaijan_mountains")->As<std::string>() == "0.40");
        
        REQUIRE(manager.GetProvinceTerrainPropertiesVariables()->Contains("@the_alps"));
        CHECK(manager.GetProvinceTerrainPropertiesVariables()->Get("@the_alps")->As<std::string>() == "0.80");
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainPropertiesFileName() == "winter.txt");
    }
}

TEST_CASE("[ProvinceManager] ExportProvincesHistory") {
    // Removes the temporary export directory if it already exists from a previous test.
    std::filesystem::remove_all("resources/tests/province_manager/test_mod_modified");
    
    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));
    
    {
        TitleManager titleManager(mod);

        ProvinceManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadHoldingTypes());
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
        REQUIRE_NOTHROW(manager.LoadProvincesHistory());
        REQUIRE_NOTHROW(titleManager.LoadTitles(manager));
        
        manager.GetProvinceById(1)->SetCulture("modified_culture");
        manager.GetProvinceById(1)->SetFaith("modified_religion");
        manager.GetProvinceById(1)->SetHolding("modified_holding");

        auto extraHistory = MakeShared<Jomini::Object>(Jomini::Type::OBJECT);
        extraHistory->Put("extra", "modified_extra", Jomini::Operator::EQUAL);
        manager.GetProvinceById(1)->SetExtraHistoryData(extraHistory);

        auto newDate = MakeShared<Jomini::Object>(Jomini::Type::OBJECT);
        newDate->Put("holding", "modified_holding", Jomini::Operator::EQUAL);
        manager.GetProvinceById(1)->AddHistory(Jomini::Date(1104, 1, 1), newDate);
        
        // 2. Export the provinces history.
        mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportProvincesDefinition());
        REQUIRE_NOTHROW(manager.ExportProvincesHistory(titleManager));
    }
    
    // Reload the provinces.
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadHoldingTypes());
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadProvincesHistory());
    
    // 3. Asserts
    SUBCASE("Check that provinces have the correct history data") {
        struct ProvinceHistoryTestData {
            int id;
            std::string culture;
            std::string religion;
            std::string holding;
            std::string extra;
            std::map<Jomini::Date, std::string> history;
        };
        const std::vector<ProvinceHistoryTestData> testData = {
            {1, "modified_culture", "modified_religion", "modified_holding", "extra = modified_extra", std::map<Jomini::Date, std::string>{{Jomini::Date(1104, 1, 1), "holding = modified_holding"}}},
            {2, "french", "insular", "none", "", std::map<Jomini::Date, std::string>{{Jomini::Date(1104, 1, 1), "holding = city_holding culture = breton"}}},
            {3, "czech", "slavic_pagan", "castle_holding", "", std::map<Jomini::Date, std::string>{}},
            {4, "breton", "catholic", "church_holding", "", std::map<Jomini::Date, std::string>{}},
            {5, "breton", "catholic", "castle_holding", "special_building_slot = kutna_hora_mines_01", std::map<Jomini::Date, std::string>{}}
        };

        for (const auto& data : testData) {
            REQUIRE(manager.HasProvinceById(data.id));
            CHECK_EQ(manager.GetProvinceById(data.id)->GetCulture(), data.culture);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetFaith(), data.religion);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetHolding(), data.holding);
            CHECK_EQ(manager.GetProvinceById(data.id)->GetExtraHistoryData()->Serialize(0, true, true), data.extra);

            CHECK_EQ(manager.GetProvinceById(data.id)->GetHistory().size(), data.history.size());
            for (const auto& [date, content] : data.history) {
                REQUIRE(manager.GetProvinceById(data.id)->GetHistory().contains(date));
                CHECK_EQ(manager.GetProvinceById(data.id)->GetHistory().at(date)->Serialize(0, true, true), content);
            }
        }
    }

    SUBCASE("Check that the variables are saved") {
        const std::string filePath = "00_k_test_prov.txt";

        REQUIRE(manager.GetProvincesHistoryVariables().contains(filePath));
        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Is(Jomini::Type::OBJECT));

        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Contains("@test"));
        CHECK(manager.GetProvincesHistoryVariables().at(filePath)->Get("@test")->As<std::string>() == "1.0");
    }
}

}