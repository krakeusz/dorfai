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

bool areTerrainsCompatible(Terrain t1, Terrain t2);

class Tile
{
public:
    static const int ROTATIONS = 6;

    explicit Tile(const std::array<Terrain, ROTATIONS> &edges);
    Tile(const std::array<Terrain, ROTATIONS> &edges, Terrain task);

    static Tile fromYaml(const YAML::Node &node);

    Terrain getEdgeAt(int position) const { return m_edges.at(position); }
    bool isLand() const { return !m_task.has_value(); }
    bool isTask() const { return m_task.has_value(); }

private:
    const std::array<Terrain, ROTATIONS> m_edges;
    std::optional<Terrain> m_task;
};