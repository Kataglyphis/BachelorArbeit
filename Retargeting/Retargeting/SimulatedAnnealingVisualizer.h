#pragma once
#include "matplotlib/matplotlibcpp.h"

namespace plt = matplotlibcpp;

#define M_PI  3.14159265359

typedef std::vector<int> Energy;

class SimulatedAnnealingVisualizer
{
public:
	SimulatedAnnealingVisualizer();
	void visualizeEnergyOverSteps(Energy energy);
};

