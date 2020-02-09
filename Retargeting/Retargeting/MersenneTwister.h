#pragma once
#include <cstdint>
#include "RandomnessStrategy.h"
#include <string>

class MersenneTwister : public RandomnessStrategy
{
public:
	uint32_t generate(uint32_t seed);
	std::string getName();
};

