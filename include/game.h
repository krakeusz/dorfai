#pragma once

#include <istream>
#include <optional>
#include <vector>
#include <unordered_map>

#include "yaml-cpp/yaml.h"

#include "board.h"
#include "move.h"
#include "tile.h"

struct Task
{
    CellId position;
    int size;
    Terrain terrain;
};

class Game
{
public:
    static constexpr int MAX_CONCURRENT_TASKS = 3;
    static constexpr int UNUSED_LANDS = 3;
    static Game fromYaml(const YAML::Node &rootNode, bool shuffle = true);

    bool canPlaceTileAt(const Tile &tile, CellId position, int rotation, std::optional<int> taskSize = std::nullopt);
    void placeTileAt(const Tile &tile, CellId position, int rotation, std::optional<int> taskSize = std::nullopt);
    void makeMove(const Move& move);

    auto getLands() const -> const std::vector<Tile> & { return m_lands; }
    auto getTasks() const -> const std::vector<Tile> & { return m_tasks; }
    auto takeNextTileToPlay() -> std::optional<Tile *>; // non-owning, the tile will live as long as the game
    int fetchTaskSize(Terrain task);
    std::vector<Move> nextMoves(); // takes next tile, so do not call twice

private:
    std::vector<Tile> m_lands; // TODO: there are so many references to these tiles. Make m_lands const.
    int m_nextLandIndex = 0;
    std::vector<Tile> m_tasks;
    int m_nextTaskIndex = 0;
    std::vector<Task> m_currentTasks;
    std::vector<Task> m_finishedTasks;
    std::unordered_multimap<Terrain, int> m_freeTaskSizes;
    Board m_board;

    void parseYamlTasks(const YAML::Node &rootNode);
    void parseYamlTiles(const YAML::Node &rootNode, bool shuffle);

    void updateFinishedOrImpossibleTasks(CellId placedTileId);
};