#include "WangHash.h"

uint64_t WangHash::generate(uint64_t seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

std::string WangHash::getName() {
    return "WangHash";
}