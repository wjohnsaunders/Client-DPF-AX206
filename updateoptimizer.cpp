#include <iostream>
#include <cstring>
#include "updateoptimizer.hpp"
#include "display.hpp"


namespace
{

#ifdef OPTIMIZE_TILE_UPDATES
size_t DetermineTileSize(size_t width, size_t height)
{
    if ((width % 16) == 0 && (height % 16) == 0)
    {
        return 16;
    }
    if ((width % 8) == 0 && (height % 8) == 0)
    {
        return 8;
    }
    return 4;
}
#endif

}

UpdateOptimizer::UpdateOptimizer(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
#ifdef OPTIMIZE_TILE_UPDATES
    const size_t bufferSize{width * height};
    m_prevFrameBuffer = new uint16_t[bufferSize];
    memset(m_prevFrameBuffer, 0, bufferSize * sizeof(uint16_t));

    m_tileSize = DetermineTileSize(width, height);
    m_tileWidth = width / m_tileSize;
    m_tileHeight = height / m_tileSize;
    m_tileChanged = new uint8_t[m_tileWidth * m_tileHeight];

    std::cout << "Tile dimensions (" << m_tileSize << ", " << m_tileWidth << ", " << m_tileHeight << ")" << std::endl;
#endif
}

// The algorithm is to break up the screen into square shaped tiles of a fixed size. We use a 16x16 tile
// size if it fits the resolution, otherwise we step down to 8x8 then 4 as the fall-back. We then compare
// each tile and mark if it has been updated. For an 800x480 LCD, we have 50x30 tiles. Updated tiles are
// then added to a list, with ajoining tiles being merged into a single larger tile.
void UpdateOptimizer::TransferToDisplay(uint16_t* frameBuffer, Display& display)
{
#ifdef OPTIMIZE_TILE_UPDATES
    if (display.MustBlitFullScreen())
    {
        const size_t length = m_width * m_height * sizeof(uint16_t);
        display.Blit(0, m_width, 0, m_height, frameBuffer, length);
    }
    else
    {
        CreateUpdatedTileTable(frameBuffer);
        CollectChangedTiles();
        BlitChangedTilesToDisplay(frameBuffer, display);
        memcpy(m_prevFrameBuffer, frameBuffer, m_width * m_height * sizeof(uint16_t));
    }
#else
    const size_t length = m_width * m_height * sizeof(uint16_t);
    display.Blit(0, m_width, 0, m_height, frameBuffer, length);
#endif
}

#ifdef OPTIMIZE_TILE_UPDATES
void UpdateOptimizer::CreateUpdatedTileTable(uint16_t* frameBuffer)
{
    memset(m_tileChanged, 0, m_tileWidth * m_tileHeight * sizeof(m_tileChanged[0]));
    const size_t numPixels = m_width * m_height;
    for (size_t p = 0; p < numPixels; ++p)
    {
        if (frameBuffer[p] != m_prevFrameBuffer[p])
        {
            MarkTileChanged(p);
        }
    }
    DumpTileState();
}

void UpdateOptimizer::MarkTileChanged(size_t pixel)
{
    const size_t tileY = (pixel / m_width) / m_tileSize;
    const size_t tileX = (pixel % m_width) / m_tileSize;
    m_tileChanged[tileY * m_tileWidth + tileX] = 1;
}

bool UpdateOptimizer::IsTileChanged(size_t tile)
{
    return m_tileChanged[tile] > 0;
}

void UpdateOptimizer::CollectChangedTiles()
{
    m_rectList.clear();
    const size_t numTiles = m_tileWidth * m_tileHeight;
    for (size_t t = 0; t < numTiles; ++t)
    {
        if (IsTileChanged(t))
        {
            MergeChangedTile(t);
        }
    }
}

void UpdateOptimizer::MergeChangedTile(size_t tile)
{
    const size_t y = tile / m_tileWidth;
    const size_t x = tile % m_tileWidth;
    const size_t size = m_tileSize;
    Rect r1{ x * size, x * size + size, y * size, y * size + size };
#ifdef GROUP_CHANGED_TILES
    bool updated;
    do
    {
        updated = false;
        for (auto iter = m_rectList.begin(); iter != m_rectList.end(); ++iter)
        {
            if (r1.Intersects(*iter))
            {
                r1.Union(*iter);
                updated = true;
                m_rectList.erase(iter);
                break;
            }
        }
    } while (updated);
#endif
    m_rectList.push_back(r1);
}

void UpdateOptimizer::BlitChangedTilesToDisplay(uint16_t* frameBuffer, Display& display)
{
    for (Rect r : m_rectList)
    {
        uint16_t* data = m_prevFrameBuffer;
        const size_t dataSize = GetRectDataSize(r);
        GatherRectData(r, data, frameBuffer);
        display.Blit(r.x0, r.x1, r.y0, r.y1, data, dataSize);

        std::cout << "Rect(" << r.x0 << "," << r.x1 << "," << r.y0 << "," << r.y1 << ") size " << dataSize << std::endl;
    }
}

size_t UpdateOptimizer::GetRectDataSize(const Rect& r)
{
    return r.GetNumPixels() * sizeof(uint16_t);
}

void UpdateOptimizer::GatherRectData(const Rect& r, uint16_t* buffer, uint16_t* frameBuffer)
{
    for (size_t row = r.y0; row < r.y1; ++row)
    {
        for (size_t col = r.x0; col < r.x1; ++col)
        {
            *buffer++ = frameBuffer[row * m_width + col];
        }
    }
}

void UpdateOptimizer::DumpTileState()
{
    const size_t numTiles = m_tileWidth * m_tileHeight;
    for (size_t t = 0; t < numTiles; ++t)
    {
        if (IsTileChanged(t))
        {
            std::cout << "*";
        }
        else
        {
            std::cout << ".";
        }
        if (((t + 1) % m_tileWidth) == 0)
        {
            std::cout << std::endl;
        }
    }
}
#endif
