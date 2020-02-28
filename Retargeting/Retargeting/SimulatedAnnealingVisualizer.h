#pragma once
#include "matplotlib/matplotlibcpp.h"
#include <sstream>
#include <string>

#include "AnnealingSchedule.h"
#include "Hajek.h"
#include "ExponentialCoolDown.h"
#include "Inverse.h"
#include "Lineary.h"

namespace plt = matplotlibcpp;

#define M_PI  3.14159265359

typedef std::vector<int> Energy;
typedef std::vector<double> Deltas;
typedef std::vector<int> Probabilities;
typedef std::vector<int> Temperatures;


class SimulatedAnnealingVisualizer
{
public:
	SimulatedAnnealingVisualizer();
	SimulatedAnnealingVisualizer(AnnealingSchedule* schedule);
	void visualizeEnergyOverSteps(Energy energy);
	void visualizeAcceptanceProbabilities(Deltas deltas, Probabilities probs);
	void visualizeTemperatureOverSteps(Temperatures temperatures);

private:
	AnnealingSchedule* schedule;
	std::string folder_energy = "pictures/Energy/";
	std::string folder_temperature = "pictures/Temperature/";
};

