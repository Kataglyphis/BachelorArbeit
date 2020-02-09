#include "MersenneTwister.h"

uint32_t MersenneTwister::generate(uint32_t seed) {

    //launchIndex.x + launchIndex.y * launchDim.x, gFrameCount
    uint32_t backoff = 16;
    uint32_t v0 = seed;
    uint32_t v1 = 1;
    uint32_t s0 = 0;

    for (uint32_t n = 0; n < backoff; n++)
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