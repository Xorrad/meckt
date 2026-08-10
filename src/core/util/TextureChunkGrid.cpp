#include "TextureChunkGrid.hpp"

#include <cmath>

void TextureChunkGrid::Build(const sf::Image& sourceImage) {
    m_Chunks.clear();
    m_Size = sourceImage.getSize();
    m_Columns = 0;
    m_Rows = 0;

    if (m_Size.x == 0 || m_Size.y == 0)
        return;

    int width = static_cast<int>(m_Size.x);
    int height = static_cast<int>(m_Size.y);

    m_Columns = (width + ChunkSize - 1) / ChunkSize;
    m_Rows = (height + ChunkSize - 1) / ChunkSize;

    m_Chunks.reserve(static_cast<size_t>(m_Columns) * m_Rows);

    for (int row = 0; row < m_Rows; row++) {
        for (int col = 0; col < m_Columns; col++) {
            // Visible (non-apron) region of this chunk, clamped to the image bounds -
            // the last row/column of chunks may be smaller than ChunkSize.
            int visibleX = col * ChunkSize;
            int visibleY = row * ChunkSize;
            int visibleWidth = std::min(ChunkSize, width - visibleX);
            int visibleHeight = std::min(ChunkSize, height - visibleY);

            // Source (apron-inclusive) region, clamped to the image bounds. The apron
            // on a given side is smaller than `Apron` (or absent) at the outer edges
            // of the map, where there is no neighboring chunk to sample from.
            int sourceX = std::max(0, visibleX - Apron);
            int sourceY = std::max(0, visibleY - Apron);
            int sourceRight = std::min(width, visibleX + visibleWidth + Apron);
            int sourceBottom = std::min(height, visibleY + visibleHeight + Apron);

            UniquePtr<TextureChunk> chunk = MakeUnique<TextureChunk>();
            chunk->bounds = sf::IntRect(
                { visibleX, visibleY },
                { visibleWidth, visibleHeight }
            );

            chunk->texture = MakeUnique<sf::Texture>();
            chunk->texture->loadFromImage(sourceImage, false, sf::IntRect(
                { sourceX, sourceY },
                { sourceRight - sourceX, sourceBottom - sourceY }
            ));
            // Chunk textures back either the province index (a lookup table, not a
            // color image) or plain heightmap/rivers imagery - neither should be
            // blurred/wrapped, and the index texture especially requires exact texel
            // values for DecodeIndex() in the shader.
            chunk->texture->setSmooth(false);
            chunk->texture->setRepeated(false);

            chunk->sprite = sf::Sprite(*chunk->texture);
            // Exclude the apron from what's actually drawn: the inset on each side
            // equals how much the source rect was expanded on that side.
            chunk->sprite->setTextureRect(sf::IntRect(
                { visibleX - sourceX, visibleY - sourceY },
                { visibleWidth, visibleHeight }
            ));
            chunk->sprite->setPosition(sf::Vector2f(chunk->bounds.position));

            m_Chunks.push_back(std::move(chunk));
        }
    }
}

sf::Vector2u TextureChunkGrid::GetSize() const {
    return m_Size;
}

sf::Vector2u TextureChunkGrid::GetChunkCount() const {
    return sf::Vector2u(static_cast<uint32_t>(m_Columns), static_cast<uint32_t>(m_Rows));
}

TextureChunk* TextureChunkGrid::GetChunk(int col, int row) {
    if (col < 0 || row < 0 || col >= m_Columns || row >= m_Rows)
        return nullptr;
    return m_Chunks[static_cast<size_t>(row) * m_Columns + col].get();
}

std::vector<sf::Vector2u> TextureChunkGrid::GetVisibleChunkCoords(const sf::FloatRect& visibleRect) const {
    std::vector<sf::Vector2u> coords;
    if (m_Columns == 0 || m_Rows == 0)
        return coords;

    // Clamp the visible rect's chunk-coordinate range to the grid bounds.
    int minCol = std::max(0, static_cast<int>(std::floor(visibleRect.position.x / ChunkSize)));
    int minRow = std::max(0, static_cast<int>(std::floor(visibleRect.position.y / ChunkSize)));
    int maxCol = std::min(m_Columns - 1, static_cast<int>(std::floor((visibleRect.position.x + visibleRect.size.x) / ChunkSize)));
    int maxRow = std::min(m_Rows - 1, static_cast<int>(std::floor((visibleRect.position.y + visibleRect.size.y) / ChunkSize)));

    for (int row = minRow; row <= maxRow; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            coords.push_back(sf::Vector2u(static_cast<uint32_t>(col), static_cast<uint32_t>(row)));
        }
    }
    return coords;
}
