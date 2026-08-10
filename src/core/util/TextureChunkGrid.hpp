#pragma once

// A single tile of a TextureChunkGrid.
//
// - `bounds` is the world/image-space rect of the chunk's *visible* region (i.e.
//   without the 1px apron).
// - `texture` is apron-inclusive (so neighbor sampling in the shader never crosses
//   into another chunk's texture at a seam).
// - `sprite`'s texture rect already excludes the apron and is positioned so it
//    draws exactly over `bounds`.
struct TextureChunk {
    sf::IntRect bounds;
    UniquePtr<sf::Texture> texture;
    std::optional<sf::Sprite> sprite;
};

// Slices a full-resolution sf::Image into a grid of fixed-size texture
// chunks, so only the chunks visible to the camera need to be drawn each frame.
class TextureChunkGrid {
public:
    static constexpr int ChunkSize = 1024;
    static constexpr int Apron = 1;

    TextureChunkGrid() = default;
    TextureChunkGrid(const TextureChunkGrid&) = delete;
    TextureChunkGrid& operator=(const TextureChunkGrid&) = delete;

    /**
     * @brief Builds every chunk's texture/sprite from the given source image.
     * @note  Should only be called when the source image itself changes, not every frame.
     * @param sourceImage The full-resolution image to slice into chunks.
     */
    void Build(const sf::Image& sourceImage);

    /**
     * @brief Retrieves the size of the source image the grid was built from.
     */
    sf::Vector2u GetSize() const;

    /**
     * @brief Retrieves the number of chunk (columns,rows) in the grid.
     */
    sf::Vector2u GetChunkCount() const;

    /**
     * @brief Retrieves the chunk at the given grid coordinate.
     * @param col The chunk column.
     * @param row The chunk row.
     * @return A pointer to the chunk, or nullptr if out of range.
     */
    TextureChunk* GetChunk(int col, int row);

    /**
     * @brief Retrieves the grid coordinates of every chunk whose bounds intersect
     *        the given world-space rect.
     * @param visibleRect The world-space rect to test chunks against.
     * @return The (col, row) coordinates of the intersecting chunks.
     */
    std::vector<sf::Vector2u> GetVisibleChunkCoords(const sf::FloatRect& visibleRect) const;

private:
    sf::Vector2u m_Size;
    int m_Columns = 0;
    int m_Rows = 0;
    std::vector<UniquePtr<TextureChunk>> m_Chunks;
};
