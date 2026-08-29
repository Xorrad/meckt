#include <cstring>
#include <filesystem>
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.hpp"

#include "core/util/SignalHandler.hpp"
#include "core/mod/Mod.hpp"

SignalHandler SIGNAL_HANDLER;

int ImportExportMod(const std::string& modPath);

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "--mod") == 0) {
        if (argc < 3)
            return 1;
        return ImportExportMod(argv[2]);;
    }

    doctest::Context context(argc, argv);
    return context.run();
}

int ImportExportMod(const std::string& modPath) {
    if (!std::filesystem::exists(modPath))
        return 1;

    // Import and then export the whole mod.
    {
        Mod mod(modPath);

        bool failed = false;

        LOG_INFO("\n\n---------------------------------------------- LOADING ----------------------------------------------\n");
        mod.Load(
            [&]() { },
            [&](LoadingState state) { },
            [&](const std::string& error) { failed = true; }
        );

        if (failed) {
            std::cout << "ERROR: Failed to import the mod" << std::endl;
            return 1;
        }

        LOG_INFO("\n\n---------------------------------------------- EXPORTING ----------------------------------------------\n");
        mod.Export();
    }

    // Check that the mod can be imported again without crash.
    {
        Mod mod(modPath);

        bool failed = false;

        LOG_INFO("\n\n---------------------------------------------- RELOADING ----------------------------------------------\n");
        mod.Load(
            [&]() { },
            [&](LoadingState state) { },
            [&](const std::string& error) { failed = true; }
        );

        if (failed) {
            std::cout << "ERROR: Failed to re-import the mod" << std::endl;
            return 1;
        }
    }

    return 0;
}