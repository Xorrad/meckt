#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "cultures/CultureManager.hpp"

TEST_SUITE("[CultureManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[CultureManager] CountCultures") {
    Mod mod("");
    CultureManager cultureManager(mod);

    REQUIRE(cultureManager.CountCultures() == 0);
}

TEST_CASE("[CultureManager] HasCulture") {
    Mod mod("");
    CultureManager cultureManager(mod);

    REQUIRE_FALSE(cultureManager.HasCulture("test_culture"));
}

//////////////////////////////////////////////////////

TEST_CASE("[CultureManager] GetCulture") {
    Mod mod("");
    CultureManager cultureManager(mod);

    REQUIRE(cultureManager.GetCulture("test_culture") == nullptr);
    REQUIRE(std::as_const(cultureManager).GetCulture("test_culture") == nullptr);

    cultureManager.AddCulture(std::make_unique<Culture>("test_culture", sf::Color::Red));

    REQUIRE(cultureManager.GetCulture("test_culture") != nullptr);
    CHECK(cultureManager.GetCulture("test_culture")->GetName() == "test_culture");

    REQUIRE(std::as_const(cultureManager).GetCulture("test_culture") != nullptr);
    CHECK(std::as_const(cultureManager).GetCulture("test_culture")->GetName() == "test_culture");
}

//////////////////////////////////////////////////////

TEST_CASE("[CultureManager] AddCulture") {
    Mod mod("");
    CultureManager cultureManager(mod);

    cultureManager.AddCulture(std::make_unique<Culture>("test_culture", sf::Color::Red));
    REQUIRE(cultureManager.HasCulture("test_culture"));
    CHECK(cultureManager.GetCulture("test_culture")->GetName() == "test_culture");
    CHECK(cultureManager.GetCulture("test_culture")->GetColor() == sf::Color::Red);
}

TEST_CASE("[CultureManager] RemoveCulture") {
    Mod mod("");
    CultureManager cultureManager(mod);

    cultureManager.AddCulture(std::make_unique<Culture>("test_culture", sf::Color::Red));
    REQUIRE(cultureManager.HasCulture("test_culture"));

    cultureManager.RemoveCulture("test_culture");
    REQUIRE_FALSE(cultureManager.HasCulture("test_culture"));
}

//////////////////////////////////////////////////////

TEST_CASE("[CultureManager] LoadCultures") {
    Mod mod("resources/tests/culture_manager/test_mod");
    CultureManager cultureManager(mod);

    REQUIRE_NOTHROW(cultureManager.LoadCultures());

    struct CultureTestData {
        std::string name;
        sf::Color color;
    };
    const std::vector<CultureTestData> expectedCultures {
        { "pictish", sf::Color(0, 95, 183) },
        { "welsh", sf::Color(255, 255, 255) },
        { "breton", sf::Color(114, 77, 193) },
        { "cornish", sf::Color(108, 73, 183) },
        { "cumbrian", sf::Color(73, 91, 183) },
    };

    CHECK(cultureManager.CountCultures() == expectedCultures.size());

    for (auto& expectedCulture : expectedCultures) {
         REQUIRE(cultureManager.HasCulture(expectedCulture.name));
         CHECK(cultureManager.GetCulture(expectedCulture.name)->GetName() == expectedCulture.name);
         CHECK(cultureManager.GetCulture(expectedCulture.name)->GetColor() == expectedCulture.color);
    }
}

}