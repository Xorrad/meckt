#include "doctest/doctest.hpp"

#include "mod/Mod.hpp"
#include "defines/DefineManager.hpp"

TEST_SUITE("[DefineManager]") {

//////////////////////////////////////////////////////

TEST_CASE("[DefineManager] LoadDefines") {
    Mod mod("resources/tests/define_manager/test_mod");
    DefineManager defineManager(mod);

    REQUIRE_NOTHROW(defineManager.LoadDefines());

    CHECK(defineManager.GetWaterLevel() == 1.5f);
}

}