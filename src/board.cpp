#include "board.h"

#include <algorithm>
#include <cassert>
#include <ostream>

auto Board::getPotentialNeighbors(CellId id) -> std::array<CellId, Tile::ROTATIONS>
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

bool Board::areNeighbors(CellId lhs, CellId rhs)
{
    auto neighbors = getPotentialNeighbors(lhs);
    return std::any_of(neighbors.begin(), neighbors.end(), [rhs](CellId id)
                       { return id == rhs; });
}

auto Board::getEdge(CellId adjacent1, CellId adjacent2) -> std::pair<int, int>
{
    auto neighbors = getPotentialNeighbors(adjacent1);
    auto the_neighbor_it = std::find(neighbors.begin(), neighbors.end(), adjacent2);
    if (the_neighbor_it == neighbors.end())
    {
        std::ostringstream oss;
        oss << "getEdge(" << adjacent1 << ", " << adjacent2 << ") was called for non-adjacent tiles!";
        throw std::runtime_error(oss.str());
    }
    int direction1 = std::distance(neighbors.begin(), the_neighbor_it);
    int direction2 = (direction1 + Tile::ROTATIONS / 2) % Tile::ROTATIONS;
    return std::make_pair(direction1, direction2);
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

std::ostream &operator<<(std::ostream &out, const CellId &cellId)
{
    return out << "(" << cellId.x << ", " << cellId.y << ")";
}
