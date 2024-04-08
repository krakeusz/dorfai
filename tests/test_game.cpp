#include <catch2/catch_test_macros.hpp>

#include "yaml-cpp/yaml.h"

#include "game.h"

TEST_CASE("CanReadTwoTiles")
{
  const char *yaml = R"(tiles:
        - edges: 'F__F__'
        - edges: 'R_RW_W'
    )";
  YAML::Node tileNode = YAML::Load(yaml);
  const auto game = Game::fromYaml(tileNode);
  REQUIRE(game.getLands().size() == 2);
  REQUIRE(game.getLands().at(0).getEdgeAt(0) == Terrain::Forest);
  REQUIRE(game.getLands().at(1).getEdgeAt(0) == Terrain::Rail);
  REQUIRE(game.getTasks().empty());
}