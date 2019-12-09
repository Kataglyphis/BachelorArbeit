#include "SimulatedAnnealing.h"

bool SimulatedAnnealing::fromBlueNoiseToRetarget(){

	return true;
}

bool SimulatedAnnealing::toroidallyShift() {
	double g = 1.32471795724474602596;
		double a1 = 1.0 / g;
		double a2 = 1.0 / (g * g);
		//x[n] = (0.5 + a1 * n) % 1;
		//y[n] = (0.5 + a2 * n) % 1;
	return true;
}