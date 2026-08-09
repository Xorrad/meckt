#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "util/Yaml.hpp"
#include "../TestUtil.hpp"

// Shared test-data shapes and verification helpers, reused by the load/export test pairs below
// so each pair doesn't redeclare the same struct and re-write the same assertion loop.
namespace {

struct ProvinceTestData {
    int id;
    sf::Color color;
    std::string name;
};

void CheckProvinces(ProvinceManager& manager, const std::vector<ProvinceTestData>& expected) {
    for (const auto& data : expected) {
        Province* province = manager.GetProvinceById(data.id);
        REQUIRE(province != nullptr);
        CHECK_EQ(province->GetColor(), data.color);
        CHECK_EQ(province->GetName(), data.name);
    }
}

struct ProvinceTerrainTestData {
    int id;
    std::string terrain;
};

void CheckProvincesTerrain(ProvinceManager& manager, const std::vector<ProvinceTerrainTestData>& expected) {
    for (const auto& data : expected) {
        Province* province = manager.GetProvinceById(data.id);
        REQUIRE(province != nullptr);
        CHECK_EQ(province->GetTerrain(), data.terrain);
    }
}

void CheckVanillaOverrideProvinceTerrainFiles(ProvinceManager& manager, bool checkFilesExist = false) {
    REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().size() == 2);

    REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("00_province_terrain.txt"));
    CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("00_province_terrain.txt") == "\xEF\xBB\xBF");

    REQUIRE(manager.GetVanillaOverrideProvinceTerrainFiles().contains("01_province_properties.txt"));
    CHECK(manager.GetVanillaOverrideProvinceTerrainFiles().at("01_province_properties.txt") == "\xEF\xBB\xBF# Vanilla Overrides");

    if (checkFilesExist) {
        CHECK(std::filesystem::exists("resources/tests/province_manager/test_mod_modified/common/province_terrain/00_province_terrain.txt"));
        CHECK(std::filesystem::exists("resources/tests/province_manager/test_mod_modified/common/province_terrain/01_province_properties.txt"));
    }
}

struct ProvinceClimateTestData {
    int id;
    ClimateType climate;
    std::string bias = "";
    std::string mildFactor = "";
    std::string normalFactor = "";
    std::string harshFactor = "";
};

void CheckProvincesClimate(ProvinceManager& manager, const std::vector<ProvinceClimateTestData>& expected) {
    for (const auto& data : expected) {
        Province* province = manager.GetProvinceById(data.id);
        REQUIRE(province != nullptr);
        CHECK_EQ(province->GetClimateType(), data.climate);
        CHECK_EQ(province->GetWinterSeverityBias(), data.bias);
        CHECK_EQ(province->GetMildWinterFactorOverride(), data.mildFactor);
        CHECK_EQ(province->GetNormalWinterFactorOverride(), data.normalFactor);
        CHECK_EQ(province->GetHarshWinterFactorOverride(), data.harshFactor);
    }
}

struct ProvinceHistoryTestData {
    int id;
    std::string culture;
    std::string religion;
    std::string holding;
    std::string extra;
    std::map<Jomini::Date, std::string> history;
};

void CheckProvincesHistory(ProvinceManager& manager, const std::vector<ProvinceHistoryTestData>& expected) {
    for (const auto& data : expected) {
        Province* province = manager.GetProvinceById(data.id);
        REQUIRE(province != nullptr);
        CHECK_EQ(province->GetCulture(), data.culture);
        CHECK_EQ(province->GetFaith(), data.religion);
        CHECK_EQ(province->GetHolding(), data.holding);
        CHECK_EQ(province->GetExtraHistoryData()->Serialize(0, true, true), data.extra);

        CHECK_EQ(province->GetHistory().size(), data.history.size());
        for (const auto& [date, content] : data.history) {
            REQUIRE(province->GetHistory().contains(date));
            CHECK_EQ(province->GetHistory().at(date)->Serialize(0, true, true), content);
        }
    }
}

struct AdjacencyTestData {
    std::pair<int, int> id;
    int fromId;
    int toId;
    std::string type;
    int throughId;
    sf::Vector2u start;
    sf::Vector2u stop;
    std::string comment;
};

void CheckAdjacencies(ProvinceManager& manager, const std::vector<AdjacencyTestData>& expected) {
    for (const auto& data : expected) {
        REQUIRE(manager.HasAdjacency(data.fromId, data.toId));
        Adjacency* adjacency = manager.GetAdjacencyByIds(data.fromId, data.toId);
        REQUIRE(adjacency != nullptr);
        CHECK_EQ(adjacency->GetId(), data.id);
        CHECK_EQ(adjacency->GetFromId(), data.fromId);
        CHECK_EQ(adjacency->GetToId(), data.toId);
        CHECK_EQ(adjacency->GetType(), data.type);
        CHECK_EQ(adjacency->GetThroughId(), data.throughId);
        CHECK_EQ(adjacency->GetStart(), data.start);
        CHECK_EQ(adjacency->GetStop(), data.stop);
        CHECK_EQ(adjacency->GetComment(), data.comment);
    }
}

}

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

TEST_CASE("[ProvinceManager] HasAdjacency") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK_FALSE(manager.HasAdjacency(1, 2));

    manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), ""));

    CHECK(manager.HasAdjacency(1, 2));
    CHECK(manager.HasAdjacency(2, 1));
    CHECK_FALSE(manager.HasAdjacency(1, 3));
    CHECK_FALSE(manager.HasAdjacency(3, 1));
    CHECK_FALSE(manager.HasAdjacency(2, 3));
    CHECK_FALSE(manager.HasAdjacency(3, 2));
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

TEST_CASE("[ProvinceManager] GetAdjacencyByIds") {
    Mod mod("");
    ProvinceManager manager(mod);

    CHECK(manager.GetAdjacencyByIds(1, 2) == nullptr);
    CHECK(manager.GetAdjacencyByIds(2, 1) == nullptr);
    CHECK(std::as_const(manager).GetAdjacencyByIds(1, 2) == nullptr);
    CHECK(std::as_const(manager).GetAdjacencyByIds(2, 1) == nullptr);

    manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), "crossing 1"));

    Adjacency* adjacency = manager.GetAdjacencyByIds(1, 2);
    REQUIRE(adjacency != nullptr);
    CHECK_EQ(adjacency->GetComment(), "crossing 1");
    CHECK_EQ(manager.GetAdjacencyByIds(1, 2), manager.GetAdjacencyByIds(2, 1));

    const Adjacency* constAdjacency = std::as_const(manager).GetAdjacencyByIds(1, 2);
    REQUIRE(constAdjacency != nullptr);
    CHECK_EQ(constAdjacency->GetComment(), "crossing 1");
    CHECK_EQ(std::as_const(manager).GetAdjacencyByIds(1, 2), std::as_const(manager).GetAdjacencyByIds(2, 1));
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

TEST_CASE("[ProvinceManager] AddAdjacency") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on adding a nullptr adjacency") {
        REQUIRE_NOTHROW(manager.AddAdjacency(nullptr));
        CHECK(manager.GetAdjacencies().size() == 0);
    }

    SUBCASE("Add a new adjacency") {
        manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), ""));
        CHECK(manager.GetAdjacencies().size() == 1);
        CHECK(manager.HasAdjacency(1, 2));
        CHECK(manager.HasAdjacency(2, 1));
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

TEST_CASE("[ProvinceManager] RemoveAdjacency") {
    Mod mod("");
    ProvinceManager manager(mod);

    SUBCASE("No crashes on removing a nullptr") {
        REQUIRE_NOTHROW(manager.RemoveAdjacency(nullptr));
    }

    SUBCASE("No crashes on removing a non-existing adjacency") {
        UniquePtr<Adjacency> adjacency = MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), "");
        REQUIRE_NOTHROW(manager.RemoveAdjacency(adjacency.get()));
        REQUIRE_NOTHROW(manager.RemoveAdjacency(1, 2));
    }

    SUBCASE("Remove an adjacency") {
        manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), ""));
        manager.RemoveAdjacency(manager.GetAdjacencyByIds(1, 2));
        CHECK_FALSE(manager.HasAdjacency(1, 2));

        manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), ""));
        manager.RemoveAdjacency(1, 2);
        CHECK_FALSE(manager.HasAdjacency(1, 2));
        CHECK_FALSE(manager.HasAdjacency(2, 1));

        manager.AddAdjacency(MakeUnique<Adjacency>(1, 2, "sea", 3, sf::Vector2u(0, 0), sf::Vector2u(0, 0), ""));
        manager.RemoveAdjacency(2, 1);
        CHECK_FALSE(manager.HasAdjacency(1, 2));
        CHECK_FALSE(manager.HasAdjacency(2, 1));
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
            "plains", "sea", "coastal_sea", "farmlands", "hills", "mountains",
            "desert", "desert_mountains", "oasis", "jungle", "forest", "taiga",
            "wetlands", "steppe", "floodplains", "drylands", "terraced_hills"
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
        CheckProvinces(manager, {
            {1, sf::Color(1, 1, 1), "TEST1"},
            {2, sf::Color(2, 2, 2), "TEST2"}
        });
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);

    SUBCASE("Load provinces from a valid definition file") {
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());

        CHECK_EQ(manager.CountProvinces(), 8);
        CheckProvinces(manager, {
            {1, sf::Color(1, 1, 1), "TEST1"},
            {2, sf::Color(2, 2, 2), "TEST2"},
            {3, sf::Color(3, 3, 3), "TEST3"},
            {4, sf::Color(4, 4, 4), "TEST4"},
            {5, sf::Color(5, 5, 5), "TEST5"},
            {6, sf::Color(6, 6, 6), "TEST6"},
            {7, sf::Color(7, 7, 7), "TEST7"},
            {8, sf::Color(8, 8, 8), "TEST8"}
        });
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

    // A tiny helper local to this test case: checks that every province in `ids` has `flag` set.
    auto CheckFlag = [&](std::vector<int> ids, ProvinceFlags flag) {
        for (int provinceId : ids) {
            REQUIRE(manager.HasProvinceById(provinceId));
            CHECK(manager.GetProvinceById(provinceId)->HasFlag(flag));
        }
    };

    SUBCASE("Check sea zones") {
        CheckFlag({1, 2, 5}, ProvinceFlags::SEA);
    }

    SUBCASE("Check rivers") {
        CheckFlag({1, 2, 5}, ProvinceFlags::RIVER);
    }

    SUBCASE("Check lakes") {
        CheckFlag({1, 2, 5}, ProvinceFlags::LAKE);
    }

    SUBCASE("Check impassable mountains") {
        CheckFlag({6, 7, 8}, ProvinceFlags::IMPASSABLE);
        CheckFlag({6, 7, 8}, ProvinceFlags::LAND);
    }

    SUBCASE("Check impassable seas") {
        CheckFlag({1, 2, 5}, ProvinceFlags::IMPASSABLE);
        CheckFlag({1, 2, 5}, ProvinceFlags::SEA);
    }

    SUBCASE("Check that other provinces have LAND flag by default") {
        CheckFlag({3, 4}, ProvinceFlags::LAND);
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
        CheckProvincesTerrain(manager, {
            {1, "mountains"},
            {2, "taiga"},
            {3, "plains"},
            {4, "hills"},
            {5, "sea"}
        });
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainFileName() == "terrain.txt");
    }

    // Check that the vanilla override files have been stored.
    SUBCASE("vanilla overrides") {
        CheckVanillaOverrideProvinceTerrainFiles(manager);
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

    SUBCASE("Check that provinces have the correct climate types and factors") {
        CheckProvincesClimate(manager, {
            {1, ClimateType::MILD_WINTER, "0.1", "0.2", "0.3", "0.4"},
            {2, ClimateType::NORMAL_WINTER, "0.2"},
            {3, ClimateType::SEVERE_WINTER, "0.3"},
            {4, ClimateType::MILD_WINTER, "@the_alps"},
            {5, ClimateType::NORMAL_WINTER}
        });
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
        CheckProvincesHistory(manager, {
            {1, "breton", "catholic", "castle_holding", "", {}},
            {2, "french", "insular", "none", "", {{Jomini::Date(1104, 1, 1), "holding = city_holding culture = breton"}}},
            {3, "czech", "slavic_pagan", "castle_holding", "", {}},
            {4, "breton", "catholic", "church_holding", "", {}},
            {5, "breton", "catholic", "castle_holding", "special_building_slot = kutna_hora_mines_01", {}}
        });
    }

    SUBCASE("Check that the variables are saved") {
        const std::string filePath = "00_k_test_prov.txt";

        REQUIRE(manager.GetProvincesHistoryVariables().contains(filePath));
        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Is(Jomini::Type::OBJECT));

        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Contains("@test"));
        CHECK(manager.GetProvincesHistoryVariables().at(filePath)->Get("@test")->As<std::string>() == "1.0");
    }
}

TEST_CASE("[ProvinceManager] LoadAdjacencies") {
    SUBCASE("No exception if there are no adjacencies file") {
        Mod mod("resources/tests/province_manager/test_mod_no_definitions");
        ProvinceManager manager(mod);
        TitleManager titleManager(mod);

        REQUIRE_NOTHROW(manager.LoadAdjacencies(titleManager));
    }

    Mod mod("resources/tests/province_manager/test_mod");
    ProvinceManager manager(mod);
    TitleManager titleManager(mod);

    // Load the provinces.
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadAdjacencies(titleManager));

    SUBCASE("Check that adjacencies have the correct data") {
        CheckAdjacencies(manager, {
            {{1, 2}, 1, 2, "sea", 1019, sf::Vector2u(655, 3608), sf::Vector2u(668, 3617), "Crossing 1"},
            {{1, 5}, 5, 1, "river_large", 699, sf::Vector2u(669, 3577), sf::Vector2u(698, 3584), "Crossing 2"}
        });
    }
}

//////////////////////////////////////////////////////

TEST_CASE("[ProvinceManager] ExportProvincesDefinition") {
    // Removes the temporary export directory if it already exists from a previous test.
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

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
    CheckProvinces(manager, {
        {1, sf::Color(1, 1, 1), "TEST1_MODIFIED"},
        {2, sf::Color(10, 10, 10), "TEST2"},
        {4, sf::Color(4, 4, 4), "TEST4"},
        {5, sf::Color(5, 5, 5), "TEST5"},
        {6, sf::Color(6, 6, 6), "TEST6"}
    });
}

TEST_CASE("[ProvinceManager] ExportDefaultMapFile") {
    // Removes the temporary export directory if it already exists from a previous test.
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

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
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

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
        CheckProvincesTerrain(manager, {
            {1, "hills"},
            {2, "mountains"},
            {3, "taiga"},
            {4, "plains"},
            {5, "plains"}
        });
    }

    SUBCASE("main file name") {
        CHECK(manager.GetProvinceTerrainFileName() == "terrain.txt");
    }

    // Check that the vanilla override files have been stored.
    SUBCASE("vanilla overrides") {
        CheckVanillaOverrideProvinceTerrainFiles(manager, /* checkFilesExist */ true);
    }
}

TEST_CASE("[ProvinceManager] ExportProvincesClimate") {
    // Removes the temporary export directory if it already exists from a previous test.
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

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
        CheckProvincesClimate(manager, {
            {1, ClimateType::MILD_WINTER, "0.5", "0.6", "0.7", "0.8"},
            {2, ClimateType::NORMAL_WINTER, "0.2"},
            {3, ClimateType::SEVERE_WINTER, "0.3"},
            {4, ClimateType::MILD_WINTER, "@the_alps"},
            {5, ClimateType::NORMAL_WINTER}
        });
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
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

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
        CheckProvincesHistory(manager, {
            {1, "modified_culture", "modified_religion", "modified_holding", "extra = modified_extra", {{Jomini::Date(1104, 1, 1), "holding = modified_holding"}}},
            {2, "french", "insular", "none", "", {{Jomini::Date(1104, 1, 1), "holding = city_holding culture = breton"}}},
            {3, "czech", "slavic_pagan", "castle_holding", "", {}},
            {4, "breton", "catholic", "church_holding", "", {}},
            {5, "breton", "catholic", "castle_holding", "special_building_slot = kutna_hora_mines_01", {}}
        });
    }

    SUBCASE("Check that the variables are saved") {
        const std::string filePath = "00_k_test_prov.txt";

        REQUIRE(manager.GetProvincesHistoryVariables().contains(filePath));
        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Is(Jomini::Type::OBJECT));

        REQUIRE(manager.GetProvincesHistoryVariables().at(filePath)->Contains("@test"));
        CHECK(manager.GetProvincesHistoryVariables().at(filePath)->Get("@test")->As<std::string>() == "1.0");
    }
}

TEST_CASE("[ProvinceManager] ExportAdjacencies") {
    // Removes the temporary export directory if it already exists from a previous test.
    TestUtil::ResetDirectory("resources/tests/province_manager/test_mod_modified");

    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/province_manager/test_mod");
    TitleManager titleManager(mod);
    REQUIRE(std::filesystem::exists(mod.GetRootDirectory()));

    {
        ProvinceManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
        REQUIRE_NOTHROW(manager.LoadAdjacencies(titleManager));

        // Edit some adjacencies.
        manager.GetAdjacencyByIds(1, 2)->SetThroughId(10);
        manager.GetAdjacencyByIds(1, 2)->SetComment("CROSSING MODIFIED");

        manager.AddAdjacency(MakeUnique<Adjacency>(100, 22, "river_large", 999, sf::Vector2u(11, 67), sf::Vector2u(50, 41), "NEW ADJACENCY !!!!"));

        // 2. Export the adjacencies.
        mod.SetRootDirectory("resources/tests/province_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportAdjacencies());
    }

    // Reload the adjacencies definition.
    ProvinceManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadProvincesDefinition());
    REQUIRE_NOTHROW(manager.LoadAdjacencies(titleManager));

    // 3. Asserts
    CheckAdjacencies(manager, {
        {{1, 2}, 1, 2, "sea", 10, sf::Vector2u(655, 3608), sf::Vector2u(668, 3617), "CROSSING MODIFIED"},
        {{1, 5}, 5, 1, "river_large", 699, sf::Vector2u(669, 3577), sf::Vector2u(698, 3584), "Crossing 2"},
        {{22, 100}, 100, 22, "river_large", 999, sf::Vector2u(11, 67), sf::Vector2u(50, 41), "NEW ADJACENCY !!!!"}
    });
}

}
