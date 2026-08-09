#include "doctest/doctest.hpp"

TEST_CASE("[Math] RandomInt: stays within bounds") {
    for (int i = 0; i < 1000; i++) {
        int value = Math::RandomInt(5, 10);
        CHECK(value >= 5);
        CHECK(value <= 10);
    }
}

TEST_CASE("[Math] RandomInt: does not always return the same value") {
    std::set<int> values;
    for (int i = 0; i < 1000; i++) {
        values.insert(Math::RandomInt(0, 1000));
    }
    CHECK(values.size() > 1);
}

TEST_CASE("[Math] RandomInt: min == max") {
    for (int i = 0; i < 10; i++) {
        CHECK_EQ(Math::RandomInt(5, 5), 5);
    }
}

TEST_CASE("[Math] RandomFloat: stays within bounds") {
    for (int i = 0; i < 1000; i++) {
        float value = Math::RandomFloat(5.f, 10.f);
        CHECK(value >= 5.f);
        CHECK(value <= 10.f);
    }
}

TEST_CASE("[Math] RandomFloat: does not always return the same value") {
    std::set<float> values;
    for (int i = 0; i < 1000; i++) {
        values.insert(Math::RandomFloat(0.f, 1000.f));
    }
    CHECK(values.size() > 1);
}

TEST_CASE("[Math] RandomFloat: min == max") {
    for (int i = 0; i < 10; i++) {
        CHECK_EQ(Math::RandomFloat(5.f, 5.f), 5.f);
    }
}

TEST_CASE("[Math] IsInt") {
    CHECK(Math::IsInt("0"));
    CHECK(Math::IsInt("123"));
    CHECK(Math::IsInt("00042"));

    CHECK_FALSE(Math::IsInt("-1"));
    CHECK_FALSE(Math::IsInt("1.5"));
    CHECK_FALSE(Math::IsInt("12a"));
    CHECK_FALSE(Math::IsInt("a"));
}
