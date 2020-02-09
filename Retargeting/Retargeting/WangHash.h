#pragma once
//http://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
#include <stdint.h>
#include "RandomnessStrategy.h"
#include <string>

class WangHash : public RandomnessStrategy {
public:
	uint32_t generate(uint32_t seed);
	std::string getName();
};