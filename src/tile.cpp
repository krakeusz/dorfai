#include "tile.h"

#include <string>
#include <unordered_map>

namespace
{
    const std::unordered_map<char, Terrain> charToTerrain = {
        {'_', Terrain::Grass},
        {'P', Terrain::Plains},
        {'F', Terrain::Forest},
        {'T', Terrain::Town},
        {'R', Terrain::Rail},
        {'W', Terrain::River},
    };

    auto parseEdges(const std::string &edgeChars) -> std::array<Terrain, Tile::ROTATIONS>
    {
        std::array<Terrain, Tile::ROTATIONS> edges;
        if (edgeChars.size() != Tile::ROTATIONS)
        {
            throw std::runtime_error("expected " + std::to_string(Tile::ROTATIONS) + " chars, but got: " + edgeChars);
        }
        for (int i = 0; i < Tile::ROTATIONS; i++)
        {
            edges[i] = getTerrainFromChar(edgeChars[i]);
        }
        return edges;
    }
}

auto getTerrainFromChar(char c) -> Terrain
{
    if (auto it = charToTerrain.find(c); it == charToTerrain.end())
    {
        throw std::runtime_error("unexpected terrain: " + std::string(c, 1));
    }
    else
    {
        return it->second;
    }
}

auto getTerrainFromString(const std::string &s) -> Terrain
{
    if (s.size() != 1)
    {
        throw std::runtime_error("unknown terrain: \"" + s + "\"");
    }
    return getTerrainFromChar(s[0]);
}

Tile Tile::fromYaml(const YAML::Node &node)
{
    if (node.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected a yaml map while reading a tile");
    }
    const std::array<Terrain, ROTATIONS> edges = parseEdges(node["edges"].as<std::string>());
    if (node["task"])
    {
        auto task = node["task"].as<std::string>();
        return Tile(edges, getTerrainFromString(task));
    }
    else
    {
        return Tile(edges);
    }
}

Tile::Tile(const std::array<Terrain, ROTATIONS> &edges)
    : m_edges(edges) {}

Tile::Tile(const std::array<Terrain, ROTATIONS> &edges, Terrain task)
    : m_edges(edges), m_task(task) {}

std::ostream &operator<<(std::ostream &out, const Terrain &terrain)
{
    switch (terrain)
    {
    case Terrain::Grass:
        return out << "Grass";
    case Terrain::Plains:
        return out << "Plains";
    case Terrain::Forest:
        return out << "Forest";
    case Terrain::Town:
        return out << "Town";
    case Terrain::Rail:
        return out << "Rail";
    case Terrain::River:
        return out << "River";
    default:
        throw std::runtime_error("Unknown terrain: " + std::to_string(static_cast<int>(terrain)));
    }
}

bool areTerrainsCompatible(Terrain t1, Terrain t2)
{
    if (t1 == Terrain::Rail || t2 == Terrain::Rail)
    {
        return t1 == t2;
    }
    if (t1 == Terrain::River || t2 == Terrain::River)
    {
        return t1 == t2;
    }
    return true;
}
