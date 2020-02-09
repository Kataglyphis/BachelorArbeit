#pragma once
#include <cstdint>
#include <string>
class RandomnessStrategy
{
public:
	virtual uint32_t generate(uint32_t seed) = 0;
	virtual std::string getName() = 0;
};

