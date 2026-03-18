#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "map/titles/TitleManager.hpp"

TEST_SUITE("[TitleManager]") {

TEST_CASE("[TitleManager] CountTitles") {
    Mod mod("");
    TitleManager manager(mod);

    CHECK_EQ(manager.CountTitles(), 0);
    CHECK_EQ(manager.CountTitles(TitleType::BARONY), 0);
    CHECK_EQ(manager.CountTitles(TitleType::COUNTY), 0);
    CHECK_EQ(manager.CountTitles(TitleType::DUCHY), 0);
    CHECK_EQ(manager.CountTitles(TitleType::KINGDOM), 0);
    CHECK_EQ(manager.CountTitles(TitleType::EMPIRE), 0);
    CHECK_EQ(manager.CountTitles(TitleType::HEGEMONY), 0);
    
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false));
    
    CHECK_EQ(manager.CountTitles(), 1);
    CHECK_EQ(manager.CountTitles(TitleType::COUNTY), 1);
    
    manager.AddTitle(MakeTitle(TitleType::BARONY, "b_test", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test2", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::DUCHY, "d_test", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::KINGDOM, "k_test", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::EMPIRE, "e_test", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::HEGEMONY, "h_test", sf::Color::Yellow, false));

    CHECK_EQ(manager.CountTitles(), 7);
    CHECK_EQ(manager.CountTitles(TitleType::BARONY), 1);
    CHECK_EQ(manager.CountTitles(TitleType::COUNTY), 2);
    CHECK_EQ(manager.CountTitles(TitleType::DUCHY), 1);
    CHECK_EQ(manager.CountTitles(TitleType::KINGDOM), 1);
    CHECK_EQ(manager.CountTitles(TitleType::EMPIRE), 1);
    CHECK_EQ(manager.CountTitles(TitleType::HEGEMONY), 1);
}

TEST_CASE("[TitleManager] HasTitle: title doesn't exist") {
    Mod mod("");
    TitleManager manager(mod);

    //3. Asserts
    // Check that it returns false when the title doesn't exist.
    CHECK_FALSE(manager.HasTitle("b_test"));
    CHECK_FALSE(manager.HasTitle("c_test"));
    CHECK_FALSE(manager.HasTitle("d_test"));
    CHECK_FALSE(manager.HasTitle("k_test"));
    CHECK_FALSE(manager.HasTitle("e_test"));
    CHECK_FALSE(manager.HasTitle("h_test"));
    CHECK_FALSE(manager.HasTitle(""));
}

TEST_CASE("[TitleManager] HasTitle: title exists") {
    Mod mod("");
    TitleManager manager(mod);
    
    // 1. Initialize and add the title.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false));
    REQUIRE(manager.GetTitles().contains("c_test"));

    // 3. Asserts
    // Check that it returns true.
    CHECK(manager.HasTitle("c_test"));
}

TEST_CASE("[TitleManager] GetTitle: nullptr when title doesn't exist") {
    Mod mod("");
    TitleManager manager(mod);
    
    // 3. Asserts
    // Check that the function returns nullptr on non-existing titles. 
    Title* title = manager.GetTitle("c_test");
    CHECK_EQ(title, nullptr);
    
    // Check that the const function returns nullptr on non-existing titles.
    const Title* constTitle = std::as_const(manager).GetTitle("c_test");
    CHECK_EQ(constTitle, nullptr);
}

TEST_CASE("[TitleManager] GetTitle") {
    Mod mod("");
    TitleManager manager(mod);

    // 1. Initialize and add the title.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false));
    REQUIRE(manager.HasTitle("c_test"));

    // 2. Get the titles using the normal and const versions of the function.
    Title* title = manager.GetTitle("c_test");
    const Title* constTitle = std::as_const(manager).GetTitle("c_test");

    // 3. Asserts
    // Check that the normal version returns a valid title pointer.
    REQUIRE(title != nullptr);
    CHECK_EQ(title->GetName(), "c_test");
    
    // Check that the const version returns a valid title pointer.
    REQUIRE(constTitle != nullptr);
    CHECK_EQ(constTitle->GetName(), "c_test");
}

TEST_CASE("[TitleManager] GetTitleAs: nullptr when title doesn't exist") {
    Mod mod("");
    TitleManager manager(mod);
    
    // 3. Asserts
    // Check that the function returns nullptr on non-existing titles. 
    CountyTitle* title = manager.GetTitleAs<CountyTitle>("c_test");
    CHECK_EQ(title, nullptr);
    
    // Check that the const function returns nullptr on non-existing titles.
    const CountyTitle* constTitle = std::as_const(manager).GetTitleAs<CountyTitle>("c_test");
    CHECK_EQ(constTitle, nullptr);
}

TEST_CASE("[TitleManager] GetTitleAs") {
    Mod mod("");
    TitleManager manager(mod);

    // 1. Initialize and add the title.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false));
    REQUIRE(manager.HasTitle("c_test"));

    // 2. Get the titles using the normal and const versions of the function.
    CountyTitle* title = manager.GetTitleAs<CountyTitle>("c_test");
    const CountyTitle* constTitle = std::as_const(manager).GetTitleAs<CountyTitle>("c_test");

    // 3. Asserts
    // Check that the normal version returns a valid title pointer.
    REQUIRE(title != nullptr);
    CHECK_EQ(title->GetName(), "c_test");
    
    // Check that the const version returns a valid title pointer.
    REQUIRE(constTitle != nullptr);
    CHECK_EQ(constTitle->GetName(), "c_test");

    // Check that both versions return a nullptr when trying to cast to the wrong type.
    CHECK_EQ(manager.GetTitleAs<DuchyTitle>("c_test"), nullptr);
    CHECK_EQ(std::as_const(manager).GetTitleAs<DuchyTitle>("c_test"), nullptr);
}

struct TitleTestData {
    TitleType type;
    std::string name;
    sf::Color color;
    int provinceId = -1;
};

TEST_CASE("[TitleManager] AddTitle") {
    Mod mod("");
    
    std::vector<TitleTestData> testSuite = {
        { TitleType::BARONY,   "b_test",    sf::Color::Yellow, 10  },
        { TitleType::BARONY,   "b_test2",   sf::Color::Blue, 0 },
        { TitleType::COUNTY,   "c_test",    sf::Color::Cyan },
        { TitleType::DUCHY,    "d_test",    sf::Color::Green },
        { TitleType::KINGDOM,  "k_test",    sf::Color::Red },
        { TitleType::EMPIRE,   "e_test",    sf::Color::White },
        { TitleType::HEGEMONY, "h_test",    sf::Color::Magenta }
    };

    for (const auto& data : testSuite) {
        // SUBCASE ensures each title type is tested in a fresh TitleManager instance
        // or a clean state if you move the manager inside the loop.
        SUBCASE(data.name.c_str()) {
            TitleManager manager(mod);
            
            // 1. Initialize the title.
            UniquePtr<Title> title = MakeTitle(data.type, data.name, data.color, false);
            
            // Sets the defined province id for baronies.
            if (data.type == TitleType::BARONY && data.provinceId != -1) {
                static_cast<BaronyTitle*>(title.get())->SetProvinceId(data.provinceId);
            }

            // 2. Add the title.
            manager.AddTitle(std::move(title));

            // 3. Asserts.
            // Check that the title has been registed in the titles map.
            REQUIRE(manager.HasTitle(data.name));
            CHECK_EQ(manager.CountTitles(), 1);
            CHECK_EQ(manager.GetTitle(data.name)->GetName(), data.name);
            
            // Checks that the title has been added to its type titles list.
            const auto& typeList = manager.GetTitlesByType().at(data.type);
            bool foundInList = std::find(typeList.begin(), typeList.end(), manager.GetTitle(data.name)) != typeList.end();
            CHECK(foundInList);

            // Checks that the barony has been registered in the province-barony map.
            if (data.type == TitleType::BARONY && data.provinceId != -1) {
                // Baronies with a null province id should be ignored.
                if (data.provinceId == 0) {
                    CHECK_FALSE(manager.GetBaroniesByProvinceId().contains(data.provinceId));
                }
                else {
                    REQUIRE(manager.GetBaroniesByProvinceId().contains(data.provinceId));
                    CHECK_EQ(manager.GetBaroniesByProvinceId().at(data.provinceId), manager.GetTitleAs<BaronyTitle>(data.name));
                }
            }
        }
    }
}

TEST_CASE("[TitleManager] RemoveTitle") {
    Mod mod("");
    
    std::vector<TitleTestData> testSuite = {
        { TitleType::BARONY,   "b_test",    sf::Color::Yellow, 10 },
        { TitleType::BARONY,   "b_test2",   sf::Color::Blue, 0 },
        { TitleType::COUNTY,   "c_test",    sf::Color::Cyan },
        { TitleType::DUCHY,    "d_test",    sf::Color::Green },
        { TitleType::KINGDOM,  "k_test",    sf::Color::Red },
        { TitleType::EMPIRE,   "e_test",    sf::Color::White },
        { TitleType::HEGEMONY, "h_test",    sf::Color::Magenta }
    };

    for (const auto& data : testSuite) {
        // SUBCASE ensures each title type is tested in a fresh TitleManager instance
        // or a clean state if you move the manager inside the loop.
        SUBCASE(data.name.c_str()) {
            TitleManager manager(mod);
            
            // 1. Initialize and add the title first.
            {
                UniquePtr<Title> title = MakeTitle(data.type, data.name, data.color, false);
                if (data.type == TitleType::BARONY) {
                    static_cast<BaronyTitle*>(title.get())->SetProvinceId(data.provinceId);
                }
                manager.AddTitle(std::move(title));
            }
            REQUIRE(manager.HasTitle(data.name));

            // 2. Remove the title.
            manager.RemoveTitle(data.name);
            
            // 3. Asserts.
            // Check that the title has been correctly deleted from the map.
            CHECK_EQ(manager.CountTitles(), 0);
            CHECK_FALSE(manager.HasTitle(data.name));

            // Check that the title has been removed from its type list.
            const auto& typeList = manager.GetTitlesByType().at(data.type);
            auto foundInList = std::find_if(typeList.begin(), typeList.end(), [&](const Title* t) {
                REQUIRE(t != nullptr);
                return t->GetName() == data.name;
            }) != typeList.end();
            CHECK_FALSE(foundInList);

            // Check that the barony isn't in in the province-barony map anymore.
            if (data.type == TitleType::BARONY) {
                CHECK_FALSE(manager.GetBaroniesByProvinceId().contains(data.provinceId));
            }
        }
    }
}

TEST_CASE("[TitleManager] RemoveTitle: reset capital title") {
    Mod mod("");
    TitleManager manager(mod);

    // 1. Initialize and add the titles first.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::DUCHY, "d_test", sf::Color::Magenta, false));
    manager.AddTitle(MakeTitle(TitleType::KINGDOM, "k_test", sf::Color::Red, false));

    CountyTitle* countyTitle = manager.GetTitleAs<CountyTitle>("c_test");
    manager.GetTitleAs<HighTitle>("d_test")->SetCapitalTitle(countyTitle);
    manager.GetTitleAs<HighTitle>("k_test")->SetCapitalTitle(countyTitle);

    REQUIRE(manager.HasTitle("c_test"));
    REQUIRE(manager.HasTitle("d_test"));
    REQUIRE(manager.HasTitle("k_test"));

    // 2. Remove the county title.
    manager.RemoveTitle("c_test");
            
    // 3. Asserts.
    // Make sure that the county title isn't there anymore.
    REQUIRE_FALSE(manager.HasTitle("c_test"));
    
    // Check that the county title isn't the capital title of d_test and k_test anymore.
    CHECK(manager.GetTitleAs<HighTitle>("d_test")->GetCapitalTitle() == nullptr);
    CHECK(manager.GetTitleAs<HighTitle>("k_test")->GetCapitalTitle() == nullptr);
}

TEST_CASE("[TitleManager] RenameTitle") {
    Mod mod("");
    TitleManager manager(mod);

    // 1. Initialize and add the titles first.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_old", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::DUCHY, "d_test", sf::Color::Magenta, false));

    // Add an history entry mentioning the to-be-renamed title.
    SharedPtr<Jomini::Object> historyObject = MakeShared<Jomini::Object>(Jomini::Type::OBJECT);
    historyObject->Put("dejure_liege", std::string("c_old"));
    manager.GetTitle("d_test")->AddHistory(Jomini::Date(1, 1, 1), historyObject);

    REQUIRE(manager.HasTitle("c_old"));
    REQUIRE(manager.HasTitle("d_test"));

    // 2. Rename the county title.
    manager.RenameTitle("c_old", "c_new");
            
    // 3. Asserts.
    // Make sure that the keys have been changed for the title.
    CHECK_FALSE(manager.HasTitle("c_old"));
    REQUIRE(manager.HasTitle("c_new"));

    // Check that the title's name has been changed.
    CHECK_EQ(manager.GetTitle("c_new")->GetName(), "c_new");
    
    //Check that the title name has been changed in the duchy history.
    const auto& history = manager.GetTitle("d_test")->GetHistory();
    REQUIRE(history.contains(Jomini::Date(1, 1, 1)));
    REQUIRE(history.at(Jomini::Date(1, 1, 1))->Contains("dejure_liege"));
    CHECK_EQ(history.at(Jomini::Date(1, 1, 1))->Get("dejure_liege")->As<std::string>(), "c_new");
}

TEST_CASE("[TitleManager] RenameTitle: throws exception when trying to rename to an already used name") {
    Mod mod("");
    TitleManager manager(mod);

    // 1. Initialize and add the titles first.
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_old", sf::Color::Yellow, false));
    manager.AddTitle(MakeTitle(TitleType::COUNTY, "c_new", sf::Color::Yellow, false));

    REQUIRE(manager.HasTitle("c_old"));
    REQUIRE(manager.HasTitle("c_new"));

    // 2. Rename the title and asserts that it throws an exception.
    CHECK_THROWS_AS(manager.RenameTitle("c_old", "c_new"), std::invalid_argument);
            
    // Check that both titles still exist.
    CHECK(manager.HasTitle("c_old"));
    CHECK(manager.HasTitle("c_new"));

    // Check that no title has been renamed
    CHECK_EQ(manager.GetTitle("c_old")->GetName(), "c_old");
    CHECK_EQ(manager.GetTitle("c_new")->GetName(), "c_new");
}

TEST_CASE("[TitleManager] LoadTitles") {
    Mod mod("resources/tests/title_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetDir()));

    TitleManager manager(mod);
    REQUIRE(manager.CountTitles() == 0);

    // 2. Load the titles from the mod directory.
    REQUIRE_NOTHROW(manager.LoadTitles());

    // 3. Asserts

    // Check that the variables have been stored.
    SUBCASE("variables") {
        std::string fileName = "00_landed_titles.txt";
        REQUIRE(manager.GetTitlesVariables().contains(fileName));
        const SharedPtr<Jomini::Object>& variables = manager.GetTitlesVariables().at(fileName);
        REQUIRE(variables->Contains("@correct_culture_primary_score"));
        CHECK(variables->Get("@correct_culture_primary_score")->As<std::string>() == "100");

        REQUIRE(variables->Contains("@better_than_the_alternatives_score"));
        CHECK(variables->Get("@better_than_the_alternatives_score")->As<std::string>() == "50");

        REQUIRE(variables->Contains("@always_primary_score"));
        CHECK(variables->Get("@always_primary_score")->As<std::string>() == "1000");

        REQUIRE(variables->Contains("@never_primary_score"));
        CHECK(variables->Get("@never_primary_score")->As<std::string>() == "-1000");
    }

    // Check that every title has been successfully parsed and added.
    SUBCASE("titles existence") {
        CHECK(manager.HasTitle("e_test"));
        CHECK(manager.HasTitle("k_test"));

        CHECK(manager.HasTitle("d_test1"));
        CHECK(manager.HasTitle("d_test2"));

        CHECK(manager.HasTitle("c_test1"));
        CHECK(manager.HasTitle("c_test2"));

        CHECK(manager.HasTitle("b_test1"));
        CHECK(manager.HasTitle("b_test2"));
        CHECK(manager.HasTitle("b_test3"));
        CHECK(manager.HasTitle("b_test4"));

        CHECK(manager.HasTitle("k_papal_state"));
    }

    // Check that every title has the correct liege title assigned to it.
    SUBCASE("titles liege") {
        CHECK(manager.GetTitle("b_test1")->GetLiegeTitle() == manager.GetTitle("c_test1"));
        CHECK(manager.GetTitle("b_test2")->GetLiegeTitle() == manager.GetTitle("c_test1"));
        CHECK(manager.GetTitle("c_test1")->GetLiegeTitle() == manager.GetTitle("d_test1"));
        CHECK(manager.GetTitle("d_test1")->GetLiegeTitle() == manager.GetTitle("k_test"));
        
        CHECK(manager.GetTitle("b_test3")->GetLiegeTitle() == manager.GetTitle("c_test2"));
        CHECK(manager.GetTitle("b_test4")->GetLiegeTitle() == manager.GetTitle("c_test2"));
        CHECK(manager.GetTitle("c_test2")->GetLiegeTitle() == manager.GetTitle("d_test2"));
        CHECK(manager.GetTitle("d_test2")->GetLiegeTitle() == manager.GetTitle("k_test"));
        
        CHECK(manager.GetTitle("k_test")->GetLiegeTitle() == manager.GetTitle("e_test"));
        CHECK(manager.GetTitle("e_test")->GetLiegeTitle() == nullptr);
        
        CHECK(manager.GetTitle("k_papal_state")->GetLiegeTitle() == nullptr);
    }

    // Check that liege titles have the correct title in their dejure list.
    SUBCASE("titles dejure") {
        // Helper to check if a parent title contains a specific child in its de jure list.
        auto HasDejure = [&](const std::string& parentName, const std::string& childName) {
            auto* parent = manager.GetTitleAs<HighTitle>(parentName);
            auto* child = manager.GetTitle(childName);
            const auto& dejureList = parent->GetDejureTitles();
            
            return std::find(dejureList.begin(), dejureList.end(), child) != dejureList.end();
        };

        // 1. Counties
        auto* c1 = manager.GetTitleAs<HighTitle>("c_test1");
        REQUIRE(c1->GetDejureTitles().size() == 2);
        CHECK(HasDejure("c_test1", "b_test1"));
        CHECK(HasDejure("c_test1", "b_test2"));
        CHECK(HasDejure("c_test2", "b_test3"));
        CHECK(HasDejure("c_test2", "b_test4"));

        auto* c2 = manager.GetTitleAs<HighTitle>("c_test2");
        REQUIRE(c2->GetDejureTitles().size() == 2);
        CHECK(HasDejure("c_test2", "b_test3"));
        CHECK(HasDejure("c_test2", "b_test4"));

        // 2. Duchies
        auto* d1 = manager.GetTitleAs<HighTitle>("d_test1");
        REQUIRE(d1->GetDejureTitles().size() == 1);
        CHECK(HasDejure("d_test1", "c_test1"));

        auto* d2 = manager.GetTitleAs<HighTitle>("d_test2");
        REQUIRE(d2->GetDejureTitles().size() == 1);
        CHECK(HasDejure("d_test2", "c_test2"));

        // 3. Kingdoms
        auto* k = manager.GetTitleAs<HighTitle>("k_test");
        REQUIRE(k->GetDejureTitles().size() == 2);
        CHECK(HasDejure("k_test", "d_test1"));
        CHECK(HasDejure("k_test", "d_test2"));

        // 4. Empires
        auto* e = manager.GetTitleAs<HighTitle>("e_test");
        REQUIRE(e->GetDejureTitles().size() == 1);
        CHECK(HasDejure("e_test", "k_test"));

        // 5. Independent Titles
        auto* papal = manager.GetTitleAs<HighTitle>("k_papal_state");
        CHECK(papal->GetDejureTitles().empty());
    }

    // Check that the titles have the correct capitals.
    SUBCASE("titles capitals") {
        CHECK_EQ(manager.GetTitleAs<HighTitle>("d_test1")->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test1"));
        CHECK_EQ(manager.GetTitleAs<HighTitle>("d_test2")->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test2"));
        CHECK_EQ(manager.GetTitleAs<HighTitle>("k_test")->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test1"));
        CHECK_EQ(manager.GetTitleAs<HighTitle>("e_test")->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test1"));
        CHECK_EQ(manager.GetTitleAs<HighTitle>("k_papal_state")->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test2"));
    }

    // Check that the papal state landless title has the correct property values.
    SUBCASE("special title properties") {
        HighTitle* papalState = manager.GetTitleAs<HighTitle>("k_papal_state");
        
        CHECK_EQ(papalState->GetName(), "k_papal_state");
        CHECK_EQ(papalState->GetColor(), sf::Color(255, 249, 198));
        CHECK_NE(papalState->GetCapitalTitle(), nullptr);
        CHECK_EQ(papalState->GetCapitalTitle(), manager.GetTitleAs<CountyTitle>("c_test2"));
        CHECK(papalState->IsLandless());
        
        const SharedPtr<Jomini::Object>& papalStateData = papalState->GetOriginalData();
        REQUIRE(papalStateData->Contains("definite_form"));
        CHECK_EQ(papalStateData->Get("definite_form")->As<std::string>(), "yes");

        REQUIRE(papalStateData->Contains("ruler_uses_title_name"));
        CHECK_EQ(papalStateData->Get("ruler_uses_title_name")->As<std::string>(), "no");

        REQUIRE(papalStateData->Contains("can_use_nomadic_naming"));
        CHECK_EQ(papalStateData->Get("can_use_nomadic_naming")->As<std::string>(), "no");

        REQUIRE(papalStateData->Contains("ai_primary_priority"));
        REQUIRE(papalStateData->Get("ai_primary_priority")->Is(Jomini::Type::OBJECT));
        REQUIRE(papalStateData->Get("ai_primary_priority")->Contains("add"));
        CHECK_EQ(papalStateData->Get("ai_primary_priority")->Get("add")->As<std::string>(), "@always_primary_score");
    }

    // Check that the titles have the correct properties (name, color...).
    SUBCASE("titles properties") {
        CHECK_EQ(manager.GetTitle("b_test1")->GetName(), "b_test1");
        CHECK_EQ(manager.GetTitle("b_test1")->GetColor(), sf::Color(20, 20, 20));
        CHECK_EQ(manager.GetTitleAs<BaronyTitle>("b_test1")->GetProvinceId(), 1);

        CHECK_EQ(manager.GetTitle("b_test2")->GetName(), "b_test2");
        CHECK_EQ(manager.GetTitle("b_test2")->GetColor(), sf::Color(20, 20, 20));
        CHECK_EQ(manager.GetTitleAs<BaronyTitle>("b_test2")->GetProvinceId(), 2);

        CHECK_EQ(manager.GetTitle("b_test3")->GetName(), "b_test3");
        CHECK_EQ(manager.GetTitle("b_test3")->GetColor(), sf::Color(50, 50, 50));
        CHECK_EQ(manager.GetTitleAs<BaronyTitle>("b_test3")->GetProvinceId(), 3);

        CHECK_EQ(manager.GetTitle("b_test4")->GetName(), "b_test4");
        CHECK_EQ(manager.GetTitle("b_test4")->GetColor(), sf::Color(50, 50, 50));
        CHECK_EQ(manager.GetTitleAs<BaronyTitle>("b_test4")->GetProvinceId(), 4);
        
        CHECK_EQ(manager.GetTitle("c_test1")->GetName(), "c_test1");
        CHECK_EQ(manager.GetTitle("c_test1")->GetColor(), sf::Color(10, 10, 10));
        
        CHECK_EQ(manager.GetTitle("c_test2")->GetName(), "c_test2");
        CHECK_EQ(manager.GetTitle("c_test2")->GetColor(), sf::Color(40, 40, 40));
        
        CHECK_EQ(manager.GetTitle("d_test1")->GetName(), "d_test1");
        CHECK_EQ(manager.GetTitle("d_test1")->GetColor(), sf::Color(170, 255, 170));
        
        CHECK_EQ(manager.GetTitle("d_test2")->GetName(), "d_test2");
        CHECK_EQ(manager.GetTitle("d_test2")->GetColor(), sf::Color(30, 30, 30));
        
        CHECK_EQ(manager.GetTitle("k_test")->GetName(), "k_test");
        CHECK_EQ(manager.GetTitle("k_test")->GetColor(), sf::Color(244, 227, 160));
        
        CHECK_EQ(manager.GetTitle("e_test")->GetName(), "e_test");
        CHECK_EQ(manager.GetTitle("e_test")->GetColor(), sf::Color(234, 217, 110));
    }
}

TEST_CASE("[TitleManager] LoadTitlesHistory") {
    // 1. Initialize the mod and load the titles.
    Mod mod("resources/tests/title_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetDir()));

    TitleManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadTitles());

    // 2. Load the titles history.
    REQUIRE_NOTHROW(manager.LoadTitlesHistory());

    // 3. Asserts

    // Check that duplicate date entries within the same title are merged.
    SUBCASE("duplicate date entries") {
        REQUIRE(manager.HasTitle("k_test"));
        auto& history = manager.GetTitle("k_test")->GetHistory();

        REQUIRE(history.contains(Jomini::Date(866, 1, 1)));
        // change_development_level = 1 is kept, change_development_level = 2 is ignored. 
        // first and second are both kept as they are unique.
        CHECK(history.at(Jomini::Date(866, 1, 1))->Serialize(0, true, true) == 
            "change_development_level = 1 first = yes second = yes");
    }

    // Check that duplicate properties keep their default behaviour by making a list.
    SUBCASE("duplicate properties in date") {
        REQUIRE(manager.HasTitle("d_test1"));
        auto& history = manager.GetTitle("d_test1")->GetHistory();

        REQUIRE(history.contains(Jomini::Date(866, 1, 1)));
        CHECK(history.at(Jomini::Date(866, 1, 1))->Serialize(0, true, false) == 
            "change_development_level = 1\n"
            "change_development_level = 2\n\n"
            "holder = char1\n"
            "holder = char2\n"
        );
    }

    // Check that non-date keys in the history block are ignored.
    SUBCASE("invalid date entries") {
        REQUIRE(manager.HasTitle("d_test2"));
        auto& history = manager.GetTitle("d_test2")->GetHistory();

        // "test = { ... }" is not a valid date, so the history map should be empty.
        CHECK(history.empty());
    }

    // Check that partial and incomplete dates are correctly parsed.
    SUBCASE("incomplete dates") {
        REQUIRE(manager.HasTitle("c_test1"));
        auto& history = manager.GetTitle("c_test1")->GetHistory();

        // Missing month and day.
        REQUIRE(history.contains(Jomini::Date(1, 1, 1)));
        CHECK(history.at(Jomini::Date(1, 1, 1))->Serialize(0, true, true) == "change_development_level = 1");
        
        // Missing day.
        REQUIRE(history.contains(Jomini::Date(10, 2, 1)));
        CHECK(history.at(Jomini::Date(10, 2, 1))->Serialize(0, true, true) == "change_development_level = 2");
    }

    // Check that duplicate title entries are handled correctly.
    SUBCASE("duplicate title entry") {
        REQUIRE(manager.HasTitle("c_test2"));
        Title* title = manager.GetTitle("c_test2");
        auto& history = title->GetHistory();

        // Check that properties inside a date are merged correctly.
        REQUIRE(history.contains(Jomini::Date(867, 1, 1)));
        REQUIRE(history.at(Jomini::Date(867, 1, 1))->Is(Jomini::Type::OBJECT));
        CHECK(history.at(Jomini::Date(867, 1, 1))->Serialize(0, true, true) ==
            "change_development_level = 1 first = yes second = yes");
        
        // Check that unique date are not removed.
        REQUIRE(history.contains(Jomini::Date(1066, 1, 1)));
        CHECK(history.at(Jomini::Date(1066, 1, 1))->Serialize(0, true, true) == "change_development_level = 3");
        
        // Check that dates in second entry are not ignored and merged.
        REQUIRE(history.contains(Jomini::Date(10, 1, 1)));
        CHECK(history.at(Jomini::Date(10, 1, 1))->Serialize(0, true, true) == "change_development_level = 4");
    }
}

TEST_CASE("[TitleManager] LoadTitlesLocalization") {
    // 1. Initialize the mod and load the titles.
    Mod mod("resources/tests/title_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetDir()));

    TitleManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadTitles());

    // 2. Load the titles localization.
    REQUIRE_NOTHROW(manager.LoadTitlesLocalization());

    // 3. Asserts

    SUBCASE("name") {
        CHECK(manager.GetTitle("b_test1")->GetLocNames().contains("english"));
        CHECK(manager.GetTitle("b_test1")->GetLocName("english") == "bTest");

        CHECK(manager.GetTitle("c_test1")->GetLocNames().contains("english"));
        CHECK(manager.GetTitle("c_test1")->GetLocName("english") == "cTest");

        CHECK(manager.GetTitle("d_test1")->GetLocNames().contains("english"));
        CHECK(manager.GetTitle("d_test1")->GetLocName("english") == "dTest");
        
        CHECK(manager.GetTitle("k_test")->GetLocNames().contains("english"));
        CHECK(manager.GetTitle("k_test")->GetLocName("english") == "kTest");
        
        CHECK(manager.GetTitle("e_test")->GetLocNames().contains("english"));
        CHECK(manager.GetTitle("e_test")->GetLocName("english") == "Test Empire");

        CHECK_FALSE(manager.GetTitle("b_test2")->GetLocNames().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test3")->GetLocNames().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test4")->GetLocNames().contains("english"));

        CHECK_FALSE(manager.GetTitle("c_test2")->GetLocNames().contains("english"));

        CHECK_FALSE(manager.GetTitle("d_test2")->GetLocNames().contains("english"));
    }
    
    SUBCASE("article") {
        CHECK(manager.GetTitle("e_test")->GetLocArticles().contains("english"));
        CHECK(manager.GetTitle("e_test")->GetLocArticle("english") == "the ");

        CHECK_FALSE(manager.GetTitle("b_test1")->GetLocArticles().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test2")->GetLocArticles().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test3")->GetLocArticles().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test4")->GetLocArticles().contains("english"));

        CHECK_FALSE(manager.GetTitle("c_test1")->GetLocArticles().contains("english"));
        CHECK_FALSE(manager.GetTitle("c_test2")->GetLocArticles().contains("english"));

        CHECK_FALSE(manager.GetTitle("d_test2")->GetLocArticles().contains("english"));
        CHECK_FALSE(manager.GetTitle("d_test1")->GetLocArticles().contains("english"));

        CHECK_FALSE(manager.GetTitle("k_test")->GetLocArticles().contains("english"));
    }
    
    SUBCASE("adjective") {
        CHECK(manager.GetTitle("e_test")->GetLocAdjectives().contains("english"));
        CHECK(manager.GetTitle("e_test")->GetLocAdjective("english") == "Testian");

        CHECK_FALSE(manager.GetTitle("b_test1")->GetLocAdjectives().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test2")->GetLocAdjectives().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test3")->GetLocAdjectives().contains("english"));
        CHECK_FALSE(manager.GetTitle("b_test4")->GetLocAdjectives().contains("english"));

        CHECK_FALSE(manager.GetTitle("c_test1")->GetLocAdjectives().contains("english"));
        CHECK_FALSE(manager.GetTitle("c_test2")->GetLocAdjectives().contains("english"));

        CHECK_FALSE(manager.GetTitle("d_test2")->GetLocAdjectives().contains("english"));
        CHECK_FALSE(manager.GetTitle("d_test1")->GetLocAdjectives().contains("english"));

        CHECK_FALSE(manager.GetTitle("k_test")->GetLocAdjectives().contains("english"));
    }
    
    SUBCASE("cultural names") {
        REQUIRE(manager.GetLocCulturalNames().size() == 1);
        REQUIRE(manager.GetLocCulturalNames("english").size() == 3);

        REQUIRE(manager.HasLocCulturalName("english", "cn_naoned"));
        CHECK(manager.GetLocCulturalName("english", "cn_naoned") == "Naoned");
        
        REQUIRE(manager.HasLocCulturalName("english", "cn_naoned_article"));
        CHECK(manager.GetLocCulturalName("english", "cn_naoned_article") == "the ");
        
        REQUIRE(manager.HasLocCulturalName("english", "cn_naoned_adj"));
        CHECK(manager.GetLocCulturalName("english", "cn_naoned_adj") == "naonedat");
    }
}

TEST_CASE("[TitleManager] ExportTitles") {
    // 1. Setup the mod and the titles.
    Mod mod("resources/tests/title_manager/test_mod");
    REQUIRE(std::filesystem::exists(mod.GetDir()));

    {
        TitleManager manager(mod);
        REQUIRE_NOTHROW(manager.LoadTitles());

        // Edit some titles.
        manager.RenameTitle("b_test1", "b_modified1");
        manager.RenameTitle("c_test1", "c_modified1");
        manager.RenameTitle("d_test1", "d_modified1");
        manager.RenameTitle("k_test", "k_modified");
        manager.RenameTitle("e_test", "e_modified");
        
        // 2. Export the titles definition.
        mod.SetDir("resources/tests/title_manager/test_mod_modified");
        REQUIRE_NOTHROW(manager.ExportTitles());
    }

    // Reload the titles.
    TitleManager manager(mod);
    REQUIRE_NOTHROW(manager.LoadTitles());

    // 3. Asserts

    SUBCASE("title keys") {
        CHECK(manager.HasTitle("b_modified1"));
        CHECK(manager.HasTitle("b_test2"));
        CHECK(manager.HasTitle("b_test3"));
        CHECK(manager.HasTitle("b_test4"));
        CHECK(manager.HasTitle("c_modified1"));
        CHECK(manager.HasTitle("c_test2"));
        CHECK(manager.HasTitle("d_modified1"));
        CHECK(manager.HasTitle("d_test2"));
        CHECK(manager.HasTitle("k_modified"));
        CHECK(manager.HasTitle("e_modified"));

        CHECK_FALSE(manager.HasTitle("b_test1"));
        CHECK_FALSE(manager.HasTitle("c_test1"));
        CHECK_FALSE(manager.HasTitle("d_test1"));
    }
}

}