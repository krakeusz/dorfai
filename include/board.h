#pragma once

#include <iosfwd>
#include <unordered_map>
#include <utility>

#include "tile.h"

struct CellId
{
    int x;
    int y;

    bool operator==(const CellId &other) const;
};

CellId operator+(const CellId &lhs, const CellId &rhs);
std::ostream &operator<<(std::ostream &out, const CellId &cellId);

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
    void removeAt(CellId id);
    auto getNeighbors(CellId id) const -> std::vector<PlacedTile>;
    auto getNeighbor(CellId id, int absoluteDirection) const -> std::optional<PlacedTile>;
    auto getEmptyNeighbors(CellId id) const -> std::vector<CellId>;
    bool isEmpty() const { return m_tiles.empty(); }

    static auto getPotentialNeighbors(CellId id) -> std::array<CellId, Tile::ROTATIONS>;
    static bool areNeighbors(CellId lhs, CellId rhs);

    // Finds (rotation1, rotation2) of adjacent tiles, aka which rotation/edge of each tile is adjacent to the other one.
    static auto getEdge(CellId adjacent1, CellId adjacent2) -> std::pair<int, int>;

private:
    std::unordered_map<CellId, PlacedTile> m_tiles;
};