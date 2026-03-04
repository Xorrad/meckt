#include "doctest/doctest.hpp"

#include "util/ResourceManager.hpp"

std::string GetFileFromMemory(const std::string& filePath) {
    auto fs = cmrc::Resources::get_filesystem();
    auto file = fs.open(filePath);
    return std::string(file.begin(), file.end());
}

TEST_SUITE("[Resources] Embeeded files") {
    #define CHECK_FILE(path) TEST_CASE("[Resources] Embeeded files" path) { \
        std::string memory = GetFileFromMemory(path); \
        std::ifstream in(path, std::ios::binary); \
        REQUIRE(in.is_open()); \
        std::string file = File::ReadString(in); \
        in.close(); \
        CHECK_EQ(memory, file); \
    }

    CHECK_FILE("resources/app/fonts/figtree_medium.ttf");
    CHECK_FILE("resources/app/fonts/notoemoji.ttf");
    CHECK_FILE("resources/app/fonts/notosans.ttf");

    CHECK_FILE("resources/app/shaders/provinces.frag");
    CHECK_FILE("resources/app/shaders/provinces.vert");
    CHECK_FILE("resources/app/shaders/heightmap_landmass.frag");
    CHECK_FILE("resources/app/shaders/provinces_landmass.frag");
}

TEST_SUITE("[Resources] ResourceManager") {

TEST_CASE("[Resources] ResourceManager: Texture") {
    // sf::Texture requires to load the texture on the graphics card.
    ResourceManager<sf::Image, int> manager("texture");
    
    REQUIRE_NOTHROW(manager.Load(1, "resources/tests/resource_manager/test.png"));

    sf::Image& image = manager.Get(1);
    REQUIRE(image.getSize() == sf::Vector2u{2, 2});
    CHECK_EQ(image.getPixel({0, 0}), sf::Color(13, 76, 127));
    CHECK_EQ(image.getPixel({1, 0}), sf::Color(13, 76, 127));
    CHECK_EQ(image.getPixel({0, 1}), sf::Color(142, 147, 152));
    CHECK_EQ(image.getPixel({1, 1}), sf::Color(142, 147, 152));
}

TEST_CASE("[Resources] ResourceManager: Font") {
    ResourceManager<sf::Font, int> manager("font");
    
    REQUIRE_NOTHROW(manager.Load(1, "resources/app/fonts/figtree_medium.ttf"));
    REQUIRE_NOTHROW(manager.Load(2, "resources/app/fonts/notoemoji.ttf"));
    REQUIRE_NOTHROW(manager.Load(3, "resources/app/fonts/notosans.ttf"));

    CHECK_EQ(manager.Get(1).getInfo().family, "Figtree");
    CHECK_EQ(manager.Get(2).getInfo().family, "Noto Emoji");
    CHECK_EQ(manager.Get(3).getInfo().family, "Noto Sans");
}

TEST_CASE("[Resources] ResourceManager: Shader") {
    ResourceManager<sf::Shader, int> manager("shader");
    REQUIRE(sf::Shader::isAvailable());
    CHECK_NOTHROW(manager.Load(1, "resources/app/shaders/provinces.vert", "resources/app/shaders/provinces.frag"));
    CHECK_NOTHROW(manager.Load(2, "resources/app/shaders/provinces_landmass.frag", sf::Shader::Type::Fragment));
    CHECK_NOTHROW(manager.Load(3, "resources/app/shaders/provinces.vert", sf::Shader::Type::Vertex));
    CHECK_NOTHROW(manager.Load(4, "resources/app/shaders/provinces.frag", sf::Shader::Type::Fragment));
}

}