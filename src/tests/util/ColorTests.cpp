#include "doctest/doctest.hpp"

TEST_CASE("[Color] Hash") {
    std::hash<sf::Color> hasher;

    CHECK(hasher(sf::Color(10, 20, 30, 40)) == hasher(sf::Color(10, 20, 30, 40)));
    CHECK(hasher(sf::Color(11, 20, 30, 40)) != hasher(sf::Color(10, 20, 30, 40)));

    std::unordered_set<sf::Color> set;
    set.insert(sf::Color(1,2,3,4));
    set.insert(sf::Color(1,2,3,4));
    set.insert(sf::Color(5,6,7,8));

    CHECK(set.size() == 2);
}

TEST_CASE("[Color] brightenColor") {}

TEST_CASE("[Color] HSVColor: fromRgb") {
    sf::HSVColor red   = sf::HSVColor::fromRgb(sf::Color::Red);
    sf::HSVColor green = sf::HSVColor::fromRgb(sf::Color::Green);
    sf::HSVColor blue  = sf::HSVColor::fromRgb(sf::Color::Blue);

    CHECK(red.h == doctest::Approx(0.f));
    CHECK(red.s == doctest::Approx(1.f));

    CHECK(green.h == doctest::Approx(120.f));
    CHECK(green.s == doctest::Approx(1.f));

    CHECK(blue.h == doctest::Approx(240.f));
    CHECK(blue.s == doctest::Approx(1.f));
}

TEST_CASE("[Color] HSVColor: RGB -> HSV -> RGB") {
    sf::Color original(100, 50, 200);

    sf::HSVColor hsv = sf::HSVColor::fromRgb(original);
    sf::Color converted = hsv.toRgb();

    CHECK_EQ(converted.r, original.r);
    CHECK_EQ(converted.g, original.g);
    CHECK_EQ(converted.b, original.b);
}

TEST_CASE("[Color] HSVColor: EQ and NE operators") {
    CHECK(sf::HSVColor(10.f, 0.5f, 0.5f) == sf::HSVColor(10.f, 0.5f, 0.5f));
    CHECK(sf::HSVColor(10.f, 0.5f, 0.5f) != sf::HSVColor(20.f, 0.5f, 0.5f));
}

TEST_CASE("[Color] HSVColor: assignment operator") {
    sf::HSVColor a(0.f, 0.f, 0.f);
    sf::HSVColor b(100.f, 0.5f, 0.7f);
    a = b;

    CHECK_EQ(a, b);
}

TEST_CASE("[Color] HSVColor: toInteger") {
    sf::Color rgb(50, 100, 150);
    sf::HSVColor hsv(rgb);

    CHECK_EQ(hsv.toInteger(), rgb.toInteger());
}

TEST_CASE("[Color] HSVColor: fmt formatting") {
    CHECK_EQ(fmt::format("{}", sf::Color(10, 20, 30, 40)), "(10, 20, 30, 40)");
    CHECK_EQ(fmt::format("{}", sf::Color(10, 20, 30)), "(10, 20, 30, 255)");
}