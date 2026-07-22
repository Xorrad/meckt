#include <cstring>
#include <filesystem>
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.hpp"

#include "core/mod/Mod.hpp"
int ImportExportMod(const std::string& modPath);

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "--mod") == 0) {
        if (argc < 3)
            return 1;
        return ImportExportMod(argv[2]);;
    }

    doctest::Context context(argc, argv);
    context.run();
    return 0;
}

int ImportExportMod(const std::string& modPath) {
    if (!std::filesystem::exists(modPath))
        return 1;
    
    // Import and then export the whole mod.
    {
        Mod mod(modPath);

        try {
            bool failed = false;
            mod.Load(
                [&]() { },
                [&](LoadingState state) { },
                [&](const std::string& error) { failed = true; }
            );

            if (failed) {
                std::cout << "ERROR: Failed to import the mod" << std::endl;
                return 1;
            }
        }
        catch (std::exception& e) {
            std::cout << "ERROR: Failed to import the mod" << "\n";
            std::cout << e.what() << std::endl;
            return 1;
        }

        try {
            mod.Export();
        }
        catch (std::exception& e) {
            std::cout << "ERROR: Failed to export the mod" << "\n";
            std::cout << e.what() << std::endl;
            return 1;
        }
    }

    // Check that the mod can be imported again without crash.
    {
        Mod mod(modPath);

        try {
            bool failed = false;
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
        catch (std::exception& e) {
            std::cout << "ERROR: Failed to re-import the mod" << "\n";
            std::cout << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}