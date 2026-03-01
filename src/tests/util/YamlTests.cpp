#include "doctest/doctest.hpp"
#include "util/Yaml.hpp"

TEST_SUITE("[Yaml] Parse") {

TEST_CASE("[Yaml] Parse: empty") {
    std::map<std::string, std::string> map = Yaml::Parse("");
    CHECK_EQ(map.size(), 0);
}

TEST_CASE("[Yaml] Parse: one pair") {
    std::map<std::string, std::string> map = Yaml::Parse("key: \"12345\"");
    REQUIRE(map.size() == 1);
    REQUIRE(map.contains("key"));
    CHECK_EQ(map.at("key"), "12345");
}

TEST_CASE("[Yaml] Parse: multiple pairs") {
    std::map<std::string, std::string> map = Yaml::Parse("a:\"12345\"\nb:\"abcd\"\nkey:\"hello world\"");
    REQUIRE(map.size() == 3);
    REQUIRE(map.contains("a"));
    REQUIRE(map.contains("b"));
    REQUIRE(map.contains("key"));
    CHECK_EQ(map.at("a"), "12345");
    CHECK_EQ(map.at("b"), "abcd");
    CHECK_EQ(map.at("key"), "hello world");
}

TEST_CASE("[Yaml] Parse: skip white-spaces") {
    std::map<std::string, std::string> map = Yaml::Parse("\t key : \t\"\t123 45\"  ");
    REQUIRE(map.size() == 1);
    REQUIRE(map.contains("key"));
    CHECK_EQ(map.at("key"), "\t123 45");
}

TEST_CASE("[Yaml] Parse: skip UTF8-BOM bytes") {
    std::map<std::string, std::string> map = Yaml::Parse("\xEF\xBB\xBFkey: \"1\"");
    REQUIRE(map.size() == 1);
    REQUIRE(map.contains("key"));
    CHECK_EQ(map.at("key"), "1");
}

TEST_CASE("[Yaml] Parse: skip number after key colon") {
    std::map<std::string, std::string> map = Yaml::Parse("key1:0 \"1\"\nkey2:10 \"hello world\"");
    REQUIRE(map.size() == 2);
    REQUIRE(map.contains("key1"));
    REQUIRE(map.contains("key2"));
    CHECK_EQ(map.at("key1"), "1");
    CHECK_EQ(map.at("key2"), "hello world");
}

TEST_CASE("[Yaml] Parse: key without value defaults to empty string") {
    std::map<std::string, std::string> map = Yaml::Parse("key1:0\nkey2:0 \"hello world\"");
    REQUIRE(map.size() == 2);
    REQUIRE(map.contains("key1"));
    REQUIRE(map.contains("key2"));
    CHECK_EQ(map.at("key1"), "");
    CHECK_EQ(map.at("key2"), "hello world");
}

TEST_CASE("[Yaml] Parse: comments skip a line") {
    std::map<std::string, std::string> map = Yaml::Parse("# a comment\nkey1: \"1\" #another one\nkey2:0 \"hello world\"# again key3: \"2\"");
    REQUIRE(map.size() == 2);
    REQUIRE(map.contains("key1"));
    REQUIRE(map.contains("key2"));
    CHECK_EQ(map.at("key1"), "1");
    CHECK_EQ(map.at("key2"), "hello world");
}

}