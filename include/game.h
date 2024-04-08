#pragma once

#include <istream>
#include <vector>

#include "yaml-cpp/yaml.h"

#include "board.h"
#include "tile.h"

class Game
{
public:
    static Game fromYaml(const YAML::Node &rootNode);

    auto getLands() const -> const std::vector<Tile> & { return m_lands; }
    auto getTasks() const -> const std::vector<Tile> & { return m_tasks; }

private:
    std::vector<Tile> m_lands;
    std::vector<Tile> m_tasks;
    Board m_board;
};