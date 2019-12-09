#pragma once
#include "helpers.h"

class SimulatedAnnealing {
	public:
		SimulatedAnnealing() {};
		bool fromBlueNoiseToRetarget();
		bool toroidallyShift();
	private:
		helpers helper;
};