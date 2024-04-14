#pragma once

#include <array>
#include <iosfwd>
#include <istream>
#include <optional>
#include <string_view>

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
std::ostream &operator<<(std::ostream &out, const Terrain &terrain);

bool areTerrainsCompatible(Terrain t1, Terrain t2);
auto getTerrainFromChar(char c) -> Terrain;
auto getTerrainFromString(const std::string &s) -> Terrain;

class Tile
{
public:
    static const int ROTATIONS = 6;

    explicit Tile(const std::array<Terrain, ROTATIONS> &edges);
    explicit Tile(std::string_view edgeChars);
    Tile(const std::array<Terrain, ROTATIONS> &edges, Terrain task);
    Tile(const Tile &other);

    static Tile fromYaml(const YAML::Node &node);

    Terrain getEdgeAt(int position) const { return m_edges.at(position); }
    bool isLand() const { return !m_task.has_value(); }
    bool isTask() const { return m_task.has_value(); }
    Terrain getTask() const { return *m_task; }
    Terrain setTask(Terrain task) { m_task = task; }

private:
    std::array<Terrain, ROTATIONS> m_edges;
    std::optional<Terrain> m_task;
};