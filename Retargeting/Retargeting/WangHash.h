#pragma once
//http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
#include <stdint.h>
class WangHash {
public:
	uint32_t generate(uint32_t seed);
};