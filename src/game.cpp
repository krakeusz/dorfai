#include "game.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_set>
#include <queue>

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
        // If a tile is placed with placementRotation, and the edge of the tile is directed to absoluteDirection in board coordinates,
        // then return the index/direction of this edge in the original, non-rotated (prototype) tile.
        return (absoluteDirection + Tile::ROTATIONS - placementRotation) % Tile::ROTATIONS;
    }

    struct SearchResult
    {
        int terrainSize;
        bool isClosed;
    };

    SearchResult searchConnectedTiles(const Board &b, Terrain terrain, CellId startPosition)
    {
        std::queue<CellId> tilesToVisit;
        tilesToVisit.push(startPosition);
        std::unordered_set<CellId> visitedTiles;
        visitedTiles.insert(startPosition);
        int terrainSize = 0;
        bool isClosed = true;
        while (!tilesToVisit.empty())
        {
            CellId position = tilesToVisit.front();
            PlacedTile tile = b.getTileAt(position);
            tilesToVisit.pop();
            terrainSize++;
            for (int r = 0; r < Tile::ROTATIONS; r++)
            {
                int edge = absoluteDirectionToTileDirection(r, tile.rotation);
                if (tile.tile.getEdgeAt(edge) != terrain)
                {
                    continue;
                }
                std::optional<PlacedTile> neighbor = b.getNeighbor(position, r);
                if (!neighbor)
                {
                    isClosed = false;
                }
                else if (visitedTiles.count(neighbor->id) == 0)
                {
                    tilesToVisit.push(neighbor->id);
                    visitedTiles.insert(neighbor->id);
                }
            }
        }
        return SearchResult{terrainSize, isClosed};
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

void Game::parseYamlTiles(const YAML::Node &rootNode, bool shuffle)
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
    if (shuffle)
    {
        std::shuffle(m_lands.begin(), m_lands.end(), getRandomEngine());
        std::shuffle(m_tasks.begin(), m_tasks.end(), getRandomEngine());
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

std::vector<Move> Game::nextMoves()
{
    auto optionalTile = takeNextTileToPlay();
    if (!optionalTile)
    {
        return std::vector<Move>{}; // no tiles left to play, game over
    }
    Tile *nextTile = *optionalTile;
    std::vector<CellId>
        placesToPutTile = m_board.getPlacesForNextTile();
    std::vector<Move> possibleMoves;
    std::optional<int> taskSize = nextTile->isTask() ? std::make_optional(fetchTaskSize(nextTile->getTask())) : std::nullopt;
    for (CellId position : placesToPutTile)
    {
        for (int rotation = 0; rotation < Tile::ROTATIONS; rotation++)
        {
            if (canPlaceTileAt(*nextTile, position, rotation, taskSize))
            {
                possibleMoves.push_back(Move{nextTile, position, rotation, taskSize});
            }
        }
    }
    return possibleMoves;
}

void Game::updateFinishedOrImpossibleTasks(CellId placedTileId)
{
    std::vector<Task> stillUnfinishedTasks;
    // precondition: last move was legal
    for (const Task &task : m_currentTasks)
    {
        SearchResult searchResult = searchConnectedTiles(m_board, task.terrain, task.position);
        if (searchResult.terrainSize == task.size)
        {
            m_finishedTasks.push_back(task);
        }
        else if (searchResult.isClosed || searchResult.terrainSize > task.size)
        {
            // Since the last move was legal, it couldn't have been a Task.
            // A task here would close another unfinished task.
            assert(!m_board.getTileAt(placedTileId).tile.isTask());
            // Task will be removed, but not finished.
        }
        else
        {
            assert(searchResult.terrainSize < task.size && !searchResult.isClosed);
            stillUnfinishedTasks.push_back(task);
        }
    }
    m_currentTasks = std::move(stillUnfinishedTasks);
}

Game Game::fromYaml(const YAML::Node &rootNode, bool shuffle)
{
    Game game;
    if (rootNode.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected map as a top-level yaml node");
    }
    game.parseYamlTiles(rootNode, shuffle);
    game.parseYamlTasks(rootNode);
    return game;
}

bool Game::canPlaceTileAt(const Tile &tile, CellId position, int rotation, std::optional<int> taskSize)
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
    if (!taskSize)
    {
        // A land can close even an unfinished task.
        return true;
    }
    // Cannot put a task tile which would prevent a task from finishing.
    assert(tile.isTask());
    m_board.putAt(position, tile, rotation);
    m_currentTasks.push_back(Task{position, *taskSize, tile.getTask()});
    bool canPlace = true;
    for (const Task &task : m_currentTasks)
    {
        SearchResult searchResult = searchConnectedTiles(m_board, task.terrain, task.position);
        if (searchResult.terrainSize == task.size)
        {
            // task would be completed, ok
        }
        else if (searchResult.isClosed || searchResult.terrainSize > task.size)
        {
            canPlace = false; // task would be impossible to finish
        }
        else
        {
            assert(searchResult.terrainSize < task.size && !searchResult.isClosed);
            // not completed, but not blocked
        }
    }
    m_currentTasks.pop_back();
    m_board.removeAt(position);
    return canPlace;
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
    updateFinishedOrImpossibleTasks(position);
}

void Game::makeMove(const Move &move)
{
    placeTileAt(*move.tile, move.position, move.rotation, move.taskSize);
}

std::optional<Tile *> Game::takeNextTileToPlay()
{
    if (m_currentTasks.size() < MAX_CONCURRENT_TASKS && m_nextTaskIndex < static_cast<int>(m_tasks.size()))
    {
        return &m_tasks.at(m_nextTaskIndex++);
    }
    if (m_nextLandIndex < static_cast<int>(m_lands.size()) - UNUSED_LANDS)
    {
        return &m_lands.at(m_nextLandIndex++);
    }
    return std::nullopt; // No tiles left
}
