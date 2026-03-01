#include "doctest/doctest.hpp"

TEST_SUITE("OrderedMap") {

TEST_CASE("[OrderedMap] insert") {
    OrderedMap<std::string, int> map;

    map.insert("a", 1);
    map.insert("b", 2);

    REQUIRE(map.size() == 2);
    CHECK_EQ(map.at("a"), 1);
    CHECK_EQ(map.at("b"), 2);
}

TEST_CASE("[OrderedMap] insert: override") {
    OrderedMap<std::string, int> map;

    map.insert("a", 1);
    map.insert("b", 2);
    map.insert("a", 3);

    REQUIRE(map.size() == 2);
    CHECK_EQ(map.at("a"), 3);
    CHECK_EQ(map.at("b"), 2);
}

TEST_CASE("[OrderedMap] insert: preserve insertion order") {
    OrderedMap<std::string, int> map;

    map.insert("b", 1);
    map.insert("c", 2);
    map.insert("a", 3);

    std::vector<std::string> expected = {"b", "c", "a"};
    size_t index = 0;
    for (const auto& [key, value] : map) {
        CHECK(key == expected[index]);
        index++;
    }
}

TEST_CASE("[OrderedMap] at: exception") {
    OrderedMap<std::string, int> map;
    CHECK_THROWS_AS(map.at("a"), std::out_of_range);
}

TEST_CASE("[OrderedMap] [] operator: insertion") {
    OrderedMap<std::string, int> map;

    map["a"] = 1;
    map["b"] = 2;

    REQUIRE(map.size() == 2);
    CHECK_EQ(map.at("a"), 1);
    CHECK_EQ(map.at("b"), 2);
}

TEST_CASE("[OrderedMap] operator[]") {
    OrderedMap<std::string, int> map;

    map["a"] = 1;
    map["b"] = 2;

    REQUIRE(map.size() == 2);
    CHECK_EQ(map.at("a"), 1);
    CHECK_EQ(map.at("b"), 2);
}


TEST_CASE("[OrderedMap] operator[]: default value") {
    OrderedMap<std::string, int> map;

    int i{};
    if (i == 0) {
        int value = map["a"];

        CHECK_EQ(value, 0);
        CHECK_EQ(map.size(), 1);
    }
}

TEST_CASE("[OrderedMap] operator[]: preserve insertion order") {
    OrderedMap<std::string, int> map;

    map["b"] = 1;
    map["c"] = 2;
    map["a"] = 3;

    std::vector<std::string> expected = {"b", "c", "a"};
    size_t index = 0;
    for (const auto& [key, value] : map) {
        CHECK_EQ(key, expected[index]);
        index++;
    }
}

TEST_CASE("[OrderedMap] size") {
    OrderedMap<int, int> map;

    for (int i = 0; i < 10; i++)
        map.insert(i, i);
    
    CHECK_EQ(map.size(), 10);
}

TEST_CASE("[OrderedMap] empty") {
    OrderedMap<int, int> map;
    
    CHECK(map.empty());

    map.insert(1, 1);

    CHECK_FALSE(map.empty());
}

TEST_CASE("[OrderedMap] erase") {
    OrderedMap<std::string, int> map;

    map.insert("a", 1);
    map.insert("b", 2);
    map.erase("a");

    CHECK_EQ(map.size(), 1);
    CHECK_FALSE(map.contains("a"));
    CHECK(map.contains("b"));
}

TEST_CASE("[OrderedMap] erase: preserve insertion order") {
    OrderedMap<std::string, int> map;

    map.insert("d", 1);
    map.insert("b", 2);
    map.insert("c", 3);
    map.insert("a", 4);
    map.erase("d");
    map.erase("c");

    REQUIRE(map.size() == 2);
    std::vector<std::string> expected = {"b", "a"};
    size_t index = 0;
    for (const auto& [key, value] : map) {
        CHECK_EQ(key, expected[index]);
        index++;
    }
}

TEST_CASE("[OrderedMap] find") {
    OrderedMap<std::string, int> map;
    
    map.insert("a", 1);

    auto b = map.find("b");
    CHECK_EQ(b, map.end());

    auto it = map.find("a");
    REQUIRE(it != map.end());
    CHECK_EQ(it->first, "a");
    CHECK_EQ(it->second, 1);
}

TEST_CASE("[OrderedMap] contains") {
    OrderedMap<std::string, int> map;

    map.insert("a", 1);

    CHECK(map.contains("a"));
    CHECK_FALSE(map.contains("b"));
}


TEST_CASE("[OrderedMap] keys") {
    OrderedMap<std::string, int> map;

    map.insert("b", 1);
    map.insert("c", 2);
    map.insert("a", 3);

    auto keys = map.keys();

    REQUIRE(keys.size() == 3);
    CHECK_EQ(keys[0], "b");
    CHECK_EQ(keys[1], "c");
    CHECK_EQ(keys[2], "a");
}


TEST_CASE("[OrderedMap] clear") {
    OrderedMap<std::string, int> map;

    map.insert("a", 1);
    map.insert("b", 2);

    map.clear();

    CHECK(map.empty());
    CHECK(map.size() == 0);
    CHECK(map.keys().empty());
}

}