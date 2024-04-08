#include <catch2/catch_test_macros.hpp>

#include "yaml-cpp/yaml.h"

#include "tile.h"

TEST_CASE("CanReadSimpleTile")
{
    YAML::Node tileNode = YAML::Load("edges: 'R_RW_W'\n");
    const auto tile = Tile::fromYaml(tileNode);
    REQUIRE(tile.getEdgeAt(0) == Terrain::Rail);
    REQUIRE(tile.getEdgeAt(1) == Terrain::Grass);
}