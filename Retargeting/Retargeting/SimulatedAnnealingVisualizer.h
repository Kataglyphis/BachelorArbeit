#pragma once
#include "matplotlib/matplotlibcpp.h"
#include <sstream>
#include <string>
#include "AnnealingSchedule.h"
#include "Hajek.h"

namespace plt = matplotlibcpp;

#define M_PI  3.14159265359

typedef std::vector<int> Energy;

class SimulatedAnnealingVisualizer
{
public:
	SimulatedAnnealingVisualizer();
	SimulatedAnnealingVisualizer(AnnealingSchedule* schedule);
	void visualizeEnergyOverSteps(Energy energy);

	AnnealingSchedule* schedule;
};

