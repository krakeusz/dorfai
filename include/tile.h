#pragma once

#include <array>
#include <istream>
#include <optional>

#include "yaml-cpp/yaml.h"

enum class Terrain
{
    Grass,
    Plains,
    Forest,
    Town,
    Rail,
    River,
};

class Tile
{
public:
    static Tile fromYaml(const YAML::Node &node);
    static const int ROTATIONS = 6;

private:
    Tile(const std::array<Terrain, ROTATIONS> &edges);
    int m_rotation = 0;
    const std::array<Terrain, ROTATIONS> m_edges;
};