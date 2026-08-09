#include "doctest/doctest.hpp"

TEST_SUITE("Date") {

TEST_CASE("[Date] Constructor") {
    // Default constructor
    Jomini::Date d;
    CHECK_EQ(d.year, 0);
    CHECK_EQ(d.month, 0);
    CHECK_EQ(d.day, 0);

    // Numbers constructor
    d = Jomini::Date(1, 2, 3);
    CHECK_EQ(d.year, 1);
    CHECK_EQ(d.month, 2);
    CHECK_EQ(d.day, 3);

    d = Jomini::Date(-100, 2, 3);
    CHECK_EQ(d.year, -100);
    CHECK_EQ(d.month, 2);
    CHECK_EQ(d.day, 3);

    // String constructor
    d = Jomini::Date("100.1.1");
    CHECK_EQ(d.year, 100);
    CHECK_EQ(d.month, 1);
    CHECK_EQ(d.day, 1);

    d = Jomini::Date("-100.1.1");
    CHECK_EQ(d.year, -100);
    CHECK_EQ(d.month, 1);
    CHECK_EQ(d.day, 1);

    CHECK_THROWS_AS(Jomini::Date("1"), std::invalid_argument);
    CHECK_THROWS_AS(Jomini::Date("1."), std::invalid_argument);
    CHECK_THROWS_AS(Jomini::Date("1.1"), std::invalid_argument);
    CHECK_THROWS_AS(Jomini::Date("1.1."), std::invalid_argument);

    CHECK_THROWS_AS(Jomini::Date("a.1.1"), std::invalid_argument);
    CHECK_THROWS_AS(Jomini::Date("1.b.1"), std::invalid_argument);
    CHECK_THROWS_AS(Jomini::Date("1.1.c"), std::invalid_argument);
}

TEST_CASE("[Date] Operators") {
    // Equal
    CHECK(Jomini::Date(1, 2, 3) == Jomini::Date(1, 2, 3));
    CHECK(Jomini::Date(-100, 2, 3) == Jomini::Date(-100, 2, 3));
    CHECK_FALSE(Jomini::Date(1, 2, 3) == Jomini::Date(-100, 2, 3));

    // Not Equal
    CHECK_FALSE(Jomini::Date(1, 2, 3) != Jomini::Date(1, 2, 3));
    CHECK_FALSE(Jomini::Date(-100, 2, 3) != Jomini::Date(-100, 2, 3));
    CHECK(Jomini::Date(1, 2, 3) != Jomini::Date(-100, 2, 3));

    // Less than
    CHECK(Jomini::Date(0, 0, 0) < Jomini::Date(1, 1, 1));
    CHECK(Jomini::Date(0, 0, 1) < Jomini::Date(1, 1, 1));
    CHECK(Jomini::Date(0, 1, 1) < Jomini::Date(1, 1, 1));
    CHECK_FALSE(Jomini::Date(1, 1, 1) < Jomini::Date(1, 1, 1));

    // Less than or equal to
    CHECK(Jomini::Date(0, 0, 0) <= Jomini::Date(1, 1, 1));
    CHECK(Jomini::Date(0, 0, 1) <= Jomini::Date(1, 1, 1));
    CHECK(Jomini::Date(0, 1, 1) <= Jomini::Date(1, 1, 1));
    CHECK(Jomini::Date(1, 1, 1) <= Jomini::Date(1, 1, 1));
    CHECK_FALSE(Jomini::Date(1, 1, 2) <= Jomini::Date(1, 1, 1));

    // Greater than
    CHECK(Jomini::Date(1, 1, 1) > Jomini::Date(0, 0, 0));
    CHECK(Jomini::Date(0, 1, 1) > Jomini::Date(0, 0, 0));
    CHECK(Jomini::Date(0, 0, 1) > Jomini::Date(0, 0, 0));
    CHECK_FALSE(Jomini::Date(0, 0, 0) > Jomini::Date(0, 0, 0));

    // Greater than or equal to
    CHECK(Jomini::Date(1, 1, 2) >= Jomini::Date(0, 0, 1));
    CHECK(Jomini::Date(0, 1, 2) >= Jomini::Date(0, 0, 1));
    CHECK(Jomini::Date(0, 0, 2) >= Jomini::Date(0, 0, 1));
    CHECK(Jomini::Date(0, 0, 1) >= Jomini::Date(0, 0, 1));
    CHECK_FALSE(Jomini::Date(0, 0, 0) >= Jomini::Date(0, 0, 1));

    // String
    CHECK_EQ(std::string(Jomini::Date(100, 1, 1)), "100.1.1");
    CHECK_EQ(std::string(Jomini::Date(0, 0, 0)), "0.0.0");
    CHECK_EQ(std::string(Jomini::Date(-100, 1, 1)), "-100.1.1");
}

TEST_CASE("[Date] ParseDate: valid inputs") {
    CHECK_EQ(Date::ParseDate("1.1.1"), Jomini::Date(1, 1, 1));
    CHECK_EQ(Date::ParseDate("1.12.31"), Jomini::Date(1, 12, 31));
    CHECK_EQ(Date::ParseDate("-100.12.31"), Jomini::Date(-100, 12, 31));
    CHECK_EQ(Date::ParseDate("100.12"), Jomini::Date(100, 12, 1));
    CHECK_EQ(Date::ParseDate("100"), Jomini::Date(100, 1, 1));
}

TEST_CASE("[Date] ParseDate: invalid inputs") {
    CHECK_THROWS_AS(Date::ParseDate(""), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("abcd"), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("1.abcd"), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("1.1.abcd"), std::invalid_argument);

    CHECK_THROWS_AS(Date::ParseDate("1.1.0"), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("1.1.32"), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("1.0.1"), std::invalid_argument);
    CHECK_THROWS_AS(Date::ParseDate("1.13.1"), std::invalid_argument);
}

}