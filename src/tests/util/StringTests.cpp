#include "doctest/doctest.hpp"

const std::string DIGITS = "0123456789";
const std::string LOWERCASE_LETTERS = "abcdefghijklmnopqrstuvwxyz";
const std::string UPPERCASE_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string PUNCTUATION_CHARACTERS = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

TEST_CASE("[String] Strip") {
    CHECK_EQ(String::Strip("hello world", "world"), "hello ");
    CHECK_EQ(String::Strip("hello world", "hello "), "world");
    CHECK_EQ(String::Strip("hello world", "good"), "hello world");
    CHECK_EQ(String::Strip("hello world", ""), "hello world");
}

TEST_CASE("[String] ToLowercase") {
    CHECK_EQ(String::ToLowercase(UPPERCASE_LETTERS), LOWERCASE_LETTERS);
    CHECK_EQ(String::ToLowercase(PUNCTUATION_CHARACTERS), PUNCTUATION_CHARACTERS);
    CHECK_EQ(String::ToLowercase(""), "");
}

TEST_CASE("[String] StripNonPrintable") {
    CHECK_EQ(String::StripNonPrintable(DIGITS), DIGITS);
    CHECK_EQ(String::StripNonPrintable(LOWERCASE_LETTERS), LOWERCASE_LETTERS);
    CHECK_EQ(String::StripNonPrintable(UPPERCASE_LETTERS), UPPERCASE_LETTERS);
    CHECK_EQ(String::StripNonPrintable(PUNCTUATION_CHARACTERS), PUNCTUATION_CHARACTERS);
}

TEST_CASE("[String] Split") {
    auto l = String::Split("hello world", " ");
    CHECK_EQ(l.size(), 2);
    CHECK_EQ(l.at(0), "hello");
    CHECK_EQ(l.at(1), "world");
    
    l = String::Split("one;two;three four", ";");
    CHECK_EQ(l.size(), 3);
    CHECK_EQ(l.at(0), "one");
    CHECK_EQ(l.at(1), "two");
    CHECK_EQ(l.at(2), "three four");
    
    l = String::Split("one1deltwo2delthree3del", "del");
    CHECK_EQ(l.size(), 4);
    CHECK_EQ(l.at(0), "one1");
    CHECK_EQ(l.at(1), "two2");
    CHECK_EQ(l.at(2), "three3");
    CHECK_EQ(l.at(3), "");
}

TEST_CASE("[String] Join") {
    CHECK_EQ(String::Join({"1"}, " | "), "1");
    CHECK_EQ(String::Join({"1", "2"}, " | "), "1 | 2");
    CHECK_EQ(String::Join({"1", "2", "3"}, " | "), "1 | 2 | 3");
    CHECK_EQ(String::Join({"one", "two", "three"}, ";"), "one;two;three");
}

TEST_CASE("[String] ReplaceAll") {
    std::string str = "hello world";
    String::ReplaceAll(str, "world", "");
    CHECK_EQ(str, "hello ");
    
    str = "123456";
    String::ReplaceAll(str, "345", "123");
    CHECK_EQ(str, "121236");
    
    str = "aaaaaaaa";
    String::ReplaceAll(str, "a", "b");
    CHECK_EQ(str, "bbbbbbbb");
    
    str = "hello world";
    String::ReplaceAll(str, "hello ", "");
    CHECK_EQ(str, "world");
}

TEST_CASE("[String] FileSizeFormat") {
    CHECK_EQ(String::FileSizeFormat(1), "1 B");
    CHECK_EQ(String::FileSizeFormat(2), "2 B");
    CHECK_EQ(String::FileSizeFormat(999), "999 B");
    CHECK_EQ(String::FileSizeFormat(1000), "1.0 KB");
    CHECK_EQ(String::FileSizeFormat(1500), "1.5 KB");
    CHECK_EQ(String::FileSizeFormat(999000), "999.0 KB");
    CHECK_EQ(String::FileSizeFormat(1000000), "1.0 MB");
    CHECK_EQ(String::FileSizeFormat(999000000), "999.0 MB");
    CHECK_EQ(String::FileSizeFormat(1000000000), "1.0 GB");
    CHECK_EQ(String::FileSizeFormat(2500000000), "2.5 GB");
}

TEST_CASE("[String] DurationFormat") {
    CHECK_EQ(String::DurationFormat(sf::microseconds(1)), "1μs");
    CHECK_EQ(String::DurationFormat(sf::microseconds(999)), "999μs");
    CHECK_EQ(String::DurationFormat(sf::milliseconds(1)), "1ms");
    CHECK_EQ(String::DurationFormat(sf::milliseconds(999)), "999ms");
    CHECK_EQ(String::DurationFormat(sf::seconds(1)), "1.0s");
    CHECK_EQ(String::DurationFormat(sf::seconds(10)), "10.0s");
    CHECK_EQ(String::DurationFormat(sf::seconds(60)), "60.0s");
    CHECK_EQ(String::DurationFormat(sf::seconds(61)), "61.0s");
}

TEST_CASE("[String] IsDigit") {
    for (char c : DIGITS)
        CHECK(String::IsDigit(c));
    for (char c : LOWERCASE_LETTERS)
        CHECK_FALSE(String::IsDigit(c));
    for (char c : UPPERCASE_LETTERS)
        CHECK_FALSE(String::IsDigit(c));
    for (char c : PUNCTUATION_CHARACTERS)
        CHECK_FALSE(String::IsDigit(c));
}

TEST_CASE("[String] IsAlpha") {
    for (char c : DIGITS)
        CHECK_FALSE(String::IsAlpha(c));
    for (char c : LOWERCASE_LETTERS)
        CHECK(String::IsAlpha(c));
    for (char c : UPPERCASE_LETTERS)
        CHECK(String::IsAlpha(c));
    for (char c : PUNCTUATION_CHARACTERS) {
        if (c == '_') continue;
        CHECK_FALSE(String::IsAlpha(c));
    }
    CHECK(String::IsAlpha('_'));
}

TEST_CASE("[String] IsAlphaNumeric") {
    for (char c : DIGITS)
        CHECK(String::IsAlphaNumeric(c));
    for (char c : LOWERCASE_LETTERS)
        CHECK(String::IsAlphaNumeric(c));
    for (char c : UPPERCASE_LETTERS)
        CHECK(String::IsAlphaNumeric(c));
    for (char c : PUNCTUATION_CHARACTERS) {
        if (c == '_') continue;
        CHECK_FALSE(String::IsAlphaNumeric(c));
    }
    CHECK(String::IsAlphaNumeric('_'));
}

TEST_SUITE("[String] ParseDouble") {

TEST_CASE("[String] ParseDouble: valid decimal numbers") {
    CHECK(String::ParseDouble("0") == doctest::Approx(0.0));
    CHECK(String::ParseDouble("123") == doctest::Approx(123.0));
    CHECK(String::ParseDouble("-123") == doctest::Approx(-123.0));
    CHECK(String::ParseDouble("3.14159") == doctest::Approx(3.14159));
    CHECK(String::ParseDouble("-0.001") == doctest::Approx(-0.001));
}

TEST_CASE("[String] ParseDouble: leading and trailing spaces") {
    CHECK(String::ParseDouble("  42") == doctest::Approx(42.0));
    CHECK(String::ParseDouble("42   ") == doctest::Approx(42.0));
    CHECK(String::ParseDouble("   42.5   ") == doctest::Approx(42.5));
}

TEST_CASE("[String] ParseDouble: scientific notation") {
    CHECK(String::ParseDouble("1e3") == doctest::Approx(1000.0));
    CHECK(String::ParseDouble("2.5e-3") == doctest::Approx(0.0025));
    CHECK(String::ParseDouble("-1E2") == doctest::Approx(-100.0));
}

TEST_CASE("[String] ParseDouble: invalid input") {
    CHECK_THROWS_AS(String::ParseDouble("abc"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseDouble(""), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseDouble("12.3abc"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseDouble("1,23"), std::invalid_argument); // comma not allowed in "C" locale.
}

TEST_CASE("[String] ParseDouble: trailing garbage") {
    CHECK_THROWS_AS(String::ParseDouble("42x"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseDouble("3.14 pi"), std::invalid_argument);
}

TEST_CASE("[String] ParseDouble: locale") {
    // Save current global locale
    std::locale oldLocale = std::locale::global(std::locale("C"));

    try {
        std::locale::global(std::locale("de_DE.UTF-8"));

        CHECK(String::ParseDouble("3.14") == doctest::Approx(3.14));
        CHECK_THROWS_AS(String::ParseDouble("3,14"), std::invalid_argument);
    } catch (...) {}

    try {
        std::locale::global(std::locale("fr_FR.UTF-8"));

        CHECK(String::ParseDouble("3.14") == doctest::Approx(3.14));
        CHECK_THROWS_AS(String::ParseDouble("3,14"), std::invalid_argument);
    } catch (...) {}

    // Restore original locale
    std::locale::global(oldLocale);
}

}

TEST_SUITE("[String] ParseInt") {

TEST_CASE("[String] ParseInt: valid integers") {
    CHECK(String::ParseInt("0") == 0);
    CHECK(String::ParseInt("123") == 123);
    CHECK(String::ParseInt("-123") == -123);
    CHECK(String::ParseInt("+42") == 42);
}

TEST_CASE("[String] ParseInt: whitespace") {
    CHECK(String::ParseInt("  42") == 42);
    CHECK(String::ParseInt("42   ") == 42);
    CHECK(String::ParseInt("   -17   ") == -17);
}

TEST_CASE("[String] ParseInt: invalid input") {
    CHECK_THROWS_AS(String::ParseInt(""), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseInt("abc"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseInt("12abc"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseInt("abc12"), std::invalid_argument);
}

TEST_CASE("[String] ParseInt: decimal numbers") {
    CHECK_THROWS_AS(String::ParseInt("3.14"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseInt("10.0"), std::invalid_argument);
}

TEST_CASE("[String] ParseInt: comma separator") {
    CHECK_THROWS_AS(String::ParseInt("1,000"), std::invalid_argument);
}

TEST_CASE("[String] ParseInt: trailing garbage") {
    CHECK_THROWS_AS(String::ParseInt("42x"), std::invalid_argument);
    CHECK_THROWS_AS(String::ParseInt("7 test"), std::invalid_argument);
}

}