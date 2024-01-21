#pragma once

// When this is defined, include code that can optimise write to the LCD.
//#define OPTIMIZE_TILE_UPDATES

// When this is defined, clusters of ajoining tiles are grouped into a single blit.
#define GROUP_CHANGED_TILES

#include <vector>
#include "updatestrategy.hpp"
#ifdef OPTIMIZE_TILE_UPDATES
#include "rect.hpp"
#endif

class UpdateOptimizer : public UpdateStrategy
{
public:
    UpdateOptimizer(size_t width, size_t height);

    void TransferToDisplay(uint16_t* frameBuffer, Display& display) override;

private:
#ifdef OPTIMIZE_TILE_UPDATES
    void CreateUpdatedTileTable(uint16_t* frameBuffer);
    void MarkTileChanged(size_t pixel);
    bool IsTileChanged(size_t tile);
    void CollectChangedTiles();
    void MergeChangedTile(size_t tile);
    void BlitChangedTilesToDisplay(uint16_t* frameBuffer, Display& display);
    size_t GetRectDataSize(const Rect& r);
    void GatherRectData(const Rect& r, uint16_t* buffer, uint16_t* frameBuffer);
    void DumpTileState();
#endif

    size_t m_width;
    size_t m_height;
#ifdef OPTIMIZE_TILE_UPDATES
    uint16_t* m_prevFrameBuffer{ nullptr };

    size_t m_tileSize{ 4 };
    size_t m_tileWidth;
    size_t m_tileHeight;
    uint8_t* m_tileChanged{ nullptr };

    std::vector<Rect> m_rectList;
#endif
};
