#include <catch2/catch_test_macros.hpp>

#include "board.h"

TEST_CASE("CanAddTwoFirstTiles")
{
    std::array<Terrain, Tile::ROTATIONS> edges{Terrain::Grass, Terrain::Grass, Terrain::Grass, Terrain::Grass, Terrain::Grass, Terrain::Grass};
    Tile tile{edges};
    Board b;
    const CellId start{0, 0};
    REQUIRE(!b.hasTileAt(start));
    b.putAt(start, tile, 0);
    REQUIRE(b.hasTileAt(start));
    auto neighbors = b.getEmptyNeighbors(start);
    REQUIRE(neighbors.size() == 6);
    b.putAt(neighbors[0], tile, 0);
    REQUIRE(b.hasTileAt(neighbors[0]));
    REQUIRE(b.getNeighbors(start).size() == 1);
    REQUIRE(b.getNeighbors(neighbors[0]).size() == 1);
}