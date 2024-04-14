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
  const auto game = Game::fromYaml(tileNode, false);
  REQUIRE(game.getLands().size() == 2);
  REQUIRE(game.getLands().at(0).getEdgeAt(0) == Terrain::Forest);
  REQUIRE(game.getLands().at(1).getEdgeAt(0) == Terrain::Rail);
  REQUIRE(game.getTasks().empty());
}

TEST_CASE("CanExtendRiverTile")
{
  const std::array<Terrain, Tile::ROTATIONS> terrains1 = {Terrain::River, Terrain::Rail, Terrain::Rail, Terrain::Rail, Terrain::Rail, Terrain::Rail};
  auto waterTile1 = Tile(terrains1);
  const std::array<Terrain, Tile::ROTATIONS> terrains2 = {Terrain::River, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains};
  auto waterTile2 = Tile(terrains2);

  Game game;
  game.placeTileAt(waterTile1, CellId{0, 0}, 2);
  REQUIRE(game.canPlaceTileAt(waterTile2, CellId{1, -1}, 5));
}

TEST_CASE("CannotEndRailWithPlains")
{
  const std::array<Terrain, Tile::ROTATIONS> terrains1 = {Terrain::River, Terrain::Rail, Terrain::Rail, Terrain::Rail, Terrain::Rail, Terrain::Rail};
  auto railTIle = Tile(terrains1);
  const std::array<Terrain, Tile::ROTATIONS> terrains2 = {Terrain::River, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains};
  auto plainsTile = Tile(terrains2);

  Game game;
  game.placeTileAt(railTIle, CellId{0, 0}, 2);
  REQUIRE(!game.canPlaceTileAt(plainsTile, CellId{1, 0}, 5));
}

TEST_CASE("CannotEndARiverWithPlains")
{
  const std::array<Terrain, Tile::ROTATIONS> terrains = {Terrain::River, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains, Terrain::Plains};
  auto waterTile1 = Tile(terrains);
  auto waterTile2 = Tile(terrains);

  Game game;
  game.placeTileAt(waterTile1, CellId{0, 0}, 2);
  REQUIRE(!game.canPlaceTileAt(waterTile2, CellId{1, -1}, 1));
}

TEST_CASE("CanOrCannotFinishTask")
{
  Tile tile1{"__F___"}, tile2{"F__F__"}, tile3{"FF____"}, tile4{"FFF___"};
  Tile tile5{"FF____"};
  Game game;
  tile1.setTask(Terrain::Forest);
  const int taskSize = 5;
  game.placeTileAt(tile1, CellId{0, 0}, 0, taskSize);
  game.placeTileAt(tile2, CellId{1, -1}, 5);
  game.placeTileAt(tile3, CellId{2, -1}, 4);
  game.placeTileAt(tile4, CellId{2, 0}, 0);

  REQUIRE(game.canPlaceTileAt(tile5, CellId{3, -1}, 5)); // land, finish closed 5-task
  REQUIRE(game.canPlaceTileAt(tile5, CellId{3, -1}, 4)); // land, finish open 5-task
  tile5.setTask(Terrain::Forest);
  REQUIRE(game.canPlaceTileAt(tile5, CellId{3, -1}, 4, 6));  // open 6-task, finish open 5-task
  REQUIRE(!game.canPlaceTileAt(tile5, CellId{3, -1}, 5, 6)); // closed 6-task with 5 tiles (impossible), finish closed 5-task
  REQUIRE(!game.canPlaceTileAt(tile5, CellId{3, -1}, 5, 4)); // closed 4-task with 5 tiles (impossible), finish closed 5-task
}

TEST_CASE("CheckFirstFourMoveTypes")
{
  // TODO: take, assert task, play, repeat x3. Take, assert land.
}