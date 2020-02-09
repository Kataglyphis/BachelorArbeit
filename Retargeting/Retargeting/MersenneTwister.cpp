#include "MersenneTwister.h"

uint64_t MersenneTwister::generate(uint64_t seed) {

    //launchIndex.x + launchIndex.y * launchDim.x, gFrameCount
    uint64_t backoff = 16;
    uint64_t v0 = seed;
    uint64_t v1 = 1;
    uint64_t s0 = 0;

    for (uint64_t n = 0; n < backoff; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}

std::string MersenneTwister::getName() {
    return "MersenneTwister";
}