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

    Terrain getEdgeAt(int position) const { return m_edges.at(position); }
    bool isLand() { return !m_task.has_value(); }
    bool isTask() { return m_task.has_value(); }

private:
    explicit Tile(const std::array<Terrain, ROTATIONS> &edges);
    Tile(const std::array<Terrain, ROTATIONS> &edges, Terrain task);
    int m_rotation = 0;
    const std::array<Terrain, ROTATIONS> m_edges;
    std::optional<Terrain> m_task;
};