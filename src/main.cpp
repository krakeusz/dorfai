#include "yaml-cpp/yaml.h"

#include "game.h"

#include <iostream>

namespace
{
    struct Args
    {
        std::string tilesYamlPath;
    };

    Args parseArgs(int argc, char **argv)
    {
        if (argc != 2)
        {
            std::cout << "Error: did not pass the expected number of args." << std::endl;
            std::cout << "Usage: " << argv[0] << " path-to-tiles-yaml" << std::endl;
            std::exit(1);
        }
        Args args;
        args.tilesYamlPath = argv[1];
        return args;
    }
}

int main(int argc, char **argv)
{
    Args args = parseArgs(argc, argv);
    YAML::Node rootNode = YAML::LoadFile(args.tilesYamlPath);
    Game game = Game::fromYaml(rootNode);
    std::cout << "Game loaded! There are " << game.getTiles().size() << " tiles." << std::endl;
    return 0;
}