#include "tile.h"

#include <string>
#include <unordered_map>

namespace
{
    std::optional<std::string> nextNonEmptyLine(std::istream &in)
    {
        std::string line;
        do
        {
            std::getline(in, line);
            if (!in)
            {
                throw std::runtime_error("Could not read line");
            }
            if (in.eof())
            {
                return std::nullopt;
            }
        } while (line.empty());
        return line;
    }

    const std::unordered_map<char, Terrain> charToTerrain = {
        {'_', Terrain::Grass},
        {'P', Terrain::Plains},
        {'F', Terrain::Forest},
        {'T', Terrain::Town},
        {'R', Terrain::Rail},
        {'W', Terrain::River},
    };
}

std::optional<Tile> Tile::fromIStream(std::istream &in)
{
    std::array<Terrain, ROTATIONS> edges;

    auto optionalLine = nextNonEmptyLine(in);
    if (!optionalLine)
    {
        return std::nullopt;
    }
    std::string tileType = *optionalLine;

    auto nextLine = [&tileType, &in]()
    {
        auto optionalLine = nextNonEmptyLine(in);
        if (!optionalLine)
        {
            throw std::runtime_error("eof encountered while reading " + tileType);
        }
        return *optionalLine;
    };
    std::string edgeChars = nextLine();
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
    int n_features = std::stoi(nextLine());
    for (int i = 0; i < n_features; i++)
    {
        std::string feature = nextLine();
    }

    return Tile(edges);
}

Tile::Tile(const std::array<Terrain, ROTATIONS> &edges)
    : m_edges(edges) {}