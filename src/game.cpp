#include "game.h"

Game Game::fromYaml(const YAML::Node &rootNode)
{
    if (rootNode.Type() != YAML::NodeType::Map)
    {
        throw std::runtime_error("expected map as a top-level yaml node");
    }
    const auto &tiles = rootNode["tiles"];
    if (tiles.Type() != YAML::NodeType::Sequence)
    {
        throw std::runtime_error("expected tiles to be a sequence");
    }
    Game game;
    for (const auto &tile : tiles)
    {
        auto the_tile = Tile::fromYaml(tile);
        if (the_tile.isLand())
        {
            game.m_lands.push_back(the_tile);
        }
        else if (the_tile.isTask())
        {
            game.m_tasks.push_back(the_tile);
        }
    }
    return game;
}