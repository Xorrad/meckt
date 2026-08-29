#include "doctest/doctest.hpp"

#include "util/TextureChunkGrid.hpp"

TEST_CASE("[TextureChunkGrid] Default Constructor") {
    TextureChunkGrid grid;

    CHECK(grid.GetSize() == sf::Vector2u{0, 0});
    CHECK(grid.GetChunkCount() == sf::Vector2u{0, 0});
    CHECK(grid.GetChunk(0, 0) == nullptr);
    CHECK(grid.GetVisibleChunkCoords(sf::FloatRect({0.f, 0.f}, {100.f, 100.f})).empty());
}

TEST_CASE("[TextureChunkGrid] Build: single chunk smaller than ChunkSize") {
    sf::Image image({10, 10}, sf::Color::White);

    TextureChunkGrid grid;
    grid.Build(image);

    CHECK(grid.GetSize() == sf::Vector2u{10, 10});
    CHECK(grid.GetChunkCount() == sf::Vector2u{1, 1});

    TextureChunk* chunk = grid.GetChunk(0, 0);
    REQUIRE(chunk != nullptr);
    CHECK(chunk->bounds == sf::IntRect({0, 0}, {10, 10}));
    REQUIRE(chunk->texture != nullptr);
    // No apron: the image is smaller than ChunkSize on every side, so there is
    // no neighboring chunk to sample from.
    CHECK(chunk->texture->getSize() == sf::Vector2u{10, 10});
    REQUIRE(chunk->sprite.has_value());
    CHECK(chunk->sprite->getPosition() == sf::Vector2f{0.f, 0.f});
    CHECK(chunk->sprite->getTextureRect() == sf::IntRect({0, 0}, {10, 10}));

    CHECK(grid.GetChunk(1, 0) == nullptr);
    CHECK(grid.GetChunk(0, 1) == nullptr);
    CHECK(grid.GetChunk(-1, 0) == nullptr);
}

TEST_CASE("[TextureChunkGrid] Build: multiple chunks with apron at the shared border") {
    constexpr int ChunkSize = TextureChunkGrid::ChunkSize;
    sf::Image image({static_cast<unsigned int>(ChunkSize * 2), static_cast<unsigned int>(ChunkSize)}, sf::Color::White);

    TextureChunkGrid grid;
    grid.Build(image);

    const bool canUploadApronedChunk =
        sf::Texture::getMaximumSize() >= static_cast<unsigned int>(ChunkSize + TextureChunkGrid::Apron);

    CHECK(grid.GetSize() == sf::Vector2u{static_cast<unsigned int>(ChunkSize * 2), static_cast<unsigned int>(ChunkSize)});
    CHECK(grid.GetChunkCount() == sf::Vector2u{2, 1});

    // Left chunk: no apron on the left/top/bottom (image edges), 1px apron on
    // the right so its texture includes the seam pixel from the right chunk.
    TextureChunk* left = grid.GetChunk(0, 0);
    REQUIRE(left != nullptr);
    CHECK(left->bounds == sf::IntRect({0, 0}, {ChunkSize, ChunkSize}));
    REQUIRE(left->texture != nullptr);
    if (canUploadApronedChunk)
        CHECK(left->texture->getSize() == sf::Vector2u{static_cast<unsigned int>(ChunkSize + 1), static_cast<unsigned int>(ChunkSize)});
    REQUIRE(left->sprite.has_value());
    CHECK(left->sprite->getPosition() == sf::Vector2f{0.f, 0.f});
    CHECK(left->sprite->getTextureRect() == sf::IntRect({0, 0}, {ChunkSize, ChunkSize}));

    // Right chunk: 1px apron on the left (mirrors the left chunk's border),
    // none on the right/top/bottom (image edges).
    TextureChunk* right = grid.GetChunk(1, 0);
    REQUIRE(right != nullptr);
    CHECK(right->bounds == sf::IntRect({ChunkSize, 0}, {ChunkSize, ChunkSize}));
    REQUIRE(right->texture != nullptr);
    if (canUploadApronedChunk)
        CHECK(right->texture->getSize() == sf::Vector2u{static_cast<unsigned int>(ChunkSize + 1), static_cast<unsigned int>(ChunkSize)});
    REQUIRE(right->sprite.has_value());
    CHECK(right->sprite->getPosition() == sf::Vector2f{static_cast<float>(ChunkSize), 0.f});
    // The visible region starts 1px into the apron-inclusive texture.
    CHECK(right->sprite->getTextureRect() == sf::IntRect({1, 0}, {ChunkSize, ChunkSize}));
}

TEST_CASE("[TextureChunkGrid] Build: rebuilding replaces the previous grid") {
    constexpr int ChunkSize = TextureChunkGrid::ChunkSize;
    sf::Image bigImage({static_cast<unsigned int>(ChunkSize * 2), static_cast<unsigned int>(ChunkSize)}, sf::Color::White);
    sf::Image smallImage({10, 10}, sf::Color::Black);

    TextureChunkGrid grid;
    grid.Build(bigImage);
    REQUIRE(grid.GetChunkCount() == sf::Vector2u{2, 1});

    grid.Build(smallImage);

    CHECK(grid.GetSize() == sf::Vector2u{10, 10});
    CHECK(grid.GetChunkCount() == sf::Vector2u{1, 1});
    CHECK(grid.GetChunk(1, 0) == nullptr);
    CHECK(grid.GetChunk(0, 0) != nullptr);
}

TEST_CASE("[TextureChunkGrid] GetVisibleChunkCoords") {
    constexpr int ChunkSize = TextureChunkGrid::ChunkSize;
    sf::Image image({static_cast<unsigned int>(ChunkSize * 2), static_cast<unsigned int>(ChunkSize)}, sf::Color::White);

    TextureChunkGrid grid;
    grid.Build(image);

    // Fully inside the left chunk.
    auto onlyLeft = grid.GetVisibleChunkCoords(sf::FloatRect({100.f, 100.f}, {50.f, 50.f}));
    CHECK(onlyLeft == std::vector<sf::Vector2u>{{0, 0}});

    // Fully inside the right chunk.
    auto onlyRight = grid.GetVisibleChunkCoords(sf::FloatRect({static_cast<float>(ChunkSize) + 100.f, 100.f}, {50.f, 50.f}));
    CHECK(onlyRight == std::vector<sf::Vector2u>{{1, 0}});

    // Straddling both chunks.
    auto both = grid.GetVisibleChunkCoords(sf::FloatRect({static_cast<float>(ChunkSize) - 50.f, 100.f}, {100.f, 50.f}));
    CHECK(both == std::vector<sf::Vector2u>{{0, 0}, {1, 0}});

    // Entirely outside the grid.
    auto outside = grid.GetVisibleChunkCoords(sf::FloatRect({-5000.f, -5000.f}, {10.f, 10.f}));
    CHECK(outside.empty());
}
