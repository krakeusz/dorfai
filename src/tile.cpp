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
            char c = edgeChars[i];
            if (auto it = charToTerrain.find(c); it == charToTerrain.end())
            {
                throw std::runtime_error("unexpected terrain: " + std::string(c, 1));
            }
            else
            {
                edges[i] = it->second;
            }
        }
        return edges;
    }
}

Tile Tile::fromYaml(const YAML::Node &node)
{
    if (node.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected a yaml map while reading a tile");
    }
    if (auto tileType = node["type"].as<std::string>(); tileType == "land")
    {
        const std::array<Terrain, ROTATIONS> edges = parseEdges(node["edges"].as<std::string>());
        return Tile(edges);
    }
    else
    {
        throw std::runtime_error("unknown tile type: " + tileType);
    }
}

Tile::Tile(const std::array<Terrain, ROTATIONS> &edges)
    : m_edges(edges) {}