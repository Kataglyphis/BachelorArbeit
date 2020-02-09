#pragma once
#include <cstdint>
#include <string>
class RandomnessStrategy
{
public:
	virtual uint64_t generate(uint64_t seed) = 0;
	virtual std::string getName() = 0;
};

