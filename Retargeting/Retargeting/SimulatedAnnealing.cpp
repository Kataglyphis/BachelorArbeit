#include "SimulatedAnnealing.h"

using namespace cppOpt;

auto toOptimize = [](OptCalculation<double>& optCalculation) {
	double
		A(10.0),
		n(2.0),
		pi(3.1415926535897932384),
		x1 = optCalculation.get_parameter("x1"),
		x2 = optCalculation.get_parameter("x2");

	double sum = (x1 * x1 - A * cos(2.0 * pi * x1)) + (x2 * x2 - A * cos(2.0 * pi * x2));

	optCalculation.result = A * n + sum;
};

bool Annealing::SimulatedAnnealing::fromBlueNoiseToRetarget() {

	using namespace std;

	OptBoundaries<double> optBoundaries;
	optBoundaries.add_boundary({ -5.12, 5.12, "x1" });
	optBoundaries.add_boundary({ -5.12, 5.12, "x2" });

	//number of calculations we are allowing
	unsigned int maxcalculations = 3000;

	//we want to find the minimum
	OptTarget optTarget = cppOpt::OptTarget::MINIMIZE;

	//how fast the simulated annealing algorithm slows down
	//http://en.wikipedia.org/wiki/Simulated_annealing
	double coolingFactor = 0.99;

	//the chance in the beginning to follow bad solutions
	double startChance = 0.25;

	//define your coordinator
	OptCoordinator<double, false> coordinator(
		maxcalculations,
		toOptimize,
		optTarget,
		0);

	//add simulated annealing as child
	coordinator.add_child(make_unique<cppOpt::OptSimulatedAnnealing<double>>(
		optBoundaries,
		coolingFactor,
		startChance));

	//let's go
	coordinator.run_optimisation();

	//print result
	OptCalculation<double> best = coordinator.get_best_calculation();

	return true;
}