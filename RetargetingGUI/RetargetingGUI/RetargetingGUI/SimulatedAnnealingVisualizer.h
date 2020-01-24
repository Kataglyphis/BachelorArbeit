#pragma once
#pragma push_macro("slots")
#undef slots
#include "libs/matplotlib/matplotlibcpp.h"
#pragma pop_macro("slots")
#include <sstream>
#include <string>
#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "Inverse.h"
#include "Lineary.h"

namespace plt = matplotlibcpp;

typedef std::vector<int> Energy;
typedef std::vector<double> Deltas;
typedef std::vector<int> Probabilities;

class SimulatedAnnealingVisualizer
{
public:
	SimulatedAnnealingVisualizer();
	SimulatedAnnealingVisualizer(AnnealingSchedule* schedule);
	void visualizeEnergyOverSteps(Energy energy);
	void visualizeAcceptanceProbabilities(Deltas deltas, Probabilities probs);

private:
	AnnealingSchedule* schedule;
	std::string folder_energy = "pictures/Energy/";
};

