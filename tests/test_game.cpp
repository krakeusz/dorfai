#include <catch2/catch_test_macros.hpp>

#include "yaml-cpp/yaml.h"

#include "game.h"

TEST_CASE("CanReadTwoTiles")
{
    const char *yaml = R"(tiles:
        - edges: 'F__F__'
          type: land
        - edges: 'R_RW_W'
          type: land
    )";
    YAML::Node tileNode = YAML::Load(yaml);
    const auto game = Game::fromYaml(tileNode);
    REQUIRE(game.getTiles().size() == 2);
    REQUIRE(game.getTiles().at(0).getEdgeAt(0) == Terrain::Forest);
    REQUIRE(game.getTiles().at(1).getEdgeAt(0) == Terrain::Rail);
}