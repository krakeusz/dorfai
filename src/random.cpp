#include "random.h"

void setSeed(unsigned seed)
{
    getEngine().seed(seed);
}

std::mt19937 &getEngine()
{
    static std::mt19937 engine{std::random_device()()};
    return engine;
}