#include "doctest/doctest.hpp"

#include "script/ScriptDocs.hpp"

TEST_SUITE("[Script::Docs]") {

TEST_CASE("[Script::Docs] Empty by default") {
    Script::Docs docs;

    CHECK_EQ(docs.CountEntries(), 0);
    CHECK(docs.GetEntries().empty());
    CHECK(docs.GetScopes().empty());
    CHECK(docs.ToString().empty());
}

TEST_CASE("[Script::Docs] Register keeps entries in registration order") {
    Script::Docs docs;

    docs.Register("provinces", "Count", "provinces.Count() -> integer", "Counts provinces.");
    docs.Register("provinces", "GetById", "provinces.GetById(id) -> Province|nil", "Finds a province.");

    REQUIRE_EQ(docs.CountEntries(), 2);
    CHECK_EQ(docs.GetEntries()[0].name, "Count");
    CHECK_EQ(docs.GetEntries()[1].name, "GetById");
    CHECK_EQ(docs.GetEntries()[0].scope, "provinces");
    CHECK_EQ(docs.GetEntries()[0].signature, "provinces.Count() -> integer");
    CHECK_EQ(docs.GetEntries()[0].description, "Counts provinces.");
}

TEST_CASE("[Script::Docs] GetScopes deduplicates and preserves first-seen order") {
    Script::Docs docs;

    docs.Register("titles", "Count", "titles.Count()", "");
    docs.Register("provinces", "Count", "provinces.Count()", "");
    docs.Register("titles", "Get", "titles.Get(name)", "");

    CHECK_EQ(docs.GetScopes(), std::vector<std::string>{"titles", "provinces"});
}

TEST_CASE("[Script::Docs] GetEntries filters by scope") {
    Script::Docs docs;

    docs.Register("titles", "Count", "titles.Count()", "");
    docs.Register("provinces", "Count", "provinces.Count()", "");
    docs.Register("titles", "Get", "titles.Get(name)", "");

    std::vector<const Script::DocEntry*> entries = docs.GetEntries("titles");
    REQUIRE_EQ(entries.size(), 2);
    CHECK_EQ(entries[0]->name, "Count");
    CHECK_EQ(entries[1]->name, "Get");

    CHECK_EQ(docs.GetEntries("cultures").size(), 0);
}

TEST_CASE("[Script::Docs] Clear removes every entry") {
    Script::Docs docs;

    docs.Register("provinces", "Count", "provinces.Count()", "");
    REQUIRE_EQ(docs.CountEntries(), 1);

    docs.Clear();

    CHECK_EQ(docs.CountEntries(), 0);
    CHECK(docs.GetScopes().empty());
}

TEST_CASE("[Script::Docs] ToString groups entries under their scope") {
    Script::Docs docs;

    docs.Register("provinces", "Count", "provinces.Count() -> integer", "Counts provinces.");

    std::string text = docs.ToString();
    CHECK(text.find("--- provinces ---") != std::string::npos);
    CHECK(text.find("provinces.Count() -> integer") != std::string::npos);
    CHECK(text.find("Counts provinces.") != std::string::npos);
}

//////////////////////////////////////////////////////

TEST_CASE("[Script::Docs] SCRIPT_FN binds the function and documents it together") {
    sol::state state;
    Script::Docs docs;

    auto table = Script::MakeBinder(docs, state.create_named_table("test"), "test");

    SCRIPT_FN(table, "Double", "test.Double(n) -> integer",
        "Doubles a number.",
        [](int n) { return n * 2; });

    // Bound: callable from a script.
    CHECK_EQ(state.script("return test.Double(21)").get<int>(), 42);

    // Documented: the entry landed in the reference with the same name.
    REQUIRE_EQ(docs.CountEntries(), 1);
    CHECK_EQ(docs.GetEntries()[0].scope, "test");
    CHECK_EQ(docs.GetEntries()[0].name, "Double");
    CHECK_EQ(docs.GetEntries()[0].signature, "test.Double(n) -> integer");
}

}
