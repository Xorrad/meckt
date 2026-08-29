#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "provinces/ProvinceManager.hpp"
#include "titles/TitleManager.hpp"
#include "cultures/CultureManager.hpp"
#include "cultures/Culture.hpp"
#include "religions/ReligionManager.hpp"
#include "religions/Faith.hpp"
#include "script/ScriptEngine.hpp"

namespace {

struct TestMod {
    Mod mod;

    TestMod() : mod("") {
        ProvinceManager& provinces = mod.GetProvinceManager();
        provinces.AddProvince(MakeUnique<Province>(1, sf::Color(10, 10, 10), "Alpha"));
        provinces.AddProvince(MakeUnique<Province>(2, sf::Color(20, 20, 20), "Beta"));
        provinces.AddProvince(MakeUnique<Province>(3, sf::Color(30, 30, 30), "Gamma"));

        provinces.GetProvinceById(1)->SetFlag(ProvinceFlags::LAND, true);
        provinces.GetProvinceById(2)->SetFlag(ProvinceFlags::SEA, true);
        provinces.GetProvinceById(3)->SetFlag(ProvinceFlags::LAND, true);

        TitleManager& titles = mod.GetTitleManager();
        titles.AddTitle(MakeTitle(TitleType::BARONY, "b_alpha", sf::Color::Red, false));
        titles.AddTitle(MakeTitle(TitleType::COUNTY, "c_alpha", sf::Color::Blue, false));

        mod.GetCultureManager().AddCulture(MakeUnique<Culture>("test_culture", sf::Color::Green));
        mod.GetReligionManager().AddFaith(MakeUnique<Faith>("test_faith", sf::Color::Yellow));
    }
};

}

TEST_SUITE("[Script::Engine]") {

//////////////////////////////////////////////////////
// Running scripts
//////////////////////////////////////////////////////

TEST_CASE("[Script::Engine] Run: reports success and captures print output") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run("print('hello') print('world')");

    CHECK(result.success);
    CHECK(result.error.empty());
    REQUIRE_EQ(result.output.size(), 2);
    CHECK_EQ(result.output[0], "hello");
    CHECK_EQ(result.output[1], "world");
}

TEST_CASE("[Script::Engine] Run: print joins its arguments with tabs") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run("print(1, 'two', true)");

    REQUIRE(result.success);
    REQUIRE_EQ(result.output.size(), 1);
    CHECK_EQ(result.output[0], "1\ttwo\ttrue");
}

TEST_CASE("[Script::Engine] Run: reports syntax errors without throwing") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result;
    REQUIRE_NOTHROW(result = engine.Run("this is not lua"));

    CHECK_FALSE(result.success);
    CHECK_FALSE(result.error.empty());
}

TEST_CASE("[Script::Engine] Run: reports runtime errors without throwing") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result;
    REQUIRE_NOTHROW(result = engine.Run("error('boom')"));

    CHECK_FALSE(result.success);
    CHECK(result.error.find("boom") != std::string::npos);
}

TEST_CASE("[Script::Engine] Run: keeps the output produced before an error") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run("print('before') error('boom')");

    CHECK_FALSE(result.success);
    REQUIRE_EQ(result.output.size(), 1);
    CHECK_EQ(result.output[0], "before");
}

TEST_CASE("[Script::Engine] Run: a binding's misuse becomes a Lua error") {
    TestMod test;
    Script::Engine engine(test.mod);

    // Duplicate id: the binding throws, which must surface as a script error
    // rather than escaping Run().
    Script::Result result;
    REQUIRE_NOTHROW(result = engine.Run("provinces.Create(1, 'Duplicate', Color.new(90, 90, 90))"));

    CHECK_FALSE(result.success);
    CHECK(result.error.find("already exists") != std::string::npos);
}

TEST_CASE("[Script::Engine] Run: clears the previous run's output") {
    TestMod test;
    Script::Engine engine(test.mod);

    engine.Run("print('first')");
    Script::Result result = engine.Run("print('second')");

    REQUIRE_EQ(result.output.size(), 1);
    CHECK_EQ(result.output[0], "second");
}

TEST_CASE("[Script::Engine] Run: aborts a script that exceeds the instruction limit") {
    TestMod test;
    Script::Engine engine(test.mod);
    engine.SetInstructionLimit(10'000);

    Script::Result result;
    REQUIRE_NOTHROW(result = engine.Run("while true do end"));

    CHECK_FALSE(result.success);
    CHECK(result.error.find("instruction limit") != std::string::npos);
}

TEST_CASE("[Script::Engine] Run: the filesystem libraries are not exposed") {
    TestMod test;
    Script::Engine engine(test.mod);

    CHECK(engine.Run("return io.open('x')").success == false);
    CHECK(engine.Run("return os.execute('x')").success == false);
    CHECK(engine.Run("return require('x')").success == false);

    // Check that the sandboxed standard libraries still work.
    CHECK(engine.Run("return string.upper('a') .. math.floor(1.5) .. #({1,2})").success);
}

//////////////////////////////////////////////////////
// Bindings
//////////////////////////////////////////////////////

TEST_CASE("[Script::Engine] provinces bindings") {
    TestMod test;
    Script::Engine engine(test.mod);

    SUBCASE("Count/GetById/GetAll") {
        Script::Result result = engine.Run(
            "print(provinces.Count())\n"
            "print(provinces.GetById(2):GetName())\n"
            "print(#provinces.GetAll())\n"
            "print(provinces.GetMaxId())\n"
        );

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "3");
        CHECK_EQ(result.output[1], "Beta");
        CHECK_EQ(result.output[2], "3");
        CHECK_EQ(result.output[3], "3");
    }

    SUBCASE("GetById returns nil for an unknown id") {
        Script::Result result = engine.Run("print(provinces.GetById(999) == nil)");

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "true");
    }

    SUBCASE("Create adds a province the managers can see") {
        Script::Result result = engine.Run(
            "local p = provinces.Create(9, 'Delta', Color.new(90, 90, 90))\n"
            "print(p:GetId(), p:GetName())\n"
        );

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "9\tDelta");

        REQUIRE(test.mod.GetProvinceManager().HasProvinceById(9));
        CHECK_EQ(test.mod.GetProvinceManager().GetProvinceById(9)->GetName(), "Delta");
    }

    SUBCASE("Remove deletes a province from the mod") {
        Script::Result result = engine.Run("provinces.Remove(provinces.GetById(2))");

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_FALSE(test.mod.GetProvinceManager().HasProvinceById(2));
    }
}

TEST_CASE("[Script::Engine] Province bindings read and write the underlying province") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run(
        "local p = provinces.GetById(1)\n"
        "print(p:GetId(), p:GetName(), p:HasFlag(ProvinceFlags.LAND), p:HasFlag(ProvinceFlags.SEA))\n"
        "p:SetName('Renamed')\n"
        "p:SetCulture('test_culture')\n"
        "p:SetFaith('test_faith')\n"
        "p:SetClimate(ClimateType.SEVERE_WINTER)\n"
        "p:SetFlag(ProvinceFlags.COASTAL, true)\n"
    );

    REQUIRE_MESSAGE(result.success, result.error);
    CHECK_EQ(result.output[0], "1\tAlpha\ttrue\tfalse");

    Province* province = test.mod.GetProvinceManager().GetProvinceById(1);
    CHECK_EQ(province->GetName(), "Renamed");
    CHECK_EQ(province->GetCulture(), "test_culture");
    CHECK_EQ(province->GetFaith(), "test_faith");
    CHECK_EQ(province->GetClimateType(), ClimateType::SEVERE_WINTER);
    CHECK(province->HasFlag(ProvinceFlags::COASTAL));
}

TEST_CASE("[Script::Engine] titles bindings") {
    TestMod test;
    Script::Engine engine(test.mod);

    SUBCASE("Count/Get/GetAllOfType") {
        Script::Result result = engine.Run(
            "print(titles.Count())\n"
            "print(titles.Get('c_alpha'):GetTypeName())\n"
            "print(#titles.GetAllOfType(TitleType.BARONY))\n"
            "print(titles.Has('c_alpha'), titles.Has('c_nope'))\n"
        );

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "2");
        CHECK_EQ(result.output[1], "County");
        CHECK_EQ(result.output[2], "1");
        CHECK_EQ(result.output[3], "true\tfalse");
    }

    SUBCASE("Create rejects a name that doesn't match the tier") {
        Script::Result result = engine.Run("titles.Create('x_bad', TitleType.DUCHY, Color.new(1, 2, 3))");

        CHECK_FALSE(result.success);
        CHECK(result.error.find("not a valid") != std::string::npos);
    }

    SUBCASE("Create adds a title the manager can see") {
        Script::Result result = engine.Run(
            "local t = titles.Create('d_test', TitleType.DUCHY, Color.new(1, 2, 3))\n"
            "print(t:GetName(), t:GetTypeName())\n"
        );

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "d_test\tDuchy");
        CHECK(test.mod.GetTitleManager().HasTitle("d_test"));
    }

    SUBCASE("Tier-specific functions error on the wrong tier") {
        // A barony holds no dejure titles, so asking for them is a script error
        // rather than a crash.
        Script::Result result = engine.Run("return titles.Get('b_alpha'):GetDejureTitles()");

        CHECK_FALSE(result.success);
        CHECK(result.error.find("b_alpha") != std::string::npos);
    }

    SUBCASE("Dejure titles can be read and edited") {
        Script::Result result = engine.Run(
            "local county = titles.Get('c_alpha')\n"
            "county:AddDejureTitle(titles.Get('b_alpha'))\n"
            "print(#county:GetDejureTitles())\n"
            "print(county:GetDejureTitles()[1]:GetName())\n"
        );

        REQUIRE_MESSAGE(result.success, result.error);
        CHECK_EQ(result.output[0], "1");
        CHECK_EQ(result.output[1], "b_alpha");
    }
}

TEST_CASE("[Script::Engine] cultures and religions bindings") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run(
        "print(cultures.Count(), religions.Count())\n"
        "print(cultures.Get('test_culture'):GetName())\n"
        "print(religions.Get('test_faith'):GetName())\n"
        "print(#cultures.GetAll(), #religions.GetAll())\n"
        "cultures.Get('test_culture'):SetColor(Color.new(1, 2, 3))\n"
    );

    REQUIRE_MESSAGE(result.success, result.error);
    CHECK_EQ(result.output[0], "1\t1");
    CHECK_EQ(result.output[1], "test_culture");
    CHECK_EQ(result.output[2], "test_faith");
    CHECK_EQ(result.output[3], "1\t1");
    CHECK_EQ(test.mod.GetCultureManager().GetCulture("test_culture")->GetColor(), sf::Color(1, 2, 3));
}

TEST_CASE("[Script::Engine] adjacencies bindings") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run(
        "local a = adjacencies.Create(1, 2, 'sea', 3, 'a comment')\n"
        "print(a:GetFromId(), a:GetToId(), a:GetThroughId(), a:GetType(), a:GetComment())\n"
        "print(adjacencies.Count(), adjacencies.Has(1, 2))\n"
    );

    REQUIRE_MESSAGE(result.success, result.error);
    CHECK_EQ(result.output[0], "1\t2\t3\tsea\ta comment");
    CHECK_EQ(result.output[1], "1\ttrue");

    CHECK(test.mod.GetProvinceManager().HasAdjacency(1, 2));
}

TEST_CASE("[Script::Engine] Color usertype") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run(
        "local c = Color.new(255, 128, 64)\n"
        "print(c.r, c.g, c.b, c.a)\n"
        "print(c:ToString())\n"
        "c.r = 1\n"
        "print(c.r)\n"
    );

    REQUIRE_MESSAGE(result.success, result.error);
    CHECK_EQ(result.output[0], "255\t128\t64\t255");
    CHECK_EQ(result.output[1], "(255, 128, 64, 255)");
    CHECK_EQ(result.output[2], "1");
}

TEST_CASE("[Script::Engine] Scripts can iterate and filter the mod's data") {
    TestMod test;
    Script::Engine engine(test.mod);

    Script::Result result = engine.Run(R"(
        local land = 0
        for _, province in ipairs(provinces.GetAll()) do
            if province:HasFlag(ProvinceFlags.LAND) then
                land = land + 1
            end
        end
        print(land)
    )");

    REQUIRE_MESSAGE(result.success, result.error);
    CHECK_EQ(result.output[0], "2");
}

//////////////////////////////////////////////////////
// Map invalidation
//////////////////////////////////////////////////////

TEST_CASE("[Script::Engine] IsMapInvalidated tracks edits that change the map") {
    TestMod test;
    Script::Engine engine(test.mod);

    SUBCASE("a read-only script leaves the map alone") {
        engine.Run("return provinces.Count()");
        CHECK_FALSE(engine.IsMapInvalidated());
    }

    SUBCASE("an edit to a drawn attribute invalidates the map") {
        engine.Run("provinces.GetById(1):SetCulture('test_culture')");
        CHECK(engine.IsMapInvalidated());
    }

    SUBCASE("map.Invalidate() invalidates it explicitly") {
        engine.Run("map.Invalidate()");
        CHECK(engine.IsMapInvalidated());
    }

    SUBCASE("the flag is reset at the start of each run") {
        engine.Run("map.Invalidate()");
        REQUIRE(engine.IsMapInvalidated());

        engine.Run("return 1");
        CHECK_FALSE(engine.IsMapInvalidated());
    }
}

//////////////////////////////////////////////////////
// Docs
//////////////////////////////////////////////////////

TEST_CASE("[Script::Engine] Every documented function exists in the Lua state") {
    TestMod test;
    Script::Engine engine(test.mod);
    sol::state& state = engine.GetState();

    const Script::Docs& docs = engine.GetDocs();
    REQUIRE(docs.CountEntries() > 0);

    for (const Script::DocEntry& entry : docs.GetEntries()) {
        // "globals" documents free functions; "Color" documents its fields
        // alongside its methods, and a field isn't a function.
        if (entry.scope == "Color" && entry.name == "r")
            continue;

        sol::object value;
        if (entry.scope == "globals")
            value = state[entry.name];
        else
            value = state[entry.scope][entry.name];

        INFO("documented entry: ", entry.signature);
        CHECK(value.valid());
        CHECK(value.get_type() == sol::type::function);
    }
}

TEST_CASE("[Script::Engine] The docs cover every bound scope") {
    TestMod test;
    Script::Engine engine(test.mod);

    std::vector<std::string> scopes = engine.GetDocs().GetScopes();

    for (const std::string& expected : {
        "globals", "Color", "Province", "Title", "Culture", "Faith", "Region", "Adjacency",
        "mod", "provinces", "titles", "cultures", "religions", "regions", "adjacencies", "map"
    }) {
        INFO("expected scope: ", expected);
        CHECK(std::find(scopes.begin(), scopes.end(), expected) != scopes.end());
    }
}

TEST_CASE("[Script::Engine] Documented entries are unique and fully filled in") {
    TestMod test;
    Script::Engine engine(test.mod);

    std::set<std::pair<std::string, std::string>> seen;

    for (const Script::DocEntry& entry : engine.GetDocs().GetEntries()) {
        INFO("entry: ", entry.scope, ".", entry.name);

        CHECK_FALSE(entry.scope.empty());
        CHECK_FALSE(entry.name.empty());
        CHECK_FALSE(entry.signature.empty());
        CHECK_FALSE(entry.description.empty());

        CHECK(seen.insert({ entry.scope, entry.name }).second);
    }
}

}
