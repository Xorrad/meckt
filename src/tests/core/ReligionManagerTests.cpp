#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "religions/ReligionManager.hpp"

TEST_SUITE("[ReligionManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] CountFaiths") {
    Mod mod("");
    ReligionManager faithManager(mod);

    REQUIRE(faithManager.CountFaiths() == 0);
}

TEST_CASE("[ReligionManager] HasFaith") {
    Mod mod("");
    ReligionManager faithManager(mod);

    REQUIRE_FALSE(faithManager.HasFaith("test_faith"));
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] GetFaith") {
    Mod mod("");
    ReligionManager faithManager(mod);

    REQUIRE(faithManager.GetFaith("test_faith") == nullptr);
    REQUIRE(std::as_const(faithManager).GetFaith("test_faith") == nullptr);

    faithManager.AddFaith(std::make_unique<Faith>("test_faith", sf::Color::Red));

    REQUIRE(faithManager.GetFaith("test_faith") != nullptr);
    CHECK(faithManager.GetFaith("test_faith")->GetName() == "test_faith");

    REQUIRE(std::as_const(faithManager).GetFaith("test_faith") != nullptr);
    CHECK(std::as_const(faithManager).GetFaith("test_faith")->GetName() == "test_faith");
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] AddFaith") {
    Mod mod("");
    ReligionManager faithManager(mod);

    faithManager.AddFaith(std::make_unique<Faith>("test_faith", sf::Color::Red));
    REQUIRE(faithManager.HasFaith("test_faith"));
    CHECK(faithManager.GetFaith("test_faith")->GetName() == "test_faith");
    CHECK(faithManager.GetFaith("test_faith")->GetColor() == sf::Color::Red);
}

TEST_CASE("[ReligionManager] RemoveFaith") {
    Mod mod("");
    ReligionManager faithManager(mod);

    faithManager.AddFaith(std::make_unique<Faith>("test_faith", sf::Color::Red));
    REQUIRE(faithManager.HasFaith("test_faith"));

    faithManager.RemoveFaith("test_faith");
    REQUIRE_FALSE(faithManager.HasFaith("test_faith"));
}

//////////////////////////////////////////////////////

TEST_CASE("[ReligionManager] LoadFaiths") {
    Mod mod("resources/tests/religion_manager/test_mod");
    ReligionManager religionManager(mod);

    REQUIRE_NOTHROW(religionManager.LoadFaiths());

    struct FaithTestData {
        std::string name;
        sf::Color color;
    };
    const std::vector<FaithTestData> expectedFaiths {
        { "catholic", sf::Color(128, 30, 153) },
        { "orthodox", sf::Color(127, 127, 127) },
        { "coptic", sf::Color(210, 235, 87) },
        { "armenian_apostolic", sf::Color(23, 15, 25) },
        { "conversos", sf::Color(102, 204, 142) },
        { "cathar", sf::Color(114, 229, 229) }
    };

    CHECK(religionManager.CountFaiths() == expectedFaiths.size());

    for (auto& expectedFaith : expectedFaiths) {
         REQUIRE(religionManager.HasFaith(expectedFaith.name));
         CHECK(religionManager.GetFaith(expectedFaith.name)->GetName() == expectedFaith.name);
         CHECK(religionManager.GetFaith(expectedFaith.name)->GetColor() == expectedFaith.color);
    }
}

}