#pragma once

#include <istream>
#include <vector>

#include "yaml-cpp/yaml.h"

#include "tile.h"

class Game
{
public:
    static Game fromYaml(const YAML::Node &rootNode);

    auto getTiles() const -> const std::vector<Tile> & { return m_tiles; }

private:
    std::vector<Tile> m_tiles;
};