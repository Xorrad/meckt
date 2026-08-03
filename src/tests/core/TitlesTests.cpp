#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "titles/TitleManager.hpp"

TEST_SUITE("[TitleType]") {

TEST_CASE("[TitleType] GetTitleTypeByName") {
    CHECK_EQ(GetTitleTypeByName("b_test"), TitleType::BARONY);
    CHECK_EQ(GetTitleTypeByName("c_test"), TitleType::COUNTY);
    CHECK_EQ(GetTitleTypeByName("d_test"), TitleType::DUCHY);
    CHECK_EQ(GetTitleTypeByName("k_test"), TitleType::KINGDOM);
    CHECK_EQ(GetTitleTypeByName("e_test"), TitleType::EMPIRE);
    CHECK_EQ(GetTitleTypeByName("h_test"), TitleType::HEGEMONY);

    CHECK_THROWS_AS(GetTitleTypeByName("b"), std::invalid_argument);
    CHECK_THROWS_AS(GetTitleTypeByName("b_"), std::invalid_argument);
    CHECK_THROWS_AS(GetTitleTypeByName("b_ "), std::invalid_argument);
    CHECK_THROWS_AS(GetTitleTypeByName("a_"), std::invalid_argument);
    CHECK_THROWS_AS(GetTitleTypeByName("a_test"), std::invalid_argument);
    CHECK_THROWS_AS(GetTitleTypeByName("test"), std::invalid_argument);
}

TEST_CASE("[TitleType] IsValidTitleName") {
    CHECK(IsValidTitleName("b_test", TitleType::BARONY));
    CHECK(IsValidTitleName("c_test", TitleType::COUNTY));
    CHECK(IsValidTitleName("d_test", TitleType::DUCHY));
    CHECK(IsValidTitleName("k_test", TitleType::KINGDOM));
    CHECK(IsValidTitleName("e_test", TitleType::EMPIRE));
    CHECK(IsValidTitleName("h_test", TitleType::HEGEMONY));
    
    CHECK_FALSE(IsValidTitleName("b", TitleType::BARONY));
    CHECK_FALSE(IsValidTitleName("b_ ", TitleType::BARONY));
    CHECK_FALSE(IsValidTitleName("test", TitleType::BARONY));
    CHECK_FALSE(IsValidTitleName("c_test", TitleType::BARONY));
    CHECK_FALSE(IsValidTitleName("b_test", TitleType::COUNTY));
    CHECK_FALSE(IsValidTitleName("c_test", TitleType::DUCHY));
    CHECK_FALSE(IsValidTitleName("d_test", TitleType::KINGDOM));
    CHECK_FALSE(IsValidTitleName("k_test", TitleType::EMPIRE));
    CHECK_FALSE(IsValidTitleName("e_test", TitleType::HEGEMONY));
    CHECK_FALSE(IsValidTitleName("h_test", TitleType::BARONY));
}

}

TEST_SUITE("[Title] MakeTitle") {

TEST_CASE("[Title] MakeTitle: barony") {
    UniquePtr<Title> title = MakeTitle(TitleType::BARONY, "b_test", sf::Color::Red, true);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<BaronyTitle>(title));
    CHECK(title->Is(TitleType::BARONY));
    CHECK_EQ(title->GetName(), "b_test");
    CHECK_EQ(title->GetColor(), sf::Color::Red);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: county") {
    UniquePtr<Title> title = MakeTitle(TitleType::COUNTY, "c_test", sf::Color::Green, false);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<CountyTitle>(title));
    CHECK(title->Is(TitleType::COUNTY));
    CHECK_EQ(title->GetName(), "c_test");
    CHECK_EQ(title->GetColor(), sf::Color::Green);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK_FALSE(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: duchy") {
    UniquePtr<Title> title = MakeTitle(TitleType::DUCHY, "d_test", sf::Color::Blue, true);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<DuchyTitle>(title));
    CHECK(title->Is(TitleType::DUCHY));
    CHECK_EQ(title->GetName(), "d_test");
    CHECK_EQ(title->GetColor(), sf::Color::Blue);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: kingdom") {
    UniquePtr<Title> title = MakeTitle(TitleType::KINGDOM, "k_test", sf::Color::Blue, true);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<KingdomTitle>(title));
    CHECK(title->Is(TitleType::KINGDOM));
    CHECK_EQ(title->GetName(), "k_test");
    CHECK_EQ(title->GetColor(), sf::Color::Blue);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: empire") {
    UniquePtr<Title> title = MakeTitle(TitleType::EMPIRE, "e_test", sf::Color::Blue, true);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<EmpireTitle>(title));
    CHECK(title->Is(TitleType::EMPIRE));
    CHECK_EQ(title->GetName(), "e_test");
    CHECK_EQ(title->GetColor(), sf::Color::Blue);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: hegemony") {
    UniquePtr<Title> title = MakeTitle(TitleType::HEGEMONY, "h_test", sf::Color::Blue, true);
    REQUIRE(title != nullptr);
    CHECK(IsInstance<HegemonyTitle>(title));
    CHECK(title->Is(TitleType::HEGEMONY));
    CHECK_EQ(title->GetName(), "h_test");
    CHECK_EQ(title->GetColor(), sf::Color::Blue);
    CHECK_EQ(title->GetLiegeTitle(), nullptr);
    CHECK(title->IsLandless());
}

TEST_CASE("[Title] MakeTitle: unknown") {
    CHECK_THROWS_AS(MakeTitle(TitleType::COUNT, "b_test", sf::Color::Blue, true), std::invalid_argument);
    CHECK_THROWS_AS(MakeTitle(TitleType::COUNT, "a_test", sf::Color::Blue, true), std::invalid_argument);
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

    CHECK_EQ(barony->GetHistory().size(), 0);

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

TEST_CASE("[Title] Title::RemoveHistory") {
    UniquePtr<Title> barony = MakeTitle(TitleType::BARONY, "b_nantes", sf::Color::White, false);

    CHECK_EQ(barony->GetCulturalNames().size(), 0);

    barony->AddCulturalName("breton", "Naoned");

    CHECK_EQ(barony->GetCulturalNames().size(), 1);

    barony->RemoveCulturalName("breton");

    CHECK_EQ(barony->GetCulturalNames().size(), 0);
}

TEST_CASE("[Title] Title::SetLocName") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    CHECK_EQ(county->GetLocNames().size(), 0);

    county->SetLocName("french", "Cologne");
    county->SetLocName("german", "Köln");

    REQUIRE(county->GetLocNames().size() == 2);
    REQUIRE(county->GetLocNames().contains("french"));
    REQUIRE(county->GetLocNames().contains("german"));

    CHECK_EQ(county->GetLocNames().at("french"), "Cologne");
    CHECK_EQ(county->GetLocNames().at("german"), "Köln");
}

TEST_CASE("[Title] Title::GetLocName") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocName("french", "Cologne");
    county->SetLocName("german", "Köln");

    CHECK_EQ(county->GetLocName("french"), "Cologne");
    CHECK_EQ(county->GetLocName("german"), "Köln");
    CHECK_EQ(county->GetLocName("english"), "");
}

TEST_CASE("[Title] Title::HasLocName") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocName("french", "Cologne");
    county->SetLocName("german", "Köln");

    CHECK(county->HasLocName("french"));
    CHECK(county->HasLocName("german"));
    CHECK_FALSE(county->HasLocName("english"));
}

///

TEST_CASE("[Title] Title::SetLocAdjective") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    CHECK_EQ(county->GetLocAdjectives().size(), 0);

    county->SetLocAdjective("french", "Colonais");
    county->SetLocAdjective("german", "Kölner");

    REQUIRE(county->GetLocAdjectives().size() == 2);
    REQUIRE(county->GetLocAdjectives().contains("french"));
    REQUIRE(county->GetLocAdjectives().contains("german"));

    CHECK_EQ(county->GetLocAdjectives().at("french"), "Colonais");
    CHECK_EQ(county->GetLocAdjectives().at("german"), "Kölner");
}

TEST_CASE("[Title] Title::GetLocAdjective") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocAdjective("french", "Colonais");
    county->SetLocAdjective("german", "Kölner");

    CHECK_EQ(county->GetLocAdjective("french"), "Colonais");
    CHECK_EQ(county->GetLocAdjective("german"), "Kölner");
    CHECK_EQ(county->GetLocAdjective("english"), "");
}

TEST_CASE("[Title] Title::HasLocAdjective") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocAdjective("french", "Colonais");
    county->SetLocAdjective("german", "Kölner");

    CHECK(county->HasLocAdjective("french"));
    CHECK(county->HasLocAdjective("german"));
    CHECK_FALSE(county->HasLocAdjective("english"));
}

TEST_CASE("[Title] Title::SetLocArticle") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    CHECK_EQ(county->GetLocArticles().size(), 0);

    county->SetLocArticle("french", "la");
    county->SetLocArticle("german", "das");

    REQUIRE(county->GetLocArticles().size() == 2);
    REQUIRE(county->GetLocArticles().contains("french"));
    REQUIRE(county->GetLocArticles().contains("german"));

    CHECK_EQ(county->GetLocArticles().at("french"), "la");
    CHECK_EQ(county->GetLocArticles().at("german"), "das");
}

TEST_CASE("[Title] Title::GetLocArticle") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocArticle("french", "la");
    county->SetLocArticle("german", "das");

    CHECK_EQ(county->GetLocArticle("french"), "la");
    CHECK_EQ(county->GetLocArticle("german"), "das");

    CHECK_EQ(county->GetLocArticle("english"), "");
}

TEST_CASE("[Title] Title::HasLocArticle") {
    UniquePtr<Title> county = MakeTitle(TitleType::COUNTY, "c_cologne", sf::Color::Yellow, false);

    county->SetLocArticle("french", "la");
    county->SetLocArticle("german", "das");

    CHECK(county->HasLocArticle("french"));
    CHECK(county->HasLocArticle("german"));
    CHECK_FALSE(county->HasLocArticle("english"));
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