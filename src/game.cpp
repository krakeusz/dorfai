#include "game.h"

#include <algorithm>
#include <cassert>

namespace
{
    bool isAdjacentToBoard(const Board &b, CellId position)
    {
        auto neighbors = Board::getPotentialNeighbors(position);
        return !b.hasTileAt(position) &&
               std::any_of(neighbors.begin(), neighbors.end(), [&b](CellId pos)
                           { return b.hasTileAt(pos); });
    }

    int absoluteDirectionToTileDirection(int absoluteDirection, int placementRotation)
    {
        return (absoluteDirection + Tile::ROTATIONS - placementRotation) % Tile::ROTATIONS;
    }
}

Game Game::fromYaml(const YAML::Node &rootNode)
{
    if (rootNode.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected map as a top-level yaml node");
    }
    const auto &tiles = rootNode["tiles"];
    if (tiles.Type() != YAML::NodeType::Sequence)
    {
        throw std::runtime_error("expected tiles to be a sequence");
    }
    Game game;
    for (const auto &tile : tiles)
    {
        auto the_tile = Tile::fromYaml(tile);
        if (the_tile.isLand())
        {
            game.m_lands.push_back(the_tile);
        }
        else if (the_tile.isTask())
        {
            game.m_tasks.push_back(the_tile);
        }
    }
    return game;
}

bool Game::canPlaceTileAt(const Tile &tile, CellId position, int rotation) const
{
    assert(isAdjacentToBoard(m_board, position) || m_board.isEmpty());
    for (PlacedTile tile2 : m_board.getNeighbors(position))
    {
        auto [direction1, direction2] = m_board.getEdge(position, tile2.id);
        int rotation1 = absoluteDirectionToTileDirection(direction1, rotation);
        int rotation2 = absoluteDirectionToTileDirection(direction2, tile2.rotation);
        auto terrain1 = tile.getEdgeAt(rotation1);
        auto terrain2 = tile2.tile.getEdgeAt(rotation2);
        if (!areTerrainsCompatible(terrain1, terrain2))
        {
            return false;
        }
    }
    // TODO: check that all tasks are still possible to be completed
    return true;
}

void Game::placeTileAt(const Tile &tile, CellId position, int rotation)
{
    assert(canPlaceTileAt(tile, position, rotation));
    m_board.putAt(position, tile, rotation);
}
