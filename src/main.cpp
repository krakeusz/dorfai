#include "tile.h"

#include <iostream>

int main()
{
    auto tile = Tile::fromIStream(std::cin);
    std::cout << "Tile loaded!" << std::endl;
    return 0;
}