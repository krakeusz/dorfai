#pragma once

#include <unordered_map>

#include "tile.h"

struct CellId
{
    int x;
    int y;

    bool operator==(const CellId &other) const;
};

CellId operator+(const CellId &lhs, const CellId &rhs);

template <>
struct std::hash<CellId>
{
    std::size_t operator()(const CellId &id) const noexcept
    {
        std::size_t h1 = std::hash<int>{}(id.x);
        std::size_t h2 = std::hash<int>{}(id.y);
        return h1 ^ (h2 << 8);
    }
};

struct PlacedTile
{
    const Tile &tile;
    CellId id;
    int rotation = 0;
};

class Board
{
public:
    bool hasTileAt(CellId id) const;
    PlacedTile getTileAt(CellId id) const;
    void putAt(CellId id, const Tile &tile, int rotation);
    auto getNeighbors(CellId id) const -> std::vector<PlacedTile>;
    auto getEmptyNeighbors(CellId id) const -> std::vector<CellId>;

private:
    std::unordered_map<CellId, PlacedTile> m_tiles;
};