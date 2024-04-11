#pragma once

#include <istream>
#include <optional>
#include <vector>
#include <unordered_map>

#include "yaml-cpp/yaml.h"

#include "board.h"
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
    static Game fromYaml(const YAML::Node &rootNode);

    bool canPlaceTileAt(const Tile &tile, CellId position, int rotation) const;
    void placeTileAt(const Tile &tile, CellId position, int rotation, std::optional<int> taskSize = std::nullopt);

    auto getLands() const -> const std::vector<Tile> & { return m_lands; }
    auto getTasks() const -> const std::vector<Tile> & { return m_tasks; }

private:
    std::vector<Tile> m_lands;
    std::vector<Tile> m_tasks;
    std::vector<Task> m_currentTasks;
    std::vector<Task> m_finishedTasks;
    std::unordered_multimap<Terrain, int> m_freeTaskSizes;
    Board m_board;

    void parseYamlTasks(const YAML::Node &rootNode);
    void parseYamlTiles(const YAML::Node &rootNode);
    int fetchTaskSize(Terrain task);
};