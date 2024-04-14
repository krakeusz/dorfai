#include "random.h"

void setSeed(unsigned seed)
{
    getRandomEngine().seed(seed);
}

std::mt19937 &getRandomEngine()
{
    static std::mt19937 engine{std::random_device()()};
    return engine;
}