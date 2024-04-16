#pragma once

#include "board.h"
#include "tile.h"

struct Move
{
public:
    Tile *tile; // non-owning
    CellId position;
    int rotation;
    std::optional<int> taskSize;
};