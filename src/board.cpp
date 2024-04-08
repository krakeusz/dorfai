#include "board.h"

#include <cassert>

namespace
{
    std::array<CellId, Tile::ROTATIONS> getPotentialNeighbors(CellId id)
    {
        // offset coordinates, "odd-q" vertical layout, shoves odd columns down
        // https://www.redblobgames.com/grids/hexagons/#coordinates-offset
        std::array<CellId, Tile::ROTATIONS> deltas;
        if (id.x % 2 == 0)
        {
            deltas = std::array<CellId, Tile::ROTATIONS>{{{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {0, 1}, {-1, 0}}};
        }
        else
        {
            deltas = std::array<CellId, Tile::ROTATIONS>{{{-1, 0}, {0, 1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}}};
        }
        for (auto &delta : deltas)
        {
            delta = delta + id;
        }
        return deltas;
    }
}

bool Board::hasTileAt(CellId id) const
{
    return m_tiles.find(id) != m_tiles.end();
}

PlacedTile Board::getTileAt(CellId id) const
{
    assert(hasTileAt(id));
    return m_tiles.find(id)->second;
}

void Board::putAt(CellId id, const Tile &tile, int rotation)
{
    assert(!hasTileAt(id));
    m_tiles.insert(std::make_pair(id, PlacedTile{tile, id, rotation}));
}

auto Board::getNeighbors(CellId id) const -> std::vector<PlacedTile>
{
    std::vector<PlacedTile> neighbors;
    for (auto neighborId : getPotentialNeighbors(id))
    {
        if (hasTileAt(neighborId))
        {
            neighbors.push_back(getTileAt(neighborId));
        }
    }
    return neighbors;
}

auto Board::getEmptyNeighbors(CellId id) const -> std::vector<CellId>
{
    std::vector<CellId> neighbors;
    for (auto neighborId : getPotentialNeighbors(id))
    {
        if (!hasTileAt(neighborId))
        {
            neighbors.push_back(neighborId);
        }
    }
    return neighbors;
}

bool CellId::operator==(const CellId &other) const
{
    return x == other.x && y == other.y;
}

CellId operator+(const CellId &lhs, const CellId &rhs)
{
    return CellId{lhs.x + rhs.x, lhs.y + rhs.y};
}
