#include "game.h"

#include <algorithm>
#include <cassert>
#include <sstream>

#include "random.h"

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

void Game::parseYamlTasks(const YAML::Node &rootNode)
{
    if (!rootNode["tasks"])
    {
        return;
    }
    const auto &tasks = rootNode["tasks"];
    if (tasks.Type() != YAML::NodeType::Sequence)
    {
        throw std::runtime_error("expected tasks to be a sequence");
    }
    for (const auto &task : tasks)
    {
        if (task.Type() != YAML::NodeType::Map)
        {
            throw std::runtime_error("expected each of the tasks to be a map");
        }
        for (const auto &item : task)
        {
            const auto terrainStr = item.first.as<std::string>();
            const Terrain terrain = getTerrainFromString(terrainStr);
            const auto taskSize = item.second.as<int>();
            m_freeTaskSizes.insert(std::make_pair(terrain, taskSize));
        }
    }
}

void Game::parseYamlTiles(const YAML::Node &rootNode)
{
    const auto &tiles = rootNode["tiles"];
    if (tiles.Type() != YAML::NodeType::Sequence)
    {
        throw std::runtime_error("expected tiles to be a sequence");
    }

    for (const auto &tile : tiles)
    {
        auto the_tile = Tile::fromYaml(tile);
        if (the_tile.isLand())
        {
            m_lands.push_back(the_tile);
        }
        else if (the_tile.isTask())
        {
            m_tasks.push_back(the_tile);
        }
    }
}

int Game::fetchTaskSize(Terrain task)
{
    auto [beginIt, endIt] = m_freeTaskSizes.equal_range(task);
    if (beginIt == endIt)
    {
        std::ostringstream oss;
        oss << "No " << task << " tasks left, but one was required.";
        throw std::runtime_error(oss.str());
    }
    std::vector<std::unordered_multimap<Terrain, int>::iterator> tasksToChooseFrom;
    for (auto it = beginIt; it != endIt; it++)
    {
        tasksToChooseFrom.push_back(it);
    }
    int randomIndex = getRandom(std::uniform_int_distribution<int>(0, tasksToChooseFrom.size() - 1));
    auto chosenIt = beginIt;
    std::advance(chosenIt, randomIndex);
    int taskSize = chosenIt->second;
    m_freeTaskSizes.erase(chosenIt);
    return taskSize;
}

Game Game::fromYaml(const YAML::Node &rootNode)
{
    Game game;
    if (rootNode.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected map as a top-level yaml node");
    }
    game.parseYamlTiles(rootNode);
    game.parseYamlTasks(rootNode);
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

void Game::placeTileAt(const Tile &tile, CellId position, int rotation, std::optional<int> taskSize)
{
    assert(canPlaceTileAt(tile, position, rotation));
    m_board.putAt(position, tile, rotation);
    if (tile.isTask())
    {
        if (!taskSize)
        {
            throw std::runtime_error("placeTileAt was called with a Task tile, but without task size");
        }
        m_currentTasks.push_back(Task{position, *taskSize, tile.getTask()});
    }
    // TODO next: Check if any tasks are finished
}