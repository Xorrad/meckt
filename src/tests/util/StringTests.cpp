#include "doctest/doctest.hpp"

TEST_CASE("[String] Strip") {
    CHECK(String::Strip("hello world", "world") == "hello ");
    CHECK(String::Strip("hello world", "hello ") == "world");
    CHECK(String::Strip("hello world", "good") == "hello world");
    CHECK(String::Strip("hello world", "") == "hello world");
}