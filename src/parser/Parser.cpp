#include "Parser.hpp"
#include <filesystem>

void Parser::Benchmark() {
    sf::Clock clock;

    // std::string filePath = "test_mod/map_data/default.map";
    // std::string filePath = "test_mod/map_data/island_region.txt";
    std::string filePath = "test_mod/map_data/geographical_regions/00_agot_geographical_region.txt";

    std::ifstream file(filePath);

    std::string content = File::ReadString(file);
    std::vector<PToken> tokens = Parser::Lex(content);

    sf::Time elapsed = clock.getElapsedTime();

    fmt::println("file path = {}", filePath);
    fmt::println("file size = {}", String::FileSizeFormat(std::filesystem::file_size(filePath)));
    fmt::println("tokens = {}", tokens.size());
    fmt::println("elapsed = {}", String::DurationFormat(elapsed));
}