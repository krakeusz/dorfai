#pragma once

#include <array>
#include <istream>
#include <optional>

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
    static std::optional<Tile> fromIStream(std::istream &in);
    static const int ROTATIONS = 6;

private:
    Tile(const std::array<Terrain, ROTATIONS> &edges);
    int m_rotation = 0;
    const std::array<Terrain, ROTATIONS> m_edges;
};