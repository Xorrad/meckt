#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "titles/TitleManager.hpp"

TEST_SUITE("[TitleType]") {

TEST_CASE("[TitleType] GetTitleTypeByName") {
    const std::vector<std::pair<std::string, TitleType>> validNames = {
        { "b_test", TitleType::BARONY },
        { "c_test", TitleType::COUNTY },
        { "d_test", TitleType::DUCHY },
        { "k_test", TitleType::KINGDOM },
        { "e_test", TitleType::EMPIRE },
        { "h_test", TitleType::HEGEMONY },
    };
    for (const auto& [name, type] : validNames)
        CHECK_EQ(GetTitleTypeByName(name), type);

    const std::vector<std::string> invalidNames = { "b", "b_", "b_ ", "a_", "a_test", "test" };
    for (const auto& name : invalidNames)
        CHECK_THROWS_AS(GetTitleTypeByName(name), std::invalid_argument);
}

TEST_CASE("[TitleType] IsValidTitleName") {
    const std::vector<std::pair<std::string, TitleType>> validNames = {
        { "b_test", TitleType::BARONY },
        { "c_test", TitleType::COUNTY },
        { "d_test", TitleType::DUCHY },
        { "k_test", TitleType::KINGDOM },
        { "e_test", TitleType::EMPIRE },
        { "h_test", TitleType::HEGEMONY },
    };
    for (const auto& [name, type] : validNames)
        CHECK(IsValidTitleName(name, type));

    const std::vector<std::pair<std::string, TitleType>> invalidNames = {
        { "b", TitleType::BARONY },
        { "b_ ", TitleType::BARONY },
        { "test", TitleType::BARONY },
        { "c_test", TitleType::BARONY },
        { "b_test", TitleType::COUNTY },
        { "c_test", TitleType::DUCHY },
        { "d_test", TitleType::KINGDOM },
        { "k_test", TitleType::EMPIRE },
        { "e_test", TitleType::HEGEMONY },
        { "h_test", TitleType::BARONY },
    };
    for (const auto& [name, type] : invalidNames)
        CHECK_FALSE(IsValidTitleName(name, type));
}

}

TEST_SUITE("[Title] MakeTitle") {

TEST_CASE("[Title] MakeTitle") {
    struct MakeTitleTestData {
        TitleType type;
        std::string name;
        sf::Color color;
        bool landless;
        std::function<bool(const UniquePtr<Title>&)> isInstance;
    };
    const std::vector<MakeTitleTestData> testData = {
        { TitleType::BARONY,   "b_test", sf::Color::Red,   true,  [](auto& t) { return IsInstance<BaronyTitle>(t); } },
        { TitleType::COUNTY,   "c_test", sf::Color::Green, false, [](auto& t) { return IsInstance<CountyTitle>(t); } },
        { TitleType::DUCHY,    "d_test", sf::Color::Blue,  true,  [](auto& t) { return IsInstance<DuchyTitle>(t); } },
        { TitleType::KINGDOM,  "k_test", sf::Color::Blue,  true,  [](auto& t) { return IsInstance<KingdomTitle>(t); } },
        { TitleType::EMPIRE,   "e_test", sf::Color::Blue,  true,  [](auto& t) { return IsInstance<EmpireTitle>(t); } },
        { TitleType::HEGEMONY, "h_test", sf::Color::Blue,  true,  [](auto& t) { return IsInstance<HegemonyTitle>(t); } },
    };

    for (const auto& data : testData) {
        SUBCASE(data.name.c_str()) {
            UniquePtr<Title> title = MakeTitle(data.type, data.name, data.color, data.landless);
            REQUIRE(title != nullptr);
            CHECK(data.isInstance(title));
            CHECK(title->Is(data.type));
            CHECK_EQ(title->GetName(), data.name);
            CHECK_EQ(title->GetColor(), data.color);
            CHECK_EQ(title->GetLiegeTitle(), nullptr);
            CHECK_EQ(title->IsLandless(), data.landless);
        }
    }

    SUBCASE("unknown") {
        CHECK_THROWS_AS(MakeTitle(TitleType::COUNT, "b_test", sf::Color::Blue, true), std::invalid_argument);
        CHECK_THROWS_AS(MakeTitle(TitleType::COUNT, "a_test", sf::Color::Blue, true), std::invalid_argument);
    }
}

}

TEST_SUITE("[Title]") {

TEST_CASE("[Title] Title::GetType") {
    CHECK_EQ(BaronyTitle().GetType(), TitleType::BARONY);
    CHECK_EQ(CountyTitle().GetType(), TitleType::COUNTY);
    CHECK_EQ(DuchyTitle().GetType(), TitleType::DUCHY);
    CHECK_EQ(KingdomTitle().GetType(), TitleType::KINGDOM);
    CHECK_EQ(EmpireTitle().GetType(), TitleType::EMPIRE);
    CHECK_EQ(HegemonyTitle().GetType(), TitleType::HEGEMONY);
}

TEST_CASE("[Title] Title::IsVassal") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::Red, false);
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Red, false);
    UniquePtr<Title> duchy = MakeTitle(TitleType::DUCHY, "d_test", sf::Color::Red, false);
    UniquePtr<Title> kingdom = MakeTitle(TitleType::KINGDOM, "k_test", sf::Color::Red, false);
    UniquePtr<Title> empire = MakeTitle(TitleType::EMPIRE, "e_test", sf::Color::Red, false);
    UniquePtr<Title> hegemony = MakeTitle(TitleType::HEGEMONY, "h_test", sf::Color::Red, false);
    UniquePtr<Title> county2 = MakeTitle(TitleType::COUNTY, "c_test2", sf::Color::Red, false);

    #define CAST_HIGHTITLE(title) dynamic_cast<HighTitle*>(title.get())

    barony->SetLiegeTitle(CAST_HIGHTITLE(county));
    county->SetLiegeTitle(CAST_HIGHTITLE(duchy));
    duchy->SetLiegeTitle(CAST_HIGHTITLE(kingdom));
    kingdom->SetLiegeTitle(CAST_HIGHTITLE(empire));
    empire->SetLiegeTitle(CAST_HIGHTITLE(hegemony));

    CHECK(barony->IsVassal(CAST_HIGHTITLE(county)));
    CHECK(barony->IsVassal(CAST_HIGHTITLE(duchy)));
    CHECK(barony->IsVassal(CAST_HIGHTITLE(kingdom)));
    CHECK(barony->IsVassal(CAST_HIGHTITLE(empire)));
    CHECK(barony->IsVassal(CAST_HIGHTITLE(hegemony)));
    CHECK(duchy->IsVassal(CAST_HIGHTITLE(hegemony)));

    CHECK_FALSE(barony->IsVassal(nullptr));
    CHECK_FALSE(barony->IsVassal(CAST_HIGHTITLE(county2)));
    CHECK_FALSE(empire->IsVassal(CAST_HIGHTITLE(barony)));
}

TEST_CASE("[Title] Title::AddHistory") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::Red, false);

    CHECK_EQ(barony->GetHistory().size(), 0);

    barony->AddHistory(Jomini::Date(1, 1, 1), MakeShared<Jomini::Object>("test"));

    REQUIRE(barony->GetHistory().size() == 1);
    REQUIRE(barony->GetHistory().contains(Jomini::Date(1, 1, 1)));

    CHECK(barony->GetHistory().at(Jomini::Date(1, 1, 1))->Is(Jomini::Type::SCALAR));
    CHECK_EQ(barony->GetHistory().at(Jomini::Date(1, 1, 1))->GetString(), "test");
}

TEST_CASE("[Title] Title::RemoveHistory") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::Red, false);

    barony->AddHistory(Jomini::Date(1, 1, 1), MakeShared<Jomini::Object>("test"));

    CHECK_EQ(barony->GetHistory().size(), 1);
    CHECK(barony->GetHistory().contains(Jomini::Date(1, 1, 1)));

    barony->RemoveHistory(Jomini::Date(1, 1, 1));

    CHECK_EQ(barony->GetHistory().size(), 0);
    CHECK_FALSE(barony->GetHistory().contains(Jomini::Date(1, 1, 1)));
}

TEST_CASE("[Title] Title::AddCulturalName") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_nantes", sf::Color::White, false);

    CHECK_EQ(barony->GetCulturalNames().size(), 0);

    barony->AddCulturalName("breton", "Naoned");

    REQUIRE(barony->GetCulturalNames().size() == 1);
    REQUIRE(barony->GetCulturalNames().contains("breton"));

    CHECK_EQ(barony->GetCulturalNames().at("breton"), "Naoned");
}

TEST_CASE("[Title] Title::RemoveCulturalName") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_nantes", sf::Color::White, false);

    barony->AddCulturalName("breton", "Naoned");
    CHECK_EQ(barony->GetCulturalNames().size(), 1);

    barony->RemoveCulturalName("breton");

    CHECK_EQ(barony->GetCulturalNames().size(), 0);
}

// Set/Get/Has for a title's localized name, adjective and article follow the exact same shape,
// so they're all verified together through one data-driven test instead of nine near-identical ones.
TEST_CASE("[Title] Title::Loc properties") {
    struct LocPropertyTestData {
        std::string label;
        std::string value1;
        std::string value2;
        std::function<std::map<std::string, std::string>&(Title&)> getAll;
        std::function<void(Title&, const std::string&, const std::string&)> set;
        std::function<std::string(Title&, const std::string&)> get;
        std::function<bool(Title&, const std::string&)> has;
    };
    const std::vector<LocPropertyTestData> properties = {
        { "name", "Cologne", "Köln",
          [](Title& t) -> std::map<std::string, std::string>& { return t.GetLocNames(); },
          [](Title& t, const std::string& lang, const std::string& v) { t.SetLocName(lang, v); },
          [](Title& t, const std::string& lang) { return t.GetLocName(lang); },
          [](Title& t, const std::string& lang) { return t.HasLocName(lang); } },
        { "adjective", "Colonais", "Kölner",
          [](Title& t) -> std::map<std::string, std::string>& { return t.GetLocAdjectives(); },
          [](Title& t, const std::string& lang, const std::string& v) { t.SetLocAdjective(lang, v); },
          [](Title& t, const std::string& lang) { return t.GetLocAdjective(lang); },
          [](Title& t, const std::string& lang) { return t.HasLocAdjective(lang); } },
        { "article", "la", "das",
          [](Title& t) -> std::map<std::string, std::string>& { return t.GetLocArticles(); },
          [](Title& t, const std::string& lang, const std::string& v) { t.SetLocArticle(lang, v); },
          [](Title& t, const std::string& lang) { return t.GetLocArticle(lang); },
          [](Title& t, const std::string& lang) { return t.HasLocArticle(lang); } },
    };

    for (const auto& property : properties) {
        SUBCASE(property.label.c_str()) {
            UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

            CHECK_EQ(property.getAll(*county).size(), 0);

            property.set(*county, "french", property.value1);
            property.set(*county, "german", property.value2);

            REQUIRE(property.getAll(*county).size() == 2);
            REQUIRE(property.getAll(*county).contains("french"));
            REQUIRE(property.getAll(*county).contains("german"));
            CHECK_EQ(property.getAll(*county).at("french"), property.value1);
            CHECK_EQ(property.getAll(*county).at("german"), property.value2);

            CHECK_EQ(property.get(*county, "french"), property.value1);
            CHECK_EQ(property.get(*county, "german"), property.value2);
            CHECK_EQ(property.get(*county, "english"), "");

            CHECK(property.has(*county, "french"));
            CHECK(property.has(*county, "german"));
            CHECK_FALSE(property.has(*county, "english"));
        }
    }
}

TEST_CASE("[Title] HighTitle::AddDejureTitle") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::White, false);
    UniquePtr<Title> _county = MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false);
    HighTitle* county = dynamic_cast<HighTitle*>(_county.get());

    CHECK_EQ(county->GetDejureTitles().size(), 0);

    county->AddDejureTitle(barony.get());

    CHECK_EQ(county->GetDejureTitles().size(), 1);
    CHECK(std::find(county->GetDejureTitles().begin(), county->GetDejureTitles().end(), barony.get()) != county->GetDejureTitles().end());
}

TEST_CASE("[Title] HighTitle::HasDejureTitle") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::White, false);
    UniquePtr<Title> barony2 = MakeTitle(TitleType::BARONY, "b_test2", sf::Color::White, false);
    UniquePtr<Title> _county = MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false);
    HighTitle* county = dynamic_cast<HighTitle*>(_county.get());

    county->AddDejureTitle(barony.get());

    CHECK(county->HasDejureTitle(barony.get()));
    CHECK_FALSE(county->HasDejureTitle(barony2.get()));
    CHECK_FALSE(county->HasDejureTitle(nullptr));
}

TEST_CASE("[Title] HighTitle::RemoveDejureTitle") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_test", sf::Color::White, false);
    UniquePtr<Title> _county = MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Yellow, false);
    HighTitle* county = dynamic_cast<HighTitle*>(_county.get());

    county->AddDejureTitle(barony.get());
    CHECK_EQ(county->GetDejureTitles().size(), 1);
    CHECK(county->HasDejureTitle(barony.get()));

    county->RemoveDejureTitle(barony.get());

    CHECK_EQ(county->GetDejureTitles().size(), 0);
    CHECK_FALSE(county->HasDejureTitle(barony.get()));
}

}
