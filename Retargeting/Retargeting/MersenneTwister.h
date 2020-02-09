#pragma once
#include <cstdint>
#include "RandomnessStrategy.h"
#include <string>

class MersenneTwister : public RandomnessStrategy
{
public:
	uint64_t generate(uint64_t seed);
	std::string getName();
};

