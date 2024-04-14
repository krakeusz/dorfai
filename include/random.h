#pragma once

#include <random>

template <class TDistribution>
auto getRandom(TDistribution &&dist);

void setSeed(unsigned seed);

std::mt19937 &getRandomEngine();

template <class TDistribution>
inline auto getRandom(TDistribution &&dist)
{
    return dist(getRandomEngine());
}
