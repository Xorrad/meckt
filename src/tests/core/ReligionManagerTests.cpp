#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "religions/ReligionManager.hpp"

TEST_SUITE("[ReligionManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] CountRites") {
    Mod mod("");
    ReligionManager religionManager(mod);

    REQUIRE(religionManager.CountRites() == 0);
}

TEST_CASE("[ReligionManager] HasRite") {
    Mod mod("");
    ReligionManager religionManager(mod);

    REQUIRE_FALSE(religionManager.HasRite("test_rite"));
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] GetRite") {
    Mod mod("");
    ReligionManager religionManager(mod);

    REQUIRE(religionManager.GetRite("test_rite") == nullptr);
    REQUIRE(std::as_const(religionManager).GetRite("test_rite") == nullptr);

    religionManager.AddRite(std::make_unique<Rite>("test_rite", sf::Color::Red));

    REQUIRE(religionManager.GetRite("test_rite") != nullptr);
    CHECK(religionManager.GetRite("test_rite")->GetName() == "test_rite");

    REQUIRE(std::as_const(religionManager).GetRite("test_rite") != nullptr);
    CHECK(std::as_const(religionManager).GetRite("test_rite")->GetName() == "test_rite");
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] AddRite") {
    Mod mod("");
    ReligionManager religionManager(mod);

    religionManager.AddRite(std::make_unique<Rite>("test_rite", sf::Color::Red));
    REQUIRE(religionManager.HasRite("test_rite"));
    CHECK(religionManager.GetRite("test_rite")->GetName() == "test_rite");
    CHECK(religionManager.GetRite("test_rite")->GetColor() == sf::Color::Red);
}

TEST_CASE("[ReligionManager] RemoveRite") {
    Mod mod("");
    ReligionManager religionManager(mod);

    religionManager.AddRite(std::make_unique<Rite>("test_rite", sf::Color::Red));
    REQUIRE(religionManager.HasRite("test_rite"));

    religionManager.RemoveRite("test_rite");
    REQUIRE_FALSE(religionManager.HasRite("test_rite"));
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] LoadRites") {
    Mod mod("resources/tests/religion_manager/test_mod");
    ReligionManager religionManager(mod);

    REQUIRE_NOTHROW(religionManager.LoadRites());

    struct RiteTestData {
        std::string name;
        sf::Color color;
    };
    const std::vector<RiteTestData> expectedRites {
        { "roman_rite", sf::Color(128, 30, 153) },
        { "insular_celtic", sf::Color(38, 76, 76) },
        { "mozarabic_church", sf::Color(124, 255, 142) },
        { "ambrosian_rite", sf::Color(63, 49, 76) },
        { "beneventan_rite", sf::Color(76, 26, 71) },
        { "carolingian_christianity", sf::Color(95, 65, 216) }
    };

    CHECK(religionManager.CountRites() == expectedRites.size());

    for (auto& expectedRite : expectedRites) {
         REQUIRE(religionManager.HasRite(expectedRite.name));
         CHECK(religionManager.GetRite(expectedRite.name)->GetName() == expectedRite.name);
         CHECK(religionManager.GetRite(expectedRite.name)->GetColor().r == expectedRite.color.r);
         CHECK(religionManager.GetRite(expectedRite.name)->GetColor().g == expectedRite.color.g);
         CHECK(religionManager.GetRite(expectedRite.name)->GetColor().b == expectedRite.color.b);
    }
}

}