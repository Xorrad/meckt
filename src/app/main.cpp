#include "app/App.hpp"

#include <CLI/CLI.hpp>
#include <fmt/base.h>

#include "core/util/SignalHandler.hpp"
SignalHandler SIGNAL_HANDLER;

int main(int argc, char** argv) {
    CLI::App cli{"meckt - A map editor for Crusader Kings 3."};
    argv = cli.ensure_utf8(argv);


    //////////////////////////////////////////////////////////////
    // - open: open and load a mod from a specified directory.
    //     --title title to select
    //     --map_mode map mode to focus
    //     --script lua script to execute
    //////////////////////////////////////////////////////////////

    auto* openCommand = cli.add_subcommand(
        "open",
        "Open and load a mod from a specified directory."
    );

    std::string directory;
    int province;
    std::string title;
    std::string mapModeStr;
    std::string script;

    CLI::Option* directoryOption = openCommand->add_option(
        "directory",
        directory,
        "Directory containing the mod."
    )->type_name("DIR")
    ->check(CLI::ExistingDirectory)
    ->required();

    CLI::Option* provinceOption = openCommand->add_option(
        "--province,-p",
        province,
        "Province to select."
    )
    ->check(CLI::PositiveNumber);
        
    CLI::Option* titleOption = openCommand->add_option(
        "--title,-t",
        title,
        "Title to select."
    );

    CLI::Option* mapModeOption = openCommand->add_option(
        "--map-mode,-m",
        mapModeStr,
        "Map mode to focus."
    )->check(CLI::IsMember(MapModeLabels, CLI::ignore_case));

    CLI::Option* scriptOption = openCommand->add_option(
        "--script,-s",
        script,
        "Lua script to execute."
    )->check(CLI::ExistingFile);

    //////////////////////////////////////////////////////////////
    // - analyze: run tests, checks, benchmarks, and output the results.
    //////////////////////////////////////////////////////////////

    // TODO: implement the analyze command.

    CLI::App* analyseCommand = nullptr;
    // auto* analyseCommand = cli.add_subcommand(
    //     "analyze",
    //     "Run tests, checks, benchmarks, and output the results."
    // );

    // analyseCommand->add_option(
    //     "directory",
    //     directory,
    //     "Directory containing the mod."
    // )->type_name("DIR")
    // ->check(CLI::ExistingDirectory)
    // ->required();

    //////////////////////////////////////////////////////////////

    CLI11_PARSE(cli, argc, argv);

    if (*openCommand) {
        App app;
        app.Init();

        EditorSetup setup;
        setup.selectedProvinceId =(provinceOption->count() > 0) ? Opt<int>(province) : std::nullopt;
        setup.selectedTitleName = (titleOption->count() > 0) ? Opt<std::string>(title) : std::nullopt;
        setup.mapMode = (mapModeOption->count() > 0) ? Opt<MapMode>(MapModeFromString(mapModeStr)) : std::nullopt;
        setup.scriptFilePath = (scriptOption->count() > 0) ? Opt<std::string>(script) : std::nullopt;

        UniquePtr<Mod> mod = MakeUnique<Mod>(directory);
        if(mod->HasMap()) {
            try {
                app.OpenMod(std::move(mod), setup);
                LOG_INFO("Opened mod at {}", directory);
            }
            catch (std::exception& e) {
                fmt::println(stderr, "Failed to load mod at {}\n{}", directory, e.what());
                return 1;
            }
        }
        else {
            fmt::println(stderr, "Failed to find required 'map_data/provinces.png' file at {}", directory);
            return 1;
        }

        app.Run();
    }
    else if (*analyseCommand) {
        std::cout << "Running analysis...\n";
        // TODO: Implement checks and output results functionality.
    }
    else {
        // If no commands have been specified, then run the default behavior of the application.
        App app;
        app.Init();
        app.Run();
    }

    return 0;
}